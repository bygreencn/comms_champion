//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "AppMgr.h"

#include <iostream>

CC_DISABLE_WARNINGS()
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
CC_ENABLE_WARNINGS()

#include "comms_champion/property/message.h"

namespace cc = comms_champion;

namespace comms_dump
{

namespace
{

const std::string Sep(", ");
const int FlushInterval = 1000;

}  // namespace

AppMgr::AppMgr()
  : m_csvDump(std::cout, Sep)
{
    m_msgMgr.setMsgAddedCallbackFunc(
        [this](cc::MessagePtr msg)
        {
            if (!msg) {
                assert(!"Application message wasn't provided");
                return;
            }

            auto timestamp = cc::property::message::Timestamp().getFrom(*msg);
            if (timestamp != 0) {
                m_csvDump.outStream() << timestamp << Sep;
            }

            msg->dispatch(m_csvDump);
        });

    m_msgSendMgr.setSendMsgsCallbackFunc(
        [this](cc::MsgSendMgr::MessagesList&& msgs)
        {
            m_msgMgr.sendMsgs(std::move(msgs));
        });

    m_msgSendMgr.setSendCompeteCallbackFunc(
        [this]()
        {
            QTimer::singleShot(m_lastWait, qApp, SLOT(quit()));
        });

    connect(
        &m_flushTimer, SIGNAL(timeout()),
        this, SLOT(flushOutput()));
}

AppMgr::~AppMgr() = default;

bool AppMgr::start(const Config& config)
{
    if (config.m_pluginsDir.isEmpty()) {
        std::cerr << "ERROR: Unknown plugins directory!" << std::endl;
        return false;
    }
    m_pluginMgr.setPluginsDir(config.m_pluginsDir);

    if (config.m_pluginConfigFile.isEmpty()) {
        std::cerr << "ERROR: The plugins configuration file wasn't provided" << std::endl;
        return false;
    }
    auto plugins = m_pluginMgr.loadPluginsFromConfigFile(config.m_pluginConfigFile);
    if (plugins.empty()) {
        std::cerr << "ERROR: No plugins were loaded" << std::endl;
        return false;
    }

    if (!applyPlugins(plugins)) {
        std::cerr << "ERROR: Failed to apply plugins" << std::endl;
        return false;
    }

    m_msgMgr.setRecvEnabled(true);
    m_msgMgr.start();
    m_lastWait = config.m_lastWait;

    if (!config.m_outMsgsFile.isEmpty()) {
        auto protocol = m_msgMgr.getProtocol();
        assert(protocol);

        auto msgsToSend =
            m_msgFileMgr.load(
                cc::MsgFileMgr::Type::Send,
                config.m_outMsgsFile,
                *protocol);

        if (!msgsToSend.empty()) {
            m_msgSendMgr.start(protocol, msgsToSend);
        }
    }
    else if (0 < m_lastWait) {
        QTimer::singleShot(m_lastWait, qApp, SLOT(quit()));
    }

    m_flushTimer.start(FlushInterval);
    return true;
}

void AppMgr::flushOutput()
{
    auto& outStream = m_csvDump.outStream();
    if (outStream) {
        outStream.flush();
    }
}

bool AppMgr::applyPlugins(const ListOfPluginInfos& plugins)
{
    typedef cc::Plugin::ListOfFilters ListOfFilters;

    struct ApplyInfo
    {
        cc::SocketPtr m_socket;
        ListOfFilters m_filters;
        cc::ProtocolPtr m_protocol;
    };

    auto applyInfo = ApplyInfo();
    for (auto& info : plugins) {
        cc::Plugin* plugin = m_pluginMgr.loadPlugin(*info);
        if (plugin == nullptr) {
            assert(!"Failed to load plugin");
            continue;
        }

        if (!applyInfo.m_socket) {
            applyInfo.m_socket = plugin->createSocket();
        }

        applyInfo.m_filters.append(plugin->createFilters());

        if (!applyInfo.m_protocol) {
            applyInfo.m_protocol = plugin->createProtocol();
        }
    }

    if (!applyInfo.m_socket) {
        std::cerr << "ERROR: Socket hasn't been set!" << std::endl;
        return false;
    }

    if (!applyInfo.m_protocol) {
        std::cerr << "ERROR: Protocol hasn't been set!" << std::endl;
        return false;
    }

    m_msgMgr.setSocket(std::move(applyInfo.m_socket));

    if (!applyInfo.m_filters.isEmpty()) {
        assert(!"Filters support hasn't been implemented yet");
        // TODO: add filters
    }

    m_msgMgr.setProtocol(std::move(applyInfo.m_protocol));

    m_pluginMgr.setAppliedPlugins(plugins);
    return true;
}

} /* namespace comms_dump */

# How to Build

This project uses [CMake](https://cmake.org) cross-platform build system to
generate required build files native to the platform.

- Create build directory somewhere and navigate there.

>$> mkdir /some/build/dir && cd /some/build/dir

- Generate Makefiles (or any other build environment) for **Release** build.

>$> cmake -DCMAKE_BUILD_TYPE=Release /path/to/comms_champion/sources

- Build and install.

>$> make install

After the build is complete, all the binaries, headers, libraries will reside
in **install** subdirectory of the directory chosen for build (/some/build/dir) .

## Available CMake Options

In addition to built-in options/variables of CMake, such as **CMAKE_BUILD_TYPE** or
**CMAKE_TOOLCHAIN_FILE**, the following ones can be used:

- **CC_COMMS_LIB_ONLY**=ON/OFF - Exclude compilation of all the tools, install only
**COMMS** Library. Default value is **OFF**, i.e. other tools get built.

- **CC_COMMS_UNIT_TESTS**=ON/OFF - Include/Exclude unittests of the **COMMS** library.
Default value is **OFF**

- **CC_COMMS_CHAMPION**=ON/OFF - Enable/Disable build of **comms_champion** application. Default value
is **ON**.

- **CC_COMMS_DUMP**=ON/OFF - Enable/Disable build of **comms_dump** application. Default value
is **ON**.

- **CC_COMMS_CHAMPION_DEMO**=ON/OFF - Enable/Disable build of internal "Demo" protocol
to demonstrate abilities of CommsChampion tool. Default value is **ON**.

- **CC_STATIC_RUNTIME**=ON/OFF - Enable/Disable static link to C++ runtime. Default value is **OFF**

- **CC_QT_DIR**=dir - Directory of QT5 installation. Can be used to provide path to QT5 if
differs from system default installation path.

## Install Only **COMMS** Library Example

>$> cd /path/to/comms_champion/sources

>$> mkdir build && cd build

>$> cmake -DCMAKE_BUILD_TYPE=Release -DCC_COMMS_LIB_ONLY=ON ..

>$> make install 

The example above will skip build of any tool available, it will just install 
the **COMMS** library headers in **install/include** subdirectory.

## Include Unit Tests to Build Example

>$> cd /path/to/comms_champion/sources

>$> mkdir build && cd build

>$> cmake -DCMAKE_BUILD_TYPE=Release -DCC_COMMS_UNIT_TESTS=ON ..

>$> make install

The example above builds and installs all the tools. 

The built **CommsChampion** tool binary will reside in
**install/bin** subdirectory and available
plugins for it will reside in **install/plugin** subdirectory.

## Building Documentation
The documentation is not created during normal build process. The documentation of
the [COMMS Library](#comms-library) can be generated by building **doc_comms**
target:
 
>$> make doc_comms

The HTML documentation will reside in **install/doc/comms/html** subdirectory.

The documentation of
the [Demo](#demo-protocol) protocol can be generated by building **doc_demo**
target:
 
>$> make doc_demo

The HTML documentation will reside in **install/doc/demo/html** subdirectory.

**NOTE**, that you must have 
[Doxygen](www.doxygen.org) 
and [Dia](https://wiki.gnome.org/Apps/Dia) 
being installed on your system in order to be able to properly generate the 
documentation.

# Working Builds
The build hast been tested with the following compilers:

- GCC v4.8.4 on Ubuntu 14.04
- GCC v5.3.1 on Ubuntu 16.04
- Visual Studio 2015 on Windows 7
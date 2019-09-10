# VP1Light Release

**Table of Contents**:

 * [Intro](#intro)
 * [License and Distribution](#license-and-distribution)
 * [Install and use VP1Light](#install-vp1light)
   * [Run VP1Light on macOS](#macos)
   * [Run VP1Light on Ubuntu](#ubuntu)
 * [Build VP1 (for developers)](#build-vp1light-for-developers)
   * [General Pre-requisites](#general-pre-requisites)
   * [Mac OS instructions](#install-vp1light-on-mac-os)
   * [Ubuntu instructions](#install-vp1light-on-ubuntu)
 * [Sample data](#sample-data)
 * [Antialiasing](#notes-on-antialiasing)

----

## Intro

This project builds `VP1Light`, a standalone 3D event display which lets you visualize the ATLAS Geometry and the content of `xAOD` data files.

If you have problems installing VP1Light, or you get errors while building and running it,
or if you see discrepancies in the geometry,

* please send us your feedback at: <mailto:hn-atlas-vp1-help@cern.ch>
* or open a ticket on <https://its.cern.ch/jira/projects/ATLASVPONE/summary>


## License and Distribution

**A NOTE:**
*Please, don't share the geometry data file outside ATLAS, even if it can be used without any other ATLAS software. They are ATLAS data.*

----

## Install VP1Light

### macOS

_[to be updated...]_


### Ubuntu

Tested on Ubuntu 18.0.4 (Bionic Beaver) and 16.0.4

```
sudo add-apt-repository ppa:kaktusjoe/rootdist
sudo apt-get update
sudo apt install vp1light
```

**Issues:**

1. **graphics display / OpenGL**

   If you get errors like those:

   ```
   libGL error: No matching fbConfigs or visuals found
   libGL error: failed to load driver: swrast
   Error: couldn't get an RGB, Double-buffered visual
   ```

   It means your OpenGL (a graphics library needed for 3D) installation is faulty or not compatible with your hardware. Installing a modern graphics driver can help to fix that. For example, we tested this NVidia driver on a 'vanilla' Ubuntu 18.0.4 (OpenStack virtual machine) and it worked:

   ```
   sudo apt install nvidia-driver-390
   ```

   After the installation, you can test your new OpenGL environment with:

   ```
   glxgears
   ```

   If a windows with 3D gears appears, that usually means that your OpenGL environment is now healthy. You can now run VP1Light.


2. **xAOD data files / ROOT**

   When you open an xAOD file in VP1Light, if VP1Light crashes with errors about xAOD classes (e.g., `xAODTracking`) or ROOT classes (e.g., `TEvent`), that usually means that you have two conflicting ROOT installations on your system, or that the ROOT version you use has not been compiled with C++14, which is needed by the ATLAS software.

   The Ubuntu VP1Light `apt` packages contain the needed version of ROOT, compiled with C++14.

   To fix the errors, you should remove the conflicting/existing ROOT and reinstall the VP1Light Ubuntu packages.

   You can start by removing the VP1Light packages:

   ```
   sudo apt remove vp1light

   ```

   Then, look for the remaining ROOT installation and remove it from your system. After that, you can reinstall VP1Light:

   ```
   sudo apt install vp1light

   ```

   Note: You can remove the version of ROOT shipped with VP1Light by simply run `sudo apt remove vp1light`.
   Then, you can install any ROOT version you want.
   Moreover, if you need a given version of ROOT for your work or your physics analysis,
   you can install it under a separate folder; then, you can set it up by running the thisroot.sh script you can find inside the `bin` directory of the ROOT folder.


----

## Build VP1Light (for developers)

If a build is not available for your machine, or you want to work on the code base of VP1Light, you can build it from source. Here below the detailed step-by-step instructions.


### General Pre-requisites

#### Install the Qt5 framework

VP1Light uses the Qt5 graphics framework. It is recommended to install it on your system before compiling/installing VP1Light.

If a Qt5 installation is not found by the VP1Light build script, the whole Qt5 framework is downloaded and compiled from source, which takes a long time. That's why it is highly recommended to install the right Qt5 package for your platform before building VP1Light.

You can find installation instructions here:

<https://www.qt.io/download>

**NOTES:**

 * Please, pay attention to download the "Open Source" version of Qt5.
 * Please, notice that you can skip the creation of a Qt account when installing the open source version. When you get the window shown below, just continue clicking on the `Skip` button. **Note:** With the latest versions of Qt5 (_e.g._, 5.12.0), the "Skip" button is hidden if an email address is set in the form, and the installer automatically sets the address if it can find one related to the user. However, if you delete the email address from the form, the "Skip" button appears. Now, you can skip the creation of the Qt account and you can continue installing the Open Source version of Qt5.


### Build VP1Light on macOS

#### Build VP1LightExternals on macOS

First, build and install the VP1LightExternals:

```
git clone https://gitlab.cern.ch/rbianchi/atlasexternals.git
cd atlasexternals
git checkout master-eigen-update
cd ../

mkdir build_ext
cd build_ext

# run CMake
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCTEST_USE_LAUNCHERS=TRUE ../athena/Projects/VP1LightExternals/

# build
make -j 4

# install
make install DESTDIR=../install/VP1LightExternals
```

##### Notes

If you get errors like this one:

```
fatal error: 'stdlib.h' file not found
```

Then, probably you have to install or update the Xcode command line tools and the C++ headers (it is often necessary also after a major update of Xcode).

At first, try this:

```
xcode-select --install
```

If that fixes the problem, that's enough.
If not, try this:

```
cd /Library/Developer/CommandLineTools/Packages/
open macOS_SDK_headers_for_macOS_10.14.pkg
```

That will install the missing headers, which are somehow "hidden" on the latest macOS versions.


#### Build VP1Light on macOS

After having built and installed `VP1LightExternals`, you can build and install `VP1Light` itself.

_[please, replace `<your-platform>` with your actual platform]_

```
git clone https://gitlab.cern.ch/rbianchi/athena.git
cd athena
git checkout master-vp1light-bundle
cd ../

mkdir build_vp1light
cd build_vp1light

# source the Externals setup script
source ../install/VP1LightExternals/InstallArea/<your-platform>/setup.sh

# run CMake
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCTEST_USE_LAUNCHERS=TRUE ../athena/Projects/VP1Light/

# build
make -j 4

# install
make install DESTDIR=../install/VP1Light
```

Then, set the environment and launch:

```
source ../install/VP1Light/InstallArea/x86_64-mac1014-clang100-opt/setup.sh
vp1light
```

If you get errors, see the Notes below.

##### Notes

###### Qt RPATH

While launching `vp1light`, if you get an error like this:

```
$ vp1light
dyld: Library not loaded: @rpath/QtCore.framework/Versions/5/QtCore
  Referenced from: /Users/rbianchi/code_work_local/atlas/vp1light-bundle-MR-Apr2019/from_code/build_vp1light/x86_64-mac1014-clang100-opt/bin/vp1light
  Reason: image not found
Abort trap: 6
```

Then, it means Qt is not installed/configured properly. We will work on a solution to be embedded in the CMake setup; however, for the moment, as a temporary fix, you can manually add the Qt lib path as `rpath` to the `vp1light` executable, like this (please, adjust the Qt path and the vp1light executable paths to your setup):

```
install_name_tool -add_rpath /Users/rbianchi/Qt5.12.0/5.12.0/clang_64/lib ../install/VP1Light/InstallArea/x86_64-mac1014-clang100-opt/bin/vp1light
```

After that, you should be able to launch `vp1light` without any other problem.

###### ROOT

If you have a local installation of ROOT 6.14 (for example installed with `brew`, you might get errors due to the different versions of the ROOT header files found on the system. For example you could get an error like this: `error: use of undeclared identifier 'R__likely'`. Disinstalling the brew ROOT package should solve the problem (you can see the list of the packages installed on your machine by using `brew list`; then, you can uninstall the unwanted packages with by typing `brew uninstall <package-name>`).

###### Blank 3D window on macOS Mojave

If you use macOS Mojave, and you get a blank 3D window (i.e., you switch on a piece of geometry, and you don't get it in the 3D window of VP1Light), then you probably should update your macOS installation. Please, be sure you are running 10.14.4 with Xcode 10.


### Install VP1Light on Ubuntu

Tested on Ubuntu 18.0.4 and 16.0.4

#### Pre-requisites

**Note**: You can install the dependencies listed below one by one, by hand, or you can run [the provided script](SETUP_VP1Light_DEPENDENCIES_UBUNTU.sh); for that, just type:

```
source SETUP_VP1Light_DEPENDENCIES_UBUNTU.sh
```

If you used the script to install all the dependencies in one go, you can now jump [to the next section](#install-the-atlasexternals).

##### Development Tools

 * Git:

        `sudo apt install git`

 * CMake:

        `sudo apt install cmake`

    *Note:* you need a fairly recent version of CMake (at least 3.9). On Ubuntu 16.0.4 the command below installs CMake 3.5 by default. In that case, please refer to the CMake website (<https://cmake.org/download/>), for a more recent version. Once downloaded, you can build and install it with:


        tar -xvf <downloaded_package>
        cd <cmake-xxx-folder>
        ./configure
        make
        sudo make install

    When built and installed, open a new shell (otherwise you get errors from the old version of CMake), and try it the new version with:

        which cmake
        cmake --version




##### Libraries

###### `AthenaExternals` dependencies.

**Please notice**: install these libraries **before** building the Externals, in order to avoid later problems (for example, if Qt does not see SSL libraries installed on the system, it will be built without SSL support, which then brakes the compilation of VP1Gui).

 * PythonLibs, needed by ROOT:

        `sudo apt-get install python-dev`

 * GLUT, needed by Coin3D:

        `sudo apt-get install freeglut3-dev`

 * ZLIB and OPENSSL, needed by XRootD:

        `sudo apt-get install zlib1g-dev`
        `sudo apt-get install libssl-dev`

 * XPM and XFT, needed by ROOT:

        sudo apt-get install libgd-dev
        sudo apt-get install libxft-dev

###### `Athena` dependencies:

 * TBB, needed by xAODRootAccess:

        `sudo apt-get install libtbb-dev`

 * RPM, which installs `rpmbuild`, needed by CPack:

        `sudo apt-get install rpm`

 * UUID, needed by CxxUtils:

        `sudo apt-get install uuid-dev`

###### Graphics Drivers

Please, install the proper driver for your graphic card. See the section ["Notes on Antialiasing"](#notes-on-antialiasing).


#### Install the AtlasExternals

##### 1. Get the code of the external packages

Clone the repository and get the proper branch:

```
git clone https://gitlab.cern.ch/rbianchi/atlasexternals.git
cd atlasexternals/
git checkout master-vp1light-new-externals-project-v2
```

##### 2. Build the externals

```
cd ../
mkdir buildax
cd buildax/
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCTEST_USE_LAUNCHERS=TRUE ../atlasexternals/Projects/VP1LightExternals/
make
```

##### 3. Install the externals

Now, you can setup the system to use the newly built VP1LightExternals. Please, replace `<platform>` with the correct string for your machine (_e.g._, `x86_64-ubuntu1604-gcc54-opt`)

```
make install DESTDIR=../build/install/VP1LightExternals/0.0.1
source ../build/install/VP1LightExternals/0.0.1/InstallArea/<platform>/setup.sh
cd ..
```

#### Install VP1Light

##### Get the code

Clone the repository and get the proper branch:

```
git clone https://gitlab.cern.ch/rbianchi/athena
cd athena
git checkout master-vp1light-bundle
cd ..
```


##### Build

```
cd athena/Projects/VP1Light
source build_env.sh
./build.sh
cd ../../../
```

##### Setup

Now, you can setup the system to use the newly built VP1Light. Please, replace `<platform>` with the correct string for your machine (_e.g._, `x86_64-ubuntu1604-gcc54-opt`)

```
cd build/install/VP1Light/0.0.1/InstallArea/<platform>/
source setup.sh
```

#### Run


`./bin/vp1light`

**Note:** If you get an error about Qt5 libraries, it is likely that the system tries to access the system libraries, even if you used a more recent version of Qt for the VP1Light compilation. In that case, before running `vplight`, try to set the `LD_LIBRARY_PATH` explicitely, by using:

```
export LD_LIBRARY_PATH=<path-to>/Qt/5.<version>/<platform>/lib:$LD_LIBRARY_PATH
```

Once VP1Light is opened, click on "Geometry studies", then load a `.db` file containing the geometry (see next section).


### Install VP1Light on CentOS7 and SLC6

```
setupATLAS
asetup none,gcc8,cmakesetup

```




----

## Sample data

To play with VP1Light, you can download a (draft) sample data containing the ATLAS geometry from here:

 * https://atlas-vp1.web.cern.ch/atlas-vp1/doc_new/sample_datafiles/geometry-ATLAS-R2-2016-01-00-01.db

You can download the file from your web browser or, in the terminal, you can use `wget` by typing:

     wget https://atlas-vp1.web.cern.ch/atlas-vp1/doc_new/sample_datafiles/geometry-ATLAS-R2-2016-01-00-01.db

----

## Notes on Antialiasing

In VP1Light, you can turn on [antialiasing](https://en.wikipedia.org/wiki/Spatial_anti-aliasing) to smooth the lines in the render of the 3D image.

Please notice, however, that antialiasing depends on the graphical drivers installed on the computer.
So, please install proper drivers on your machine, if you can, without relying on generic drivers.
For example, if you have a NVidia card installed on your machine, consider to [install proper NVidia drivers](http://www.nvidia.com/Download/index.aspx?lang=en-us), without relying on generic drivers (e.g. the MESA driver on Linux): this will let you fully exploit your hardware, and the antialiasing capabilities of VP1Light.

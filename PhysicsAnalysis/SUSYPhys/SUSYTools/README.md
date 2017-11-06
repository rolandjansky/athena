SUSYTools
====================================
**Current Main Developers**: stefano.zambito@cern.ch, keisuke.yoshihara@cern.ch

Note that the newest recommendations are always on the [background forum TWiki page](https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/BackgroundStudies).  Bugs can be reported on [JIRA](https://its.cern.ch/jira/projects/ATLSUSYSW).  In case you are starting out, here are some useful tutorials for [GIT](https://twiki.cern.ch/twiki/bin/view/AtlasComputing/GitTutorial) and [CMake](https://twiki.cern.ch/twiki/bin/view/AtlasComputing/CMakeTestProjectInstructions) as they are used in ATLAS.

------------------------------------
Recommended tags
------------------------------------

The recommended tags are on the [background forum TWiki page](https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/BackgroundStudies).

------------------------------------
AnalysisBase / AthAnalysisBase Setup
------------------------------------

Because these two releases are becoming more similar, most of the instructions are the same.  These instructions refer to 21.2.8 as an example, but you should pick the appropriate version when setting up.  Set up the latest recommended AnalysisBase release::

```bash
setupATLAS
lsetup git
asetup AnalysisBase,21.2.8
```

Or the latest AthAnalysis release::

```bash
setupATLAS
asetup AthAnalysis,21.2.8
```

For working with code, a sparse checkout is pretty straightforward.  
(In order for this to work, you need your own fork of the athena project, see the [ATLAS git tutorial](https://atlassoftwaredocs.web.cern.ch/gittutorial/gitlab-fork/))  

```bash
git atlas init-workdir https://:@gitlab.cern.ch:8443/atlas/athena.git
```

And then the version of SUSYTools in the release can be checked out via::

```bash
cd athena
git atlas addpkg SUSYTools
git checkout -b my21.2 remotes/origin/21.2
```

Then, to compile::

```bash
cd ..
mkdir build
cd build
cmake ../athena/Projects/WorkDir
gmake
source x86_64-slc6-gcc62-opt/setup.sh
```

Additional packages needed on top of the recommended release are documented in `doc/packages.txt`; for now you can add those to your work area via git atlas addpkg.  Afterwards, be sure to recompile everything::

```bash   
cmake ../athena/Projects/WorkDir
gmake
source x86_64-slc6-gcc62-opt/setup.sh
```

and you are ready to go!

## Still to add ##

Convenience script for getting packages we recommend -- at the moment, this is not something that is a part of a git workflow.

Testing
--------------

Unit tests now use [CTest](https://cmake.org/Wiki/CMake/Testing_With_CTest).  To run unit tests, simply go to your build area and type::

```bash
gmake test
```

More complex testing is best done directly with the `ctest` command.  To see what tests are available::

```bash
ctest -N
```

To run specific tests (or tests for specific packages), use the `-R` option to restrict running to tests with a string in the name (e.g. `-R SUSYTools`) or `-E` to exclude certain tests (e.g. `-E SUSYToolsTester`).  To get logs / verbose output, use `-V`.

To test locally in an AnalysisBase release, get your favourite benchmark sample (e.g. `mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.DAOD_SUSY1.e3698_s2608_s2183_r7725_r7676_p2596/`), and run::

```bash
SUSYToolsTester <myAOD.pool.root> maxEvents=100 isData=0 isAtlfast=0 Debug=0 NoSyst=0 2>&1 | tee log
```

The standard `SUSYToolsTester` code is meant to test the configuration of all the relevant CP tools and apply a minimal analysis flow. It includes the recalibration and correction of all physics objects, the recalculation of the MissingET, the extraction of SFs and other specific functionalities that are enabled based on the DxAOD stream. All systematic variations available in the central registry are tested by default, and a cutflow is displayed for each of them. This can be disabled by setting `NoSyst=1`.

To test locally in an AthAnalysis release, run the test job options as::

```bash
cd ..
athena.py SUSYTools/minimalExampleJobOptions_mc.py
```

which is the athena-friendly equivalent of the `SUSYToolsTester` code above for running on MC.  You can also change "mc" to "data" or "atlfast" to run on data or fast simulation if you would prefer.

--------------------------------
Retrieving Cross sections
--------------------------------
All cross-sections are stored in text files in the directories like `data/mc15_13TeV/`.
The `Backgrounds.txt` file contains all cross-sections per background sample (characterized by its dataset (DS) id).
Each signal grid has a corresponding text file with the cross-sections per grid point and per process (characterized by its dataset id and process id (finalState)).

A parser is provided with `SUSYTools` to access it::

```c++
root [0] gSystem->Load("../build/x86_64-slc6-gcc49-opt/lib/libSUSYToolsLib.so")
root [1] #include "SUSYTools/SUSYCrossSection.h"
root [2] SUSY::CrossSectionDB myDB("SUSYTools/data/mc15_13TeV/");
root [3] myDB.xsectTimesEff(410000)
(const float)1.37378997802734375e+02
root [4] myDB.xsectTimesEff(410000,0)
(const float)1.37378997802734375e+02
root [5] myDB.xsectTimesEff(164949, 111)
(const float)8.08409880846738815e-04
root [6] myDB.xsectTimesEff(164949, 1000022, 1000022)
(const float)8.08409880846738815e-04
```

------------------------------------

# Pythia 8 in ATLAS

For any issues or advice, please contact the support team via atlas-generators-pythia@cern.ch.

Significant problems requiring code fixes should be submitted via the ATLAS
Generators JIRA: https://its.cern.ch/jira/projects/AGENE/issues/AGENE-1427


## Introduction

Pythia 8 can be run within Athena by using the `Pythia8_i` interface algorithm.
The resulting HepMC objects are put into the `StoreGate` data store, and may be
used by other packages (e.g full simulation, Atlfast, Rivet, etc.) in the usual
way. The `Pythia_i` interface code is located in [this
package](https://gitlab.cern.ch/atlas/athena/tree/21.6/Generators/Pythia8_i).


## Running Pythia 8 in Athena

`Pythia8_i` can be used from any MC production release, set up with
e.g. `setupATLAS; lsetup asetup; asetup 21.6.20,AthGeneration`.  Releases not
used for MC production will not have been tested, but may nevertheless work.

Pythia8_i should be run via the `Gen_tf.py` transform script in release 21
(formerly `Generate_tf.py` in MC15/releases 19-20, and `Generate_trf.py` in
MC12/release 17).  The transform can download standard "job option" run
configuration scripts automatically, given a job-config/dataset ID number, so if
you just want to run an existing MC process just run like this:

    Gen_tf.py --ecmEnergy=13000 --jobConfig=421113 --maxEvents=10 --outputEVNTFile=test_minbias_inelastic.EVNT.pool.root

If the process you want does not already exist, you will need to make your own
job option script and run it locally: this is covered in the following section.


## Writing job option files

Athena is steered using job option scripts (JOs), written in Python. To run
"just" Pythia8, i.e. internal hard-process simulation rather than external
partonic events via LHE format (for which special settings are often required)
you should start your JO by `include()`ing a base JO fragment from
https://gitlab.cern.ch/atlas-physics/pmg/infrastructure/mc15joboptions/-/tree/master/share

    include("MC15JobOptions/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")

This file itself builds upon a non-"user-facing" set of more fundamental setup
fragments, but you should use these versions with an MC tune (A14) and parton
density set (NNPDF23LO) configured, and usually with some dedicated particle
decays handled by the EvtGen afterburner program.

Any valid Pythia8 command, including those to set the process and generation cuts, may be passed in using

    genSeq.Pythia8.Commands += ['foo=bar']

The beam specifications are set on the command line by `Generate_tf.py` and do not need to be specified in the JO.


## Using a development version of Pythia8_i

To use a development version not already in a release, you will need to check
out and build the relevant packages from the ATLAS Git repository. Before doing
this, follow the ATLAS Git workflow instructions at
https://atlassoftwaredocs.web.cern.ch/gittutorial/workflow-quick/ to get a
personal fork of the ATLAS codebase. Then make and a sparse checkout of the
`Pythia8_i` package with your preferred Git branch or tag:

    setupATLAS; lsetup asetup; lsetup git
    git atlas init-workdir https://:@gitlab.cern.ch/USERNAME/athena.git
    cd athena
    git atlas addpkg Pythia8_i
    git checkout -b 21.6 release/21.6.20 --no-track   #< use your own preferred tag or branch
    cd ..

Now make a build directory separate from the source checkout, set up the release
to build against with your local modifications, and build & set up the run:

    mkdir athena-build && cd athena-build
    asetup 21.6.20,AthGeneration
    cmake -DATLAS_PACKAGE_FILTER_FILE=../package_filters.txt ../athena/Projects/WorkDir
    make
    source x86_64-*/setup.sh


## Using a custom libPythia8 library

Using a new copy of the Pythia8 library for testing is a bit more involved,
because you will also need to check out the `Pythia8` externals package and
override some paths to point at the new version. Here is a full set of commands
to run (thanks to Marvin Flores), again starting from a new checkout of your Git
Athena fork:

    setupATLAS; lsetup asetup; lsetup git
    git atlas init-workdir https://:@gitlab.cern.ch/USERNAME/athena.git athena-py8custom
    cd athena-py8custom
    git atlas addpkg Pythia8 Pythia8_i

You now need to modify the project build instructions to pick up the external
library override, by editing `athena/Projects/WorkDir/CMakeLists.txt`:

1. Just after `find_package( AtlasCMake QUIET )` add, for example,

        set( PYTHIA8_LCGVERSION 301p3 )
        set( PYTHIA8_LCGROOT  /cvmfs/sft.cern.ch/lcg/releases/LCG_88/MCGenerators/pythia8/${PYTHIA8_LCGVERSION}/${LCG_PLATFORM} )
        set( PYTHIA8_VERSION ${PYTHIA8_LCGVERSION})
        set( PYTHIA8_ROOT    ${PYTHIA8_LCGROOT})

2. Just after `# Find the project that we depend on:` add

        find_package(AthGenerationExternals REQUIRED)

And in `athena/External/Pythia8/CMakeLists.txt`, add LCG version overrides just
before `find_package( Pythia8 )`:

    set( PYTHIA8_LCGVERSION 301p3 )
    set( PYTHIA8_LCGROOT /cvmfs/sft.cern.ch/lcg/releases/LCG_88/MCGenerators/pythia8/${PYTHIA8_LCGVERSION}/${LCG_PLATFORM})

Before building, you should also create a `package_filters.txt` file, perhaps
based on `athena/Projects/WorkDir/package_filters_example.txt`. This should contain the
following lines, specifying which packages to build:

    + External/Pythia8
    + Generators/Pythia8_i
    - .*

Finally, (re)build:

    mkdir athena-py8custom-build && cd athena-py8custom-build
    asetup 21.6.20,AthGeneration
    # or: asetup 21.6,latest,AthGeneration,slc6
    cmake -DATLAS_PACKAGE_FILTER_FILE=../package_filters.txt ../athena/Projects/WorkDir
    make
    source x86_64-*/setup.sh

You may wish to run a `make clean` before `make`, to ensure that everything is
definitely rebuilt as intended.


## Sacrifice standalone steering package

In addition to the Athena interface, James Monk has written a standalone package
that steers Pythia 8 without needing Athena, and provides Photos++, LHAPDF, LHEF
and HepMC interfaces and can be more easily steered from the command line.  The
package is available from the AGILe project on HepForge: https://agile.hepforge.org/

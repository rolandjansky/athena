PREPARATION
===========

Use one of the two options for running trnaforms, either run directly from
release, or checkout (newer) transform version locally.


To use transforms from release
------------------------------

If you want to use transforms from release setup AtlasProduction release:
    
    asetup AtlasProduction,devval,rel_1

Then pick an arbitrary place to work:
    
    mkdir -p develop; cd develop


To check out transforms locally
-------------------------------

Alternatively if you want to update transforms which are in `TrigSimTransforms`
package, check it out and build (with cmake):
    
    mkdir -p legacysim/source; cd legacysim/source
    
    # setup release and test area
    asetup AtlasProduction,devval,rel_1,here
    
    # checkout package
    svnco TrigSimTransforms
    cd ..

    # usually build directory is called "build" but some tools are
    # still broken if you don't call it InstallArea
    mkdir InstallArea; cd InstallArea
    cmake ../source
    make

    # update environment for test area
    source $CMTCONFIG/setup.sh

    # go to working directory
    cd ..
    mkdir -p run; cd run


Running individual transform steps
==================================

Now we can run any of the transforms, as long as the input is available.


Digitization step
-----------------

* Input: always available on afs, or check and update the script to change defaults.
* Output: `./digitization/` directory by default, file name is `digiRDO.pool.root`.

To run it:
    
    get_files -scripts runLegacySimulation_digitization.sh
    # modify script if necessary
    ./runLegacySimulation_digitization.sh


RDO to BS step
--------------

* Input: `digitization/digiRDO.pool.root`.
* Output: `./rdo2bs/` directory by default, file name is `digiBS.data`.

To run it:
    
    get_files -scripts runLegacySimulation_rdo2bs.sh
    # modify script if necessary
    ./runLegacySimulation_rdo2bs.sh

To check the ROD versions in produced output:
    
    printRODVersions.py rdo2bs/digiBS.data


Trigger Simulation step
-----------------------

* Input: `./rdo2bs/digiBS.data`.
* Output: `./bs2trigbs/` directory by default, file name is `trigBS.data`

To run it:
    
    get_files -scripts runLegacySimulation_trigsim.sh
    # modify script if necessary
    ./runLegacySimulation_trigsim.sh


RDO+BS merging step
-------------------

* Input: `digitization/digiRDO.pool.root` and `./bs2trigbs/trigBS.data`.
* Output: `./trigbs2rdo/` directory by default, file name is `trigRDO.pool.root`.

To run it:
    
    get_files -scripts runLegacySimulation_merge.sh
    # modify script if necessary
    ./runLegacySimulation_merge.sh


Running three-step transform
============================

Separate script will run three of the above steps in one go: RDO to BS, Trigger
Simulation, and RDO+BS merge.

* Input: check `runLegacySimulation_combined.sh`, modify if necessary
* Output: `./rdo2trigger2rdo/` directory by default, file name is `trigRDO.pool.root`

To run it:
    
    get_files -scripts runLegacySimulation_combined.sh
    # modify script if necessary
    ./runLegacySimulation_combined.sh

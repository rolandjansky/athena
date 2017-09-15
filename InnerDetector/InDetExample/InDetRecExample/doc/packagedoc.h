/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page InDetRecExample_page InDetRecExample Package
@author Edward.Moyse@cern.ch, Markus.Elsing@cern.ch
@section InDetRecExample_InDetRecExampleIntro Introduction

InDetRecExample is the runtime package for running Inner Detector reconstruction and also contains the InnerDetector jobOption fragments that are included in the overall RecExample package. Running the InDetRecExample will only use Inner Detector software and hence is better suited for development of tracking code (being faster and using less memory).

@section InDetRecExample_InDetRecExampleSetup Setup
To set it please do :

   - cd cmt
   - make
   - cd ../run

@section InDetRecExample_InDetRecExampleRunning Running
And then to run the Example:             

   - cd ../run
   - athena ../share/jobOptions.py

@section InDetRecExample_InDetRecExamplejo JobOptions

The main jobOption files are:
   - jobOptions.py ... run ID reconstruction from clusterization to particle creation and produce ESD and AOD
   - ReadInDet_jobOptions.py ... read the produced ESD or AOD with various possibilities i.e. redo the primary vertex finding. Can also read 
bytestrem
   - WriteInDetBS_jobOptions.py ... can be used to create ID bytestream filel from RDO

In addition there are some jobOptions Fragments (kept in share/). You should not need to modify these files, but for completeness they are:

   - InDetRec_all.py               - configures the ID reco to run, included from jobOptions.py.
   - InDetRec_jobOptions.py        - the main InDetRec fragment. This sets up the various algorithms used in ID reconstruction. This is the 
joboptions which is used by RecExCommon 
   - ConfiguredInDetFlags.py       - contains the flags needed to use InDet. Include this file in any top-level jobOptions which use ID. Details of the flags are in the section "InDetFlags"
   - ConfiguredInDetCutvalues.py   - contains the cut values for the ID pattern (only the main ones)
   - ConfiguredInDetKeys.py        - contains the StoreGate Keys for ID containers that might be written to ESD/AOD

The setup of the reconstruction itself is split into several InDetRec*.py files.

@section InDetRecExample_InDetRecExampleJobProperties 
Since release 14 InDetRecExample has entirely migrated to JobProperties (ConfiguredInDetCutValues.py is a small exception where it is questionable if 
it should be jobproperties). There two kind of jobproperties, both are in the python subdirectory:
   - InDetJobProperties.py ... holds the steering flags for ID reconstruction
   - InDetKeys.py ... holds storegate keys for our containers
 
In addition - for convenience - there are still some pure python flags in the top level job options of the package. Notice that these should only be 
used to e.g. ease the steering in the top level job options and to setup up some jobproperty stuff. They should never appear in joboptions below the
top level runtime scripts.

Some flags are:

   - doTruth : controls whether the various truth algorithms are run (default is TRUE)
   - doJiveXML : if true, produce an atlantis data file for each of the events (default is FALSE)
   - doVP1: run the 3D event visualization

The following control what is written out. ESD includes AOD, so it's normally enough
   - doWriteESD (default is TRUE)
   - doWriteAOD (default is TRUE)

@section InDetRecExample_Event Data Model Monitor
In debug the EventDataModelMonitor will run and do instance counting of EDM objects to help flagging memory leaks.



*/

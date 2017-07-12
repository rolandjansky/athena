/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page InDetAlignExample_page InDetAlignExample Package

@author Grant Gorfine
@author Vicente Lacuesta <vicente.lacuesta@ific.uv.es>
@author Muge Karagoz Unel <muge.karagoz.unel@cern.ch>
@author Salva Marti <martis@ific.uv.es>


@section InDetAlignExample_RunInDetAlignExample How to run InDetAlignExample
@subsection Rel13040 Release 13.0.40
 - Checkout the package:
 \code cmt co -r InDetAlignExample-00-00-18 InnerDetector/InDetExample/InDetAlignExample
 \endcode
 - Compile the package, in InDetAlignExample/cmt:
\code 
cmt config
source setup.csh or setup.sh
gmake
\endcode
 - If you want, copy the jobOptions.py to the directory run and modify it

@section InDetAlignExample_InDetAlignExampleIntro Introduction

The package is the common repository for InDet Alignment jobs. 
The bulk data processing scripts with proper algorithm flag settings are placed here.   

This package contains joboptions for creating and reading alignment
constants, testing the alignments and examples of running simulation with
misalignments.


@section InDetAlignExample_InDetAlignExampleJobOptions Job Options

 - WriteDBS.py : Job option fragment for generating random alignment
   constants and putting them in COOL. It is configured to use an
   sqlite file. The recommended may to write to the public database
   is to first write an sqlite file and then use the mergeCoolDB 
   utility. See https://uimon.cern.ch/twiki/bin/view/Atlas/CoolPublishing 

 - TestWriteDBS.py : Standalone job options to use WriteDBS.py

 - SiWriteDBS_CSC.py: Write CSC alignment set

 - SiWriteDBS_Nominal.py: Write a null alignment set

 - SiWriteDBS_ResIni.py: Write alignment set with expected residual misalignments.
 
 - InDetAlignable.py: Job option fragment for common items needed for reading in 
   alignment form conditions database. This is already called in the geometry 
   initialization.
 
 - InDetAlignableOverride.py: Job option fragment to help override alignment folders.

 - ReadDBS.py : Deprecated. Please use InDetAlignableOverride.py.  
   Job option fragment for setting up to read alignment constants from
   COOL.  It is configure for using the CSC alignment sets but can be
   easily configured for using an sqlite file by
   commenting/uncommenting lines. Based on InDetAlignGenAlgs/ReadDBS.py.

 - ReadPool.py: Job option fragment for reading alignments directly
   from POOL file using CondProxyProvider instead of IOVSvc. Based on
   InDetAlignGenAlgs/ReadPool.py

 - TestInDetAlignment_jobOptions.py : Standalone job options for testing
   alignment shifts. Loops through all detector elements and prints
   out their shifts from the nominal position. Uses ReadDBS.py. Various
   use cases can be tested. See the joboptions file for more details.
   You can edit the DetFlags if you wish to turn off detectors.

 - InDetG4Atlas_Sim.py : Standalone job options for running
   Inner Detector simulation with misalignments.

 - InDetG4Atlas_SimGeomTest.py : Standalone job options for
   running Inner Detector simulation geometry test with
   misaligments. This is the standard Geant4 geometry test for
   detecting overlaps. Takes a long time to run (about two hours).


@section InDetAlignExample_InDetAlignExampleUsageNotes Usage Notes

The job options for writing alignment constants are configured for using an sqlite file.

To create this file run

@verbatim
athena InDetAlignExample/TestWriteDBS.py
@endverbatim

This should create the files \p alignobjs.root (the POOL file containing
the alignment constants) \p mycool.db (the sqlite file) and
\p PoolFileCatalog.xml.

The job options for reading the alignments are configured to use the production COOL database.
They can be easily configured for using an sqlite file by commenting/uncommenting lines. 
See ReadDBS.py and WriteDBS.py.

The standalone job options should run out of the box.



*/

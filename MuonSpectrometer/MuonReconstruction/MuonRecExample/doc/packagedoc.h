/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page MuonRecExample_page MuonRecExample

@author Martin Woudstra (current main maintainer)
@author Rosy Nikolaidou (contributor)
@author Kevin Black (contributor)
@author Niels van Eldik (contributor)
@author Jochem Snuverink (contributor)
@author Ketevi Assamagan (contributor)
@author Stephane Willocq (contributor)
@author Ed Moyse (contributor)

@section MuonRecExample_MuonRecExampleIntro Introduction

This package configures the Muon Standalone Reconstruction. It contains only python jobOptions 
and python modules for configuration purposes. It contains no C++ code.

It has a dual role:
 - It provides the main entry point to setup the muon standalone reconstruction within RecExCommon:
    - share/MuonRec_jobOptions.py
 - It includes some example top level jobOptions for users to run only the muon standalone reconstruction:
   - share/MuonRec_myTopOptions.py: run on Monte-Carlo events
   - share/MuonDataRec_myTopOptions.py: run on real data events

@section MuonRecExample_MuonRecExampleRun Running Muon Standalone Reconstruction
To run on Monte Carlo:<br>
<code>
get_joboptions MuonRecExample/MuonRec_myTopOptions.py<br>
</code>
edit MuonRec_myTopOptions.py to set the flags according to your wishes<br>
<code>
athena MuonRec_myTopOptions.py<br>
</code>
To run on real data use MuonRecExample/MuonDataRec_myTopOptions.py instead.

@subsection MuonRecExampleCustom Customize the muon reconstruction for your purpose
There are two complementary ways to configure the muon reconstruction: Flags and Configurable Properties.
 - Flags (a.k.a. JobProperties) are set in the beginning of the jobOptions file, and generally configure
   a number of algorithm, services and/or tools, add algorithms to the topSequence etc. 
   This provides a 'coarse' configurability.
 - Configurable Properties are set at the end of the jobOptions file and directly set individuel Properties
   of individual algorithms, services and/or tools. This is to fine-tune the configuration.
 - If your desired configuration is not provided by the Flags, then you will need to use Configurable Properties.

@subsubsection MuonRecExampleFlags Flags to steer Muon Reconstruction
There are several %Flags containers (JobPropertyContainers) that influence the configuration:
 - @link python/MuonRecFlags.py <code>from MuonRecExample.MuonRecFlags import muonRecFlags</code> @endlink
   @copydoc python::MuonRecFlags
 - @link python/MooreFlags.py <code>from MuonRecExample.MooreFlags import mooreFlags</code> @endlink
   @copydoc python::MooreFlags
 - @link python/MuonboyFlags.py <code>from MuonRecExample.MuonboyFlags import muonboyFlags</code> @endlink
   @copydoc python::MuonboyFlags
 - @link python/MuonRecStandaloneFlags.py <code>from MuonRecExample.MuonRecStandaloneFlags import muonRecFlags</code> @endlink
   @copydoc python::MuonRecStandaloneFlags
 - <code>from AthenaCommon.DetFlags import DetFlags</code> Switch on/off specific tasks per detector technology
 - <code>from AthenaCommon.GlobalFlags import globalflags</code> DetDescrVersion, ConditionsTag, DataSource, InputFormat, ...
 - <code>from AthenaCommon.AthenaCommonFlags athenaCommonFlags</code> Input and output filenames
 - <code>from AthenaCommon.BeamFlags import jobproperties<br>
   beamFlags = jobproperties.Beam</code> beamType ("collisions", "cosmics", "singlebeam"), ...
 - <code>from AthenaCommon.BFieldFlags import jobproperties<br>
   bFieldFlags = jobproperties.BField</code> Which magnetic fields are on/off: Solonoid, Toriod (Barrel/Endcap)
 - <code>from RecExConfig.RecFlags import rec</code> readRDO, doWriteESD, ...
 - <code>from RecExConfig.RecAlgsFlags import recAlgs</code> (only using doTrackRecordFilter)

 
See the respective modules for the full list of available flags and their meaning.

@subsubsection MuonRecExampleConverters Configuring the data converters
The data converters convert BS (bytestream) into RDO (Raw Data Object), and RDO in PRD (Prepared Raw Data).
The converter algorithms for BS to RDO are setup in the jobOptions files share/MuonReadBS_jobOptions.py.
The converter tools and services are configured in the python modules python/MuonReadBSConfig.py and
python/MuonRecTools.py.The converter algorithms for RDO to PRD are configured in the jobOptions file share/MuonRDO_to_PRD_jobOptions.py, which calls the external jobOptions MuonRdoToPrepData/MuonRdoToMuonPrepData_jobOptions.py, which also sets up the converter tools (by including other jobOptions).

\todo Move configuration of all converter tools to module python/MuonRecCnvTools.py.

The converters are configured by the following flags:
- BS to RDO converters
  - <code>DetFlags.readRDOBS.MDT_setOn()</code> Run the MDT BS to RDO converters (tools + algorithms)
  - <code>DetFlags.readRDOBS.RPC_setOn()</code> Run the RPC BS to RDO converters (tools + algorithms)
  - <code>DetFlags.readRDOBS.CSC_setOn()</code> Run the CSC BS to RDO converters (tools + algorithms)
  - <code>DetFlags.readRDOBS.TGC_setOn()</code> Run the TGC BS to RDO converters (tools + algorithms)
  - <code>DetFlags.readRDOBS.Muon_setOn()</code> Run the BS to RDO converters for all technologies
  - <code>DetFlags.readRDOBS.Muon_setOff()</code> Don't run any BS to RDO converters
  - @link python::MuonRecFlags::doSpecialTGC <code>muonRecFlags.doSpecialTGC</code> @endlink Use special TGC ROD decoder to fix bug in DAQ (2008 real data).
- RDO to PRD converters
  - <code>DetFlags.makeRIO.MDT_setOn()</code> Run the MDT RDO to PRD converters (tools + algorithms)
  - <code>DetFlags.makeRIO.RPC_setOn()</code> Run the RPC RDO to PRD converters (tools + algorithms)
  - <code>DetFlags.makeRIO.CSC_setOn()</code> Run the CSC RDO to PRD converters (tools + algorithms)
  - <code>DetFlags.makeRIO.TGC_setOn()</code> Run the TGC RDO to PRD converters (tools + algorithms)
  - <code>DetFlags.makeRIO.Muon_setOn()</code> Run the RDO to PRD converters for all technologies
  - <code>DetFlags.makeRIO.Muon_setOff()</code> Don't run any BS to RDO converters
- To switch off a converter, replace <code>_setOn()</code> above with <code>_setOff()</code>
- IMPORTANT: These DetFlags are also controlled by <code>muonRecFlags.doMDTs, doRPCs, doCSCs and doTGCs</code>, in conjuction with <code>rec.readRDO (True/False)</code> and <code>globalflags.DataFormat ("pool","bytestream")</code>.
  The setting of these flags will be forwarded to the above converter @c DetFlags as appropriate. This is done in the function @c MuonRecFlags.sync_DetFlags(), and you normally don't have to set the @c DetFlags manually.
  In case you do want to override these @c DetFlags directly compared to what the @c muonRecFlags.doXXXs flags set them to, you need to set the @c DetFlags after<br>
<code>include("RecExCond/RecExCommon_Flags.py")</code><br>
because that calls the function @c MuonRecFlags.sync_DetFlags() and you will lose the changes you set before.




*/

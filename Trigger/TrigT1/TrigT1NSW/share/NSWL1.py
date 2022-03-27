# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
import glob
import os

###################################################
# Some basic settings and input file submission
###################################################
MessageSvc.defaultLimit=100
MessageSvc.useColors = True
MessageSvc.Format = "% F%30W%S%7W%R%T %0W%M"

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags, jobproperties
athenaCommonFlags.AllowIgnoreConfigError=False #This job will stop if an include fails
athenaCommonFlags.EvtMax = -1
athenaCommonFlags.SkipEvents = 0

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
import AthenaPoolCnvSvc.ReadAthenaPool # needed to use EventSelector
#to run on mutliple files at once please use -c "customInput='/some/path/*pattern*.root'"
if 'customInput' not in locals() or 'customInput' not in globals():
  print("customInput not defined: the job will be launched on the GRID")
  svcMgr.EventSelector.InputCollections=jobproperties.AthenaCommonFlags.FilesInput.get_Value()
else:
  if(os.path.isdir(customInput)): customInput+="/*.root"
  svcMgr.EventSelector.InputCollections=glob.glob(customInput)

###################################################
# Detector flags: needed to setup geometry tools
###################################################
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.Muon_setOn()
DetFlags.sTGC_setOn()
DetFlags.Micromegas_setOn()
DetFlags.digitize.Micromegas_setOn()
DetFlags.digitize.sTGC_setOn()
DetFlags.Truth_setOn()
DetFlags.Print()

###################################################
# Initialize ATLAS geometry
###################################################
jobproperties.Global.DetDescrVersion="ATLAS-R3S-2021-01-00-02"
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import MuonGM
from AtlasGeoModel import Agdd2Geo

from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
from MuonIdHelpers.MuonIdHelpersConf import Muon__MuonIdHelperSvc
svcMgr += Muon__MuonIdHelperSvc("MuonIdHelperSvc",HasCSC=MuonGeometryFlags.hasCSC(), HasSTgc=MuonGeometryFlags.hasSTGC(), HasMM=MuonGeometryFlags.hasMM())

###################################################
# Initialize the trigger related things
###################################################
include('TrigT1NSW/TrigT1NSW_jobOptions.py')

#Switch on and off trigger simulaton components sTGC / MicroMegas
topSequence.NSWL1Simulation.DosTGC=True
topSequence.NSWL1Simulation.DoStrip=True
topSequence.NSWL1Simulation.UseLookup=False #use lookup table for the pad trigger
topSequence.NSWL1Simulation.DoMM=True
topSequence.NSWL1Simulation.DoMMDiamonds=True

#Toggle Ntuple making for Tools, if set to False for NSWL1Simulation, all the tools are set to False automatically as well
topSequence.NSWL1Simulation.DoNtuple=True
topSequence.NSWL1Simulation.PadTdsTool.DoNtuple=True
topSequence.NSWL1Simulation.PadTriggerTool.DoNtuple=True
topSequence.NSWL1Simulation.StripTdsTool.DoNtuple=True
topSequence.NSWL1Simulation.StripClusterTool.DoNtuple=True
topSequence.NSWL1Simulation.StripSegmentTool.DoNtuple=True
topSequence.NSWL1Simulation.MMTriggerTool.DoNtuple=True

#useful for validation of geometry and offline analyses
topSequence.NSWL1Simulation.PadTriggerLookupTool.DumpSectorGeometry=False

#Tools' Messaging Levels
topSequence.NSWL1Simulation.OutputLevel=INFO
topSequence.NSWL1Simulation.PadTdsTool.OutputLevel=INFO
topSequence.NSWL1Simulation.PadTriggerTool.OutputLevel=INFO
topSequence.NSWL1Simulation.StripTdsTool.OutputLevel=INFO
topSequence.NSWL1Simulation.StripClusterTool.OutputLevel=INFO
topSequence.NSWL1Simulation.StripSegmentTool.OutputLevel=INFO

if MuonGeometryFlags.hasSTGC():
  from MuonRegionSelector.MuonRegionSelectorConf import sTGC_RegSelCondAlg
  from AthenaCommon.AlgSequence import AthSequencer
  from AthenaConfiguration.ComponentFactory import CompFactory
  condseq = AthSequencer('AthCondSeq')
  if not hasattr( condseq, "MuonDetectorCondAlg" ):
    import MuonRecExample.MuonAlignConfig

  from RegionSelector.RegSelToolConfig import makeRegSelTool_sTGC
  tool =  makeRegSelTool_sTGC()
  
#-----------------------------------------------------------------------------
# save ROOT histograms and Tuple
#-----------------------------------------------------------------------------

#S.I 2019 : Below code handles enabling/disabling of ntuples for the Tools according to the master flag (NSWL1Simulation)
#in principle we wouldnt need the tuning here but ntuple making and the trigger code is quite tangled so this is just a workaround for now
#ntuple code must be totally stripped off from trigger Tools. One way of doing is to create a separate tool and implement methods that takes
#    std::vector<std::shared_ptr<PadData>> pads;
#    std::vector<std::unique_ptr<PadTrigger>> padTriggers;
#    std::vector<std::unique_ptr<StripData>> strips;
#    std::vector< std::unique_ptr<StripClusterData> > clusters;
#    as arguments (see NSWL1Simulation.cxx)

if topSequence.NSWL1Simulation.DoNtuple:

    if not hasattr( ServiceMgr, "THistSvc" ):
        from GaudiSvc.GaudiSvcConf import THistSvc
        ServiceMgr += THistSvc()

    ServiceMgr.THistSvc.Output += [ "NSWL1Simulation DATAFILE='NSWL1Simulation.root'  OPT='RECREATE'" ]
    print(ServiceMgr)

else:#to avoid any possible crash. If DoNtuple is set to true for a tool but false for NSWL1Simulation the code will crash
     # ideally that should have been already handled in C++ side
    topSequence.NSWL1Simulation.PadTdsTool.DoNtuple=False
    topSequence.NSWL1Simulation.PadTriggerTool.DoNtuple=False
    topSequence.NSWL1Simulation.StripTdsTool.DoNtuple=False
    topSequence.NSWL1Simulation.StripClusterTool.DoNtuple=False
    topSequence.NSWL1Simulation.StripSegmentTool.DoNtuple=False
    topSequence.NSWL1Simulation.MMTriggerTool.DoNtuple=False

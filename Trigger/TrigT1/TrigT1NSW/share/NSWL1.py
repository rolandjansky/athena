# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#######################################
# some basic settings
#######################################
MessageSvc.defaultLimit=100
MessageSvc.useColors = True
MessageSvc.Format = "% F%30W%S%7W%R%T %0W%M"

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax = -1
athenaCommonFlags.SkipEvents = 0

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
import AthenaPoolCnvSvc.ReadAthenaPool # needed to use EventSelector
svcMgr.EventSelector.InputCollections=["input.rdo.pool.root"]
#######################################


#######################################
# initialize the geometry
#######################################
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion="ATLAS-R3S-2021-01-00-00"
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
# initialize the MuonIdHelperService
from MuonIdHelpers.MuonIdHelpersConf import Muon__MuonIdHelperSvc
svcMgr += Muon__MuonIdHelperSvc("MuonIdHelperSvc",HasCSC=MuonGeometryFlags.hasCSC(), HasSTgc=MuonGeometryFlags.hasSTGC(), HasMM=MuonGeometryFlags.hasMM())
# create the MuonDetectorTool (which creates the MuonDetectorManager needed by PadTdsOfflineTool)
from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
GeoModelSvc.DetectorTools += [ MuonDetectorTool(HasCSC=MuonGeometryFlags.hasCSC(), HasSTgc=MuonGeometryFlags.hasSTGC(), HasMM=MuonGeometryFlags.hasMM()) ]
#######################################


#######################################
# now the trigger related things
#######################################
from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
svcMgr += RegSelSvcDefault()

include('TrigT1NSW/TrigT1NSW_jobOptions.py')

#Switch on and off trigger simulaton components sTGC / MicroMegas
#October 2019 : MM not working so keep it False until fixed

topSequence.NSWL1Simulation.DosTGC=True
topSequence.NSWL1Simulation.UseLookup=False #use lookup table for the pad trigger
topSequence.NSWL1Simulation.DoMM=False
topSequence.NSWL1Simulation.NSWTrigRDOContainerName="NSWTRGRDO"
topSequence.NSWL1Simulation.StripSegmentTool.rIndexScheme=0

#Toggle Ntuple making for Tools, if set to False for NSWL1Simulation, all the tools are set to False automatically as well
topSequence.NSWL1Simulation.DoNtuple=True
topSequence.NSWL1Simulation.PadTdsTool.DoNtuple=True
topSequence.NSWL1Simulation.PadTriggerTool.DoNtuple=True
topSequence.NSWL1Simulation.StripTdsTool.DoNtuple=True
topSequence.NSWL1Simulation.StripClusterTool.DoNtuple=True
topSequence.NSWL1Simulation.StripSegmentTool.DoNtuple=True

#useful for validation of geometry and offline analyses
topSequence.NSWL1Simulation.PadTriggerLookupTool.DumpSectorGeometry=False

#Tools' Messaging Levels
topSequence.NSWL1Simulation.OutputLevel=INFO
topSequence.NSWL1Simulation.PadTdsTool.OutputLevel=INFO
topSequence.NSWL1Simulation.PadTriggerTool.OutputLevel=INFO
topSequence.NSWL1Simulation.StripTdsTool.OutputLevel=INFO
topSequence.NSWL1Simulation.StripClusterTool.OutputLevel=INFO
topSequence.NSWL1Simulation.StripSegmentTool.OutputLevel=INFO

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
    ServiceMgr.THistSvc.Output = ["EXPERT DATAFILE='Monitoring.root' OPT='RECREATE'"];

    if not hasattr( theApp.Dlls, "RootHistCnv" ):
        theApp.Dlls += [ "RootHistCnv" ]
        theApp.HistogramPersistency = "ROOT"

    if not hasattr( ServiceMgr, "NTupleSvc" ):
        from GaudiSvc.GaudiSvcConf import NTupleSvc
        ServiceMgr += NTupleSvc()

    ServiceMgr.THistSvc.Output += [ "NSWL1Simulation DATAFILE='NSWL1Simulation.root'  OPT='RECREATE'" ]
    print ServiceMgr

else:#to avoid any possible crash. If DoNtuple is set to true for a tool but false for NSWL1Simulation the code will crash
     # ideally that should have been already handled in C++ side
    topSequence.NSWL1Simulation.PadTdsTool.DoNtuple=False
    topSequence.NSWL1Simulation.PadTriggerTool.DoNtuple=False
    topSequence.NSWL1Simulation.StripTdsTool.DoNtuple=False
    topSequence.NSWL1Simulation.StripClusterTool.DoNtuple=False
    topSequence.NSWL1Simulation.StripSegmentTool.DoNtuple=False

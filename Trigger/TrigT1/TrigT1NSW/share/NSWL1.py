# This is the configuration file to run ATLAS Digitization
# Use the following switches for:
#   athenaCommonFlags.EvtMax = <NEvents>         Number of events to digitize (set to -1 for all)
#   athenaCommonFlags.SkipEvents = <NSkip>       Number of events from input file collection to skip
#   athenaCommonFlags.PoolHitsInput=<FileName>   Input collections. The list of files with the hits to digitize
#   athenaCommonFlags.PoolRDOOutput=<FileName>   Output file name

#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------


import glob
import os
#to run on mutliple files at once please use -c "customInput='/some/path/*pattern*.root'" 
#otherwise it looks for input.rdo.pool.root
if 'customInput' not in locals() or 'customInput' not in globals():
    print("customInput not defined yet setting the default as input.rdo.pool.root")
    customInput='input.rdo.pool.root'

if(not os.path.isdir(customInput) and not os.path.isfile(customInput) ):
    checklist=glob.glob(customInput)
    if len(checklist)==0:
        print("Invalid INPUT : "+customInput)
        os.sys.exit()
if(os.path.isdir(customInput)):
    customInput+="/*.root"

MessageSvc.defaultLimit=2000
MessageSvc.useColors = True
MessageSvc.Format = "% F%30W%S%7W%R%T %0W%M"


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.AllowIgnoreConfigError=False #This job will stop if an include fails.
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag.set_Value_and_Lock("OFLCOND-MC16-SDR-25") #("OFLCOND-RUN12-SDR-25")
globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-R3S-2021-01-00-00")#("ATLAS-R3-2021-00-00-00")

from RecExConfig.RecFlags import rec as recFlags 
recFlags.doNameAuditor = True

globalflags.InputFormat.set_Value_and_Lock('pool')
import AthenaPoolCnvSvc.ReadAthenaPool
#theApp.EvtMax = 575
athenaCommonFlags.EvtMax = -1
#athenaCommonFlags.SkipEvents = 585     # This one does not works
#svcMgr.EventSelector.SkipEvents = 580  # This one works
import AthenaPoolCnvSvc.ReadAthenaPool


svcMgr.EventSelector.InputCollections=glob.glob(customInput)


#svcMgr.EventSelector.InputCollections = [ "input.rdo.pool.root" ]

from AthenaCommon.DetFlags import DetFlags

#with tgc setOn we get sagfault and crash
#DetFlags.ID_setOff()
#DetFlags.Calo_setOff()
#DetFlags.Muon_setOff()
#DetFlags.MDT_setOff()
#DetFlags.CSC_setOff()
#DetFlags.TGC_setOff()
#DetFlags.RPC_setOff()
DetFlags.sTGC_setOn()
DetFlags.Micromegas_setOff()

#DetFlags.digitize.MDT_setOff() 
#DetFlags.digitize.TGC_setOff() 
#DetFlags.digitize.RPC_setOff() 
#DetFlags.digitize.CSC_setOff() 
#DetFlags.digitize.Micromegas_setOff() 
DetFlags.digitize.sTGC_setOn() 
#DetFlags.Truth_setOff()

# initialize GeoModel with layout set in globalflags.DetDescrVersion
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.MuonVersionOverride = "MuonSpectrometer-R.09.00.NSW" #"MuonSpectrometer-R.09.00.NSW"
from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )


# get AGDD service
from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
Agdd2GeoSvc = AGDDtoGeoSvc()
from AthenaCommon import CfgGetter

# get AGDD tool for inert material
ToolSvc += CfgGetter.getPublicTool("MuonSpectrometer", checkType=True)
Agdd2GeoSvc.Builders += ["MuonAGDDTool/MuonSpectrometer"]

#get AGDD tool for NSW geometry
ToolSvc += CfgGetter.getPublicTool("NewSmallWheel", checkType=True)
Agdd2GeoSvc.Builders += ["NSWAGDDTool/NewSmallWheel"]

theApp.CreateSvc += ["AGDDtoGeoSvc"]
ServiceMgr += Agdd2GeoSvc

include('RegionSelector/RegionSelector_jobOptions.py')
include ('TrigT1NSW/TrigT1NSW_jobOptions.py')

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

# This is the configuration file to run ATLAS Digitization
# Use the following switches for:
#   athenaCommonFlags.EvtMax = <NEvents>         Number of events to digitize (set to -1 for all)
#   athenaCommonFlags.SkipEvents = <NSkip>       Number of events from input file collection to skip
#   athenaCommonFlags.PoolHitsInput=<FileName>   Input collections. The list of files with the hits to digitize
#   athenaCommonFlags.PoolRDOOutput=<FileName>   Output file name

#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------

#from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
#GeoModelSvc = GeoModelSvc()
#GeoModelSvc.MuonVersionOverride="MuonSpectrometer-R.08.01-NSW"
#serhat
import glob
import os
#serhat 
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
#eof serhat

MessageSvc.defaultLimit=10000
MessageSvc.useColors = True
MessageSvc.Format = "% F%30W%S%7W%R%T %0W%M"


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.AllowIgnoreConfigError=False #This job will stop if an include fails.
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag.set_Value_and_Lock("OFLCOND-RUN12-SDR-25")
globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-R3-2021-00-00-00")

from RecExConfig.RecFlags import rec as recFlags 
recFlags.doNameAuditor = True

globalflags.InputFormat.set_Value_and_Lock('pool')

athenaCommonFlags.EvtMax = -1
athenaCommonFlags.SkipEvents = 0




import AthenaPoolCnvSvc.ReadAthenaPool


svcMgr.EventSelector.InputCollections=glob.glob(customInput)


#svcMgr.EventSelector.InputCollections = [ "input.rdo.pool.root" ]

from AthenaCommon.DetFlags import DetFlags

#with tgc setOn we get sagfault and crash
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.MDT_setOff()
DetFlags.CSC_setOff()
DetFlags.TGC_setOff()
DetFlags.RPC_setOff()
DetFlags.sTGC_setOn()
DetFlags.Micromegas_setOff()

DetFlags.digitize.MDT_setOff() 
DetFlags.digitize.TGC_setOff() 
DetFlags.digitize.RPC_setOff() 
DetFlags.digitize.CSC_setOff() 
DetFlags.digitize.Micromegas_setOff() 
DetFlags.digitize.sTGC_setOff() 
DetFlags.Truth_setOff()

# initialize GeoModel with layout set in globalflags.DetDescrVersion
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

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

#-----------------------------------------------------------------------------
# Algorithms:  NSW L1 simulation
#-----------------------------------------------------------------------------
# please import configurables yourself instead.
from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
from AthenaCommon.AppMgr import ServiceMgr
theRegSelSvc = RegSelSvcDefault()
ServiceMgr += theRegSelSvc


include ('TrigT1NSW/TrigT1NSW_jobOptions.py')

#Switch on and off trigger simulaton components sTGC / MicroMegas
topSequence.NSWL1Simulation.DosTGC=True
topSequence.NSWL1Simulation.DoPadTrigger=True
topSequence.NSWL1Simulation.DoMM=False


#Select ntuples to be created
topSequence.NSWL1Simulation.DoNtuple=True
topSequence.NSWL1Simulation.PadTdsTool.DoNtuple=True
topSequence.NSWL1Simulation.PadTriggerTool.DoNtuple=True
topSequence.NSWL1Simulation.StripTdsTool.DoNtuple=True
topSequence.NSWL1Simulation.StripSegmentTool.DoNtuple=True
topSequence.NSWL1Simulation.StripSegmentTool.NSWTrigRDOContainerName="dummyRDO"




#Tools' Messaging Levels
topSequence.NSWL1Simulation.OutputLevel=DEBUG
topSequence.NSWL1Simulation.PadTdsTool.OutputLevel=DEBUG
topSequence.NSWL1Simulation.PadTriggerTool.OutputLevel=DEBUG
topSequence.NSWL1Simulation.StripTdsTool.OutputLevel=DEBUG
topSequence.NSWL1Simulation.StripClusterTool.OutputLevel=INFO
topSequence.NSWL1Simulation.StripSegmentTool.OutputLevel=DEBUG

# Simulation parameters
#topSequence.NSWL1Simulation.PadTdsTool.VMM_DeadTime=3
#topSequence.NSWL1Simulation.PadTdsTool.ApplyVMM_DeatTime=True

#-----------------------------------------------------------------------------
# save ROOT histograms and Tuple
#-----------------------------------------------------------------------------

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


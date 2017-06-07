# This is the configuration file to run ATLAS Digitization
# Use the following switches for:
#   athenaCommonFlags.EvtMax = <NEvents>         Number of events to digitize (set to -1 for all)
#   athenaCommonFlags.SkipEvents = <NSkip>       Number of events from input file collection to skip
#   athenaCommonFlags.PoolHitsInput=<FileName>   Input collections. The list of files with the hits to digitize
#   athenaCommonFlags.PoolRDOOutput=<FileName>   Output file name

#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.AllowIgnoreConfigError=False #This job will stop if an include fails.
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag.set_Value_and_Lock("OFLCOND-RUN12-SDR-25")
globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-R2-2015-03-01-00")

from RecExConfig.RecFlags import rec as recFlags 
recFlags.doNameAuditor = True

globalflags.InputFormat.set_Value_and_Lock('pool')

athenaCommonFlags.EvtMax = 500
athenaCommonFlags.SkipEvents = 0

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = [ "test.rdos.pool.root" ]

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOff()

DetFlags.Muon_setOn()
DetFlags.MDT_setOn()
DetFlags.CSC_setOn()
DetFlags.TGC_setOn()
DetFlags.RPC_setOn()
DetFlags.sTGC_setOn()
DetFlags.Micromegas_setOn()

DetFlags.digitize.MDT_setOn() 
DetFlags.digitize.TGC_setOn() 
DetFlags.digitize.RPC_setOn() 
DetFlags.digitize.CSC_setOn() 
DetFlags.digitize.Micromegas_setOn() 
DetFlags.digitize.sTGC_setOn() 

DetFlags.Truth_setOff()
 
include('MuonGeoModelTest/NSWGeoSetup.py')
GeoModelSvc.MuonVersionOverride="MuonSpectrometer-R.07.00-NSW"
#-----------------------------------------------------------------------------
# Algorithms:  NSW L1 simulation
#-----------------------------------------------------------------------------

include ('TrigT1NSW/TrigT1NSW_jobOptions.py')

topSequence.NSWL1Simulation.OutputLevel=DEBUG
topSequence.NSWL1Simulation.DoNtuple=True
topSequence.NSWL1Simulation.PadTdsTool.DoNtuple=True
topSequence.NSWL1Simulation.PadTriggerTool.DoNtuple=True
topSequence.NSWL1Simulation.StripTdsTool.DoNtuple=True
#topSequence.NSWL1Simulation.PadTdsTool.OutputLevel=VERBOSE
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

ServiceMgr.THistSvc.Output += [ "NSWL1Simulation DATAFILE='NSWL1Simulation.root' OPT='RECREATE'" ]

print ServiceMgr

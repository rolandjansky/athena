#Example jO with autoconfiguration. Runs truth on MC and offline on data
#Taken from version in https://twiki.cern.ch/twiki/bin/viewauth/Atlas/JiveXMLWithAODorESD#Autoconfiguration_e_g_with_mc09
## RTT jOs run in RecExCommon environment
## (RecExCommon_links.sh), but this doesn't work
## with Cosmics, need to remove static db link
## with shell command:
import os
#### remove db link:
os.system('rm -rf sqlite200')
#remove Pool catalogue
os.system('rm -f PoolFileCatalog.xml*')
##
from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

#athenaCommonFlags.FilesInput=['/tmp/tneep/AOD.785324._000001.pool.root.1']

athenaCommonFlags.FilesInput=['/afs/cern.ch/work/b/bsmart/monitoring/mc12_8TeV.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1484_s1470_r3553_tid778631_00/ESD.778631._000093.pool.root.1',
                              '/afs/cern.ch/work/b/bsmart/monitoring/mc12_8TeV.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1484_s1470_r3553_tid778631_00/ESD.778631._000089.pool.root.1',
                              '/afs/cern.ch/work/b/bsmart/monitoring/mc12_8TeV.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1484_s1470_r3553_tid778631_00/ESD.778631._000004.pool.root.1',
                              '/afs/cern.ch/work/b/bsmart/monitoring/mc12_8TeV.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1484_s1470_r3553_tid778631_00/ESD.778631._000011.pool.root.1',
                              '/afs/cern.ch/work/b/bsmart/monitoring/mc12_8TeV.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1484_s1470_r3553_tid778631_00/ESD.778631._000026.pool.root.1',
                              '/afs/cern.ch/work/b/bsmart/monitoring/mc12_8TeV.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1484_s1470_r3553_tid778631_00/ESD.778631._000101.pool.root.1']

### all events:
athenaCommonFlags.EvtMax.set_Value_and_Lock(-1)
### for test, some events:
#athenaCommonFlags.EvtMax.set_Value_and_Lock(5)

#rec.doJiveXML.set_Value_and_Lock(True)
rec.doCBNT.set_Value_and_Lock(False)
rec.doMonitoring.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doHist.set_Value_and_Lock(False)
rec.doFloatingPointException.set_Value_and_Lock(True)
#If you don't want truth in MC
#rec.doTruth.set_Value_and_Lock(False)
### comment-in the following line for
### pathena-setup to get single-event-ESD:
#rec.doWriteESD.set_Value_and_Lock(True)

include("RecExCommon/RecExCommon_topOptions.py")
##
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


##### Trigger Decision Part #####################

## set up trigger decision tool
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
tdt.OutputLevel = ERROR
ToolSvc += tdt

##### Histogram File Part #################

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "HLTMonManager")
HLTMonManager = topSequence.HLTMonManager

##### TrigIDtrkMonitoring Part #####################

from TrigIDtrkMonitoring.TrigIDpvtxMonitoringConfig import TrigIDpvtxMonitoringTool
HLTMonManager.AthenaMonTools += TrigIDpvtxMonitoringTool()
#from TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig import TrigIDtrkMonitoringTool
#HLTMonManager.AthenaMonTools += TrigIDtrkMonitoringTool()

##### Histogram File Part #################

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='TrigIDpvtxMonitoring_ESDAOD.root' OPT='RECREATE'"]
HLTMonManager.FileKey = "AANT"

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = ERROR



#
## Number of Events to process
theApp.EvtMax = -1
MessageSvc.infoLimit  = 1000000
MessageSvc.debugLimit = 1000000


#svcMgr.AuditorSvc.Auditors += [ "ChronoAuditor"]

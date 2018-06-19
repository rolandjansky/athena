
## -- Overview of all default local settings that one can change 
## -- The default values are also shown.

## ------------------------------------------- flags set in: RecExOnline_jobOptions.py  
isOnline          = True
isOnlineStateless = False

## ------------------------------------------- flags set in: RecExOnline_monitoring.py  
isGlobalMonitoring = False

#eMon can only be used ONLINE (please see RecExOnline_Partition_Online.py)
useEmon           = False
useAtlantisEmon   = False
evtMax            = 10 #100

keycount          = 250
buffersize        = 10
updateperiod      = 200
timeout           = 600000
keyname           = 'SFI'
#streamName        ='CosmicCalo_physics'
streamName        ='express_express'


#COND tag and GEO are needed for running over a test partition or against ATLAS cosmics, calib
ConditionsTag     = 'COMCOND-HLTP-004-03'  # was -01
DetDescrVersion   = 'ATLAS-R1-2012-03-01-00'
if (not 'beamType' in dir()):
    beamType = 'collisions'

## ------------------------------------------- flags set in: RecExOnline_globalconfig.py  (from RecExOnline_jobOptions.py)
#read the pickle file if you want to use the AMI tag info
#stored in ami_recotrf.pickle (produced by 'tct_getAmiTag.py f140 ami_recotrf.cmdargs ami_recotrf.pickle') 
usePickleConfig   = False
pickleconfigfile  = './ami_recotrf.pickle'
DataSource        = 'data'
InputFormat       = 'bytestream'
#fileName          = '/afs/cern.ch/user/k/koutsman/OnlineNightly/data/data12_8TeV.00208931.express_express.daq.RAW._lb0123._SFO-9._0001.data'
fileName          = 'root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/tct/rec_input/00204416/express_express/data12_8TeV.00204416.express_express.merge.RAW._lb0015._SFO-ALL._0001.1'
from RecExConfig.RecFlags import rec
rec.RunNumber.set_Value (204416)

# update for comsics
if beamType == 'cosmics':
    fileName          = '/afs/cern.ch/atlas/offline/test/data11_cos.00182609.physics_CosmicCalo.merge.RAW._lb0100._SFO-ALL._0001.1.SFO-ALL._0001.1.10evts.data'
    streamName        ='CosmicCalo_physics'
    rec.RunNumber.set_Value (182609)
    
doESD             = True
writeESD          = False
doAOD             = False
writeAOD          = False
IOVDbSvcMessage   = False

## ------------------------------------------ flags set in: RecExOnline_recoflags.py (from RecExOnline_jobOptions.py)
doAllReco   = True
doInDet     = doAllReco
doMuon      = doAllReco
doLArg      = doAllReco
doTile      = doAllReco
doTrigger   = False
doHist      = doAllReco
doJiveXML   = False
doEgammaTau = doAllReco

#set to True in the JO
#doCommissionig = False

## ------------------------------------------ flags set in : RecExOnline_monitoring.py (from from RecExOnline_jobOptions.py)
doAllMon  = True
doCaloMon = doAllMon
doPhysMon = doAllMon
doTrigMon = False
doIDMon   = doAllMon
doTRTMon  = doAllMon
doMuonMon = doAllMon
#doPixelOnlyMon = False

## ------------------------------------------ flags set in : RecExOnline_postconfig.py    (called from RecExOnline_jobOptions.py)


#### CHANGES TO GET 19.1.0.1 RECO WORKING (M4)
from AthenaMonitoring.DQMonFlags import DQMonFlags
DQMonFlags.doMuonCombinedMon.set_Value_and_Lock(False)
DQMonFlags.doMuonPhysicsMon.set_Value_and_Lock(False)

# Doesn't work with run 1 data (conditions issue).
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTIDE_Ambi.set_Value_and_Lock(False)

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
preExec = ['cfg =  TriggerConfigGetter(); cfg.setupxAODWriting()']

## main online reco scripts
include ("RecExOnline/RecExOnline_jobOptions.py")

from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODMenuWriter
w=TrigConf__xAODMenuWriter()
w.MetaDataStore='InputMetaDataStore'
topSequence.remove(w)
from AthenaCommon import AlgSequence
athBeginSeq = AlgSequence.AthSequencer('AthBeginSeq')
athBeginSeq += w

ToolSvc.GSFBuildPixelToTPIDTool.ReadFromCOOL=False
ToolSvc.CombinedMuonPixelToTPID.ReadFromCOOL=False

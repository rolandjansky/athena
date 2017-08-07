
## -- Overview of all default local settings that one can change 
## -- The default values are also shown.

## ------------------------------------------- flags set in: RecExOnline_jobOptions.py  
isOnline          = True

### isOnlineStateless = False
isOnlineStateless = True

# for testing on lxplus
isOfflineTest     = True

## ------------------------------------------- flags set in: RecExOnline_monitoring.py  
isGlobalMonitoring = False

#eMon can only be used ONLINE (please see RecExOnline_Partition_Online.py)
useEmon           = False
useAtlantisEmon   = False
evtMax            = 100

keycount          = 250
buffersize        = 10
updateperiod      = 200
timeout           = 600000
keyname           = 'SFI'
#streamName        ='CosmicCalo_physics'
streamName        ='express_express'


#COND tag and GEO are needed for running over a test partition or against ATLAS cosmics, calib
#ConditionsTag     = 'CONDBR2-ES1PA-2015-03'
#ConditionsTag     = 'CONDBR2-HLTP-2015-03'    # online database
#ConditionsTag     = 'CONDBR2-HLTP-2015-07'   # created 2015-08-10
ConditionsTag     = 'CONDBR2-HLTP-2015-08'   # created 2015-11-04, used in M10
#ConditionsTag     = 'CONDBR2-HLTP-2016-01'   # 2016-02-05 Based on CONDBR2-HLTP-2015-08, for M11. 
#ConditionsTag     = 'CONDBR2-BLKPA-2016-06'

#DetDescrVersion   = 'ATLAS-R2-2015-02-00-00'
DetDescrVersion   = 'ATLAS-R2-2015-03-01-00'

##
## Beam Type and Project Name
##
#beamType          = 'cosmics'
#projectName       = 'data15_cos'
##
beamType          = 'collisions'
projectName       = 'data15_comm'



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
#fileName          = 'root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/tct/rec_input/00204416/express_express/data12_8TeV.00204416.express_express.merge.RAW._lb0015._SFO-ALL._0001.1'

## cosmic
## fileName          = '/afs/cern.ch/work/c/cylin/public/data15_cos.00258389.express_express.merge.RAW._lb0894._SFO-ALL._0001.1'
###
## collision
### fileName          = '/afs/cern.ch/work/c/cylin/public/data15_comm.00264034.express_express.merge.RAW._lb0784._SFO-ALL._0001.1'
#fileName          = '/afs/cern.ch/work/s/smwang/public/DQM/data/data15_13TeV.00284484.express_express.merge.RAW._lb0098._SFO-ALL._0001.1'
### fileName          = 'root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/data15_13TeV/express_express/00284484/data15_13TeV.00284484.express_express.merge.RAW/data15_13TeV.00284484.express_express.merge.RAW._lb0487._SFO-ALL._0001.1'
### projectName       = 'data15_comm'
#projectName       = 'data15_13TeV'

fileName          = '/afs/cern.ch/work/y/yunju/public/GM_raw2016/data16_13TeV.00302347.express_express.merge.RAW._lb0432._SFO-ALL._0001.1'
projectName       = 'data16_13TeV'


# update for comsics
if beamType == 'cosmics':
    ### fileName          = '/afs/cern.ch/atlas/offline/test/data11_cos.00182609.physics_CosmicCalo.merge.RAW._lb0100._SFO-ALL._0001.1.SFO-ALL._0001.1.10evts.data '
    ### fileName          = '/afs/cern.ch/work/c/cylin/public/data15_cos.00258389.express_express.merge.RAW._lb0894._SFO-ALL._0001.1'
    fileName          = '/afs/cern.ch/work/s/smwang/public/DQM/data/data15_cos.00287824.express_express.merge.RAW._lb0015._SFO-ALL._0001.1'
    ### fileName          = 'root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/data15_cos/express_express/00287824/data15_cos.00287824.express_express.merge.RAW/data15_cos.00287824.express_express.merge.RAW._lb0015._SFO-ALL._0001.1'
    ### streamName        ='CosmicCalo_physics'
    streamName        ='express_express'
    projectName       = 'data15_cos'
    

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
doTrigger   = doAllReco
doHist      = doAllReco
doJiveXML   = False
doEgammaTau = doAllReco

#set to True in the JO
#doCommissionig = False

## ------------------------------------------ flags set in : RecExOnline_monitoring.py (from from RecExOnline_jobOptions.py)
doAllMon  = True
doCaloMon = doAllMon
doPhysMon = doAllMon
doTrigMon = doAllMon
doIDMon   = doAllMon
doTRTMon  = doAllMon
doMuonMon = doAllMon
#doPixelOnlyMon = False

## ------------------------------------------ flags set in : RecExOnline_postconfig.py    (called from RecExOnline_jobOptions.py)

#### CHANGES TO GET 19.1.0.1 RECO WORKING (M4)
from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.beamType.set_Value_and_Lock(beamType)

#from AthenaCommon.GlobalFlags import globalflags
#globalflags.DatabaseInstance.set_Value_and_Lock("CONDBR2")
#globalflags.ConditionsTag.set_Value_and_Lock(ConditionsTag)
#globalflags.DetDescrVersion.set_Value_and_Lock(DetDescrVersion)


from AthenaCommon.BFieldFlags import jobproperties
jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
###jobproperties.BField.solenoidOn.set_Value_and_Lock(False)


from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doLArHVCorr.set_Value_and_Lock(False)
jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr.set_Value_and_Lock(False)
jobproperties.CaloCellFlags.doLArDeadOTXCorr.set_Value_and_Lock(False)  ####? for trigger, added 11th March, 2015, by CY


from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock(True)
InDetFlags.useBroadClusterErrors.set_Value_and_Lock(False)


from AthenaMonitoring.DQMonFlags import DQMonFlags
DQMonFlags.doStreamAwareMon.set_Value_and_Lock(False)
DQMonFlags.enableLumiAccess.set_Value_and_Lock(False)
DQMonFlags.doCTPMon.set_Value_and_Lock(False)
DQMonFlags.doLVL1CaloMon.set_Value_and_Lock(False)
### DQMonFlags.doMuonTrkPhysMon.set_Value_and_Lock(False) #xx
DQMonFlags.doMuonTrkPhysMon.set_Value_and_Lock(True) #xx
### DQMonFlags.doJetTagMon.set_Value_and_Lock(False)


from RecExConfig.RecFlags import rec
rec.doBTagging.set_Value_and_Lock(True)

from JetRec.JetRecFlags import jetFlags
jetFlags.useBTagging.set_Value_and_Lock(True)


###from JetRec.JetRecFlags import jetFlags
###jetFlags.useTracks.set_Value_and_Lock(False)


from MuonDQAMonFlags.MuonDQAProperFlags import MuonDQADetFlags
### MuonDQADetFlags.doMDTTGCL1Mon.set_Value_and_Lock(True)
MuonDQADetFlags.doTGCL1Mon.set_Value_and_Lock(True)
MuonDQADetFlags.doTGCMon.set_Value_and_Lock(True)


### from AthenaMonitoring.DQMonFlags import DQMonFlags
### DQMonFlags.doMuonCombinedMon.set_Value_and_Lock(False)
### DQMonFlags.doMuonPhysicsMon.set_Value_and_Lock(False)
### #DQMonFlags.doIDMon.set_Value_and_Lock(False)

##---------------------------------------ID suggestion from Alex 18.Feb.2015  --------
jobproperties.InDetJobProperties.doTrackSegmentsSCT.set_Value_and_Lock(False)
jobproperties.InDetJobProperties.doTrackSegmentsPixel.set_Value_and_Lock(False)
jobproperties.InDetJobProperties.doTrackSegmentsTRT.set_Value_and_Lock(False)


##---------------------------------------Bunch Spacing 25ns  --------
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.InDet25nsec.set_Value_and_Lock(True)

jobproperties.Beam.bunchSpacing.set_Value_and_Lock(25)

# --------------------------------------------- Monitoring
### replaced by MuonTrackMonitoring (SMW Jan 23 2016)
### from AthenaMonitoring.DQMonFlags import DQMonFlags
### DQMonFlags.doMuonPhysicsMon.set_Value_and_Lock(True)


## main online reco scripts
include ("RecExOnline/RecExOnline_jobOptions.py")



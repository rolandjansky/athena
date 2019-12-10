## -- Overview of all default local settings that one can change 
## -- The default values are also shown.

isHI_2016 = False

## ------------------------------------------- flags set in: RecExOnline_jobOptions.py  

isOnline          = True
isOnlineStateless = True

# for testing on lxplus
isOfflineTest     = False

#for the time being, running over file is not possible ONLINE (please see RecExOnline_File_Offline.py) 
useEmon           = True
## KeyCount value in the EMON service defined as 1-byte, i.e. the maximum allowed value is 255. 
## keycount          = 150
keycount          = 300
## keycount          = 500
buffersize        = 10
updateperiod      = 200
# timeout is in ms
timeout           = 600000

#keyname           = 'efd'   #AK: 21-05-2014
keyname           = 'dcm'

import os

partitionName     = os.environ.get("TDAQ_PARTITION", "ATLAS")
publishNumber     = os.environ.get("GLOBAL_JOB_NUMBER", "1")
publishName       = os.environ.get("TDAQ_APPLICATION_NAME", "GlobalMonitoring")

print publishName
isserverName = 'Histogramming-Global-iss'  # Ak: 26-05-2014 - needed to write out the gathere hsitograms to the correct server

#streamName       = 'MinBias'
streamName        = 'express'
streamLogic       = 'Or'

# When using ATLAS partition
# Use different streams for "atlas standby" and "atlas ready"
#
if (partitionName == 'ATLAS' or partitionName == 'ATLAS_MP1'):
    streamLogic       = 'Or'
    
    import RecExOnline.OnlineISConfiguration
    import ispy
    from ispy import *
    from ipc import IPCPartition
    from ispy import ISObject

    obj = ispy.ISObject(ispy.IPCPartition(partitionName), 'RunParams.RunParams', 'RunParams')
    obj.checkout()
    ### if ( obj.T0_project_tag == 'data15_comm' or obj.T0_project_tag == 'data15_13TeV'):
    if ( obj.T0_project_tag == 'data18_900GeV' or obj.T0_project_tag == 'data18_comm' or obj.T0_project_tag == 'data18_13TeV' or obj.T0_project_tag == 'data18_5TeV' or obj.T0_project_tag == 'data18_1beam' or obj.T0_project_tag == 'data17_comm' or obj.T0_project_tag == 'data17_13TeV' or obj.T0_project_tag == 'data17_5TeV' or obj.T0_project_tag == 'data17_1beam' or obj.T0_project_tag == 'data16_comm' or obj.T0_project_tag == 'data16_13TeV' or obj.T0_project_tag == 'data16_5TeV' or obj.T0_project_tag == 'data16_1beam' or obj.T0_project_tag == 'data16_hip' or obj.T0_project_tag == 'data16_hip5TeV' or obj.T0_project_tag == 'data16_hip8TeV'):
        #streamName        = 'MinBias'
        #streamName        = 'Main' # Switching due to missingg Minbias stream -= 13/06/2015 AK
        try:
            if RecExOnline.OnlineISConfiguration.GetAtlasReady():
                print "ATLAS READY, reading express stream"
                streamName = 'express'
                ### streamName = 'express:MinBias:Standby'
                ### streamName = 'express:MinBias'
                ### streamName = 'MinBias:L1Calo'
            else:
                print "ATLAS NOT READY, reading standby stream"
                ### streamName = 'standby'
                ### streamName = 'physics_Standby'
                ### streamName = 'Standby'
                ### streamName = 'MinBias:Standby'
                streamName = 'express:IDCosmic:HLT_IDCosmic:CosmicMuons:MinBias:Standby'
                ### streamName = 'MinBias:L1Calo'
        except:
            streamName = 'express'
    else:
        streamName        = 'express:IDCosmic:HLT_IDCosmic:CosmicMuons:MinBias:Standby'
else:
    streamLogic       = 'Ignore'
    



#
# When NOT using ATLAS partition
#
if (partitionName != 'ATLAS' and partitionName != 'ATLAS_MP1'):
    if isHI_2016:
       publishName     = 'GMTest_hiconf'
    else:
       publishName     = 'GMTest_ppconf_21_1_45_17Oct18_1'
    isserverName    = 'Histogramming'


print "GlobalMonitoring : streamName streamLogic= ",streamName,streamLogic
print "publishName      : ",publishName


#import time
#myfloat = float(publishNumber)
#tosleep = myfloat*5
#time.sleep(tosleep)

import commands
pids=commands.getoutput("/sbin/pidof -o %u python" % os.getpid()).split(" ")
print "pids",pids

for pid in pids:
  print "pid",pid
  #if (False):
  #if (1):
  if (0):
    print "pid = ",pid
    #thisnumber=commands.getoutput("grep GLOBAL_JOB_NUMBER /proc/%u/fd/1 -m 1 | cut -f2 -d'='" % int(pid))
    thisname=commands.getoutput("grep TDAQ_APPLICATION_NAME /proc/%u/fd/1 -m 1 | cut -f2 -d'='" % int(pid))
    print "thisname = ",thisname
    print "publishName = ",publishName
    if (thisname == publishName):
      print "found a match! Will Kill pid = ",pid
      killreturn = commands.getoutput("kill %u" % int(pid))


useAtlantisEmon   = False

### evtMax            = -1
### evtMax            = 50

if (partitionName == 'ATLAS' or partitionName == 'ATLAS_MP1'):
    evtMax            = -1

if (partitionName != 'ATLAS' and partitionName != 'ATLAS_MP1'):
    evtMax            = 200
    # evtMax            = -1 # lshi: for Gatherer test


## ------------------------------------------- flags set in: RecExOnline_emonsvc.py (from RecExOnline_jobOptions.py)                    

isGlobalMonitoring = True

## ------------------------------------------- flags set in: RecExOnline_globalconfig.py  (from RecExOnline_jobOptions.py)
#read the pickle file if you want to use the AMI tag info
#stored in ami_recotrf.pickle (produced by 'tct_getAmiTag.py f140 ami_recotrf.cmdargs ami_recotrf.pickle') 
usePickleConfig   = False
pickleconfigfile  = './ami_recotrf.pickle'
DataSource        = 'data'
InputFormat       = 'bytestream'
fileName          = './0.data'
#beamType          = 'cosmics'  
beamType          = 'collisions'  #AK: 21-05-2014

#COND tag and GEO are needed for running over a test partition online
#ConditionsTag     = 'CONDBR2-HLTP-2015-08' # 20160317
#ConditionsTag     = 'CONDBR2-HLTP-2016-01'
#DetDescrVersion   = 'ATLAS-R2-2015-03-01-00'  ### new for offline (19.3.0.1)
#ConditionsTag     = 'CONDBR2-HLTP-2017-03'
ConditionsTag     = 'CONDBR2-HLTP-2018-01'  # Aug 21st 2018
DetDescrVersion   = 'ATLAS-R2-2016-01-00-01'  ### new for offline (19.3.0.1)

### test run1
if isHI_2016:
   ConditionsTag     = 'CONDBR2-HLTP-2016-01'
   DetDescrVersion   = 'ATLAS-R2-2015-03-01-00'

### ConditionsTag     = 'CONDBR2-HLTP-2016-01' # 2016-02-05 Based on CONDBR2-HLTP-2015-08, for M11.
### ConditionsTag     = 'CONDBR2-HLTP-2015-08' # 20160317
### ConditionsTag     = 'CONDBR2-HLTP-2015-07'   # created 2015-08-10
### ConditionsTag     = 'CONDBR2-HLTP-2015-05'
### ConditionsTag     = 'CONDBR2-HLTP-2015-04'
### ConditionsTag     = 'CONDBR2-HLTP-2015-01'
### ConditionsTag     = 'CONDBR2-ES1PA-2014-01'  ### new for offline (19.3.0.1)

### DetDescrVersion   = 'ATLAS-R2-2015-01-01-00'
### DetDescrVersion   = 'ATLAS-R2-2015-02-00-00'  ### new for offline (19.3.0.1)
### DetDescrVersion   = 'ATLAS-R2-2015-03-01-00'  ### new for offline (19.3.0.1)


doESD             = True
writeESD          = False
doAOD             = False
writeAOD          = False
IOVDbSvcMessage   = False

abortonuncheckedstatuscode = False

## ------------------------------------------ flags set in: RecExOnline_recoflags.py (from RecExOnline_jobOptions.py)
doAllReco   = True

doInDet     = doAllReco
### doInDet     = False

doMuon      = doAllReco
###doMuon      = False

doLArg      = doAllReco
###doLArg      = False

doTile      = doAllReco

doTrigger   = doAllReco
###doTrigger   = False

doHist      = doAllReco
doJiveXML   = False

doEgammaTau = doAllReco
### doEgammaTau = False

doHIPRec     = False

doHIRec     = False

#set to True in the JO
#doCommissioning = False

## ------------------------------------------ flags set in : RecExOnline_monitoring.py (from from RecExOnline_jobOptions.py)
doAllMon  = True

doCaloMon = doAllMon
###doCaloMon = False

doTileMon = doAllMon #AK: new since 09 july 2014 (M4)
# doTileMon = False

doPhysMon = doAllMon
###doPhysMon = False

doTrigMon = doAllMon

doIDMon   = doAllMon
###doIDMon   = False

doTRTMon  = doAllMon
###doTRTMon  = False

doMuonMon = doAllMon
### doMuonMon = False

doCTPMon  = False #doAllMon #AK: 25-01-2014
doHIMon   = False # doAllMon

if isHI_2016:
   #doTrigger   = False
   doHIPRec    = doAllReco
   doZdc       = doAllReco
   #doTrigMon = False
   doHIMon   = doAllMon
### doHIMon   = False # doAllMon

## ------------------------------------------ flags set in : RecExOnline_postconfig.py    (called from RecExOnline_jobOptions.py)

from AthenaCommon.GlobalFlags import globalflags
globalflags.DatabaseInstance.set_Value_and_Lock("CONDBR2")

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
#print "#### yunju Here is DQMonFlags.monManEnvironment:"+DQMonFlags.monManEnvironment
## for egmma monitoring in lxplus
#if isHI_2016:
#   DQMonFlags.monManEnvironment.set_Value_and_Lock('tier0')

### AFP Monitoring
DQMonFlags.doAFPMon.set_Value_and_Lock(True)

from RecExConfig.RecFlags import rec
rec.doBTagging.set_Value_and_Lock(True)
### AFP reconstruction
rec.doAFP.set_Value_and_Lock(True)
rec.doForwardDet.set_Value_and_Lock(True)
rec.doAlfa.set_Value_and_Lock(False)

from JetRec.JetRecFlags import jetFlags
jetFlags.useBTagging.set_Value_and_Lock(True)

### from JetRec.JetRecFlags import jetFlags
### jetFlags.useTracks.set_Value_and_Lock(False)

from MuonDQAMonFlags.MuonDQAProperFlags import MuonDQADetFlags
### MuonDQADetFlags.doMDTTGCL1Mon.set_Value_and_Lock(True)
MuonDQADetFlags.doTGCL1Mon.set_Value_and_Lock(True)
MuonDQADetFlags.doTGCMon.set_Value_and_Lock(True)


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


### ------------------- inherited from 20.1.0.2 -----------
#from RecExConfig.RecFlags import rec
#rec.doJetMissingETTag.set_Value_and_Lock(False)


#from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags
#HLTMonFlags.doBphys=False


#jobproperties.CaloCellFlags.doLArDeadOTXCorr.set_Value_and_Lock(False)

#from LArConditionsCommon.LArCondFlags import larCondFlags
#larCondFlags.OFCShapeFolder.set_Value_and_Lock("")

## main online reco scripts
include ("RecExOnline/RecExOnline_jobOptions.py")
if isHI_2016:
   include ("HIRecExample/hip_outputPostExec.py")

## -- Overview of all default local settings that one can change 
## -- The default values are also shown.

## ------------------------------------------- flags set in: RecExOnline_jobOptions.py  
isOnline          = True
isOnlineStateless = True
isOfflineTest     =False

#for the time being, running over file is not possible ONLINE (please see RecExOnline_File_Offline.py)
useEmon           = True
keycount          = 90
buffersize        = 10
updateperiod      = 100
timeout           = 240000
keyname           = 'dcm'
useAtlantisEmon   = False
evtMax            = -1

streamName        = 'IDCosmic'
streamName        = 'MinBias'   # first collisions (05/05/2015 - cgrefe)
streamType	  = 'physics'
streamLogic       = 'Or'

## ------------------------------------------- flags set in: RecExOnline_emonsvc.py (from RecExOnline_jobOptions.py)                    
import os
partitionName = os.environ.get("TDAQ_PARTITION", "TestPartitionGlobalMonitoring")
publishName   = os.environ.get("TDAQ_APPLICATION_NAME", "ID-Test-Application")
isserverName  = 'Histogramming-CombinedInDet-1-iss'

if(partitionName == "ATLAS"):
    streamLogic       = 'Or'
    
    import RecExOnline.OnlineISConfiguration
    import ispy
    from ispy import *
    from ipc import IPCPartition
    from ispy import ISObject
    
    obj = ispy.ISObject(ispy.IPCPartition(partitionName), 'RunParams.RunParams', 'RunParams')
    obj.checkout()

    if ( obj.T0_project_tag == 'data16_comm' or obj.T0_project_tag == 'data16_13TeV' or obj.T0_project_tag == 'data16_5TeV' or obj.T0_project_tag == 'data16_hi'):
        try:
            if obj.T0_project_tag == 'data15_hi':
                 from InDetRecExample.InDetJobProperties import InDetFlags
                 InDetFlags.doHeavyIon.set_Value_and_Lock(True)
                 InDetFlags.cutLevel = 2
            if RecExOnline.OnlineISConfiguration.GetAtlasReady():
                printfunc ("ATLAS READY, reading express stream")
                streamName = 'express:IDMonDummyStream'
                streamType = 'express'
                if obj.T0_project_tag == 'data15_hi':
                    streamName = 'MinBias:IDMonDummyStream'  # intermediate change for HI running
                    streamType = 'physics'
                #streamName = 'IDTrack:IDMonDummyStream'
                #streamType = 'calib'
            else:
                printfunc ("ATLAS NOT READY, reading standby stream")
                streamName = 'Standby:IDMonDummyStream'
        except:
            printfunc ("EXCEPTION")
            streamName = 'express:IDMonDummyStream'
    elif obj.T0_project_tag == 'data16_1beam':
        streamName = 'express'
    else:
        streamName        = 'express:IDCosmic:HLT_IDCosmic:MinBias:IDMonDummyStream'

else:
    streamLogic       = 'Ignore'

if ( publishName == "ID-AthenaMon-Noise" ):
    publishName       = "ID_PT_Noise"
    streamType        = 'monitoring'
    streamName        = 'IDMonitoring'
    streamLogic       = 'Or'# This should be taken out when we have a proper IDMonitoring stream
    groupName        = ''
    lvl1Logic         = 'Or' if partitionName == "ATLAS" else 'Ignore' # cgrefe 08.09.15: see if the Or works in beam 
    lvl1Name          = 'L1_RD0_EMPTY'
    doIdGlobalErrorMon = True
elif ( publishName == "ID-AthenaMon-PixelDetails" ):
    doPixelOnlyMon    = True
    publishName       = 'CombinedInDet_PixelDetails'
    from InDetRecExample.InDetJobProperties import InDetFlags
    InDetFlags.doTRTStandalone.set_Value_and_Lock(False)
    InDetFlags.doTrackSegmentsSCT.set_Value_and_Lock(False)
    InDetFlags.doTrackSegmentsTRT.set_Value_and_Lock(False)
    InDetFlags.doBackTracking.set_Value_and_Lock(False)
elif ( publishName == "ID-AthenaMon-PixelDetails-Test" ):
    doPixelOnlyMon    = True
    publishName       = 'CombinedInDet_PixelDetails'
    from InDetRecExample.InDetJobProperties import InDetFlags
    InDetFlags.doTRTStandalone.set_Value_and_Lock(False)
    InDetFlags.doTrackSegmentsSCT.set_Value_and_Lock(False)
    InDetFlags.doTrackSegmentsTRT.set_Value_and_Lock(False)
    InDetFlags.doBackTracking.set_Value_and_Lock(False)
    streamLogic       = 'Ignore'
elif ( publishName == "ID-AthenaMon-NCB" ):
    #Ensure we reconstruct only 01X hits in the SCT!
    from InDetRecExample.InDetJobProperties import InDetFlags
    InDetFlags.InDet25nsec.set_Value_and_Lock(True)
    keyname           = 'dcm'
    keycount          = 255      # increasing dcm to increase statistics
    streamLogic       = 'Or'
    streamType        = 'physics'
    streamName        = 'Background'
    lvl1Logic         = 'Or'
    lvl1Name          = 'L1_BCM_AC_UNPAIRED_ISO:L1_BCM_AC_UNPAIRED_NONISO:L1_BCM_CA_UNPAIRED_ISO:L1_BCM_CA_UNPAIRED_NONISO:L1_J12_UNPAIRED_ISO:L1_J12_UNPAIRED_NONISO'
    doIdNCBMon        = True
elif ( publishName == "ID-Test-Application" ):
    ##keyname='CompleteEvent'
    #streamLogic       = 'Or'
    isserverName  = 'Histogramming'
    streamType = 'express'
    streamName = 'express'

# --- Trigger selection for TRT beam monitoring
elif ( publishName == 'ID-AthenaMon-TRTBeam' ):
    keyname      = 'dcm'
    keycount     = 255           # there are only around 100 SFIs - connect to all of them
    groupName   = ''         # process the same events as other monitoring jobs with the same event selection
    streamType   = 'monitoring'
    streamLogic  = 'Or'
    streamName   = 'IDMonitoring'# avoid PixelNoise and SCTNoise, because those streams contain mostly partial events without TRT data
    lvl1Logic    = 'Or'
    lvl1Name     = 'L1_RD0_FILLED'
    doTRTBeamMon = True          # use the special monitoring tool InDetMonitoringTRT_Beam.py in InDetMonitoring.py
    doESD        = False         # switch off any reconstruction to gain some speed, we only need raw data here

elif ( publishName == 'ID-AthenaMon-TRTBeam-Test' ):
    keyname      = 'dcm'
    keycount     = 255           # there are only around 100 SFIs - connect to all of them
    groupName   = ''         # process the same events as other monitoring jobs with the same event selection
    streamType   = 'monitoring'
    streamLogic  = 'Or'
    streamName   = 'IDMonitoring'# avoid PixelNoise and SCTNoise, because those streams contain mostly partial events without TRT data
    lvl1Logic    = 'Or'
    lvl1Name     = 'L1_RD0_FILLED'
    doTRTBeamMon = True          # use the special monitoring tool InDetMonitoringTRT_Beam.py in InDetMonitoring.py
    doESD        = False         # switch off any reconstruction to gain some speed, we only need raw data here


## ------------------------------------------- flags set in: RecExOnline_globalconfig.py  (from RecExOnline_jobOptions.py)
#read the pickle file if you want to use the AMI tag info
#stored in ami_recotrf.pickle (produced by 'tct_getAmiTag.py f140 ami_recotrf.cmdargs ami_recotrf.pickle') 
usePickleConfig   = False
pickleconfigfile  = './ami_recotrf.pickle'
DataSource        = 'data'
InputFormat       = 'bytestream'

#COND tag and GEO are needed for running over a test partition or against ATLAS cosmics, calib
#ConditionsTag     = 'CONDBR2-HLTP-2015-08'
#DetDescrVersion   = 'ATLAS-R2-2015-03-01-00'  ### new for offline (19.3.0.1)
#beamType          = 'cosmics'

doESD             = True
writeESD          = False
doAOD             = False
writeAOD          = False
IOVDbSvcMessage   = False

## ------------------------------------------ flags set in: RecExOnline_recoflags.py (from RecExOnline_jobOptions.py)
doAllReco   = False
doInDet     = True
doMuon      = False
doLArg      = False
doTile      = False
doTrigger   = False
doHist      = doAllReco
doJiveXML   = False
doEgammaTau = doAllReco

#set to True in the JO
#doCommissioning = False

## ------------------------------------------ flags set in : RecExOnline_monitoring.py (from from RecExOnline_jobOptions.py)
doAllMon  = False
doCaloMon = doAllMon
doPhysMon = doAllMon
doTrigMon = False
doIDMon   = True   # cgrefe: disabled for now until pixel monitoing is fixed (19.08.2015)
doSCTMon  = True
doTRTMon  = True
doMuonMon = doAllMon

# TRT beam monitoring uses no event reconstruction - switch of all other ID monitoring
if ('doTRTBeamMon' in dir() and doTRTBeamMon):
    doIDMon   = False
    doSCTMon  = False
    doTRTMon  = True  # this needs to be true in order to initialize the ID monitoring config. Will not run the TRT_Monitoring tool

## ------------------------------------------ flags set in : RecExOnline_postconfig.py    (called from RecExOnline_jobOptions.py)
from AthenaCommon.GlobalFlags import globalflags
globalflags.DatabaseInstance.set_Value_and_Lock("CONDBR2")

from InDetRecExample.InDetJobProperties import InDetFlags
#InDetFlags.doIBL.set_Value_and_Lock(True)
InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock(True)
InDetFlags.doPrintConfigurables.set_Value_and_Lock(True)

if ( publishName != "ID-AthenaMon-PixelDetails" ):
    InDetFlags.doTrackSegmentsPixel.set_Value_and_Lock(False)
    InDetFlags.doTrackSegmentsSCT.set_Value_and_Lock(False)
    InDetFlags.doTrackSegmentsTRT.set_Value_and_Lock(False)

from JetRec.JetRecFlags import jetFlags
jetFlags.Enabled = False

from InDetRecExample.InDetJobProperties import InDetFlags                                                                               
InDetFlags.InDet25nsec.set_Value_and_Lock(True)                                                                                         

#jobproperties.Beam.bunchSpacing.set_Value_and_Lock(25)                                                                                  

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.TRT_BuildStrawLayers.set_Value(True) # needed for proper efficiency determination during online monitoring

## main online reco scripts
include ("RecExOnline/RecExOnline_jobOptions.py")

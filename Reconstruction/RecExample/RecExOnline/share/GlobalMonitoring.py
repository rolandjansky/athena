## -- Overview of all default local settings that one can change 
## -- The default values are also shown.

## ------------------------------------------- flags set in: RecExOnline_jobOptions.py  
isOnline          = True
isOnlineStateless = True

#for the time being, running over file is not possible ONLINE (please see RecExOnline_File_Offline.py) 
useEmon           = True
## KeyCount value in the EMON service defined as 1-byte, i.e. the maximum allowed value is 255. 
keycount          = 250
buffersize        = 10
updateperiod      = 200
# timeout is in ms
timeout           = 600000
#keyname           = 'efd'   #AK: 21-05-2014
keyname           = 'dcm'

import os

#streamName        = os.environ.get("GLOBAL_STREAM_NAME")  #AK: 21-05-2014
#streamLogic       = os.environ.get("GLOBAL_STREAM_LOGIC")  #AK: 21-05-2014
partitionName     = os.environ.get("TDAQ_PARTITION", "ATLAS")
publishNumber     = os.environ.get("GLOBAL_JOB_NUMBER", "1")
publishName       = os.environ.get("TDAQ_APPLICATION_NAME", "GlobalMonitoring")

print publishName
isserverName = 'Histogramming-Global-iss'  # Ak: 26-05-2014 - needed to write out the gathere hsitograms to the correct server

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
evtMax            = -1

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
#beamType          = 'cosmics'  #AK: 21-05-2014
beamType          = 'collisions'  #AK: 21-05-2014

#COND tag and GEO are needed for running over a test partition online
ConditionsTag     = 'COMCOND-HLTP-004-01'
DetDescrVersion   = 'ATLAS-GEO-20-00-01'

doESD             = True
writeESD          = False
doAOD             = False
writeAOD          = False
IOVDbSvcMessage   = False

abortonuncheckedstatuscode = False

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
#doCommissioning = False

## ------------------------------------------ flags set in : RecExOnline_monitoring.py (from from RecExOnline_jobOptions.py)
doAllMon  = True
doCaloMon = doAllMon
doPhysMon = doAllMon
doTrigMon = False
doIDMon   = doAllMon
doTRTMon  = doAllMon
doMuonMon = doAllMon
doCTPMon  = False #doAllMon #AK: 25-01-2014

## ------------------------------------------ flags set in : RecExOnline_postconfig.py    (called from RecExOnline_jobOptions.py)

## main online reco scripts
include ("RecExOnline/RecExOnline_jobOptions.py")

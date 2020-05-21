## ------------------------------------------ flags for RecExOnline_jobOptions.py
fileName        = None

## ------------------------------------------ flags for RecExOnline_globalconfig.py
ConditionsTag   = 'COMCOND-HLTP-004-01'
DetDescrVersion = 'ATLAS-GEO-20-00-01'
beamType        = 'collisions'

doESD           = True
writeESD        = False
doAOD           = False
writeAOD        = False
IOVDbSvcMessage = False

## ------------------------------------------ flags for RecExOnline_recoflags.py
doAllReco       = False
doInDet         = True
doMuon          = doAllReco
doLArg          = doAllReco
doTile          = doAllReco
doTrigger       = False
doHist          = False
doJiveXML       = False
doEgammaTau     = False

## ------------------------------------------ flags for RecExOnline_monitoring.py
doAllMon        = False
doTRTMon        = True
doCaloMon       = doAllMon
doPhysMon       = doAllMon
doTrigMon       = doAllMon
doIDMon         = doAllMon
doMuonMon       = doAllMon

## ------------------------------------------ flags for RecExOnline_postconfig.py
isOnline        = True
useEmon         = True
evtMax          = -1

## ------------------------------------------ TRT-specific flags
import os
partitionName   = os.environ.get('TDAQ_PARTITION', 'ATLAS')
jobName         = os.environ.get('TDAQ_APPLICATION_NAME', 'TRT-Monitoring-Extra')

publishName     = jobName
isserverName    = 'Histogramming-TRT'
keyname         = 'efd'
keycount        = 12
updateperiod    = 30
timeout         = 3600000

# --- Event selection for proton-proton collisions - this is the default
if (jobName == 'TRT-Monitoring-01'):
    streamLogic = 'Or'
    streamName  = 'Egamma'
if (jobName == 'TRT-Monitoring-02'):
    streamLogic = 'Or'
    streamName  = 'Egamma'
if (jobName == 'TRT-Monitoring-03'):
    streamLogic = 'Or'
    streamName  = 'Muons'
if (jobName == 'TRT-Monitoring-04'):
    streamLogic = 'Or'
    streamName  = 'Muons'
if (jobName == 'TRT-Monitoring-05'):
    streamLogic = 'Or'
    streamName  = 'JetTauEtmiss'
if (jobName == 'TRT-Monitoring-06'):
    streamLogic = 'Or'
    streamName  = 'JetTauEtmiss'

# --- Event selection for cosmics - override the per-job settings manually
if False:                       # it would be nice if this could be set automatically
    keycount    = 48            # listen to more EFDs because of the low rate
    streamLogic = 'Or'          # enable stream-based event selection
    streamName  = 'IDCosmic'    # probably better for us than CosmicCalo or CosmicMuons

# --- Trigger selection for noise monitoring
if (jobName == 'TRT-Monitoring-Noise'):
    keyname     = 'SFI'         # take the events straight from L2 - the IDMonitoring stream does not go through the Event Filter, anyway
    keycount    = 255           # there are only around 100 SFIs - connect to all of them
    groupName  = ''         # process the same events as other monitoring jobs with the same event selection
    streamLogic = 'Or'
    streamName  = 'IDMonitoring' # avoid PixelNoise and SCTNoise, because those streams contain mostly partial events without TRT data
    lvl1Logic   = 'Or'
    lvl1Name    = 'L1_RD0_EMPTY'

# --- Trigger selection for beam monitoring
if (jobName == 'TRT-Monitoring-Beam'):
    keyname     = 'SFI'         # take the events straight from L2 - the IDMonitoring stream does not go through the Event Filter, anyway
    keycount    = 255           # there are only around 100 SFIs - connect to all of them
    groupName  = ''         # process the same events as other monitoring jobs with the same event selection
    streamLogic = 'Or'
    streamName  = 'IDMonitoring' # avoid PixelNoise and SCTNoise, because those streams contain mostly partial events without TRT data
    lvl1Logic   = 'Or'
    lvl1Name    = 'L1_RD0_FILLED'
    doBeamMon   = True          # use the special monitoring tool InDetMonitoringTRT_Beam.py in InDetMonitoring.py
    doESD       = False         # switch off any reconstruction to gain some speed, we only need raw data here

# --- Special settings for the stand-alone test partition
if (jobName.startswith('TRT-Monitoring-Test')):
    isserverName = 'Histogramming' # the test partition uses the default IS Server
    keyname     = 'SFI'         # the test partition does not have an Event Filter
    streamLogic = 'Ignore'      # take all events from the test partition
    lvl1Logic   = 'Ignore'      # take all events from the test partition

# --- Special settings for independent running on the command line
if (jobName == 'TRT-Monitoring-Extra'):
    streamLogic = 'Or'
    streamName  = 'Egamma'      # default selection for stand-alone test jobs

### two lines copied from RecExCommission/RecExCommission.py and uncommented
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.TRT_BuildStrawLayers.set_Value(True) # needed for proper efficiency determination during online monitoring

### main online reco scripts
include ("RecExOnline/RecExOnline_jobOptions.py")

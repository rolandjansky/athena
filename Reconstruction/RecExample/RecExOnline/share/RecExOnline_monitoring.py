## -------------------------------------------------------------------
## Main settings of online reco script, can be set from outside

## default, all turned on
if (not 'doAllMon' in dir()):
  doAllMon = True

if (not 'doCaloMon' in dir()):
  doCaloMon = doAllMon

if (not 'doPhysMon' in dir()):
  doPhysMon = doAllMon

if (not 'doTrigMon' in dir()):
  doTrigMon = doAllMon

if (not 'doLVL1CaloMon' in dir()):
  doLVL1CaloMon = doTrigMon

if (not 'doHLTMon' in dir()):
  #doHLTMon = doTrigMon
  doHLTMon = False 
if (not 'doCTPMon' in dir()):
  #doCTPMon = doTrigMon
  doCTPMon = False
if (not 'doIDMon' in dir()):
  doIDMon = doAllMon

if (not 'doPixelOnlyMon' in dir()):
  doPixelOnlyMon = False # doAllMon

if (not 'doSCTMon' in dir()):
  doSCTMon = doAllMon

if (not 'doMuonMon' in dir()):
  doMuonMon = doAllMon

if (not 'doTRTMon' in dir()):
  doTRTMon = doAllMon

if (not 'doLucidMon' in dir()):
  doLucidMon = doAllMon

if (not 'isOnline' in dir()):
  isOnline = False

if (not 'doTileMon' in dir()):
  doTileMon = False #doAllMon

if (not 'doHIMon' in dir()):
  doHIMon = False

if (not 'isOfflineTest' in dir()):
  isOfflineTest   = False

## if ('isGlobalMonitoring' not in dir()):
##     isGlobalMonitoring = False


# --------------------------------------------- Monitoring
from AthenaMonitoring.DQMonFlags import DQMonFlags

if isOnline and useEmon:
  DQMonFlags.histogramFile = ''
  DQMonFlags.monManFileKey = 'ourGLOBAL'                                   # must use a different stream w.r.t what sets in DQ steering JO
  DQMonFlags.monManEnvironment.set_Value_and_Lock('online')

DQMonFlags.monManDataType.set_Value_and_Lock('data')
#DQMonFlags.monManDataType.set_Value_and_Lock('cosmics')

orMon = doAllMon or doCaloMon or doPhysMon or doIDMon or doMuonMon or doTRTMon or doCTPMon or doLucidMon or doSCTMon or doPixelOnlyMon or doLVL1CaloMon or doHLTMon or doHIMon
DQMonFlags.doMonitoring.set_Value_and_Lock(orMon)

from RecExConfig.RecFlags import rec
rec.doMonitoring.set_Value_and_Lock(orMon)

# ******************* Calorimeter Monitorings
DQMonFlags.doLArMon.set_Value_and_Lock(doCaloMon)
DQMonFlags.doTileMon.set_Value_and_Lock(doTileMon) #False)

# ******************* Physics Monitorings
DQMonFlags.doMissingEtMon.set_Value_and_Lock(doPhysMon)
DQMonFlags.doEgammaMon.set_Value_and_Lock(doPhysMon)
DQMonFlags.doJetMon.set_Value_and_Lock(doPhysMon)
DQMonFlags.doJetTagMon.set_Value_and_Lock(doPhysMon)
DQMonFlags.doTauMon.set_Value_and_Lock(doPhysMon)
DQMonFlags.doCaloMon.set_Value_and_Lock(doCaloMon)
### DQMonFlags.doMuonPhysicsMon.set_Value_and_Lock(doPhysMon) ### replaced by MuonTrackMonitoring (SMW Jan 23 2016)
DQMonFlags.doGlobalMon.set_Value_and_Lock(doPhysMon)

# ******************* Trigger monitorings
useTrigger = doHLTMon or doLVL1CaloMon or doCTPMon
DQMonFlags.useTrigger.set_Value_and_Lock(useTrigger)
DQMonFlags.doHLTMon.set_Value_and_Lock(doHLTMon)
DQMonFlags.doLVL1CaloMon.set_Value_and_Lock(doLVL1CaloMon)
DQMonFlags.doCTPMon.set_Value_and_Lock(doCTPMon)

# ******************* ID monitorings
DQMonFlags.doInDetAlignMon.set_Value_and_Lock(doIDMon)
DQMonFlags.doInDetGlobalMon.set_Value_and_Lock(doIDMon)
DQMonFlags.doInDetPerfMon.set_Value_and_Lock(doIDMon)
DQMonFlags.doPixelMon.set_Value_and_Lock(doIDMon or doPixelOnlyMon)
DQMonFlags.doSCTMon.set_Value_and_Lock(doSCTMon)
DQMonFlags.doTRTElectronMon.set_Value_and_Lock(False)
DQMonFlags.doTRTMon.set_Value_and_Lock(doTRTMon)

# ******************* Muon monitorings
from MuonDQAMonFlags.MuonDQAProperFlags import MuonDQADetFlags
DQMonFlags.doMuonAlignMon.set_Value_and_Lock(False)                      #### set False by default
DQMonFlags.doMuonCombinedMon.set_Value_and_Lock(doMuonMon)

#MuonDQADetFlags.doTGCMon.set_Value_and_Lock(False)
#MuonDQADetFlags.doTGCL1Mon.set_Value_and_Lock(False)
#MuonDQADetFlags.doMDTTGCL1Mon.set_Value_and_Lock(False)

### DQMonFlags.doMuonRawMon.set_Value_and_Lock(False) #AK - 04/07/2014 doMuonMon)
DQMonFlags.doMuonRawMon.set_Value_and_Lock(doMuonMon)
DQMonFlags.doMuonSegmentMon.set_Value_and_Lock(doMuonMon)
DQMonFlags.doMuonTrackMon.set_Value_and_Lock(doMuonMon)

# ******************* Additional monitorings
DQMonFlags.doLucidMon.set_Value_and_Lock(doLucidMon)

# ******************* Heavy Ion monitorings
DQMonFlags.doHIMon.set_Value_and_Lock(doHIMon)


#toRun='GM-Application:GlobalMonitoringSegment:pc-tdq-gmon-02:1'
#if (toRun==publishName):
#if (isGlobalMonitoring):     #AK: 21-05-2014
#  include("BFieldTools/DQTBCurrentTool.py")  #AK: 21-05-2014


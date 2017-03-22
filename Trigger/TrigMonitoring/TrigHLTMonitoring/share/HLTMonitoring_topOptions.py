from AthenaCommon.Logging import logging
log = logging.getLogger( 'TrigHLTMonitoring/HLTMonitoring_topOptions' )
######## flags ###########

if not 'HLTMonFlags' in dir():
  from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags

if not 'DQMonFlags' in dir():
  from AthenaMonitoring.DQMonFlags import DQMonFlags

########## control step assignment #########
if DQMonFlags.monManEnvironment == 'tier0Raw':
  # we are in RAW->ESD step
  # run all tools *except* the following (these are run in ESD->AOD)
  log.info('Environment is tier0Raw')
  HLTMonFlags.doGeneral  = False
  HLTMonFlags.doBjet     = False
  HLTMonFlags.doBphys    = False
  HLTMonFlags.doMET      = False
  HLTMonFlags.doJet      = False
  HLTMonFlags.doEgamma   = False
  HLTMonFlags.doMuon     = False
  HLTMonFlags.doIDtrk    = False
  HLTMonFlags.doTau      = False
  HLTMonFlags.doMinBias  = False
  HLTMonFlags.doDump     = False
  HLTMonFlags.doOfflineTauTTP = False
  HLTMonFlags.doIDJpsiMon  = False
  HLTMonFlags.doMaM = False
elif DQMonFlags.monManEnvironment == 'tier0ESD':
  log.info('Environment is tier0ESD')
  # we are in ESD->AOD step
  # run all tools *except* the following (these are run in RAW->ESD)
  # HLTMonFlags.doCalo     = False
  # HLTMonFlags.doMaM      = True
  # HLTMonFlags.doMaM_ApplyMCK = True
elif DQMonFlags.monManEnvironment == 'tier0':
  # we are in RAW -> ALL, run everything
  log.info('Environment is tier0')
else :
  log.info('Environment is neither tier0Raw nor tier0ESD')
  log.info('Switching all tools off...')
  HLTMonFlags.doGeneral = False
  HLTMonFlags.doMonTier0 = False
  HLTMonFlags.doMaM = False

# temporarily disabling IDJpsiMon to deal with ATR-12037
HLTMonFlags.doIDJpsiMon = False

log.info("HLTMonFlags are:")
print HLTMonFlags

include( "TrigHLTMonitoring/addMonTools.py" )

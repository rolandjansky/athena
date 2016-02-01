print "hello from TrigHLTMonitoringJobOptions_forRecExCommission.py"

######## flags ###########

if not 'HLTMonFlags' in dir():
  from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags

if not 'DQMonFlags' in dir():
  from AthenaMonitoring.DQMonFlags import DQMonFlags

    ########## control step assignment #########

if DQMonFlags.monManEnvironment == 'tier0Raw':
  # we are in RAW->ESD step
  # run all tools *except* the following (these are run in ESD->AOD)
  print 'HLTMonitoring_topOptions.py: environment is tier0Raw'
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
  ### Menu-aware monitoring flags ###
  #HLTMonFlags.doMaM                       = True # default is False
  #HLTMonFlags.doMaM_ExtractAndDumpConfigs = True # default is False
  #HLTMonFlags.MaM_OutputJSON              = "mam_configs.json" # default is "mam_configs.json"
  #HLTMonFlags.doMaM_ApplyMCK              = True # default is False
  #HLTMonFlags.MCK                         = -1 # default is -1
elif DQMonFlags.monManEnvironment == 'tier0ESD':
  # we are in ESD->AOD step
  # run all tools *except* the following (these are run in RAW->ESD)
  print 'HLTMonitoring_topOptions.py: environment is tier0ESD'
  # HLTMonFlags.doCalo     = False 
elif DQMonFlags.monManEnvironment == 'tier0':
  # we are in RAW -> ALL, run everything
  print 'HLTMonitoring_topOptions.py: environment is tier0'
else :
  print 'HLTMonitoring_topOptions.py: environment is neither tier0Raw nor tier0ESD'
  print 'HLTMonitoring_topOptions.py: switching all tools off...'
  HLTMonFlags.doGeneral = False
  HLTMonFlags.doMonTier0 = False

# temporarily disabling IDJpsiMon to deal with ATR-12037
HLTMonFlags.doIDJpsiMon = False
include( "TrigHLTMonitoring/addMonTools.py" )

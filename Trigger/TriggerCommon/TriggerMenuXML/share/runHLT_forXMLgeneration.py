######################################################################################
# Minimal job options to generate the trigger menu XML files
# DO NOT USE THESE JOB OPTIONS TO RUN THE TRIGGER
#
from AthenaCommon.Logging import logging
log = logging.getLogger('runHLT_forXMLgeneration.py')

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

# Input file is only needed so that TileCal picks up the correct cabling according to the year
athenaCommonFlags.FilesInput=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data18_13TeV.00360026.physics_EnhancedBias.merge.RAW._lb0151._SFO-1._0001.1"]
athenaCommonFlags.BSRDOInput=[]
athenaCommonFlags.PoolRDOInput=[]
athenaCommonFlags.isOnline = True
globalflags.InputFormat='bytestream'
globalflags.DataSource='data'

# in Standalone mode, don't allow any configuration errors
athenaCommonFlags.AllowIgnoreConfigError = False

from RecExConfig.RecFlags import rec
rec.doESD=False
rec.doAOD=False
rec.doWriteESD=False
rec.doWriteAOD=False
rec.doWriteTAG=False
rec.doTruth=False
rec.doCBNT=False
rec.doTrigger=True
rec.AutoConfiguration.set_Value_and_Lock([])

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.outputLVL1configFile="outputLVL1config.xml"
TriggerFlags.outputHLTconfigFile="outputHLTconfig.xml"
TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False
TriggerFlags.enableMonitoring = [ 'Time' ]

# Do not configure L1 simulation
TriggerFlags.doLVL1=False

#------------ This is for ATN/RTT tests only ---------
TriggerFlags.triggerMenuSetup = 'default'
if 'TriggerMenuSetup' in dir():
   if TriggerMenuSetup in TriggerFlags.triggerMenuSetup.allowedValues:
      TriggerFlags.triggerMenuSetup = TriggerMenuSetup
      log.info('Overwriting menu: %s' % TriggerFlags.triggerMenuSetup())
   else:
      # The "> Warning:" string is important to recognize this as build warning in NICOS
      log.error('--> Warning: Menu %s not available in this version of TriggerMenu. Skipping...' % TriggerMenuSetup)
      theApp.exit()

if 'L1PrescaleSet' in dir():
   TriggerFlags.L1PrescaleSet = L1PrescaleSet
if 'HLTPrescaleSet' in dir():
   TriggerFlags.HLTPrescaleSet = HLTPrescaleSet
if 'PrescaleSet' in dir():
   log.info('Prescaleset: %s' % PrescaleSet)
   if PrescaleSet == 'None':
      TriggerFlags.L1PrescaleSet = 'None'
      TriggerFlags.HLTPrescaleSet = 'None'
   elif PrescaleSet == '':
      TriggerFlags.L1PrescaleSet = ''
      TriggerFlags.HLTPrescaleSet = ''
   else:
      TriggerFlags.L1PrescaleSet = 'L1'+ PrescaleSet
      TriggerFlags.HLTPrescaleSet = 'HLT'+ PrescaleSet

#-----------------------------------------------------------
HLTOutputLevel=INFO
include("TriggerJobOpts/Trigger_topOptions_standalone.py")
#-----------------------------------------------------------

log.info("Trigger menu generation done. Exiting...")
theApp.exit()

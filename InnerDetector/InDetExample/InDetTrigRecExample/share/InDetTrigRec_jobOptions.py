# +++++++++++++++++++ beginning of InDetTrigRec_jobOptions.py

# jobOptions Fragment for ID software
# -----------------------------------
# Patricia.Conde.Muino@cern.ch
# Jiri.Masik@cern.ch
# -----------------------------------

#retrieve common trigger settings (truth)
from TriggerJobOpts.TriggerFlags import TriggerFlags

from AthenaCommon.Logging import logging 
log = logging.getLogger("InDetTrigRec_jobOptions.py")

if not 'InDetTrigFlags' in dir():
   # --- setup flags with default values
   log.info("InDetTrigRec_jobOptions: InDetTrigFlags not set - setting to defaults")
   from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
   InDetTrigFlags.doNewTracking.set_Value_and_Lock(True)
   #InDetTrigFlags.InDet25nsec = True      #autoconfig and runHLT_standalone setting for BS 
   InDetTrigFlags.primaryVertexSetup = "IterativeFinding"
   #InDetTrigFlags.primaryVertexSetup = "DefaultFastFinding"
   InDetTrigFlags.doRefit = True    # switched on for ATR-12226 (z0 uncertainties in bjets)
   InDetTrigFlags.doPixelClusterSplitting = False
   InDetTrigFlags.doPrintConfigurables = False    #
   #InDetTrigFlags.doPrintConfigurables = True    #

#moved the truth setting (can be overriden with set&lock)
InDetTrigFlags.doTruth = TriggerFlags.doTruth()

InDetTrigFlags.init()
InDetTrigFlags.print_JobProperties()

#
# ----------- printout the setup
#
InDetTrigFlags.printInfo()

# if not 'InDetTrigCutValues' in dir():
#    #
#    # --- setup physics cuts with default values
#    #
#    log.info("InDetRec_jobOptions: InDetTrigCutValues not set - setting to defaults")
#    include ("InDetTrigRecExample/ConfiguredInDetCutValues.py" )
#    InDetTrigCutValues = ConfiguredInDetCutValues()
#
# ----------- printout the setup
#
#InDetTrigCutValues.printInfo()


#
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

#conditions setup
if InDetTrigFlags.useConditionsClasses():
  log.info("Setting up conditions using the new classes")
else:
  include ("InDetRecExample/InDetRecConditionsAccess.py")

# remove all the old RegSelSvc setting - leave commented 
# while we determinte whether the RegSel migration has been successful
# manipulate RegSelSvc settings to be aware of the inactive detelems
# from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
# RegSelSvc = RegSelSvcDefault()

# RegSelSvc.DisableFromConditions = False
# RegSelSvc.DisablePixelFromConditions = False
# RegSelSvc.DisableTRTFromConditions = False
# RegSelSvc.DisableSCTFromConditions = False
from InDetTrigRecExample.InDetTrigConfigConditions import \
         SCT_ConditionsSetup, TRT_ConditionsSetup
# RegSelSvc.PixConditionsSvc = ""      #empty as it does not work
# RegSelSvc.SCTConditionsTool = "SCT_ConditionsSummaryTool/"+SCT_ConditionsSetup.instanceName("InDetSCT_ConditionsSummaryTool")
# RegSelSvc.TRTConditionsSvc = "TRT_ConditionsSummarySvc/"+TRT_ConditionsSetup.instanceName("InDetTRTConditionsSummaryService")

#this should be our common cabling setup/
# online running data are dealt with in the TriggerJobOpts 
include ("InDetRecExample/InDetRecCabling.py")



# ------------------------------------------------------------
#
# ----------- Loading the Tracking Tools and Services
#
# ------------------------------------------------------------
#moved to python
include ("InDetTrigRecExample/InDetTrigRecLoadTools.py")


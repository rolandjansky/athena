#################################################################
#
# InnerDetector (iPatRec requirements) jobOptions fragment
#
#==============================================================

#--------------------------------------------------------------
# set InDet defaults
#--------------------------------------------------------------

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
from RecExConfig.RecFlags import rec
InDetFlags.doTruth      = rec.doTruth()
InDetFlags.doStatistics = False

# --- uncomment to disable one of the following tracking algorithms
InDetFlags.doNewTracking = False
InDetFlags.doxKalman     = False
#InDetFlags.doiPatRec     = False

#
# ----------- lock and printout the setup
#
InDetFlags.init()
InDetFlags.printInfo()
InDetFlags.print_JobProperties()

if not 'InDetNewTrackingCuts' in dir():
   #
   # --- setup physics cuts with default values
   #
   print "InDetRec_jobOptions: InDetCutValues not set - setting to defaults"
   from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
   InDetNewTrackingCuts = ConfiguredNewTrackingCuts("Offline")
   from InDetRecExample.ConfiguredVertexingCuts import ConfiguredVertexingCuts 
   InDetPrimaryVertexingCuts      = ConfiguredVertexingCuts("NoLumi")
   
#
# ----------- printout the setup
#
InDetNewTrackingCuts.printInfo()

if not 'InDetKeys' in dir():
   #
   # --- setup StoreGate keys (JobProperties!)
   #
   print "InDetRec_jobOptions: InDetKeys not set - setting to defaults"
   from InDetRecExample.InDetKeys import InDetKeys
   InDetKeys.lock_JobProperties()
#
# ----------- printout the setup
#
InDetKeys.print_JobProperties()

# ------------------------------------------------------------
# 
# ----------- Configuring the conditions access
#
# ------------------------------------------------------------
    
include ("InDetRecExample/InDetRecConditionsAccess.py")
  
# ------------------------------------------------------------
# 
# ----------- Loading the Tracking Tools and Services 
#
# ------------------------------------------------------------

include ("InDetRecExample/InDetRecLoadTools.py")
  
# ------------------------------------------------------------
#
# ----------- Data-Preparation stage
#
# ------------------------------------------------------------

#include ("InDetRecExample/InDetRecPreProcessing.py")

include ("InDetRecExample/InDetRecPreProcessingSilicon.py")

#include ("InDetRecExample/InDetRecPreProcessingTRT.py")
include ("InDetRecExample/ConfiguredInDetPreProcessingTRT.py")
# --- TRT, no drift information if cosmics, do not use extrenal phase in any case
InDetPreProcessingTRT = ConfiguredInDetPreProcessingTRT(not InDetFlags.doCosmics(),False)

#==============================================================
# End of InnerDetector (iPatRec requirements) jobOptions fragment
#
###############################################################

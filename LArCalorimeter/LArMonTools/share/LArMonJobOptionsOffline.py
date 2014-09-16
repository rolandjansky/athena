###########################################################################
#
# <Christophe.Le.Maner@cern.ch>
#
###########################################################################

#
# Common header to all LArMon jobs
#
include("LArMonTools/LArMonCommonHeader_jobOptions.py") # contains the setting up of athena for the monitoring + rootfile naming  + LArBuilders

include("LArMonTools/LArMonManager.py") # LArMon topSequence defined here

#
# Specific LAr Monitoring tools you want to include according to the processed run type 
#
include("LArMonTools/LArMonTools.py") 

#
# Common trailer to all LArMon jobs
#
include("LArMonTools/LArMonCommonTrailer_jobOptions.py")  # here are set the output message levels

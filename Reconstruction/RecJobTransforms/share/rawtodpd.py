###############################################################
#
# Skeleton top job options for RAW->DESD
# Put here outputs that require rec.doDPD=True
#
# TODO: Review of options supported here...
#
#==============================================================

rec.doDPD = True
rec.DPDMakerScripts.append("PrimaryDPDMaker/PrimaryDPDMaker.py")
include( "RecExCommon/RecExCommon_topOptions.py" )
#####################################################################
## @file EventSplit_jobOptions.py
## @brief JobOptions: Defaults.
## @author Jack Cranshaw
## $Id: EventSplit_jobOptions.py,v 1.2 2005-01-07 23:26:18 cranshaw Exp $
#====================================================================
theApp.Dlls  += [ "AthenaPoolMultiTest" ]
EventSplit = Algorithm("EventSplit")
EventSplit.L1bitmask = 0 # Level 1 bit mask to accept 
EventSplit.L2bitmask = 0 # Level 2 bit mask to accept 
#==============================================================
#
# End of job options file
#
###############################################################

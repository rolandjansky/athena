#**************************************************************
#
#  MessageSvc configuration file
#
#==============================================================
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import *

# make the message service available
svcMgr.MessageSvc = theApp.service( "MessageSvc" )     # already instantiated
MessageSvc = svcMgr.MessageSvc

MessageSvc.OutputLevel  = OutputLevel

# Use color for different message levels
# --------------------------------------
MessageSvc.useColors    = False

# Format string for all messages
# ------------------------------
MessageSvc.Format       = "%t  % F%35W%S%7W%R%T %0W%M"

# Message suppression
# -------------------
MessageSvc.enableSuppression    = False
MessageSvc.suppressRunningOnly  = True
MessageSvc.resetStatsAtBeginRun = True

# 0 = no suppression, negative = log-suppression, positive = normal suppression
# Do not rely on the defaultLimit property, always set each limit separately
MessageSvc.defaultLimit = -100
MessageSvc.verboseLimit = MessageSvc.defaultLimit
MessageSvc.debugLimit   = MessageSvc.defaultLimit
MessageSvc.infoLimit    = MessageSvc.defaultLimit
MessageSvc.warningLimit = MessageSvc.defaultLimit
MessageSvc.errorLimit   = 0
MessageSvc.fatalLimit   = 0

# set message limit to unlimited when general DEBUG is requested
if (("OutputLevel" in dir()) and (OutputLevel<=DEBUG)) :
    MessageSvc.defaultLimit = 0
    MessageSvc.enableSuppression = False
if (("HLTOutputLevel" in dir()) and (HLTOutputLevel<=DEBUG)) :
    MessageSvc.defaultLimit = 0
    MessageSvc.enableSuppression = False

# show summary statistics of messages in finalize
# -----------------------------------------------
MessageSvc.showStats = True
MessageSvc.statLevel = WARNING
MessageSvc.statLevelRun = VERBOSE

# publish message counts during RUNNING in histogram
# --------------------------------------------------
MessageSvc.publishStats = True
MessageSvc.publishLevel = INFO


# report messages via ERS
# -----------------------
# report given message severities via ERS
# MessageSvc.useErsVerbose = ['*']
# MessageSvc.useErsDebug = ['*']
# MessageSvc.useErsInfo = ['*']
# MessageSvc.useErsWarning = ['*']
# MessageSvc.useErsError = ['*']
# MessageSvc.useErsFatal = ['*']

# Set to True, if ERS messages should be duplicated to the MsgStream
# MessageSvc.alwaysUseMsgStream = True
# Only forward message during RUNNING
# MessageSvc.useErsRunningOnly = True

#==============================================================
#
# End of MessageSvc configuration file
#
#**************************************************************

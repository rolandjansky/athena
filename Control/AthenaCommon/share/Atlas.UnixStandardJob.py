#**************************************************************
#
# Unix standard job options file
#
#==============================================================

## Unix job standard configuration
import AthenaCommon.AtlasUnixStandardJob

## get a handle on the ServiceMgr
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#--------------------------------------------------------------
# units, as defined from CLHEP (not shown on screen)
#--------------------------------------------------------------
from AthenaCommon.SystemOfUnits import *

# make sure StatusCodeSvc messages are always printed
StatusCodeSvc = svcMgr.StatusCodeSvc

# make the message service available
MessageSvc = svcMgr.MessageSvc
MessageSvc.enableSuppression = True

#--------------------------------------------------------------
# StoreGate services configuration
#--------------------------------------------------------------
StoreGateSvc  = svcMgr.StoreGateSvc
DetectorStore = svcMgr.DetectorStore
HistoryStore  = svcMgr.HistoryStore

#--------------------------------------------------------------
# Main alg sequence
#--------------------------------------------------------------
from AthenaCommon.AppMgr import athAlgSeq

#==============================================================
#
# End of Unix standard job options file
#
#**************************************************************

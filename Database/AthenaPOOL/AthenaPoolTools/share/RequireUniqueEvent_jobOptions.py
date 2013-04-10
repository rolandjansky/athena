## @file RequireUniqueEvent_jobOptions.py
## @brief JobOptions: Defaults.
## @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
## $Id: RequireUniqueEvent_jobOptions.py,v 1.4 2007-10-18 13:46:20 gemmeren Exp $
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr

from CLIDComps.CLIDCompsConf import ClassIDSvc
svcMgr += ClassIDSvc()

from AthenaCommon.Constants   import INFO
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaPoolTools.AthenaPoolToolsConf import RequireUniqueEvent
topSequence += RequireUniqueEvent( OutputLevel = INFO )

#==============================================================
#
# End of job options file
#
###############################################################

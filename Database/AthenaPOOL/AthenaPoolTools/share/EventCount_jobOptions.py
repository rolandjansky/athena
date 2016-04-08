## @file EventCount_jobOptions.py
## @brief JobOptions: Defaults.
## @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
## $Id: EventCount_jobOptions.py,v 1.6 2007-10-18 13:46:20 gemmeren Exp $
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr

from CLIDComps.CLIDCompsConf import ClassIDSvc
svcMgr += ClassIDSvc()

from AthenaCommon.Constants   import INFO
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
alg = xAODMaker__EventInfoCnvAlg()
alg.xAODKey = ""
topSequence += alg

from AthenaPoolTools.AthenaPoolToolsConf import EventCount
topSequence += EventCount( OutputLevel = INFO,
                           Dump        = True )
#==============================================================
#
# End of job options file
#
###############################################################

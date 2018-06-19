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

include("EventSelectorAthenaPool/MetaDataSvc_jobOptions.py")
from AthenaServices.AthenaServicesConf import MetaDataSvc
svcMgr += MetaDataSvc()
svcMgr.MetaDataSvc.OutputLevel = 2
svcMgr.MetaDataSvc.MetaDataContainer = "MetaDataHdr"
svcMgr.ProxyProviderSvc.ProviderNames += [ "MetaDataSvc" ]

from IOVSvc.IOVSvcConf import MetaInputLoader
metain = MetaInputLoader()
metain.Dump = True
metain.OutputLevel = DEBUG
metain.Load = [('EventStreamInfo','StreamAOD'),('xAOD::CutBookkeeperContainer','CutBookkeepers'),('xAOD::CutBookkeeperAuxContainer','CutBookkeepersAux.')]
topSequence += metain

from AthenaPoolTools.AthenaPoolToolsConf import MetadataTest
topSequence += MetadataTest( OutputLevel = INFO )
#==============================================================
#
# End of job options file
#
###############################################################

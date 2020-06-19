#=================================================================
#
# JobOption for testing the reading of the LVL1 content of a BS
# file. The jobO is designed to be used together with
# WriteLVL1BS_jobOption.py.
#
#=================================================================

#
# User configuration:
#
CBNTAAOutput = "LVL1BSTest.root"
EvtMax = -1

#
# Create the necessary Python objects:
#
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.Logging import logging
log = logging.getLogger( "ReadLVL1BS" )

from AthenaCommon.AppMgr import ServiceMgr

#
# Set up the reading of the BS file:
#
include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
svcMgr.EventSelector.Input = [
  "daq.lvl1test.0000000.Single.Stream.LB0000.Athena._0001.data"
]

include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )
include( "ByteStreamCnvSvcBase/BSAddProvSvc_RIO_jobOptions.py" )

log.info( "Set input BS file" )

#
# Set up writing of the AANT file:
#
from GaudiSvc.GaudiSvcConf import THistSvc
histSvc = THistSvc()
histSvc.Output = [ "AANT DATAFILE='" + CBNTAAOutput + "' OPT='RECREATE'" ]
ServiceMgr += histSvc

from AnalysisTools.AnalysisToolsConf import AANTupleStream
ntupleStream = AANTupleStream()
ntupleStream.OutputName = CBNTAAOutput
ntupleStream.ExistDataHeader = False
topSequence += ntupleStream

theApp.HistogramPersistency = "ROOT"

log.info( "Configured output AANT file" )

#
# Configure the LVL1 config svc.:
#
from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc
ServiceMgr += LVL1ConfigSvc( "LVL1ConfigSvc" )
ServiceMgr.LVL1ConfigSvc.XMLFile = "Rel3_Sept_12_outputLVL1config.xml"
ServiceMgr.LVL1ConfigSvc.CreateLegacyObjects = True
log.info( "Set up the LVL1 configuration service" )

#
# The CBNTAA algorithms:
#
from CBNT_Athena.CBNT_AthenaAwareCfg import CBNT_AthenaAware
topSequence += CBNT_AthenaAware()

from TrigT1CTP.TrigT1CTPConf import LVL1CTP__CBNTAA_CTP_RDO
topSequence.CBNT_AthenaAware += LVL1CTP__CBNTAA_CTP_RDO( "CBNTAA_CTP_RDO" )

from TrigT1Muctpi.TrigT1MuctpiConf import CBNTAA_ReadMuCTPI_RDO
topSequence.CBNT_AthenaAware += CBNTAA_ReadMuCTPI_RDO()

from TrigT1RoIB.TrigT1RoIBConf import CBNTAA_ReadRoIBResult
topSequence.CBNT_AthenaAware += CBNTAA_ReadRoIBResult()

log.info( "Added all CBNTAA algorithms" )

#
# Additional configuration:
#
theApp.EvtMax = EvtMax
ServiceMgr.MessageSvc.OutputLevel = 3
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
#ToolSvc.OutputLevel = 2

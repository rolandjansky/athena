
from AthenaCommon.Configurable import Configurable

Configurable.allConfigurables.get('CTPSimulation').OutputLevel = ERROR

from TrigT1Lucid.TrigT1LucidConf import LVL1__TrigT1Lucid

Lucid = LVL1__TrigT1Lucid("LVL1::TrigT1Lucid")
Lucid.OutputLevel  = ERROR
Lucid.qdcThreshold = 500

theApp.Dlls += ["RootHistCnv"]
theApp.HistogramPersistency = "ROOT"

from CBNT_Athena.CBNT_AthenaConf import *
from CBNT_Utils.CBNT_UtilsConf   import *

CBNT_AthenaAware = CBNT_AthenaAware()
topSequence     += CBNT_AthenaAware

from CBNT_Athena.CBNT_AthenaConf import *
from CBNT_Utils.CBNT_UtilsConf   import *
from TrigT1Lucid.TrigT1LucidConf import *

topSequence.CBNT_AthenaAware.Members += ["CBNTAA_EventInfo"]
topSequence.CBNT_AthenaAware.Members += ["LVL1CTP::CBNTAA_CTP_RDO/CTP_RDO"]
topSequence.CBNT_AthenaAware.Members += ["LVL1::CBNTAA_TrigT1Lucid/CBNTAA_TrigT1Lucid"]

from AnalysisTools.AnalysisToolsConf import AANTupleStream

topSequence   += AANTupleStream()
AANTupleStream = topSequence.AANTupleStream
AANTupleStream.ExistDataHeader = False

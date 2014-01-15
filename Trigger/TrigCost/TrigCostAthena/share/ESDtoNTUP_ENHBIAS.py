from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

import TrigCostAthena.TrigCostAthenaConfig as costAthena
alg = costAthena.prepareCostAlg()
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()
topSeq += [alg]


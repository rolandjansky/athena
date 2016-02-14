# topOptions for egammaRec_RTT
#
from RecExConfig.RecFlags import rec
from RecExConfig.RecFlags import recAlgs
#
rec.doESD = False # if false, all algorithms are switched off by defaults 
doESD = rec.doESD() # if false, all algorithms are switched off by defaults 
#only switched minimal algorithm
recAlgs.donewTracking = True
donewTracking = recAlgs.donewTracking()
#
recAlgs.doEmCluster = True
doEmCluster = recAlgs.doEmCluster()
#
recAlgs.doEgamma = True
doEgamma = recAlgs.doEgamma()
#
recAlgs.doConversion = True
doConversion = recAlgs.doConversion()
#
rec.doAOD = False
doAOD = rec.doAOD()
#
rec.doWriteAOD = False
doWriteAOD = rec.doWriteAOD()
#
rec.doWriteESD = False
doWriteESD = rec.doWriteESD()
#
rec.doWriteTAG = False
doWriteTAG = rec.doWriteTAG()
# number of event to process
#from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.EvtMax= 2000
EvtMax= 2000

include ("RecExCommon/RecExCommon_flags.py")

DetFlags.Muon_setOff()

include ("RecExCommon/RecExCommon_topOptions.py")

CBNT_MuonParticle=Algorithm("CBNT_MuonParticle")
CBNT_MuonParticle.Enable=False
CBNT_AodTrigger=Algorithm("CBNT_AodTrigger")
CBNT_AodTrigger.Enable=False

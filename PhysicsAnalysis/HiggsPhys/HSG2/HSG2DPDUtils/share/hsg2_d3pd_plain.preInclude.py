
## preInclude to be used by Reco_trf
## this is not meant to the run standalone

from AthenaCommon.AlgSequence import AlgSequence
from GaudiAlg.GaudiAlgConf import Sequencer
from AthenaCommon.AlgSequence import AthSequencer
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

#from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutHelper

from PrimaryDPDMaker import PrimaryDPDFlags


theJob = AlgSequence()


from RecExConfig.RecFlags import rec
# Turn off most of RecExCommon... (optional)
#rec.doCBNT       = False
#rec.doWriteESD   = False
#rec.doWriteAOD   = False
#rec.doAOD        = False
#rec.doWriteTAG   = False
#rec.doPerfMon    = False
#rec.doHist       = False
#rec.doTruth      = False
#
#
#
#rec.doCBNT.set_Value_and_Lock(False)
#rec.doWriteESD.set_Value_and_Lock(False)
#rec.doWriteAOD.set_Value_and_Lock(False)
#rec.doAOD.set_Value_and_Lock(False)
#rec.doESD.set_Value_and_Lock(False)
#rec.doDPD.set_Value_and_Lock(False)
#rec.doWriteTAG.set_Value_and_Lock(False)
#rec.doForwardDet.set_Value_and_Lock(False)
#

theJob = AlgSequence()

include("HSG2DPDUtils/hsg2_dpd_utils_fragment.py")

from RecExConfig.RecFlags import rec

rec.DPDMakerScripts.append("HSG2DPDUtils/hsg2_d3pdmaker_plain.py")
rec.doDPD.set_Value_and_Lock(True)


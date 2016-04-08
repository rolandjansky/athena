
## preInclude to be used by Reco_trf
## this is not meant to the run standalone

from AthenaCommon.AlgSequence import AlgSequence
from GaudiAlg.GaudiAlgConf import Sequencer
from AthenaCommon.AlgSequence import AthSequencer
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

#from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutHelper

from PrimaryDPDMaker import PrimaryDPDFlags

#PrimaryDPDFlags.WritePhysDPD2LHSG2=True

theJob = AlgSequence()

from RecExConfig.RecFlags import rec

rec.DPDMakerScripts.append("HSG2DPDUtils/hsg2_daodmaker_2lepton.py")
#rec.DPDMakerScripts.append("HSG2DPDUtils/RandomFilter.py")
rec.doDPD.set_Value_and_Lock(True)

# jobOptions for BeamCondSvc - just define conditions data access

include.block("InDetBeamSpotService/BeamCondSvc.py")
from IOVDbSvc.CondDB import conddb
try:
    from RecExConfig.RecFlags import rec
    # If express processing, point beam spot to online folder results
    if(rec.doExpressProcessing()):
        conddb.addFolder('INDET_ONL','/Indet/Onl/Beampos <key>/Indet/Beampos</key>')
    else:
        conddb.addFolderSplitOnline("INDET","/Indet/Onl/Beampos","/Indet/Beampos")
except ImportError:
    # Protection for AthSimulationBase release which does not contain RecExConfig
    conddb.addFolderSplitOnline("INDET","/Indet/Onl/Beampos","/Indet/Beampos")


#Code to help transition to MT system, remove once this package is removed
if not DetFlags.simulate.any_on() :
   from AthenaCommon.AlgSequence import AthSequencer
   condSeq = AthSequencer("AthCondSeq")
   if not hasattr(condSeq, "BeamSpotCondAlg"):
       from BeamSpotConditions.BeamSpotConditionsConf import BeamSpotCondAlg
       condSeq += BeamSpotCondAlg( "BeamSpotCondAlg" )


include( "TBCnv/TBReadH8BS_EventStorage_jobOptions.py" )
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
#theApp.Dlls += [ "TBRec" ]
#theApp.TopAlg += ["TBScintillatorRec"]
#TBScintRec =  Algorithm("TBScintillatorRec")
#TBScintRec.OutputLevel=DEBUG
from TBRec.TBRecConf import TBScintillatorRec
TBScintRec = TBScintillatorRec("TBScintRec")
TBScintRec.SGkey="ScintillatorRawCont"
topSequence += TBScintRec
include( "TBRec/H8ScintRec_jobOptions.py" ) 
#theApp.TopAlg += ["TBBPCRec"]
#TBBPCRec =  Algorithm("TBBPCRec")
from TBRec.TBRecConf import TBBPCRec
TBBPCRec = TBBPCRec()
TBBPCRec.SGkey="BPCRawCont"
include( "TBRec/H8BPCRec_jobOptions.py" )
TBBPCRec.CalibFileName = "H8BPCCalib.txt"
topSequence += TBBPCRec
#theApp.TopAlg += ["TBPhaseRec/TBPhaseRec"]
#TBPhaseRec =  Algorithm("TBPhaseRec")
from TBRec.TBRecConf import TBPhaseRec
TBPhaseRec = TBPhaseRec()
include( "TBRec/H8PhaseRec_jobOptions.py" )
topSequence += TBPhaseRec

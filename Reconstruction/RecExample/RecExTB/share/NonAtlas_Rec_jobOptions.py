theApp.Dlls += [ "TBRec" ]

theApp.TopAlg += ["TBBPCRec/TBBPCRec"]
TBBPCRec =  Algorithm("TBBPCRec")
include( "TBRec/H8BPCRec_jobOptions.py" )

theApp.TopAlg += ["TBScintillatorRec"]
TBScintRec =  Algorithm("TBScintillatorRec")
include( "TBRec/H8ScintRec_jobOptions.py" )

#CBNT_Athena.Members += [ "CBNT_BeamInstrumentation" ]
#CBNT_BeamInstrumentation = Algorithm( "CBNT_BeamInstrumentation" )
from TBRec.TBRecConf import CBNTAA_BeamInstrumentation
CBNT_BeamInstrumentation = CBNTAA_BeamInstrumentation()
CBNT_BeamInstrumentation.BPCnames=["BPC-2","BPC-1","BPC0","BPC1","BPC2"];
CBNT_BeamInstrumentation.Scintnames=["S0","S1","S2-Up","S2-Down","S3-Left","S3-Right","C1","C2","muTag","muHalo","muVeto","SC1","SC2","TRTSci"];
CBNT_BeamInstrumentation.NeverReturnFailure = TRUE
CBNT_AthenaAware += CBNT_BeamInstrumentation


from TBRec.TBRecConf import CBNTAA_TBTDCRaw
CBNTAA_TBTDCRaw = CBNTAA_TBTDCRaw()
CBNTAA_TBTDCRaw.NeverReturnFailure = TRUE
CBNT_AthenaAware += CBNTAA_TBTDCRaw


from TBRec.TBRecConf import CBNTAA_TBADCRaw
CBNTAA_TBADCRaw = CBNTAA_TBADCRaw()
CBNTAA_TBADCRaw.NeverReturnFailure = TRUE
CBNT_AthenaAware += CBNTAA_TBADCRaw

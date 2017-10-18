from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
topSequence = AlgSequence()

from AthenaCommon.CFElements import parOR, seqAND

TopHLTSeq = seqAND("TopHLTSeq")
topSequence += TopHLTSeq




L1UnpackingSeq = parOR("L1UnpackingSeq")
from L1Decoder.L1DecoderConf import CTPUnpackingEmulationTool, RoIsUnpackingEmulationTool, L1Decoder
l1Decoder = L1Decoder( OutputLevel=DEBUG, RoIBResult="" )

ctpUnpacker = CTPUnpackingEmulationTool( OutputLevel =  DEBUG, ForceEnableAllChains=False , InputFilename="ctp.dat" )
#ctpUnpacker.CTPToChainMapping = [ "0:HLT_g100",  "1:HLT_e20", "2:HLT_mu20", "3:HLT_2mu8", "3:HLT_mu8", "33:HLT_2mu8", "15:HLT_mu8_e8" ]
l1Decoder.ctpUnpacker = ctpUnpacker

emUnpacker = RoIsUnpackingEmulationTool("EMRoIsUnpackingTool", OutputLevel=DEBUG, InputFilename="l1emroi.dat", OutputTrigRoIs="L1EMRoIs", Decisions="L1EM" )
emUnpacker.ThresholdToChainMapping = ["EM7 : HLT_mu8_e8", "EM20 : HLT_e20", "EM50 : HLT_2g50",   "EM100 : HLT_g100" ]

muUnpacker = RoIsUnpackingEmulationTool("MURoIsUnpackingTool", OutputLevel=DEBUG, InputFilename="l1muroi.dat",  OutputTrigRoIs="L1MURoIs", Decisions="L1MU" )
muUnpacker.ThresholdToChainMapping = ["MU6 : HLT_mu6", "MU8 : HLT_mu8", "MU8 : HLT_2mu8",  "MU8 : HLT_mu8_e8",  "MU10 : HLT_mu20",   "EM100 : HLT_g100" ]

l1Decoder.roiUnpackers = [emUnpacker, muUnpacker]

#print l1Decoder
L1UnpackingSeq += l1Decoder

TopHLTSeq += L1UnpackingSeq

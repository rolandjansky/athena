include("TrigUpgradeTest/testHLT_MT.py")


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


topSequence.L1DecoderTest.ctpUnpacker.OutputLevel=DEBUG
for unpack in topSequence.L1DecoderTest.roiUnpackers:
    unpack.OutputLevel=DEBUG
    print unpack
    
    
#topSequence.L1DecoderTest.roiUnpackers[3].OutputLevel=DEBUG
#topSequence.L1DecoderTest.OutputFSTrigRoI="FSRoI"
#for unpack in topSequence.L1DecoderTest.roiUnpackers:
#    if unpack.name() is "JRoIsUnpackingTool":
#        unpack.Decisions="L1JET"
#        unpack.FSRoIKey = "FSRoI"
# this is empty        unpack.OutputTrigRoIs= "JETRoIs"


CTPToChainMapping = {"HLT_j85":       "L1_J20"   }
testChains =[x for x, y in CTPToChainMapping.items()]
topSequence.L1DecoderTest.ChainToCTPMapping = CTPToChainMapping
print testChains
print topSequence.L1DecoderTest

#from TrigUpgradeTest.jetDefs import jetRecoSequence
#(recoSequence, sequenceOut) = jetRecoSequence("FSRoI")
 
#topSequence += recoSequence
    

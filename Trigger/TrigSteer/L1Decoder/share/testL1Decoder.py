include("TrigUpgradeTest/testHLT_MT.py")


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


topSequence.L1Decoder.ctpUnpacker.OutputLevel=DEBUG
for unpack in topSequence.L1Decoder.roiUnpackers:
    unpack.OutputLevel=DEBUG
    print unpack
    
  

CTPToChainMapping = {"HLT_j85":       "L1_J20"   }
testChains =[x for x, y in CTPToChainMapping.items()]
topSequence.L1Decoder.ChainToCTPMapping = CTPToChainMapping
print testChains
print topSequence.L1Decoder

#from TrigUpgradeTest.jetDefs import jetRecoSequence
#(recoSequence, sequenceOut) = jetRecoSequence("FSRoI") 
#topSequence += recoSequence
    

include("TrigUpgradeTest/testHLT_MT.py")


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


topSequence.L1Decoder.ctpUnpacker.OutputLevel=DEBUG
for unpack in topSequence.L1Decoder.roiUnpackers:
    unpack.OutputLevel=DEBUG
    print unpack
    

# from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
# from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp
# c = ChainProp( name="HLT_j85_L1J20" )
# from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
# from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain

# chainConfig = Chain( name="HLT_j85_L1J20", L1Thresholds=['J20'], ChainSteps=[] )
# TriggerConfigHLT.registerChain( dictFromChainName( c ), chainConfig )

from TriggerMenuMT.HLTMenuConfig.Menu.HLTMenuJSON import generateJSON
generateJSON()

print topSequence.L1Decoder

#from TrigUpgradeTest.jetDefs import jetRecoSequence
#(recoSequence, sequenceOut) = jetRecoSequence("FSRoI") 
#topSequence += recoSequence

from TriggerJobOpts.TriggerFlags import TriggerFlags    
hltJsonFile = TriggerFlags.inputHLTconfigFile().replace(".xml",".json").replace("HLTconfig","HLTmenu")
from TrigConfigSvc.TrigConfigSvcConfig import HLTConfigSvc, findFileInXMLPATH
hltJsonFile = findFileInXMLPATH(hltJsonFile)
svcMgr += HLTConfigSvc()
svcMgr.HLTConfigSvc.JsonFileName = hltJsonFile

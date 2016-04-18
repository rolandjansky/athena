
# L1 menu
include('TrigSteering/pureSteering_l1menu.py')

from TriggerMenu.menu.TriggerPythonConfig import TriggerPythonConfig
from TriggerMenu.menu.HLTObjects import HLTChain

menu = TriggerPythonConfig("pureSteering_menu.xml")

defaultOutputLevel = VERBOSE

from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo, PESA__dummyHypo, PESA__dummyAlgoHLTErrorCode

# L2
# examples below show all the posibilities of configuring Sequences
Fex1 = PESA__dummyAlgo('Fex1')                                     
Fex2 = PESA__dummyAlgo('Fex2')
eg1 =  PESA__dummyAlgoHLTErrorCode('eg1')
eg1.HLTErrorCode = "ABORT_CHAIN ALGO_ERROR  MISSING_ROD"
eg1.OutputLevel=defaultOutputLevel

eg2 =  PESA__dummyAlgoHLTErrorCode('eg2')
eg2.HLTErrorCode = "ABORT_CHAIN  ALGO_ERROR MISSING_ROD"
eg2.ErrorCodeMap = { "ABORT_CHAIN  ALGO_ERROR MISSING_ROD" : "CONTINUE UNKNOWN MISSING_ROD" }
eg2.OutputLevel=defaultOutputLevel

eg3 =  PESA__dummyAlgoHLTErrorCode('eg3')
eg3.HLTErrorCode = "ABORT_CHAIN  ALGO_ERROR CORRUPTED_ROD"
eg3.ErrorCodeMap = { "ABORT_CHAIN ALGO_ERROR  MISSING_ROD" : "CONTINUE UNKNOWN MISSING_ROD",  "ABORT_CHAIN  ALGO_ERROR NAV_ERROR" : "CONTINUE UNKNOWN NAV_ERROR"}
eg3.OutputLevel=defaultOutputLevel


eg4 =  PESA__dummyAlgoHLTErrorCode('eg4')
eg4.HLTErrorCode = "ABORT_CHAIN  ALGO_ERROR NAV_ERROR"
eg4.OutputLevel=defaultOutputLevel


eg5 =  PESA__dummyAlgoHLTErrorCode('eg5')
eg5.HLTErrorCode = "CONTINUE  ALGO_ERROR NAV_ERROR"
eg5.OutputLevel = defaultOutputLevel

# this error shoudl be masked at the steering level
eg6 =  PESA__dummyAlgoHLTErrorCode('eg6')
eg6.HLTErrorCode = "ABORT_CHAIN ALGO_ERROR GAUDI_EXCEPTION"
eg6.OutputLevel = defaultOutputLevel

# timeout test
from AthenaCommon.SystemOfUnits import *
eg7 =  PESA__dummyAlgoHLTErrorCode('eg7')
eg7.HLTErrorCode = "CONTINUE UNKNOWN UNKNOWN"
eg7.BurnTimeAndCheckTimeout = 1. * Units.second
eg7.OutputLevel = defaultOutputLevel

menu.addSequence("EM15i" , [Fex1, eg1], "em25unmapped" )
menu.addSequence("EM15i" , [Fex1, eg1], "em25unmapped2" )
menu.addSequence("EM25i" , [Fex1, eg2], "em25mapped" )
menu.addSequence("EM15i" , [Fex1, eg3, eg4], "em25badmap" )
menu.addSequence("em25mapped" , [eg4], "em25second" )
menu.addSequence("EM12i", [eg5], "em25small_problem")
menu.addSequence("EM12i" , [Fex1, eg6], "em25steeringmasked" )
menu.addSequence("EM12i" , [Fex1, eg7], "em25timeout" )


from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig, ErrorCodeMonitoringToolConfig

ecmon = ErrorCodeMonitoringToolConfig("ECmon")
ecmon.monitorUnmappedCode()
ecmon.monitorActionFixups()
ecmon.renameUserCodes(u1="WrongPhiRange", u4="PTunphysiacl")




eg1.AthenaMonTools += [ecmon]
eg2.AthenaMonTools += [ecmon]
eg3.AthenaMonTools += [ecmon]
eg4.AthenaMonTools += [ecmon]


# chains

# L2

chain = HLTChain( chain_name="L2_e25_unmapped", chain_counter="1", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="1")
chain.addHLTSignature( "em25unmapped" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_mapped", chain_counter="2", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="1")
chain.addHLTSignature( "em25mapped" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25_badmap", chain_counter="3", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="1")
chain.addHLTSignature( "em25badmap" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25_second_unmapped", chain_counter="4", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="1")
chain.addHLTSignature( "em25unmapped" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25_two_steps", chain_counter="5", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="1")
chain.addHLTSignature( "em25mapped" ).addHLTSignature("em25second")
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_small_problem", chain_counter="6", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="1")
chain.addHLTSignature( "em25small_problem" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
menu.addHLTChain(chain)


#this chain is to test that algothm which is gives an erros cashes the error result and  this chain has teh same err as orignal L2_e25_unmapped
chain = HLTChain( chain_name="L2_e25_unmapped2", chain_counter="7", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="1")
chain.addHLTSignature( "em25unmapped2" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25_steeringmasked", chain_counter="8", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( "em25steeringmasked" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_timeout", chain_counter="9", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( "em25timeout" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
menu.addHLTChain(chain)


menu.writeConfigFiles();
menu.dot(algs=True)


#this command helps understanding error propagation
#  egrep "start proce|Most severe|mapping| EC |with error code" log  | egrep -v "Holder"

# see expert-monitoring.root to see the moni histograms

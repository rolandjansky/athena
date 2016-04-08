from TriggerMenuPython.TriggerPythonConfig import *

menu = TriggerPythonConfig("pureSteering_menu.xml", 'l1.xml' )

defaultOutputLevel = VERBOSE

# L1 necessary

# menu.addEMThreshold("TM10", value="10", emiso='10', hadiso='11' )
# TE, XE
thr_te10 = menu.registerLvl1Threshold('TE10', 'TE', mapping=0, slot='SLOT8', connector='CON2')\
           .addEnergyThresholdValue(10, -49, 49, 0, 64)


# menu.addItem('EM15i', prescale='1', ctpid='2')
# menu.addEMThreshold('EM15i', value='15', emiso='10', hadiso='11' )

thr_em15i = menu.registerLvl1Threshold(name='EM15i', type='EM', mapping=3, slot='SLOT7', connector='CON1')\
            .addEMThresholdValue(13, -49, 49, 0, 64, 4, 3, 2)

item_em15i = LVL1MenuItem('L1_EM15i', ctpid=0x3).setLogic(thr_em15i.condition(1))
item_2em15i = LVL1MenuItem('2EM15i', ctpid=0xc).setLogic(thr_em15i.condition(2))

thr_em25i = menu.registerLvl1Threshold(name='EM25i', type='EM', mapping=6, slot='SLOT7', connector='CON1')\
            .addEMThresholdValue(25, -49, 49, 0, 64, 4, 3, 2)
item_em25i = LVL1MenuItem('EM25i', ctpid=0x6).setLogic(thr_em25i.condition(1))

thr_mu6  = menu.registerLvl1Threshold(name='MU06', type='MUON', mapping=1, slot='SLOT9', connector='CON0')\
           .addMuonThresholdValue(5, -49, 49, 0, 64)
item_mu6  = LVL1MenuItem('MU06',  ctpid=0x21).setLogic(thr_mu6.condition(1))

item_2mu6  = LVL1MenuItem('2MU06',  ctpid=0x27).setLogic(thr_mu6.condition(2))

thr_mu20 = menu.registerLvl1Threshold(name='MU20', type='MUON', mapping=4, slot='SLOT9', connector='CON0')\
           .addMuonThresholdValue(20, -49, 49, 0, 64)
item_mu20 = LVL1MenuItem('MU20', ctpid=0x24).setLogic(thr_mu20.condition(1))

thr_j200 = menu.registerLvl1Threshold('J200', 'JET', mapping=7, slot='SLOT8', connector='CON0')\
                   .addJetThresholdValue(100, -49, 49, 0, 64, 8)

item_j200 = LVL1MenuItem('J200', ctpid=0x67, group='1', prescale=1).setLogic(thr_j200.condition(1))

thr_j90 = menu.registerLvl1Threshold('J90', 'JET', mapping=6, slot='SLOT8', connector='CON0')\
          .addJetThresholdValue(90, -49, 49, 0, 64, 8)

item_3j90 = LVL1MenuItem('3J90', ctpid=0x6a, group='1', prescale=1).setLogic(thr_j90.condition(3))

thr_j60 = menu.registerLvl1Threshold('J60', 'JET', mapping=5, slot='SLOT8', connector='CON0')\
          .addJetThresholdValue(60, -49, 49, 0, 64, 8)
item_j60  = LVL1MenuItem('J60',  ctpid=0x65, group='1', prescale=50).setLogic(thr_j60.condition(1))

thr_j50 = menu.registerLvl1Threshold('J50', 'JET', mapping=4, slot='SLOT8', connector='CON0')\
          .addJetThresholdValue(50, -49, 49, 0, 64, 8)
item_j50  = LVL1MenuItem('J50',  ctpid=0x64, group='1', prescale=500).setLogic(thr_j50.condition(1))

thr_tau25 = menu.registerLvl1Threshold('HA25', 'TAU', mapping=5, slot='SLOT7', connector='CON2')\
            .addTauThresholdValue(25, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)

item_tau25 = LVL1MenuItem('TAU25', ctpid=0x45).setLogic(thr_tau25.condition(1))


thr_xe30 = menu.registerLvl1Threshold('XE30', 'XE', mapping=5, slot='SLOT8', connector='CON2')\
           .addEnergyThresholdValue(30, -49, 49, 0, 64)

item_xe30 = LVL1MenuItem('XE30', ctpid=0x83, group='1', prescale=256).setLogic(thr_xe30.condition(1))

item_tau25_xe30 = LVL1MenuItem('TAU25+XE30', ctpid=0xd8)\
                  .setLogic(thr_tau25.condition(1)&thr_xe30.condition(1))

thr_xe60 = menu.registerLvl1Threshold('XE60', 'XE', mapping=6, slot='SLOT8', connector='CON2')\
           .addEnergyThresholdValue(60, -49, 49, 0, 64)

thr_tau50 = menu.registerLvl1Threshold('HA50', 'TAU', mapping=7, slot='SLOT7', connector='CON2')\
            .addTauThresholdValue(50, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)

item_j50_xe60 = LVL1MenuItem('J50+XE60', ctpid=0xd9)\
                   .setLogic(thr_j50.condition(1)&thr_xe60.condition(1))

item_xe60 = LVL1MenuItem('XE60', ctpid=0x86, group='1', prescale=1).setLogic(thr_xe60.condition(1))


for a in dir():
    if a.startswith('item_'):
        eval("menu.addLvl1Item( %s)" % (a))







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

chain = HLTChain( chain_name="L2_e25_unmapped", chain_counter="1", lower_chain_name="EM25i", level="L2", prescale="1", pass_through="1")
chain.addHLTSignature( "em25unmapped" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_mapped", chain_counter="2", lower_chain_name="EM25i", level="L2", prescale="1", pass_through="1")
chain.addHLTSignature( "em25mapped" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25_badmap", chain_counter="3", lower_chain_name="EM25i", level="L2", prescale="1", pass_through="1")
chain.addHLTSignature( "em25badmap" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25_second_unmapped", chain_counter="4", lower_chain_name="EM25i", level="L2", prescale="1", pass_through="1")
chain.addHLTSignature( "em25unmapped" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25_two_steps", chain_counter="5", lower_chain_name="EM25i", level="L2", prescale="1", pass_through="1")
chain.addHLTSignature( "em25mapped" ).addHLTSignature("em25second")
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_small_problem", chain_counter="6", lower_chain_name="EM25i", level="L2", prescale="1", pass_through="1")
chain.addHLTSignature( "em25small_problem" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
menu.addHLTChain(chain)


#this chain is to test that algothm which is gives an erros cashes the error result and  this chain has teh same err as orignal L2_e25_unmapped
chain = HLTChain( chain_name="L2_e25_unmapped2", chain_counter="7", lower_chain_name="EM25i", level="L2", prescale="1", pass_through="1")
chain.addHLTSignature( "em25unmapped2" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25_steeringmasked", chain_counter="8", lower_chain_name="EM25i", level="L2", prescale="1", pass_through="0")
chain.addHLTSignature( "em25steeringmasked" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_timeout", chain_counter="9", lower_chain_name="EM25i", level="L2", prescale="1", pass_through="0")
chain.addHLTSignature( "em25timeout" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
menu.addHLTChain(chain)


menu.writeConfigFiles();
menu.dot(algs=True)


#this command helps understanding error propagation
#  egrep "start proce|Most severe|mapping| EC |with error code" log  | egrep -v "Holder"

# see expert-monitoring.root to see the moni histograms

from TriggerMenuPython.TriggerPythonConfig import *

menu = TriggerPythonConfig('pureSteering_menu.xml', 'l1.xml' )



# there seems to be 10 cases to be tested (X signifies prescaled item/chain, 0 means unprescalled item/chain, 
#L1 X X X X 0 0 0 
#L2 O X X O X 0 X X 0 X 
#EF 0 0 X X 0 X X 0 X X

# to complete the test we need 2 items which we can put on/off as needed in LVL11 seeds file





# L1 necessary
thr_em3 = menu.registerLvl1Threshold(name='EM3', type='EM', mapping=2, slot='SLOT7', connector='CON1')\
            .addEMThresholdValue(3, -49, 49, 0, 64, 1, 3, 2)

item_em3 = LVL1MenuItem('L1_EM3', ctpid=0x2).setLogic(thr_em3.condition(1))


thr_em15i = menu.registerLvl1Threshold(name='EM15i', type='EM', mapping=3, slot='SLOT7', connector='CON1')\
            .addEMThresholdValue(13, -49, 49, 0, 64, 4, 3, 2)

item_em15i = LVL1MenuItem('L1_EM15i', ctpid=0x3).setLogic(thr_em15i.condition(1))


thr_em25i = menu.registerLvl1Threshold(name='EM25i', type='EM', mapping=6, slot='SLOT7', connector='CON1')\
            .addEMThresholdValue(25, -49, 49, 0, 64, 4, 3, 2)
item_em25i = LVL1MenuItem('L1_EM25i', ctpid=0x6).setLogic(thr_em25i.condition(1))


for a in dir():
    if a.startswith('item_'):
        eval("menu.addLvl1Item( %s)" % (a))







from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo, PESA__dummyHypo, PESA__newDummyAlgo2To1, PESA__newDummyAlgoAllTEs, PESA__dummyAlgoHLTErrorCode, PESA__dummyMonitoredFex, PESA__dummyAlgoForCalibration

# L2
# examples below show all the posibilities of configuring Sequences
Egamma = PESA__dummyAlgo('Egamma_L2')                                     # create configurable instance
Egamma2 = PESA__dummyAlgo('Egamma2_L2')                                   # create configurable another instance
EgammaAdv3 = PESA__dummyAlgo("EgammaAdv3")
EgammaAdv_L2 = PESA__dummyAlgo("EgammaAdv_L2")
menu.addSequence("EM15i" , [ Egamma, Egamma2, EgammaAdv3 ] , "L2_em15i" )  # use bot here, mixing with old fassioned way
menu.addSequence("L2_em15i" , EgammaAdv_L2, "L2_em15i'" )    # create configurable on the fly

defalgo = PESA__dummyAlgo('Em25')                                               # no arg given ...default name

menu.addSequence("EM25i" , defalgo, "L2_em25i" )

EgammaAdv_g2 = PESA__dummyAlgo("EgammaAdv_g2")
menu.addSequence("L2_em25i" ,EgammaAdv_g2 ,"L2_em25i'" )





# EF

gamma_EF = PESA__dummyAlgo("gamma_EF")
gammaAdv_EF = PESA__dummyAlgo("gammaAdv_EF")
menu.addSequence("L2_em15i'", gamma_EF, "EF_g15" )
menu.addSequence("EF_g15", gammaAdv_EF, "EF_g15'" )

electron_EF = PESA__dummyAlgo("electron_EF")
electronAdv_EF = PESA__dummyAlgo("electronAdv_EF")
menu.addSequence("L2_em15i'", electron_EF , "EF_e15i" )
menu.addSequence("EF_e15i", electronAdv_EF , "EF_e15i'" )


menu.addSequence("L2_em25i'", gamma_EF , "EF_g25" )
menu.addSequence("EF_g25", gammaAdv_EF , "EF_g25'" )

menu.addSequence("L2_em25i'", electron_EF , "EF_e25i" )
menu.addSequence("EF_e25i", electronAdv_EF , "EF_e25i'" )


# chains
# passage based on EM3
chain = HLTChain( chain_name="L2_e3", chain_counter="100", lower_chain_name="L1_EM3", level="L2", prescale="1", pass_through="0")
chain.addHLTSignature( "L2_em25i" ).addHLTSignature( "L2_em25i'" )
chain.addHLTSignature( "L2_em15i" ).addHLTSignature( "L2_em15i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons")

menu.addHLTChain(chain)

chain = HLTChain( chain_name="EF_e3", chain_counter="100", lower_chain_name="L2_e3", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1')
chain.addGroup("electrons")

menu.addHLTChain(chain)




# unprescalled seeded from prescalled item
chain = HLTChain( chain_name="L2_e25", chain_counter="1", lower_chain_name="L1_EM25i", level="L2", prescale="1", pass_through="0")
chain.addHLTSignature( "L2_em25i" ).addHLTSignature( "L2_em25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons")

menu.addHLTChain(chain)

chain = HLTChain( chain_name="EF_e25i", chain_counter="1", lower_chain_name="L2_e25", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1')
chain.addGroup("electrons")

menu.addHLTChain(chain)


# seeded from prescalled items & L2 chain is prescalled
chain = HLTChain( chain_name="L2_e15_prescalled", chain_counter="2", lower_chain_name="L1_EM15i", level="L2", prescale="2", pass_through="0")
chain.addHLTSignature( "L2_em25i" ).addHLTSignature( "L2_em25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons")

menu.addHLTChain(chain)



chain = HLTChain( chain_name="EF_e15_on_prescalled", chain_counter="2", lower_chain_name="L2_e15_prescalled", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1')
chain.addGroup("electrons")

menu.addHLTChain(chain)



# seeded from prescalled items & L2 & EF chains are prescalled
chain = HLTChain( chain_name="EF_e15_prescalled", chain_counter="3", lower_chain_name="L2_e15_prescalled", level="EF", prescale="2", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1')
chain.addGroup("electrons")

menu.addHLTChain(chain)



# seeded from prescalled items & L2 & EF chains are prescalled
chain = HLTChain( chain_name="EF_e25_prescalled_on_unprescalled", chain_counter="4", lower_chain_name="L2_e25", level="EF", prescale="2", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1')
chain.addGroup("electrons")

menu.addHLTChain(chain)



# seeded from prescalled items & L2 & EF chains are prescalled
chain = HLTChain( chain_name="L2_e15_pass", chain_counter="5", lower_chain_name="L1_EM15i", level="L2", prescale="1", pass_through="1")
chain.addHLTSignature( "L2_em25i" ).addHLTSignature( "L2_em25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons")

menu.addHLTChain(chain)



chain = HLTChain( chain_name="EF_e15_on_pass", chain_counter="5", lower_chain_name="L2_e15_pass", level="EF", prescale="1", pass_through="1")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1')
chain.addGroup("electrons")

menu.addHLTChain(chain)



# unseeded & prescalled

chain = HLTChain( chain_name="L2_unseeded", chain_counter="11", lower_chain_name="", level="L2", prescale="3", pass_through="0")
chain.addTriggerTypeBit('11')
chain.addStreamTag('blaStream', prescale='1')
chain.addGroup("fakes")

menu.addHLTChain(chain)





chain = HLTChain( chain_name="EF_on_unseeded", chain_counter="11", lower_chain_name="L2_unseeded", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addStreamTag('blaStream', prescale='1')
chain.addGroup("fakes")

menu.addHLTChain(chain)




# EF

chain = HLTChain( chain_name="EF_unseeded", chain_counter="12", lower_chain_name="", level="EF", prescale="2", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1')
chain.addGroup("electrons")

menu.addHLTChain(chain)







menu.writeConfigFiles();
menu.dot(algs=True)

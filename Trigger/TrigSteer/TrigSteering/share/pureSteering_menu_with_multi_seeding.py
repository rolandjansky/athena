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

item_em3 = LVL1MenuItem('L1_MU6', ctpid=0x2).setLogic(thr_em3.condition(1))


thr_em25i = menu.registerLvl1Threshold(name='EM25i', type='EM', mapping=6, slot='SLOT7', connector='CON1')\
            .addEMThresholdValue(25, -49, 49, 0, 64, 4, 3, 2)
item_em25i = LVL1MenuItem('L1_EM25i', ctpid=0x6).setLogic(thr_em25i.condition(1))


for a in dir():
    if a.startswith('item_'):
        eval("menu.addLvl1Item( %s)" % (a))


from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo, PESA__dummyHypo, PESA__newDummyAlgo2To1, PESA__newDummyAlgoAllTEs, PESA__dummyAlgoHLTErrorCode, PESA__dummyMonitoredFex, PESA__dummyAlgoForCalibration
Egamma = PESA__dummyAlgo('Egamma_L2')                                     # create configurable instance
Egamma2 = PESA__dummyAlgo('Egamma2_L2')                                   # create configurable another instance
EgammaAdv3 = PESA__dummyAlgo("EgammaAdv3")
EgammaAdv_L2 = PESA__dummyAlgo("EgammaAdv_L2")
menu.addSequence("EM15i" , [ Egamma, Egamma2, EgammaAdv3 ] , "em15i" )  # use bot here, mixing with old fassioned way
menu.addSequence("em15i" , EgammaAdv_L2, "em15i'" )    # create configurable on the fly





# chains
# passage based on EM3
chain = HLTChain( chain_name="L2_Ored", chain_counter="100", lower_chain_name="L1_MU6,L1_EM25i", level="L2", prescale="1", pass_through="0")
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons")

menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25i", chain_counter="101", lower_chain_name="L1_EM25i", level="L2", prescale="1", pass_through="0")
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons")

menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_mu6", chain_counter="102", lower_chain_name="L1_MU6", level="L2", prescale="1", pass_through="0")
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons")

menu.addHLTChain(chain)





menu.writeConfigFiles();
menu.dot(algs=True)

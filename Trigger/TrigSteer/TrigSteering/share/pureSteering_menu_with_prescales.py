
# L1 menu
include('TrigSteering/pureSteering_l1menu.py')

from TriggerMenu.menu.TriggerPythonConfig import TriggerPythonConfig
from TriggerMenu.menu.HLTObjects import HLTChain

menu = TriggerPythonConfig('pureSteering_menu.xml')


from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo, PESA__dummyHypo, PESA__newDummyAlgo2To1, PESA__newDummyAlgoAllTEs, PESA__dummyAlgoHLTErrorCode, PESA__dummyMonitoredFex, PESA__dummyAlgoForCalibration

# L2
# examples below show all the posibilities of configuring Sequences
defalgo = PESA__dummyAlgo('Em25')                                               # no arg given ...default name

menu.addSequence("EM25i" , defalgo, "em25i" )

EgammaAdv_g2 = PESA__dummyAlgo("EgammaAdv_g2")
menu.addSequence("em25i" ,EgammaAdv_g2 ,"em25i'" )
EgammaAdv_g3 = PESA__dummyAlgo("EgammaAdv_g3")
EverRecjectingHypo = PESA__dummyHypo("g25fail")
EverRecjectingHypo.PreScale=1000000
menu.addSequence("em25i" ,[EgammaAdv_g3, EverRecjectingHypo ] ,"L2_e25fail" )

# EF 
EFeg1 = PESA__dummyAlgo("EFeg1")
EFeg2 = PESA__dummyAlgo("EFeg2")
menu.addSequence("em25i'" ,EFeg1 ,"EF_e25i" )
menu.addSequence("EF_e25i" ,EFeg2 ,"EF_e25i'" )
EverRecjectingEFHypo = PESA__dummyHypo("efg25fail")
EverRecjectingEFHypo.PreScale=1000000
menu.addSequence("EF_e25i" ,[EFeg2, EverRecjectingEFHypo] ,"EF_e25fail" )


# chains

# L2
chain = HLTChain( chain_name="L2_e25", chain_counter="1", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="1")
chain.addHLTSignature( "em25i" ).addHLTSignature( "em25i'" )
chain.addHLTSignature( "EF_e25i" ).addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons_unp")
chain.addGroup("electrons")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_pres2", chain_counter="2", lower_chain_name="L1_EM25i", level="HLT", prescale="2", pass_through="0", rerun_prescale="1")
chain.addHLTSignature( "em25i" ).addHLTSignature( "em25i'" )
chain.addHLTSignature( "EF_e25i" ).addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons_pres")
chain.addGroup("electrons")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25_pt", chain_counter="3", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="1")
chain.addHLTSignature( "em25i" ).addHLTSignature("em25i'").addHLTSignature( "L2_e25fail" )
chain.addHLTSignature( "EF_e25i" ).addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addStreamTag('express', type='express', prescale='2')
chain.addGroup("electrons_pt")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25_pt2", chain_counter="4", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="2")
chain.addHLTSignature( "em25i" ).addHLTSignature("em25i'").addHLTSignature( "L2_e25fail" )
chain.addHLTSignature( "EF_e25i" ).addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons_pt2")
chain.addGroup("electrons")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_pt2_pres2", chain_counter="5", lower_chain_name="L1_EM25i", level="HLT", prescale="2", pass_through="2")
chain.addHLTSignature( "em25i" ).addHLTSignature( "em25i'" )
chain.addHLTSignature( "EF_e25i" ).addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons_tricky")
chain.addGroup("electrons")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_pres1.5", chain_counter="6", lower_chain_name="L1_EM25i", level="HLT", prescale="1.5", pass_through="0")
chain.addHLTSignature( "em25i" ).addHLTSignature( "em25i'" )
chain.addHLTSignature( "EF_e25i" ).addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons_float")
chain.addGroup("electrons")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_pres_inf", chain_counter="7", lower_chain_name="L1_EM25i", level="HLT", prescale="0", pass_through="0")
chain.addHLTSignature( "em25i" ).addHLTSignature( "em25i'" )
chain.addHLTSignature( "EF_e25i" ).addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons_inf")
chain.addGroup("electrons")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25_disabled_by_negative_prescale", chain_counter="8", lower_chain_name="L1_EM25i", level="HLT", prescale="-1", pass_through="1", rerun_prescale="1")
chain.addHLTSignature( "em25i" ).addHLTSignature( "em25i'" )
chain.addHLTSignature( "EF_e25i" ).addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons_inf")
chain.addGroup("electrons")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25_run_but_not_contributing_to_stream", chain_counter="9", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="0", rerun_prescale="0")
chain.addHLTSignature( "em25i" ).addHLTSignature( "em25i'" )
chain.addHLTSignature( "EF_e25i" ).addHLTSignature( "EF_e25i'" )
chain.addStreamTag('exotic', prescale='0', obeyLB="0")
chain.addGroup("electrons")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25_prescaled_passthough", chain_counter="10", lower_chain_name="L1_EM25i", level="HLT", prescale="7", pass_through="2", rerun_prescale="0")
chain.addHLTSignature( "em25i" ).addHLTSignature( "em25i'" )
chain.addHLTSignature( "EF_e25i" ).addHLTSignature( "EF_e25i'" )
chain.addStreamTag('exotic', prescale='1', obeyLB="0")
chain.addGroup("electrons")
menu.addHLTChain(chain)



# coherence test. Idea of the test -- the chains should have pass factors as configured by the "prescale" params (2,5,10, -1)
# in addition the stream e25stream should show up only as many times as the chain of lowest prescale
# rate measurement per group (PRESCALEGROUP:em25) should be identical as L2_e25_coherent_prescale_0)
chain = HLTChain( chain_name="L2_e25_coherent_prescale_0", chain_counter="11", lower_chain_name="L1_EM25i", level="HLT", prescale="2", pass_through="0", rerun_prescale="0")
chain.addStreamTag('e25stream', prescale='1', obeyLB="0")
chain.addGroup("RATE:CPS:em25")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_coherent_prescale_1", chain_counter="12", lower_chain_name="L1_EM25i", level="HLT", prescale="2.5", pass_through="0", rerun_prescale="0")
chain.addStreamTag('e25stream', prescale='1', obeyLB="0")
chain.addGroup("RATE:CPS:em25")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_coherent_prescale_2", chain_counter="13", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="0", rerun_prescale="0")
chain.addStreamTag('e25stream', prescale='1', obeyLB="0")
chain.addGroup("RATE:CPS:em25")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_coherent_prescale_3", chain_counter="14", lower_chain_name="L1_EM25i", level="HLT", prescale="-1", pass_through="0", rerun_prescale="0")
chain.addStreamTag('e25stream', prescale='1', obeyLB="0")
chain.addGroup("RATE:CPS:em25")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_coherent_prescale_4", chain_counter="15", lower_chain_name="L1_EM25i", level="HLT", prescale="0", pass_through="0", rerun_prescale="0")
chain.addStreamTag('e25stream', prescale='1', obeyLB="0")
chain.addGroup("RATE:CPS:em25")
menu.addHLTChain(chain)


# the same on unseeded chains (both have the same PS value)
chain = HLTChain( chain_name="L2_e25_coherent_prescale_5", chain_counter="16", lower_chain_name="", level="HLT", prescale="10", pass_through="0", rerun_prescale="0")
chain.addStreamTag('calib', prescale='1', obeyLB="0")
chain.addGroup("RATE:CPS:uns")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_coherent_prescale_6", chain_counter="17", lower_chain_name="", level="HLT", prescale="10", pass_through="0", rerun_prescale="0")
chain.addStreamTag('calib', prescale='1', obeyLB="0")
chain.addGroup("RATE:CPS:uns")
menu.addHLTChain(chain)




menu.writeConfigFiles();
menu.dot(algs=True)

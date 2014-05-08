from TriggerMenuPython.TriggerPythonConfig import *

menu = TriggerPythonConfig('pureNewSteering_menu.xml', 'l1.xml' )



# L1 necessary
menu.addItem('EM15i', prescale='1', ctpid='2')
menu.addEMThreshold('EM15i', value='15', emiso='10', hadiso='11' )

item = LVL1MenuItem('2EM15i', prescale='1', ctpid='2')
item.addAndedCondition(name='EM15i', multi='2')
menu.addLVL1Item(item)

menu.addItem('EM25i',  prescale='50', ctpid='3', priority='HIGH')
menu.addEMThreshold('EM25i', value='20', emiso='10', etamin='-20', etamax='20' )

menu.addItem('MU06', prescale='0', ctpid='122')
menu.addMuonThreshold('MU06', value='6')

item = LVL1MenuItem('2MU06', prescale='1', ctpid='2')
item.addAndedCondition(name='MU06', multi='2')
menu.addLVL1Item(item)


menu.addItem('MU20', prescale='0', ctpid='123')
menu.addMuonThreshold('MU20', value='21')

menu.addItem('J200', prescale='0', ctpid='56')
menu.addJetThreshold('J200', value='200')

menu.addItem('J90', prescale='20', ctpid='56')
menu.addJetThreshold('J90', value='90')

item = LVL1MenuItem('3J90', prescale='1', ctpid='2')
item.addAndedCondition(name='MU06', multi='3')
menu.addLVL1Item(item)


menu.addItem('J65', prescale='30', ctpid='56')
menu.addJetThreshold('J65', value='60')

menu.addItem('J50', prescale='0', ctpid='56')
menu.addJetThreshold('J50', value='40')

menu.addItem('TAU25', prescale='0', ctpid='23')
menu.addTauThreshold('TAU25', value='20')

item = LVL1MenuItem('TAU25+XE30', prescale='1', ctpid='2')
item.addAndedCondition(name='TAU25', multi='1')
item.addAndedCondition(name='XE30', multi='1')
menu.addLVL1Item(item)

item = LVL1MenuItem('J50+XE60', prescale='1', ctpid='2')
item.addAndedCondition(name='J50', multi='1')
item.addAndedCondition(name='XE60', multi='1')
menu.addLVL1Item(item)

menu.addItem('XE30', prescale='0', ctpid='23')
menu.addEtMissThreshold('XE30', value='20')

menu.addItem('XE60', prescale='0', ctpid='23')
menu.addEtMissThreshold('XE60', value='50')


from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo, PESA__newDummyAlgo2To1, PESA__newDummyAlgoAllTEs

# L2
# examples below show all the posibilities of configuring Sequences
Egamma = PESA__dummyAlgo('Egamma_L2')                                     # create configurable instance
Egamma2 = PESA__dummyAlgo('Egamma2_L2')                                   # create configurable another instance

menu.addSequence("EM15i" , [ Egamma, Egamma2, "PESA::dummyAlgo/EgammaAdv3" ] , "em15i" )  # use bot here, mixing with old fassioned way
menu.addSequence("em15i" , PESA__dummyAlgo("EgammaAdv_L2"), "em15i'" )    # create configurable on the fly

defalgo = PESA__dummyAlgo('Em25')                                               # no arg given ...default name
menu.addSequence("EM25i" , defalgo, "em25i" )
menu.addSequence("em25i" ,"PESA::dummyAlgo/EgammaAdv/g10" ,"em25i'" )     # ols stryle again; with parameter

Muon = PESA__dummyAlgo('Muon')
Muon2 = PESA__dummyAlgo('Muon2')
menu.addSequence("MU20" , [Muon, Muon2 ] ,"mu20" )
menu.addSequence("mu20" ,"PESA::dummyAlgo/MuonAdv" ,"mu20'" )       # both are equivalent

menu.addSequence("MU06" ,Muon ,"mu6" )
menu.addSequence("mu6" , PESA__dummyAlgo('MuonAdv'),"mu6'" )        # see above, both are equivalent

menu.addSequence("J200" ,"PESA::dummyAlgo/Jet_L2" ,"j200" )
menu.addSequence("j200" ,"PESA::dummyAlgo/JetAdv_L2" ,"j200'" )

menu.addSequence("J90" ,"PESA::dummyAlgo/Jet_L2" ,"j90" )
menu.addSequence("j90" ,"PESA::dummyAlgo/JetAdv_L2" ,"j90'" )

menu.addSequence("J65" ,"PESA::dummyAlgo/Jet_L2" ,"j65" )
menu.addSequence("j65" ,"PESA::dummyAlgo/JetAdv_L2" ,"j65'" )

menu.addSequence("J50" ,"PESA::dummyAlgo/Jet_L2" ,"j50" )
menu.addSequence("j50" ,"PESA::dummyAlgo/JetAdv_L2" ,"j50'" )

menu.addSequence("TAU25" ,"PESA::dummyAlgo/Tau_L2" ,"tau25" )
menu.addSequence("tau25" ,"PESA::dummyAlgo/TauAdv_L2" ,"tau25'" )

menu.addSequence("XE30" ,"PESA::dummyAlgo/MissingEt_L2" ,"xE30" )
menu.addSequence("xE30" ,"PESA::dummyAlgo/MissingEt_L2" ,"xE30'" )

menu.addSequence("XE60" ,"PESA::dummyAlgo/MissingEt_L2" ,"xE60" )
menu.addSequence("xE60" ,"PESA::dummyAlgo/MissingEt_L2" ,"xE60'" )

ZFinder_L2 = PESA__newDummyAlgo2To1('ZFinder_L2')           # the same for other algo types
menu.addSequence(["em25i", "em25i"] ,ZFinder_L2 ,"Z" )

HiggsFinder_L2 = PESA__newDummyAlgo2To1('HtoZZ')
menu.addSequence(["Z", "Z"] ,HiggsFinder_L2 ,"Higgs" )

MissETRefiner_L2 = PESA__newDummyAlgoAllTEs("MissETRefiner_L2")
menu.addSequence(["MU20", "EM25i"] ,MissETRefiner_L2 ,"MET_L2" )


# EF
menu.addSequence("em15i'" ,"PESA::dummyAlgo/gamma_EF" ,"g15" )
menu.addSequence("g15" ,"PESA::dummyAlgo/gammaAdv_EF" ,"g15'" )

menu.addSequence("em15i'" ,"PESA::dummyAlgo/electron_EF" ,"e15i" )
menu.addSequence("e15i" ,"PESA::dummyAlgo/electronAdv_EF" ,"e15i'" )

menu.addSequence("em25i'" ,"PESA::dummyAlgo/gamma_EF" ,"g25" )
menu.addSequence("g25" ,"PESA::dummyAlgo/gammaAdv_EF" ,"g25'" )

menu.addSequence("em25i'" ,"PESA::dummyAlgo/electron_EF" ,"e25i" )
menu.addSequence("e25i" ,"PESA::dummyAlgo/electronAdv_EF" ,"e25i'" )

menu.addSequence("mu20'" ,"PESA::dummyAlgo/MuonMoore_EF" ,"muon20" )
menu.addSequence("muon20" , PESA__dummyAlgo("xuxMuon_EF"), "muon20'" )

menu.addSequence("mu6'" ,"PESA::dummyAlgo/MuonMoore_EF" ,"muon6" )
menu.addSequence("muon6" , PESA__dummyAlgo('MuonAdvEF1'),"muon6'" )

menu.addSequence("j200'" ,"PESA::dummyAlgo/JetKt" ,"jet200" )
menu.addSequence("jet200" ,"PESA::dummyAlgo/JetAdvKt" ,"jet200'" )

menu.addSequence("j90'" ,"PESA::dummyAlgo/JetKt" ,"jet90" )
menu.addSequence("jet90" ,"PESA::dummyAlgo/JetAdvKt" ,"jet90'" )

menu.addSequence("j65'" ,"PESA::dummyAlgo/JetKt" ,"jet65" )
menu.addSequence("jet65" ,"PESA::dummyAlgo/JetAdvKt" ,"jet65'" )

menu.addSequence("j50'" ,"PESA::dummyAlgo/JetKt" ,"jet50" )
menu.addSequence("jet50" ,"PESA::dummyAlgo/JetAdvKt" ,"jet50'" )

menu.addSequence("tau25'" ,"PESA::dummyAlgo/TauClu" ,"Tau25" )
menu.addSequence("Tau25" ,"PESA::dummyAlgo/TauAdvEF" ,"Tau25'" )

menu.addSequence("xE30'" ,"PESA::dummyAlgo/MissingEt" ,"missingE30" )
menu.addSequence("missingE30" ,"PESA::dummyAlgo/MissingEt" ,"missingE30'" )

menu.addSequence("xE60'" ,"PESA::dummyAlgo/MissingEt" ,"missingE60" )
menu.addSequence("missingE60" ,"PESA::dummyAlgo/MissingEt" ,"missingE60'" )



menu.addSequence(["e25i", "e25i"] ,"PESA::newDummyAlgo2To1/ZFinder_EF" ,"Z'" )

menu.addSequence(["Z'", "Z'"] ,"PESA::newDummyAlgo2To1/HtoZZ_EF" ,"Higgs'" )



# chains

# L2
chain = HLTChain( chain_name="em25i_L2", chain_counter="1", lower_chain_name="EM25i", level="L2", prescale="1", pass_through="0")
chain.addHLTSignature( "em25i" ).addHLTSignature( "em25i'" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1')
menu.addHLTChain(chain)


chain = HLTChain( chain_name="mu20_L2", chain_counter="2", lower_chain_name="MU20", level="L2", prescale="1", pass_through="0")
chain.addHLTSignature( "mu20" )
chain.addHLTSignature( "mu20'" )
chain.addTriggerTypeBit('10')
chain.addStreamTag('muons', prescale='1')
menu.addHLTChain(chain)


chain = HLTChain( chain_name="2em15i_L2", chain_counter="3", lower_chain_name="2EM15i", level="L2", prescale="1", pass_through="0")
chain.addHLTSignature( [ "em15i", "em15i" ] )
chain.addHLTSignature( [ "em15i'", "em15i'"] )
chain.addTriggerTypeBit('3')
chain.addStreamTag('electrons', prescale='1')
menu.addHLTChain(chain)


chain = HLTChain( chain_name="2mu6_L2", chain_counter="4", lower_chain_name="2MU06", level="L2", prescale="1", pass_through="0")
chain.addHLTSignature( "mu6" )
chain.addHLTSignature( "mu6'" )
chain.addTriggerTypeBit('9')
chain.addStreamTag('muons', prescale='1')
menu.addHLTChain(chain)





chain = HLTChain( chain_name="j200_L2", chain_counter="5", lower_chain_name="J200", level="L2", prescale="1", pass_through="0")
chain.addHLTSignature( "j200" )
chain.addHLTSignature( "j200'" )
chain.addTriggerTypeBit('24')
chain.addStreamTag('jet', prescale='1')
menu.addHLTChain(chain)


chain = HLTChain( chain_name="3j90_L2", chain_counter="6", lower_chain_name="3J90", level="L2", prescale="1", pass_through="0")
chain.addHLTSignature( ["j90", "j90", "j90"] )
chain.addHLTSignature( [ "j90'", "j90'", "j90'" ] )
chain.addTriggerTypeBit('28')
chain.addStreamTag('jet', prescale='1')
menu.addHLTChain(chain)





chain = HLTChain( chain_name="tau25_xE30_L2", chain_counter="7", lower_chain_name="TAU25+XE30", level="L2", prescale="1", pass_through="0")
chain.addHLTSignature( [ "tau25", "xE30" ] )
chain.addHLTSignature( [ "tau25'", "xE30'" ] )
chain.addTriggerTypeBit('48')
chain.addStreamTag('tau', prescale='1')
menu.addHLTChain(chain)





chain = HLTChain( chain_name="j50_xE60_L2", chain_counter="8", lower_chain_name="J50+XE60", level="L2", prescale="1", pass_through="0")
chain.addHLTSignature( [ "j50", "xE60" ] )
chain.addHLTSignature( [ "j50'", "xE60'" ] )
chain.addTriggerTypeBit('31')
chain.addStreamTag('missingET', prescale='1')
menu.addHLTChain(chain)





chain = HLTChain( chain_name="Z_L2", chain_counter="9", lower_chain_name="EM25i", level="L2", prescale="1", pass_through="0")
chain.addHLTSignature( ["em25i", "em25i" ])
chain.addHLTSignature( ["em25i'", "em25i'" ])
chain.addHLTSignature( "Z" )
chain.addStreamTag('electrons', prescale='1')
chain.addStreamTag('zee', type='calibration', prescale='1')
chain.addTriggerTypeBit('4')
menu.addHLTChain(chain)




chain = HLTChain( chain_name="H_L2", chain_counter="10", lower_chain_name="EM25i", level="L2", prescale="1", pass_through="0")
chain.addHLTSignature( [ "em25i", "em25i", "em25i", "em25i"] )
chain.addHLTSignature( [ "em25i'", "em25i'", "em25i'", "em25i'" ] )
chain.addHLTSignature( [ "Z", "Z" ] )
chain.addHLTSignature( "Higgs" )
chain.addStreamTag('higgs', prescale='1')
chain.addStreamTag('higgs', type='express', prescale='100')
chain.addTriggerTypeBit('4')
chain.addTriggerTypeBit('67')
menu.addHLTChain(chain)





chain = HLTChain( chain_name="empty_L2", chain_counter="11", lower_chain_name="", level="L2", prescale="1", pass_through="0")




chain = HLTChain( chain_name="MissingET_L2", chain_counter="12", lower_chain_name="", level="L2", prescale="1", pass_through="0")
chain.addHLTSignature( "MET_L2", sigcounter=2 )
chain.addTriggerTypeBit('36')
chain.addStreamTag('missingET', prescale='1')
#menu.addHLTChain(chain)




# EF
chain = HLTChain( chain_name="e25i_EF", chain_counter="1", lower_chain_name="em25i_L2", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( "e25i" )
chain.addHLTSignature( "e25i'" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1')
menu.addHLTChain(chain)




chain = HLTChain( chain_name="mu20_EF", chain_counter="2", lower_chain_name="mu20_L2", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( "muon20" )
chain.addHLTSignature( "muon20'" )
chain.addTriggerTypeBit('12')
chain.addStreamTag('muons', prescale='1')
menu.addHLTChain(chain)





chain = HLTChain( chain_name="2e15i_EF", chain_counter="3", lower_chain_name="2em15i_L2", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( [ "e15i", "e15i" ] )
chain.addHLTSignature( [ "e15i'","e15i'" ] )
chain.addTriggerTypeBit('5')
chain.addStreamTag('electrons', prescale='1')
menu.addHLTChain(chain)



chain = HLTChain( chain_name="2mu6_EF", chain_counter="4", lower_chain_name="2mu6_L2", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( [ "muon6", "muon6" ] )
chain.addHLTSignature( [ "muon6'", "muon6'" ] )
chain.addTriggerTypeBit('11')
chain.addStreamTag('emuons', prescale='1')
menu.addHLTChain(chain)





chain = HLTChain( chain_name="g25_EF", chain_counter="5", lower_chain_name="em25i_L2", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( "g25" ).addHLTSignature( "g25'" )
chain.addTriggerTypeBit('5')
chain.addStreamTag('electrons', prescale='1')
menu.addHLTChain(chain)


chain = HLTChain(chain_name="j200_EF", chain_counter="6", lower_chain_name="j200_L2", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( "jet200" )
chain.addHLTSignature( "jet200'" )
chain.addTriggerTypeBit('45')
chain.addStreamTag('jetss', prescale='1')
menu.addHLTChain(chain)




chain = HLTChain( chain_name="3j90_EF", chain_counter="7", lower_chain_name="3j90_L2", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( [ "jet90", "jet90", "jet90" ] )
chain.addHLTSignature( [ "jet90'", "jet90'", "jet90'" ] )
chain.addTriggerTypeBit('46')
chain.addStreamTag('jetss', prescale='1')
menu.addHLTChain(chain)





chain = HLTChain( chain_name="tau25_xE30_EF", chain_counter="8", lower_chain_name="tau25_xE30_L2", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( [ "Tau25", "missingE30" ] )
chain.addHLTSignature( [ "Tau25'", "missingE30'" ] )
chain.addTriggerTypeBit('36')
chain.addStreamTag('Bphys', prescale='1')
menu.addHLTChain(chain)



chain = HLTChain( chain_name="j50_xE60_EF", chain_counter="9", lower_chain_name="j50_xE60_L2", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( [ "jet50", "missingE60" ] )
chain.addHLTSignature( [ "jet50'", "missingE60'" ] )
chain.addTriggerTypeBit('37')
chain.addStreamTag('Bphys', prescale='1')
#menu.addHLTChain(chain)






chain = HLTChain( chain_name="Z_EF", chain_counter="10", lower_chain_name="Z_L2", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( [ "e25i","e25i" ] )
chain.addHLTSignature( [ "e25i'", "e25i'" ] )
chain.addHLTSignature( "Z'" )
chain.addStreamTag('higgs', prescale='1')
chain.addStreamTag('zee', type='calibration', prescale='1')
chain.addStreamTag('higgs', type='express', prescale='200')
chain.addTriggerTypeBit('4')
chain.addTriggerTypeBit('67')
menu.addHLTChain(chain)



chain = HLTChain( chain_name="H_EF", chain_counter="11", lower_chain_name="H_L2", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature(  [ "e25i", "e25i", "e25i", "e25i" ] )
chain.addHLTSignature( [ "e25i'", "e25i'", "e25i'", "e25i'" ] )
chain.addHLTSignature( [ "Z'", "Z'" ] )
chain.addHLTSignature( "Higgs'" )
chain.addStreamTag('higgs', prescale='1')
chain.addStreamTag('higgs', type='express', prescale='1')
chain.addTriggerTypeBit('4')
chain.addTriggerTypeBit('67')
menu.addHLTChain(chain)

menu.writeConfigFiles();
menu.dot(algs=True)

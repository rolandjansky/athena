# L1 menu
include('TrigSteering/pureSteering_l1menu.py')

from TriggerMenu.menu.TriggerPythonConfig import TriggerPythonConfig
from TriggerMenu.menu.HLTObjects import HLTChain
menu = TriggerPythonConfig('pureSteering_menu.xml')


from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo, PESA__dummyHypo, PESA__newDummyAlgo2To1, PESA__newDummyAlgoAllTEs, PESA__dummyAlgoHLTErrorCode, PESA__dummyMonitoredFex, PESA__dummyAlgoForCalibration, dummyFexWithRobReqPrep

# L2
# examples below show all the posibilities of configuring Sequences
Egamma = PESA__dummyAlgo('Egamma_L2')                                     # create configurable instance
Egamma2 = PESA__dummyAlgo('Egamma2_L2')                                   # create configurable another instance
EgammaAdv3 = PESA__dummyAlgo("EgammaAdv3")
EgammaAdv_L2 = PESA__dummyAlgo("EgammaAdv_L2")




menu.addSequence("EM15i" , [ Egamma, Egamma2, EgammaAdv3 ] , "em15i" )  # use bot here, mixing with old fassioned way
menu.addSequence("em15i" , EgammaAdv_L2, "em15i'" )    # create configurable on the fly
Topo1 = PESA__dummyAlgo('L2_Topo1')
menu.addSequence("TOPO0",[Topo1],"topo0")





# topo with seeding constraints
roughDiElectronMass = PESA__newDummyAlgo2To1("RoughEMassCutOnL1Objects") 
killSomeCombinations = PESA__dummyHypo('KillSomeCombinations', PreScale=2)
fineDiElectronMass = PESA__newDummyAlgo2To1("FineEMassCutOnHLTObjects")
perfectDiElectronMass = PESA__newDummyAlgo2To1("PerfectEMassCutOnHLTObjects")


menu.addSequence("EM15i EM15i", [roughDiElectronMass, killSomeCombinations], "RoughECut")
menu.addSequence("em15i em15i", fineDiElectronMass, "FineECut", topo_start_from="RoughECut")
menu.addSequence("em15i' em15i", perfectDiElectronMass, "GoodDielectron", topo_start_from="RoughECut") # notice that one arm is using refined em15i (one with') another is still em15i

menu.addSequence("em15i' em15i'", perfectDiElectronMass, "PerfectDielectron", topo_start_from="FineECut") # this topo sequence is based on final TEs



defalgo = PESA__dummyAlgo('Em25')                                               # no arg given ...default name

menu.addSequence("EM25i" , defalgo, "em25i" )

EgammaAdv_g2 = PESA__dummyAlgo("EgammaAdv_g2")
menu.addSequence("em25i" ,EgammaAdv_g2 ,"em25i'" )

Muon = PESA__dummyAlgo('Muon')
Muon2 = PESA__dummyAlgo('Muon2')
moniMuon = PESA__dummyMonitoredFex("moniMuon")
moniMuon.doTiming = True
moniMuon.OutputLevel = VERBOSE

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig, ErrorCodeMonitoringToolConfig
moniMuonTool = TrigGenericMonitoringToolConfig("AllInOneToolForMoniMuon")
moniMuonTool.Histograms = [ defineHistogram("TotalTime", title="Total time per RoI", xmin=-0.1, xmax=5., xbins=50) ]
# the same varaible but aliased differnetly
moniMuonTool.Histograms += [ defineHistogram("TotalTime;tot", title="Total time per RoI", xmin=-0.1, xmax=10., xbins=20) ]

moniMuonTool.Histograms += [ defineHistogram("TimerFake", title="Just fake measures some message time", xmin=-0.1, xmax=10., xbins=10) ]




moniMuonTool.Histograms += [ defineHistogram("TimerGetInitDescriptors", title="Time to get Initial RoIs", xmin=0.0, xmax=10., xbins=100) ]
moniMuonTool.Histograms += [ defineHistogram("TimerGetInitDescriptors_NRoIs", title="Initial RoIDescriptors count", xmin=-0.5, xmax=99.5, xbins=100) ]
moniMuonTool.Histograms += [ defineHistogram("TimerGetLastDescriptor", title="Time to get last RoI descriptor", xmin=0.0, xmax=10., xbins=100) ]
moniMuonTool.Histograms += [ defineHistogram("TimerGetLastDescriptor_NRoIs", title="Recent RoIDescriptors count", xmin=-0.5, xmax=99.5, xbins=100) ]
moniMuonTool.Histograms += [ defineHistogram("Del", title="#Delta #phi", xmin=-5, xmax=5, xbins=10) ]

moniMuonTool.Histograms += [ defineHistogram("hist1", title="vec and hist match", xmin=1, xmax=3, xbins=3, opt="kVec") ]
moniMuonTool.Histograms += [ defineHistogram("hist2", title="vect and hist missmatch", xmin=1, xmax=3, xbins=3, opt="kVec") ]

moniMuonTool.Histograms += [ defineHistogram("TotalTime,Del", type="TH2F", title="Total time .vs. Delta Phis", xmin=-0.1, xmax=5., xbins=50, ymin=-5, ymax=6, ybins=20) ]

moniMuonTool.Histograms += [ defineHistogram("TotalTime,Del;timeprofile", type="TProfile", title="Total time .vs. Delta Phis", xmin=-0.1, xmax=5., xbins=50) ]

moniMuonTool.Histograms += [ defineHistogram("TotalTime,Del;performance", type="TH2F", title="Total time .vs. Delta Phis", xmin=-0.1, xmax=2., xbins=20, ymin=-6, ymax=6, ybins=20) ]

moniMuon.AthenaMonTools = [ moniMuonTool ]


# algorithm caching test:
MufastHypo_Muon_2GeV = PESA__dummyHypo('MufastHypo_Muon_2GeV')
MufastHypo_Muon_6GeV = PESA__dummyHypo('MufastHypo_Muon_6GeV')
MufastHypo_Muon_6GeV.PreScale=1000000

muFast_Muon = PESA__dummyAlgo('muFast_Muon')
cMuon2 = PESA__dummyAlgo('cMuon2')

#<SEQUENCE algorithm="muFast/muFast_Muon MufastHypo/MufastHypo_Muon_2GeV" input="MU6" output="L2_muon_standalone_mu6l"/>
#<SEQUENCE algorithm="TrigIDSCAN/TrigIDSCAN_Muon TrigSiTrack/TrigSiTrack_Muon muComb/muComb_Muon_IDSCAN MucombHypo/MucombHypo_Muon_2GeV" input="L2_muon_standalone_mu6l" output="L2_mu6l"/>

#menu.addSequence("MU20" , [muFast_Muon, MufastHypo_Muon_2GeV ] ,"L2_muon_standalone_mu6l" )
menu.addSequence("MU20" , [MufastHypo_Muon_2GeV] ,"L2_muon_standalone_mu6l" )
menu.addSequence("MU20" , [muFast_Muon, MufastHypo_Muon_6GeV ] ,"L2_muon_standalone_mu6" )


menu.addSequence("MU20" , [Muon, Muon2,  moniMuon ] ,"mu20" )

MuonAdv = PESA__dummyAlgo("MuonAdv")
menu.addSequence("mu20" ,MuonAdv ,"mu20'" )       # both are equivalent

menu.addSequence("MU06" ,Muon , "mu6" )
menu.addSequence("mu6" , MuonAdv, "mu6'" )        # see above, both are equivalent

Jet_L2 = PESA__dummyAlgo("Jet_L2")
menu.addSequence("J200", Jet_L2 , "j200" )

JetAdv_L2 = PESA__dummyAlgo("JetAdv_L2")
menu.addSequence("j200", JetAdv_L2, "j200'" )


menu.addSequence("J90", Jet_L2, "j90" )
menu.addSequence("j90", JetAdv_L2, "j90'" )

menu.addSequence("J65", Jet_L2 , "j65" )
menu.addSequence("j65", JetAdv_L2 , "j65'" )

menu.addSequence("J50", Jet_L2 , "j50" )
menu.addSequence("j50", JetAdv_L2 , "j50'" )

Tau_L2 = PESA__dummyAlgo("Tau_L2")
menu.addSequence("TAU25", Tau_L2 , "tau25" )
TauAdv_L2 = PESA__dummyAlgo("TauAdv_L2")
menu.addSequence("tau25", TauAdv_L2 , "tau25'" )

MissingEt_L2 = PESA__dummyAlgo("MissingEt_L2")
menu.addSequence("XE30", MissingEt_L2 , "xE30" )
menu.addSequence("xE30", MissingEt_L2 , "xE30'" )

menu.addSequence("XE60", MissingEt_L2 , "xE60" )
menu.addSequence("xE60", MissingEt_L2 , "xE60'" )

ZFinder_L2 = PESA__newDummyAlgo2To1('ZFinder_L2')           # the same for other algo types
menu.addSequence(["em15i", "em25i"] ,ZFinder_L2 , "Z" )
menu.addSequence(["em25i", "em25i"] ,ZFinder_L2 , "Z_" )


HiggsFinder_L2 = PESA__newDummyAlgo2To1('HtoZZ')
menu.addSequence(["Z", "Z"] ,HiggsFinder_L2 , "Higgs" )

L2MetFex_met10 = PESA__newDummyAlgoAllTEs("L2MetFex_met10")
menu.addSequence(["MU06", "TM10"] ,L2MetFex_met10 , "met10_L2" )

algoEC = PESA__dummyAlgoHLTErrorCode('algoEC')
algoEC.HLTErrorCode = "ABORT_CHAIN TIMEOUT"
algoEC.ThrowUnknownException = False
algoEC.ThrowGaudiException = False
algoEC.ThrowStdException = False
menu.addSequence("EM25i", algoEC, "ErrorCode_L2")
ecodeMonitoring = ErrorCodeMonitoringToolConfig("EcodeForThisAlg")
ecodeMonitoring.renameUserCodes(u1="WrongPhiRange", u4="PTunphysical")
algoEC.AthenaMonTools += [ ecodeMonitoring ]


algoRobReqPrep_0 = dummyFexWithRobReqPrep('algoRobReqPrep_0')
algoRobReqPrep_1 = dummyFexWithRobReqPrep('algoRobReqPrep_1')
algoRobReqPrep_2 = dummyFexWithRobReqPrep('algoRobReqPrep_2')
algoRobReqPrep_3 = dummyFexWithRobReqPrep('algoRobReqPrep_3')
algoRobReqPrep_4 = dummyFexWithRobReqPrep('algoRobReqPrep_4')
algoRobReqPrep_5 = dummyFexWithRobReqPrep('algoRobReqPrep_5')
algoRobReqPrep_6 = dummyFexWithRobReqPrep('algoRobReqPrep_6')
menu.addSequence("MU06", algoRobReqPrep_0, "RobReqMU_L2")
menu.addSequence(["EM15i", "RobReqMU_L2"], [algoRobReqPrep_1, algoRobReqPrep_2], "RobReqS1_L2")
menu.addSequence("RobReqS1_L2", [algoRobReqPrep_3, algoRobReqPrep_4], "RobReqS2_L2")
menu.addSequence("RobReqS1_L2", [algoRobReqPrep_3, algoRobReqPrep_4], "RobReqS3_L2")
menu.addSequence("RobReqS1_L2", [algoRobReqPrep_5, algoRobReqPrep_6], "RobReqS4_L2")


# EF

gamma_EF = PESA__dummyAlgo("gamma_EF")
gammaAdv_EF = PESA__dummyAlgo("gammaAdv_EF")
menu.addSequence("em15i'", gamma_EF, "g15" )
menu.addSequence("g15", gammaAdv_EF, "g15'" )

electron_EF = PESA__dummyAlgo("electron_EF")
electronAdv_EF = PESA__dummyAlgo("electronAdv_EF")
menu.addSequence("em15i'", electron_EF , "e15i" )
menu.addSequence("e15i", electronAdv_EF , "e15i'" )


menu.addSequence("em25i'", gamma_EF , "g25" )
menu.addSequence("g25", gammaAdv_EF , "g25'" )

menu.addSequence("em25i'", electron_EF , "e25i" )
menu.addSequence("e25i", electronAdv_EF , "e25i'" )

MuonMoore_EF = PESA__dummyAlgo("MuonMoore_EF")
xuxMuon_EF = PESA__dummyAlgo("xuxMuon_EF")
menu.addSequence("mu20'", MuonMoore_EF , "muon20" )
menu.addSequence("muon20",  xuxMuon_EF, "muon20'" )

MuonAdvEF1 = PESA__dummyAlgo('MuonAdvEF1')
menu.addSequence("mu6'", MuonMoore_EF , "muon6" )
menu.addSequence("muon6",  MuonAdvEF1 , "muon6'" )

JetKt = PESA__dummyAlgo("JetKt")
JetAdvKt = PESA__dummyAlgo("JetAdvKt")
JetCaloCalibrator = PESA__dummyAlgoForCalibration("JetCaloCalibrator")
menu.addSequence("j200'", JetKt , "jet200" )
menu.addSequence("jet200", [JetAdvKt, JetCaloCalibrator], "jet200'" )

menu.addSequence("j90'", JetKt , "jet90" )
menu.addSequence("jet90", JetAdvKt , "jet90'" )

menu.addSequence("j65'", JetKt , "jet65" )
menu.addSequence("jet65", JetAdvKt , "jet65'" )

menu.addSequence("j50'", JetKt , "jet50" )
menu.addSequence("jet50", JetAdvKt , "jet50'" )

TauClu = PESA__dummyAlgo("TauClu")
TauAdvEF = PESA__dummyAlgo("TauAdvEF")
menu.addSequence("tau25'", TauClu , "Tau25" )
menu.addSequence("Tau25", TauAdvEF , "Tau25'" )

MissingEt = PESA__dummyAlgo("MissingEt")
menu.addSequence("xE30'", MissingEt , "missingE30" )
menu.addSequence("missingE30", MissingEt , "missingE30'" )

menu.addSequence("xE60'", MissingEt , "missingE60" )
menu.addSequence("missingE60", MissingEt , "missingE60'" )


ZFinder_EF = PESA__newDummyAlgo2To1("ZFinder_EF")
menu.addSequence(["e25i", "e25i"], ZFinder_EF , "Z'" )

HtoZZ_EF = PESA__newDummyAlgo2To1("HtoZZ_EF")
menu.addSequence(["Z'", "Z'"], HtoZZ_EF , "Higgs'" )



# chains

# L2
chain = HLTChain( chain_name="L2_e25", chain_counter="1", lower_chain_name="EM25i", level="HLT", prescale="1", pass_through="1")
chain.addHLTSignature( "em25i" ).addHLTSignature( "em25i'" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addStreamTag('IDCalibration', prescale='1', obeyLB="0", type="calibration")
chain.addGroup("electrons")

menu.addHLTChain(chain)

chain = HLTChain( chain_name="mu20_L2", chain_counter="2", lower_chain_name="MU20", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( "mu20" )
chain.addHLTSignature( "mu20'" )
chain.addTriggerTypeBit('10')
chain.addStreamTag('muons', prescale='1')
chain.addGroup("muons")
menu.addHLTChain(chain)





chain = HLTChain( chain_name="2em15i_L2", chain_counter="3", lower_chain_name="2EM15i", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( [ "em15i", "em15i" ] )
chain.addHLTSignature( [ "em15i'", "em15i'"] )
chain.addTriggerTypeBit('3')
chain.addStreamTag('electrons', prescale='1')
chain.addGroup("electrons")
menu.addHLTChain(chain)



chain = HLTChain( chain_name="2mu6_L2", chain_counter="4", lower_chain_name="2MU06", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( "mu6" )
chain.addHLTSignature( "mu6'" )
chain.addTriggerTypeBit('9')
chain.addStreamTag('muons', prescale='1')
chain.addGroup("mons")
menu.addHLTChain(chain)





chain = HLTChain( chain_name="j200_L2", chain_counter="5", lower_chain_name="J200", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( "j200" )
chain.addHLTSignature( "j200'" )
chain.addTriggerTypeBit('24')
chain.addStreamTag('jet', prescale='1')
chain.addGroup("jets")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="3j90_L2", chain_counter="6", lower_chain_name="3J90", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( ["j90", "j90", "j90"] )
chain.addHLTSignature( [ "j90'", "j90'", "j90'" ] )
chain.addTriggerTypeBit('28')
chain.addStreamTag('jet', prescale='1')
chain.addGroup("jets")
menu.addHLTChain(chain)





chain = HLTChain( chain_name="tau25_xE30_L2", chain_counter="7", lower_chain_name="TAU25+XE30", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( [ "xE30", "tau25" ] )
chain.addHLTSignature( [ "xE30'", "tau25'" ] )
chain.addTriggerTypeBit('48')
chain.addStreamTag('tau', prescale='1')
chain.addGroup("tau")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="2tau25_xE30_L2", chain_counter="27", lower_chain_name="TAU25+XE30", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( [  "tau25", "xE30", "tau25" ] )
chain.addHLTSignature( [ "xE30'", "tau25'", "tau25'" ] )
chain.addTriggerTypeBit('48')
chain.addStreamTag('tau', prescale='1')
chain.addGroup("tau")
menu.addHLTChain(chain)





chain = HLTChain( chain_name="j50_xE60_L2", chain_counter="8", lower_chain_name="J50+XE60", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( [ "j50", "xE60" ] )
chain.addHLTSignature( [ "j50'", "xE60'" ] )
chain.addTriggerTypeBit('31')
chain.addStreamTag('missingET', prescale='1')
chain.addGroup("MET")
menu.addHLTChain(chain)





chain = HLTChain( chain_name="Z_L2", chain_counter="9", lower_chain_name="EM25i", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( ["em15i", "em25i" ])
chain.addHLTSignature( ["em15i'", "em25i'" ])
chain.addHLTSignature( "Z" )
chain.addStreamTag('electrons', prescale='1')
chain.addStreamTag('zee', type='calibration', prescale='1')
chain.addTriggerTypeBit('4')
chain.addGroup("electronss")
menu.addHLTChain(chain)



chain = HLTChain( chain_name="H_L2", chain_counter="10", lower_chain_name="EM25i", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( [ "em25i", "em25i", "em25i", "em25i"] )
chain.addHLTSignature( [ "em25i'", "em25i'", "em25i'", "em25i'" ] )
chain.addHLTSignature( [ "Z", "Z" ] )
chain.addHLTSignature( "Higgs" )
chain.addStreamTag('higgs', prescale='1')
chain.addStreamTag('higgs', type='express', prescale='100')
chain.addTriggerTypeBit('4')
chain.addTriggerTypeBit('67')
chain.addGroup("electrons")
menu.addHLTChain(chain)





chain = HLTChain( chain_name="empty_L2", chain_counter="11", lower_chain_name="2MU06", level="HLT", prescale="1", pass_through="0")
chain.addTriggerTypeBit('11')
chain.addStreamTag('blaStream', prescale='1')
chain.addGroup("fakes")
menu.addHLTChain(chain)



chain = HLTChain( chain_name="MissingET_L2", chain_counter="12", lower_chain_name="2MU06", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( "topo0" )
chain.addTriggerTypeBit('36')
chain.addStreamTag('missingET', prescale='1')
chain.addGroup("MET")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="ErrorCode_Chain_L2", chain_counter="13", lower_chain_name="EM25i", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( "ErrorCode_L2", sigcounter=1 )
chain.addTriggerTypeBit('36')
chain.addStreamTag('aStreamTag', prescale='1')
chain.addGroup("fakes")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="NO_LOWER_CHAIN_L2", chain_counter="14", lower_chain_name="", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( "em25i" )
chain.addStreamTag('test',  prescale='1')
chain.addTriggerTypeBit('99')
chain.addGroup("fakes")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="Z2_L2", chain_counter="15", lower_chain_name="EM25i", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( ["em25i", "em25i" ])
chain.addHLTSignature( ["em25i'", "em25i'" ])
chain.addHLTSignature( "Z_" )
chain.addStreamTag('electrons', prescale='1')
chain.addStreamTag('zee', type='calibration', prescale='1')
chain.addTriggerTypeBit('4')
chain.addGroup("electrons")
menu.addHLTChain(chain)


# caching tests
chain = HLTChain( chain_name="L2_mu6", chain_counter="17", lower_chain_name="MU20", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( ["L2_muon_standalone_mu6"])
chain.addStreamTag('muons', prescale='1')
chain.addTriggerTypeBit('4')
chain.addGroup("muons")
menu.addHLTChain(chain)

# caching tests
chain = HLTChain( chain_name="L2_mu6l", chain_counter="18", lower_chain_name="MU20", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( ["L2_muon_standalone_mu6l" ])
chain.addStreamTag('muons', prescale='1')
chain.addTriggerTypeBit('4')
chain.addGroup("muons")
menu.addHLTChain(chain)

# ROB req prep test
chain = HLTChain( chain_name="RobRequest_Chain_L2", chain_counter="19", lower_chain_name="L1_EM15i", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( ["RobReqS2_L2", "RobReqS3_L2"] )
chain.addTriggerTypeBit('36')
chain.addStreamTag('aStreamTag', prescale='1')
chain.addGroup("RobTest")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="topoSeqSeedingTest1_L2", chain_counter="20", lower_chain_name="L1_EM15i", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature("RoughECut")
chain.addHLTSignature("FineECut")
chain.addHLTSignature("GoodDielectron")
chain.addTriggerTypeBit('4')
chain.addStreamTag('aStreamTag', prescale='1')
chain.addGroup("electrons")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="topoSeqSeedingTest2_L2", chain_counter="21", lower_chain_name="L1_EM15i", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature("RoughECut")
chain.addHLTSignature("FineECut")
chain.addHLTSignature("PerfectDielectron")
chain.addTriggerTypeBit('4')
chain.addStreamTag('aStreamTag', prescale='1')
chain.addGroup("electrons")
menu.addHLTChain(chain)

menu.writeConfigFiles();
menu.dot(algs=True)

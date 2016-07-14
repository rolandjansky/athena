
# L1 menu
include('TrigSteering/pureSteering_l1menu.py')

# HLT menu

from TriggerMenu.menu.TriggerPythonConfig import TriggerPythonConfig
from TriggerMenu.menu.HLTObjects import HLTChain

from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo, PESA__dummyHypo, PESA__newDummyAlgo2To1, PESA__newDummyAlgoAllTEs, PESA__dummyAlgoHLTErrorCode, PESA__dummyMonitoredFex, PESA__dummyAlgoForCalibration, dummyFexWithRobReqPrep

menu = TriggerPythonConfig('pureSteering_menu.xml')


# L2
# examples below show all the posibilities of configuring Sequences
Egamma = PESA__dummyAlgo('Egamma_L2')                                     # create configurable instance
Egamma2 = PESA__dummyAlgo('Egamma2_L2')                                   # create configurable another instance
EgammaAdv3 = PESA__dummyAlgo("EgammaAdv3")
EgammaAdv_L2 = PESA__dummyAlgo("EgammaAdv_L2")
menu.addSequence("EM15i" , [ Egamma, Egamma2, EgammaAdv3 ] , "em15i" )  # use bot here, mixing with old fassioned way
menu.addSequence("em15i" , EgammaAdv_L2, "em15i'" )    # create configurable on the fly

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

# HLT
chain1 = HLTChain( chain_name="HLT_e25", chain_counter="1", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="1")
chain1.addHLTSignature( "em25i" ).addHLTSignature( "em25i'" )
chain1.addTriggerTypeBit('4')
chain1.addStreamTag('electrons', prescale='1')
chain1.addStreamTag('IDCalibration', prescale='1', type="calibration")
chain1.addGroup("electrons")
menu.addHLTChain(chain1)

chain = HLTChain( chain_name="HLT_g25", chain_counter="55", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( "g25" ).addHLTSignature( "g25'" )
chain.addTriggerTypeBit('5')
chain.addStreamTag('electrons', prescale='1')
chain.addGroup("electrons")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="HLT_mu20", chain_counter="2", lower_chain_name="L1_MU20", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( "mu20" )
chain.addHLTSignature( "mu20'" )
chain.addTriggerTypeBit('10')
chain.addStreamTag('muons', prescale='1')
chain.addGroup("muons")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="HLT_2em15i", chain_counter="3", lower_chain_name="L1_2EM15i", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( [ "em15i", "em15i" ] )
chain.addHLTSignature( [ "em15i'", "em15i'"] )
chain.addTriggerTypeBit('3')
chain.addStreamTag('electrons', prescale='1')
chain.addGroup("electrons")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="HLT_2mu6", chain_counter="4", lower_chain_name="L1_2MU06", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( "mu6" )
chain.addHLTSignature( "mu6'" )
chain.addHLTSignature( [ "muon6", "muon6" ] )
chain.addHLTSignature( [ "muon6'", "muon6'" ] )
chain.addTriggerTypeBit('9')
chain.addStreamTag('muons', prescale='1')
chain.addGroup("mons")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="HLT_j200", chain_counter="5", lower_chain_name="L1_J200", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( "j200" )
chain.addHLTSignature( "j200'" )
chain.addTriggerTypeBit('24')
chain.addStreamTag('jet', prescale='1')
chain.addGroup("jets")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="HLT_3j90", chain_counter="6", lower_chain_name="L1_3J90", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( ["j90", "j90", "j90"] )
chain.addHLTSignature( [ "j90'", "j90'", "j90'" ] )
chain.addHLTSignature( [ "jet90", "jet90", "jet90" ] )
chain.addHLTSignature( [ "jet90'", "jet90'", "jet90'" ] )
chain.addTriggerTypeBit('28')
chain.addStreamTag('jet', prescale='1')
chain.addGroup("jets")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="HLT_tau25_xE30", chain_counter="7", lower_chain_name="L1_TAU25_XE30", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( [ "xE30", "tau25" ] )
chain.addHLTSignature( [ "xE30'", "tau25'" ] )
chain.addHLTSignature( [ "Tau25", "missingE30" ] )
chain.addHLTSignature( [ "Tau25'", "missingE30'" ] )
chain.addTriggerTypeBit('48')
chain.addStreamTag('tau', prescale='1')
chain.addGroup("tau")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="HLT_2tau25_xE30", chain_counter="27", lower_chain_name="L1_TAU25_XE30", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( [  "tau25", "xE30", "tau25" ] )
chain.addHLTSignature( [ "xE30'", "tau25'", "tau25'" ] )
chain.addTriggerTypeBit('48')
chain.addStreamTag('tau', prescale='1')
chain.addGroup("tau")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="HLT_j50_xE60", chain_counter="8", lower_chain_name="L1_J50_XE60", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( [ "j50", "xE60" ] )
chain.addHLTSignature( [ "j50'", "xE60'" ] )
chain.addHLTSignature( [ "jet50", "missingE60" ] )
chain.addHLTSignature( [ "jet50'", "missingE60'" ] )
chain.addTriggerTypeBit('31')
chain.addStreamTag('missingET', prescale='1')
chain.addGroup("MET")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="HLT_Z", chain_counter="9", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( ["em15i", "em25i" ])
chain.addHLTSignature( ["em15i'", "em25i'" ])
chain.addHLTSignature( "Z" )
chain.addStreamTag('electrons', prescale='1')
chain.addStreamTag('zee', type='calibration', prescale='1')
chain.addTriggerTypeBit('4')
chain.addGroup("electronss")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="HLT_H", chain_counter="10", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="0")
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

chain = HLTChain( chain_name="HLT_empty", chain_counter="11", lower_chain_name="L1_2MU06", level="HLT", prescale="1", pass_through="0")
chain.addTriggerTypeBit('11')
chain.addStreamTag('blaStream', prescale='1')
chain.addGroup("fakes")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="HLT_MissingET", chain_counter="12", lower_chain_name="L1_2MU06", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( "met10_L2", sigcounter=2 )
chain.addTriggerTypeBit('36')
chain.addStreamTag('missingET', prescale='1')
chain.addGroup("MET")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="HLT_ErrorCode_Chain", chain_counter="13", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( "ErrorCode_L2", sigcounter=1 )
chain.addTriggerTypeBit('36')
chain.addStreamTag('aStreamTag', prescale='1')
chain.addGroup("fakes")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="HLT_NO_LOWER_CHAIN", chain_counter="14", lower_chain_name="", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( "em25i" )
chain.addStreamTag('test',  prescale='1')
chain.addTriggerTypeBit('99')
chain.addGroup("fakes")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="HLT_Z2", chain_counter="15", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( ["em25i", "em25i" ])
chain.addHLTSignature( ["em25i'", "em25i'" ])
chain.addHLTSignature( "Z_" )
chain.addStreamTag('electrons', prescale='1')
chain.addStreamTag('zee', type='calibration', prescale='1')
chain.addTriggerTypeBit('4')
chain.addGroup("electrons")
menu.addHLTChain(chain)

# caching tests
chain = HLTChain( chain_name="HLT_mu6", chain_counter="17", lower_chain_name="L1_MU20", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( ["L2_muon_standalone_mu6"])
chain.addStreamTag('muons', prescale='1')
chain.addTriggerTypeBit('4')
chain.addGroup("muons")
menu.addHLTChain(chain)

# caching tests
chain = HLTChain( chain_name="HLT_mu6l", chain_counter="18", lower_chain_name="L1_MU20", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( ["L2_muon_standalone_mu6l" ])
chain.addStreamTag('muons', prescale='1')
chain.addTriggerTypeBit('4')
chain.addGroup("muons")
menu.addHLTChain(chain)

# ROB req prep test
chain = HLTChain( chain_name="HLT_RobRequest_Chain", chain_counter="19", lower_chain_name="L1_EM15i", level="HLT", prescale="1", pass_through="0")
chain.addHLTSignature( ["RobReqS2_L2", "RobReqS3_L2"] )
chain.addTriggerTypeBit('36')
chain.addStreamTag('aStreamTag', prescale='1')
chain.addGroup("RobTest")
menu.addHLTChain(chain)


menu.writeConfigFiles();
menu.dot(algs=True)


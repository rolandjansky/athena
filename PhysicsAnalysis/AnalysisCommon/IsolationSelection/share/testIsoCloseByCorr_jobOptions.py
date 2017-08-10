# Input file
# inputFile = "root://eosatlas//eos/atlas/atlastier0/tzero/prod/valid1/PowhegPythia8_AU2CT10_Zmumu/147807/valid1.147807.PowhegPythia8_AU2CT10_Zmumu.recon.AOD.e2658_s1967_s1964_r5787_v114/valid1.147807.PowhegPythia8_AU2CT10_Zmumu.recon.AOD.e2658_s1967_s1964_r5787_v114._000187.1";
#inputFile = "root://eosatlas//eos/atlas/atlastier0/tzero/prod/valid1/PowhegPythia_P2011C_ttbar/117050/valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5787_v111/valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5787_v111._000001.4";
#inputFile = "root://eosatlas//eos/atlas/atlastier0/tzero/prod/valid1/PowhegPythia8_AU2CT10_Zee/147806/valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_v114/valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_v114._000001.1";
#inputFile = "root://eosatlas//eos/atlas/atlastier0/tzero/prod/valid1/PowhegPythia8_AU2CT10_ggH125_gamgam/160009/valid1.160009.PowhegPythia8_AU2CT10_ggH125_gamgam.recon.AOD.e2658_s1967_s1964_r5787_v114/valid1.160009.PowhegPythia8_AU2CT10_ggH125_gamgam.recon.AOD.e2658_s1967_s1964_r5787_v114._000001.1";
inputFile = "root://eosatlas//eos/atlas/atlastier0/tzero/prod/valid1/PowhegPythia_P2011C_ttbar/117050/valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5787_v111/valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5787_v111._000001.4";

# n events ( use -1 to process all events )
nEvents = 100

import AthenaPoolCnvSvc.ReadAthenaPool
# svcMgr.EventSelector.InputCollections= [ os.environ["ASG_TEST_FILE_MC"] ]
svcMgr.EventSelector.InputCollections= [ inputFile ]
algseq = CfgMgr.AthSequencer("AthAlgSeq")


## Configure an isolation selection tool with your desired working points
ToolSvc += CfgMgr.CP__IsolationSelectionTool("MySelectionTool", MuonWP = "FixedCutLoose", ElectronWP = "Loose", PhotonWP = "FixedCutTightCaloOnly")
# ToolSvc += CfgMgr.CP__IsolationSelectionTool("MySelectionTool", MuonWP = "FixedCutLoose", PhotonWP = "FixedCutTightCaloOnly")


## Configure CorrectionTool, feeding it our selection tool
ToolSvc += CfgMgr.CP__IsolationCloseByCorrectionTool("IsolationCloseByCorrectionTool",
                                                     IsolationSelectionTool=ToolSvc.MySelectionTool)


## Test algorithm
from IsolationSelection.IsolationSelectionConf import CP__TestIsolationCloseByCorrAthenaAlg 
algseq += CfgMgr.CP__TestIsolationCloseByCorrAthenaAlg("TestAlg",IsoSelectorTool = ToolSvc.MySelectionTool, 
                                                       IsoCloseByCorrTool=ToolSvc.IsolationCloseByCorrectionTool)


# svcMgr.EventSelector.SkipEvents = 67
theApp.EvtMax = nEvents
#####################################################################################################
#
# top level jobOptions to run Muon chains in the RTT or standalone
# sets some global variables that adjust the execution of TrigInDetValidation_RTT_Common.py
#
# Jiri.Masik@manchester.ac.uk
#
#####################################################################################################

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

#set athenaCommonFlags.FilesInput to be able to use this job options standalone without RTT
#secondSet of files can be activated by the if statement below 

if athenaCommonFlags.FilesInput()==[]:
  athenaCommonFlags.FilesInput=[
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000001.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000002.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000003.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000004.pool.root.2",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000005.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000006.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000007.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000008.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000009.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000010.pool.root.1",
    ]

###XMLDataSet='TrigInDetValidation_mu_single_mu_100_run2' # <-- RTT jobID

rec.RootNtupleOutput="ntupleInDetMuonSlice.root"

include("TrigInDetValidation/TrigInDetValidation_RTT_Chains.py")

rMC = False
if 'runMergedChain' in dir() and runMergedChain==True:
  rMC = True
(idtrigChainlist, tidaAnalysischains) = muonChains(rMC)
def resetSigs():
  TriggerFlags.Slices_all_setOff()
  TriggerFlags.MuonSlice.setAll();
  TriggerFlags.MuonSlice.signatures = idtrigChainlist


PdgId=13


include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")

if 'runMergedChain' in dir() and runMergedChain==True:
  if 'robust' in dir() and robust==True:
    topSequence.TrigSteer_HLT.TrigFastTrackFinder_Muon.UseNewSeeding = False
    topSequence.TrigSteer_HLT.TrigFastTrackFinder_Muon.RoadMakerTool.LayerOneDepth=3
    topSequence.TrigSteer_HLT.TrigFastTrackFinder_Muon.RoadMakerTool.LayerTwoDepth=3 
  if 'newSeeding' in dir() and newSeeding==False:
    topSequence.TrigSteer_HLT.TrigFastTrackFinder_Muon.UseNewSeeding = False


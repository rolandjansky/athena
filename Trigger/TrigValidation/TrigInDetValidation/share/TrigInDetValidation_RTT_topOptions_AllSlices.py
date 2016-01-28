#####################################################################################################
#
# top level jobOptions to run Electron chains in the RTT or standalone
# sets some global variables that adjust the execution of TrigInDetValidation_RTT_Common.py
#
# Jiri.Masik@manchester.ac.uk
#
#####################################################################################################

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

#set athenaCommonFlags.FilesInput to be able to use this job options standalone without RTT

if athenaCommonFlags.FilesInput()==[]:
  athenaCommonFlags.FilesInput=[
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000016.pool.root.1",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000019.pool.root.1",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000028.pool.root.1",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000031.pool.root.1",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000035.pool.root.1",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000046.pool.root.1",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000054.pool.root.1 ",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000057.pool.root.1 ",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000070.pool.root.1",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000071.pool.root.1",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000072.pool.root.1",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000079.pool.root.1",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000090.pool.root.1",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000096.pool.root.1",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000103.pool.root.1"
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000009.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000036.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000037.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000089.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000095.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000138.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000162.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000180.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000182.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000185.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000186.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000201.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000212.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000247.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000278.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000283.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000335.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000386.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000400.pool.root.2",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000435.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000446.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000457.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000470.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000474.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000475.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000478.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000502.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000510.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000535.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000550.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000551.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000552.pool.root.1",
    #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc11_14TeV.105568.ttbar_Pythia.digit.RDO.e842_s1321_d602_tid579396_00/RDO.579396._000562.pool.root.1",
  ]


rMC = False
rID = False
rFTK=False
if 'runMergedChain' in dir() and runMergedChain==True:
  rMC = True
if 'doIDNewTracking' in dir() and doIDNewTracking==True:
  rID = True

tidaAnalysischains = []

include("TrigInDetValidation/TrigInDetValidation_RTT_Chains.py")

(electronChainlist, electronAnalysischains) = electronChains(rMC, rID)
tidaAnalysischains += electronAnalysischains

(muonChainlist, muonAnalysischains) = muonChains(rMC, rID, rFTK)
tidaAnalysischains += muonAnalysischains

(tauChainlist, tauAnalysischains) = tauChains(rMC, rID, rFTK)
tidaAnalysischains += tauAnalysischains

(bjetChainlist, bjetAnalysischains) = bjetChains(rMC, rID, rFTK)
tidaAnalysischains += bjetAnalysischains

(minBiasChainlist, minBiasAnalysischains) = minBiasChains(rMC, rID)
tidaAnalysischains += minBiasAnalysischains

def resetSigs():
  TriggerFlags.Slices_all_setOff()

  TriggerFlags.EgammaSlice.setAll()
  TriggerFlags.EgammaSlice.signatures = electronChainlist

  TriggerFlags.MuonSlice.setAll();
  TriggerFlags.MuonSlice.signatures = muonChainlist

  TriggerFlags.TauSlice.setAll()
  TriggerFlags.TauSlice.signatures = tauChainlist

  TriggerFlags.BjetSlice.setAll();
  TriggerFlags.BjetSlice.signatures = bjetChainlist

  TriggerFlags.MinBiasSlice.setAll();
  TriggerFlags.MinBiasSlice.signatures = minBiasChainlist

include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")

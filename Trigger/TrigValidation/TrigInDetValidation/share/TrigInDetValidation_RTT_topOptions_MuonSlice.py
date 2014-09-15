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
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_muon/mc09_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e478_s616_d284_tid107022_00/RDO.107022._000076.pool.root.1",       
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_muon/mc09_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e478_s616_d284_tid107022_00/RDO.107022._000423.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_muon/mc09_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e478_s616_d284_tid107022_00/RDO.107022._000476.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_muon/mc09_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e478_s616_d284_tid107022_00/RDO.107022._000642.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_muon/mc09_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e478_s616_d284_tid107022_00/RDO.107022._000683.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_muon/mc09_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e478_s616_d284_tid107022_00/RDO.107022._000713.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_muon/mc09_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e478_s616_d284_tid107022_00/RDO.107022._000718.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_muon/mc09_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e478_s616_d284_tid107022_00/RDO.107022._000767.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_muon/mc09_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e478_s616_d284_tid107022_00/RDO.107022._000845.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_muon/mc09_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e478_s616_d284_tid107022_00/RDO.107022._000857.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_muon/mc09_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e478_s616_d284_tid107022_00/RDO.107022._000983.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_muon/mc09_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e478_s616_d284_tid107022_00/RDO.107022._001004.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_muon/mc09_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e478_s616_d284_tid107022_00/RDO.107022._001105.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_muon/mc09_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e478_s616_d284_tid107022_00/RDO.107022._001768.pool.root.2",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_muon/mc09_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e478_s616_d284_tid107022_00/RDO.107022._001988.pool.root.1"
    ]

  secondSet=True
  if secondSet: 
    athenaCommonFlags.FilesInput=[
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_muon/mc10_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e662_s1107_d459_tid285222_00/RDO.285222._000005.pool.root.1",       
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_muon/mc10_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e662_s1107_d459_tid285222_00/RDO.285222._000006.pool.root.1",       
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_muon/mc10_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e662_s1107_d459_tid285222_00/RDO.285222._000018.pool.root.1",       
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_muon/mc10_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e662_s1107_d459_tid285222_00/RDO.285222._000019.pool.root.1",       
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_muon/mc10_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e662_s1107_d459_tid285222_00/RDO.285222._000030.pool.root.1",       
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_muon/mc10_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e662_s1107_d459_tid285222_00/RDO.285222._000033.pool.root.1",       
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_muon/mc10_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e662_s1107_d459_tid285222_00/RDO.285222._000047.pool.root.1",       
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_muon/mc10_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e662_s1107_d459_tid285222_00/RDO.285222._000048.pool.root.1",       
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_muon/mc10_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e662_s1107_d459_tid285222_00/RDO.285222._000049.pool.root.1",       
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_muon/mc10_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e662_s1107_d459_tid285222_00/RDO.285222._000064.pool.root.1",       
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_muon/mc10_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e662_s1107_d459_tid285222_00/RDO.285222._000069.pool.root.1",       
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_muon/mc10_14TeV.106047.PythiaZmumu_no_filter.digit.RDO.e662_s1107_d459_tid285222_00/RDO.285222._000073.pool.root.1",       
    ]
  

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


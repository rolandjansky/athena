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
  

rID=False
if 'doIDNewTracking' in dir() and doIDNewTracking==True:
  rID = True

(idtrigChainlist, tidaAnalysischains) = muonChains(rID)


def resetSigs():
  TriggerFlags.Slices_all_setOff()
  TriggerFlags.MuonSlice.setAll();
  TriggerFlags.MuonSlice.signatures = idTrigChainlist


PdgId=13


include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")



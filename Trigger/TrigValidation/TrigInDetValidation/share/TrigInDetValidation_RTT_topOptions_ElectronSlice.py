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
#second set of files can be activated by the if statement below 

if athenaCommonFlags.FilesInput()==[]:
  athenaCommonFlags.FilesInput=[
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_electron/mc09_valid.107020.singlepart_e_Et7-80.digit.RDO.e342_s564_s584_d204_tid091713/RDO.091713._000005.pool.root.2",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_electron/mc09_valid.107020.singlepart_e_Et7-80.digit.RDO.e342_s564_s584_d204_tid091713/RDO.091713._000007.pool.root.2",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_electron/mc09_valid.107020.singlepart_e_Et7-80.digit.RDO.e342_s564_s584_d204_tid091713/RDO.091713._000025.pool.root.1",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_electron/mc09_valid.107020.singlepart_e_Et7-80.digit.RDO.e342_s564_s584_d204_tid091713/RDO.091713._000040.pool.root.1",
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_electron/mc09_valid.107020.singlepart_e_Et7-80.digit.RDO.e342_s564_s584_d204_tid091713/RDO.091713._000042.pool.root.1"
  ]

  secondSet=True
  if secondSet: 
    athenaCommonFlags.FilesInput=[
      
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000005.pool.root.2",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000014.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000023.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000028.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000042.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000048.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000049.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000053.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000057.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000066.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000075.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000087.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000088.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000090.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000095.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000103.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000106.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000115.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000120.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000125.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000135.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000140.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000151.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000160.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000162.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000177.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000183.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000196.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_electron/mc10_14TeV.106046.PythiaZee_no_filter.digit.RDO.e662_s1107_d459_tid285219_00/RDO.285219._000200.pool.root.1",
      #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.159010.ParticleGenerator_e_Et7to80.recon.RDO.e1948_s1682_s1691_r4708_tid01392866_00/RDO.01392866._000005.pool.root.1",
      #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.159010.ParticleGenerator_e_Et7to80.recon.RDO.e1948_s1682_s1691_r4708_tid01392866_00/RDO.01392866._000020.pool.root.1",
      #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.159010.ParticleGenerator_e_Et7to80.recon.RDO.e1948_s1682_s1691_r4708_tid01392866_00/RDO.01392866._000027.pool.root.1",
      #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.159010.ParticleGenerator_e_Et7to80.recon.RDO.e1948_s1682_s1691_r4708_tid01392866_00/RDO.01392866._000040.pool.root.1",
      #"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.159010.ParticleGenerator_e_Et7to80.recon.RDO.e1948_s1682_s1691_r4708_tid01392866_00/RDO.01392866._000041.pool.root.1",
      
    ]

include("TrigInDetValidation/TrigInDetValidation_RTT_Chains.py")

rID=False
if 'doIDNewTracking' in dir() and doIDNewTracking==True:
  rID = True

(idtrigChainlist, tidaAnalysischains) = electronChains(rID)


def resetSigs():
  TriggerFlags.Slices_all_setOff()
  TriggerFlags.EgammaSlice.setAll()
  TriggerFlags.EgammaSlice.signatures = idtrigChainlist


PdgId=11

include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")


###############################################################
#
# Set Flags for Running the Trigger in TransientBS Mode
#==============================================================

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags


if athenaCommonFlags.FilesInput()==[]:
  athenaCommonFlags.FilesInput=[
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000004.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000005.pool.root.2",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000008.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000013.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000019.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000026.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000040.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000044.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000047.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000048.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000053.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000054.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000060.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000062.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000065.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000073.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000078.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000083.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000099.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000102.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000116.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000125.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000137.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000149.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000166.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000167.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000176.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000196.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000198.pool.root.1",
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_taus/mc10_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e662_s1107_d459_tid285214_00/RDO.285214._000199.pool.root.1",

       ]

rec.RootNtupleOutput="ntupleInDetTauSlice.root"

include("TrigInDetValidation/TrigInDetValidation_RTT_Chains.py")

rMC = False
if 'runMergedChain' in dir() and runMergedChain==True:
  rMC = True
(idtrigChainlist, tidaAnalysischains) = tauChains(rMC)

def resetSigs():
  TriggerFlags.Slices_all_setOff()
  TriggerFlags.TauSlice.setAll()
  #TriggerFlags.Lvl1.items = TriggerFlags.Lvl1.items() + [ 'L1_TAU5', ]
  TriggerFlags.TauSlice.signatures = idtrigChainlist

PdgId=15

include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")

if 'runMergedChain' in dir() and runMergedChain==True:
  if 'robust' in dir() and robust==True:
    topSequence.TrigSteer_HLT.TrigFastTrackFinder_Tau.doSeedRedundancyCheck = True
    topSequence.TrigSteer_HLT.TrigFastTrackFinder_Tau.RoadMakerTool.LayerOneDepth=3
    topSequence.TrigSteer_HLT.TrigFastTrackFinder_Tau.RoadMakerTool.LayerTwoDepth=3 
  if 'newSeeding' in dir() and newSeeding==False:
    topSequence.TrigSteer_HLT.TrigFastTrackFinder_Tau.UseNewSeeding = False



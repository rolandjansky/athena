###############################################################
#
# Set Flags for Running the Trigger in TransientBS Mode
# includes Trigger_topOptions.py to do the real work.
# Reads input from ZEBRA.P (readG3=true)
#
# use:
# athena.py -bs testIDtransBS.py
# 
#==============================================================
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

#athenaCommonFlags.FilesInput=["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_minBias/mc10_7TeV.105012.J3_pythia_jetjet.digit.RDO.e574_s1023_d421_tid212390_00/RDO.212390._000284.pool.root.1"]

if athenaCommonFlags.FilesInput()==[]:
  athenaCommonFlags.FilesInput=[
       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_minBias/mc10_7TeV.105012.J3_pythia_jetjet.digit.RDO.e574_s1023_d421_tid212390_00/RDO.212390._000284.pool.root.1",
#       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_minBias/mc10_7TeV.105012.J3_pythia_jetjet.digit.RDO.e574_s1023_d421_tid212390_00/RDO.212390._000280.pool.root.1",
#       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/igrabows/TrigInDetValidation_minBias/mc10_7TeV.105012.J3_pythia_jetjet.digit.RDO.e574_s1023_d421_tid212390_00/RDO.212390._000182.pool.root.1",
#       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000031.pool.root.1", 
#       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000035.pool.root.1",
#       "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000046.pool.root.1",     
       ]

from TriggerMenu import useNewTriggerMenu
use_new_tm = useNewTriggerMenu()

RTTchainlist=[]
chainNames = [
    "Truth",
]

include("TrigInDetValidation/TrigInDetValidation_RTT_Chains.py")

rMC = False
if 'runMergedChain' in dir() and runMergedChain==True:
  rMC = True
(idtrigChainlist, tidaAnalysischains) = minBiasChains(rMC)

def resetSigs():
  TriggerFlags.Slices_all_setOff()
  TriggerFlags.MinBiasSlice.setAll();
  TriggerFlags.MinBiasSlice.signatures = idtrigChainlist
                                           
  

rec.RootNtupleOutput="ntupleInDetMonitorSlice.root"

include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")

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

#if athenaCommonFlags.FilesInput()==[]:
#  athenaCommonFlags.FilesInput=[
#       "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc15_13TeV.361036.Pythia8_A2MSTW2008LO_minbias_inelastic.recon.RDO.e3580_s2726_r7011/RDO.07344311._000057.pool.root.1"
#       ]

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
rID=False
if 'doIDNewTracking' in dir() and doIDNewTracking==True:
  rID = True

(idtrigChainlist, tidaAnalysischains) = minBiasChains(rMC,rID)

def resetSigs():
  TriggerFlags.Slices_all_setOff()
  TriggerFlags.MinBiasSlice.setAll();
  TriggerFlags.MinBiasSlice.signatures = idtrigChainlist
                                           
  

include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")

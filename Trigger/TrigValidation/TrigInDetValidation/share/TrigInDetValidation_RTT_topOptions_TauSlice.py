###############################################################
#
# Set Flags for Running the Trigger in TransientBS Mode
#==============================================================

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags


#if athenaCommonFlags.FilesInput()==[]:
#  athenaCommonFlags.FilesInput=[
#    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc15_13TeV.361108.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.recon.RDO.e3601_s2757_r7245/RDO.06885566._001556.pool.root.1"    
#       ]

include("TrigInDetValidation/TrigInDetValidation_RTT_Chains.py")


rID=False
if 'doIDNewTracking' in dir() and doIDNewTracking==True:
  rID = True

(idtrigChainlist, tidaAnalysischains) = tauChains(rID)

def resetSigs():
  TriggerFlags.Slices_all_setOff()
  TriggerFlags.TauSlice.setAll()
  #TriggerFlags.Lvl1.items = TriggerFlags.Lvl1.items() + [ 'L1_TAU5', ]
  TriggerFlags.TauSlice.signatures = idtrigChainlist

PdgId=15



from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doHypo.set_Value_and_Lock(True)


include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")


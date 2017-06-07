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

#from AthenaCommon.AppMgr import release_metadata
#d = release_metadata()
##TestMonTool.releaseMetaData = d['nightly name'] + " " + d['nightly release'] + " " + d['date'] + " " + d['platform'] + " " + d['release']
#print d['nightly name']
#if d['nightly name']=='20.1.X.Y.Z-VAL-TrigMC' or d['nightly name']=='20.X.Y-VAL' or d['nightly name']=='21.X.Y' or d['nightly name']=='20.7.X-VAL' or '20.7.3.Y-VAL' in d['nightly name'] or '20.7.4.Y-VAL' in d['nightly name'] :
#  print '***JK This is a realease with FTK, will include chains '
#else:
#  print '***JK This release does not include FTK, will set doFTK=False'
#  doFTK=False

rMC = False
if 'runMergedChain' in dir() and runMergedChain==True:
  rMC = True
rID=False
if 'doIDNewTracking' in dir() and doIDNewTracking==True:
  rID = True
rFTK=False
if 'doFTK' in dir() and doFTK==True:
  from TriggerJobOpts.TriggerFlags import TriggerFlags
  TriggerFlags.doFTK=True
  rFTK=True

(idtrigChainlist, tidaAnalysischains) = tauChains(rMC,rID,rFTK)

def resetSigs():
  TriggerFlags.Slices_all_setOff()
  TriggerFlags.TauSlice.setAll()
  #TriggerFlags.Lvl1.items = TriggerFlags.Lvl1.items() + [ 'L1_TAU5', ]
  TriggerFlags.TauSlice.signatures = idtrigChainlist

PdgId=15



from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doHypo.set_Value_and_Lock(True)


include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")

#if 'doFTK' in dir() and doFTK==True:
#  topSequence.TrigSteer_HLT.TrigFastTrackFinder_Tau_IDTrig.FTK_Mode=True
#  topSequence.TrigSteer_HLT.TrigFastTrackFinder_TauCore.FTK_Mode=True
#  topSequence.TrigSteer_HLT.TrigFastTrackFinder_TauIso.FTK_Mode=True


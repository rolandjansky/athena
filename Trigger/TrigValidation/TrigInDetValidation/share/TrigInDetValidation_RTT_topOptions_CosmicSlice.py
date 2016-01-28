#####################################################################################################
#
# top level jobOptions to run Cosmics chains in the RTT or standalone
# sets some global variables that adjust the execution of TrigInDetValidation_RTT_Common.py
#
# Jiri.Masik@manchester.ac.uk
#
#####################################################################################################



doIDNewTracking=True
from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.doCosmics.set_Value_and_Lock(True)

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags


if athenaCommonFlags.FilesInput()==[]:
  thirdSet=True
  if thirdSet: 
    athenaCommonFlags.FilesInput=[
      "/afs/cern.ch/user/m/masik/afswork/group.det-indet.valid1.108865.CosSimPixVolSolOnTorOn.s1922_s1927_RDO_12June_maxEvents.140611233257/group.det-indet.630146_006090.EXT0._00014.RDO.root"
]

from TriggerMenu import useNewTriggerMenu
use_new_tm = useNewTriggerMenu()


idTrigChainlist = []
cosmicTrigChainlist = []
tidaAnalysischains = [
  "Truth",
  "Offline",
]

if not use_new_tm:
  pass
else:
  cosmicTrigChainlist.append(['id_cosmic',               '', [], ['IDCosmic', 'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], 1])
#  cosmicTrigChainlist.append(['id_cosmic_trtxk',         '', [], ['IDCosmic'], ['RATE:CosmicSlice', 'BW:Detector'], 1])
#  cosmicTrigChainlist.append(['id_cosmic_trtxk_central', '', [], ['IDCosmic'], ['RATE:CosmicSlice', 'BW:Detector'], 1])

  tidaAnalysischains.append('HLT_id_cosmic*:InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID')



if 'runMergedChain' in dir() and runMergedChain==True:
  if use_new_tm:
    pass
  else:
    pass

def resetSigs():
  print 'jmasik in resetSigs'
  TriggerFlags.Slices_all_setOff()
  #TriggerFlags.MuonSlice.setAll();
  TriggerFlags.CosmicSlice.setAll();
  TriggerFlags.CosmicSlice.signatures = cosmicTrigChainlist

PdgId=13

#disable pixel cluster splitting in the offline reco
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doPixelClusterSplitting.set_Value_and_Lock(False)

include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")

TestMonTool.KeepAllEvents=True


# from IOVDbSvc.CondDB import conddb 
# conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterialV2','AtlasLayerMat_x16_ATLAS-IBL-03') 

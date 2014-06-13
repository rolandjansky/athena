# theApp.TopAlg += [ "MakePixelTracklets" ]

# from HIGlobal.HIGlobalConf import MakePixelTracklets
# MakePixelTracklets = MakePixelTracklets()
# MakePixelTracklets.EtaMax = 2.0
# MakePixelTracklets.EtaCut = 0.01
# MakePixelTracklets.PhiCut = 0.08
# MakePixelTracklets.EtaMatch = 0.007
# MakePixelTracklets.PhiMatch = 0.01
# MakePixelTracklets.CaloClusterContainerName = "EMTopoCluster430"
# MakePixelTracklets.doHolesEfficiency = False
# if rec.doTruth() :
#   MakePixelTracklets.McAvailable = False
# else :
#   MakePixelTracklets.McAvailable = False
# MakePixelTracklets.NtupleLoc = "AANT" # "TRKVAL"

theApp.TopAlg += [ "HIPixelTrackletsMaker" ]
from HIGlobal.HIGlobalConf import HIPixelTrackletsMaker
HIPixelTrackletsMaker = HIPixelTrackletsMaker()
HIPixelTrackletsMaker.nBinsEta = 50
HIPixelTrackletsMaker.nBinsPhi = 40

if rec.doTruth() :
  from HIGlobal.HIGlobalConf import HIPixelTrackletsTruthMaker
  HIPixelTrackletsTruthMaker = HIPixelTrackletsTruthMaker()
  theApp.TopAlg += [ "HIPixelTrackletsTruthMaker" ]

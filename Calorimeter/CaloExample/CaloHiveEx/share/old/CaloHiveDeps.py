#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#


#
## Algorithm Data Dependencies for CaloBottoUp.py
#

algDep = {}
algDep["SGInputLoader"] = []
algDep["EventCounter"] = []
algDep["xAODMaker::EventInfoCnvAlg"] = ["2101/McEventInfo"]
algDep["xAODMaker::TriggerTowerCnvAlg"] = ["2101/McEventInfo","6207/TriggerTowers"]
algDep["CaloCellMaker"] = ["2101/McEventInfo","2721/LArRawChannels","2927/TileRawChannelCnt","6207/TriggerTowers","1174216566/xAODTriggerTowers"]
algDep["CmbTowerBldr"] = ["2802/AllCalo"]
algDep["CaloClusterMakerSWCmb"] = ["2804/CombinedTower"]
algDep["CaloTopoCluster"] = ["1312841250/LArCalibrationHitActive","1312841250/LArCalibrationHitDeadMaterial","1312841250/LArCalibrationHitInactive","2802/AllCalo"]
algDep["StreamESD"] = ["2802/AllCalo","2804/CombinedTower", "1219821989/CombinedCluster","1219821989/CaloTopoCluster"]
algDep["AthOutSeq"] = []
algDep["AthRegSeq"] = []

# no different calorimeter clusters yet
if useJets:
    algDep["jetalg4"] = ["1219821989/CaloTopoCluster"]
    for x in range(0, nJets):
        app=str((0.5+x)/10)
        app=app.replace('.', 'p')
        # because of ParticleJetMap mess
        # algDep["jetalg"+app] = ["1219821989/CaloTopoCluster"]
        algDep["jetalg"+app] = ["1244316195/AntiKt4EMTopoJets"]
    algDep["StreamESD"] += ["1244316195/AntiKt4EMTopoJets"]
    for x in range(0, nJets):
        app=str((0.5+x)/10)
        app=app.replace('.', 'p')
        out="AntiKt"+app+"EMTopoJets"
        algDep["StreamESD"] += ["1244316195/"+out]

#
## xAODMaker::EventInfoCnvAlg produces:
#
#   "2101/McEventInfo" *
#   "38853911/EventInfoAux."
#   "45903698/EventInfo"
#
## CaloCellMaker produces:
#
#   "2802/AllCalo"
#   "2721/LArRawChannels" *
#   "2927/TileRawChannelCnt" *
#   "2931/MBTSContainer"
#
## CmbTowerBldr produces:
#
#   "2804/CombinedTower"
#
## CaloClusterMakerSWCmb produces:
#
#   "1219821989/CombinedCluster"
#   "1302486236/CombinedClusterAux."
#   "1134620868/CombinedCluster_links"
#
## CaloTopoCluster produces:
#
#    "1219821989/CaloCalTopoClusters"
#    "1302486236/CaloCalTopoClustersAux."
#    "1312841250/LArCalibrationHitInactive"
#    "1312841250/LArCalibrationHitActive"
#    "1312841250/LArCalibrationHitDeadMaterial"
#    "1219821989/CaloTopoCluster"
#    "1302486236/CaloTopoClusterAux."
#    "1134620868/CaloTopoCluster_links"
#    "1134620868/CaloCalTopoClusters_links"
#

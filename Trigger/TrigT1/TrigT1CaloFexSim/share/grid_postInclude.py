StreamAOD.ItemList+=["CaloCellContainer#AllCalo"]
StreamAOD.ItemList+=["CaloCellContainer#SCell"]
StreamAOD.ItemList+=["xAOD::TriggerTowerContainer#xAODTriggerTowers","xAOD::TriggerTowerAuxContainer#xAODTriggerTowersAux."]
StreamAOD.ItemList+=["xAOD::JGTowerContainer#JTower","xAOD::JGTowerAuxContainer#JTowerAux."]
StreamAOD.ItemList+=["xAOD::JGTowerContainer#GTower","xAOD::JGTowerAuxContainer#GTowerAux."]
algseq = CfgMgr.AthSequencer("AthAlgSeq")
algseq += CfgMgr.JGTowerMaker(useSCQuality = True) 

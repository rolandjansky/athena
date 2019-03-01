
def fillAOD (SuperCellContainer="SCellBCID", WriteAllCalo=False):

    StreamAOD.ItemList += ["CaloCellContainer#%s" % SuperCellContainer]
    if WriteAllCalo:
        StreamAOD.ItemList += [ "CaloCellContainer#AllCalo" ]

    # Run 2 towers
    StreamAOD.ItemList += [ "xAOD::TriggerTowerContainer#xAODTriggerTowers",
                            "xAOD::TriggerTowerAuxContainer#xAODTriggerTowersAux." ]                                                       

    # New JTowers and GTowers
    for tower in [ "JTower", "GTower" ]:
        StreamAOD.ItemList += [ "xAOD::JGTowerContainer#%s" % tower, "xAOD::JGTowerAuxContainer#%sAux.*" % tower ]

    # gFEX jets and MET
    StreamAOD.ItemList += [ "xAOD::JetRoIContainer#*",
                            "xAOD::JetRoIAuxContainer#*",
                            "xAOD::EnergySumRoI#*",
                            "xAOD::EnergySumRoIAuxInfo#*" ]

    # eFEX electrons
    for cl in [ "SCluster", "SClusterCl", "SClusterSimple", "SClusterBCIDCl", "LArLayer1Vars1Sig", "LArLayer1Vars2Sig", "LArLayer1Vars3Sig" ]:
        StreamAOD.ItemList += [ "xAOD::TrigEMClusterContainer#%s" % cl,
                                "xAOD::TrigEMClusterAuxContainer#%sAux." % cl ]

    # eFEX taus
    StreamAOD.ItemList += [ "xAOD::EmTauRoIContainer#SClusterTau",
                            "xAOD::EmTauRoIAuxContainer#SClusterTauAux.*" ]

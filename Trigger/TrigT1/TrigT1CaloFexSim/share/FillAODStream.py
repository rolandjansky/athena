
def fillOutStream ( stream, SuperCellContainer="SCellBCID", WriteAllCalo=False ):

    stream.ItemList += ["CaloCellContainer#%s" % SuperCellContainer]
    if WriteAllCalo:
        stream.ItemList += [ "CaloCellContainer#AllCalo" ]

    # Run 2 towers
    stream.ItemList += [ "xAOD::TriggerTowerContainer#xAODTriggerTowers",
                            "xAOD::TriggerTowerAuxContainer#xAODTriggerTowersAux." ]                                                       

    # New JTowers and GTowers
    for tower in [ "JTower", "GTower" ]:
        stream.ItemList += [ "xAOD::JGTowerContainer#%s" % tower, "xAOD::JGTowerAuxContainer#%sAux.*" % tower ]

    # gFEX jets and MET
    stream.ItemList += [ "xAOD::JetRoIContainer#*",
                            "xAOD::JetRoIAuxContainer#*",
                            "xAOD::EnergySumRoI#*",
                            "xAOD::EnergySumRoIAuxInfo#*" ]

    # eFEX electrons
    for cl in [ "SCluster", "SClusterCl", "SClusterSimple", "SClusterBCIDCl", "LArLayer1Vars1Sig", "LArLayer1Vars2Sig", "LArLayer1Vars3Sig" ]:
        stream.ItemList += [ "xAOD::TrigEMClusterContainer#%s" % cl,
                             "xAOD::TrigEMClusterAuxContainer#%sAux." % cl ]

    # eFEX taus
    stream.ItemList += [ "xAOD::EmTauRoIContainer#SClusterTau",
                         "xAOD::EmTauRoIAuxContainer#SClusterTauAux.*" ]


def fillAOD ( SuperCellContainer="SCellBCID", WriteAllCalo=False ):

    fillOutStream( StreamAOD, SuperCellContainer, WriteAllCalo )

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

def fillOutStream ( stream, SuperCellContainer="SCellBCID", WriteAllCalo=False, otherOutput=[] ):

    itemList = []

    itemList += ["CaloCellContainer#%s" % SuperCellContainer]
    if WriteAllCalo:
        itemList += [ "CaloCellContainer#AllCalo" ]

    # Run 2 towers
    itemList += [ "xAOD::TriggerTowerContainer#xAODTriggerTowers",
                  "xAOD::TriggerTowerAuxContainer#xAODTriggerTowersAux." ]                                                  

    # New JTowers and GTowers
    for tower in [ "JTowers", "GTowers", "GCaloTowers", "JTowerRhoSubtracted", "pu_subTowers"]:
        itemList += [ "xAOD::JGTowerContainer#%s" % tower, 
                      "xAOD::JGTowerAuxContainer#%sAux.*" % tower ]

    # gFEX jets and MET
    itemList += [ "xAOD::JetRoIContainer#*",
                  "xAOD::JetRoIAuxContainer#*",
                  "xAOD::EnergySumRoI#*",
                  "xAOD::EnergySumRoIAuxInfo#*" ]

    # eFEX electrons
    for cl in [ "SCluster", "SClusterCl", "SClusterSimple", "SClusterBCIDCl", 
                "LArLayer1Vars1Sig", "LArLayer1Vars2Sig", "LArLayer1Vars3Sig" ]:
        itemList += [ "xAOD::TrigEMClusterContainer#%s" % cl,
                      "xAOD::TrigEMClusterAuxContainer#%sAux." % cl ]

    # eFEX taus
    itemList += [ "xAOD::EmTauRoIContainer#SClusterTau",
                  "xAOD::EmTauRoIAuxContainer#SClusterTauAux.*" ]

    itemList += otherOutput
    

    from OutputStreamAthenaPool.MultipleStreamManager import AugmentedPoolStream
    if isinstance(stream,AugmentedPoolStream):
        stream.AddItem(itemList)
    else:
        stream.ItemList += itemList

def fillAOD ( SuperCellContainer="SCellBCID", WriteAllCalo=False, otherOutput=[] ):
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    stream = MSMgr.GetStream( "StreamAOD" )
    fillOutStream( stream, SuperCellContainer, WriteAllCalo, otherOutput )
    return stream

def fillESD ( SuperCellContainer="SCellBCID", WriteAllCalo=False, otherOutput=[] ):
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    stream = MSMgr.GetStream( "StreamESD" )
    fillOutStream( stream, SuperCellContainer, WriteAllCalo, otherOutput )
    return stream

def fillRDO ( SuperCellContainer="SCellBCID", WriteAllCalo=False, otherOutput=[] ):
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    stream = MSMgr.GetStream( "StreamRDO" )
    fillOutStream( stream, SuperCellContainer, WriteAllCalo, otherOutput )
    return stream


def printStreamingInfo():
    from AthenaCommon.Logging import logging
    log = logging.getLogger( 'TrigT1CaloFexSim.SetupOutput.StreamingInfo' )
    log.setLevel(logging.INFO)

    log.info("MultiStreamManager")
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

    import AthenaCommon.Configurable as Configurable
    configLogLevel = Configurable.log.level
    Configurable.log.level = logging.INFO # to show more than just the title of the algs
    MSMgr.Print()
    Configurable.log.level = configLogLevel

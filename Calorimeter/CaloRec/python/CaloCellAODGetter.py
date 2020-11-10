# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# This can be used to write a sparse cell container into the AOD, provided
# that this jO fragment is called between the ESD and AOD writing, the
# ElementLinks of the cluster are updated and the corresponding CellLink 
# Container is written to the AOD as well. 
#
# This function to use to add cell of a given cluster in AOD
# usage:
# from CaloRec.CaloCellAODGetter import addClusterToCaloCellAOD
# addClusterToCaloCellAOD("MuonClusterCollection")

def addClusterToCaloCellAOD(clustersInputName):
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    samplings = ["TileGap1", "TileGap2", "TileGap3", "TileBar0","TileExt0", "HEC0"]
    from CaloRec.CaloRecConf import CaloThinCellsByClusterAlg
    alg = CaloThinCellsByClusterAlg ('CaloThinCellsByClusterAlg_' + clustersInputName,
                                     StreamName = 'StreamAOD',
                                     Clusters = clustersInputName,
                                     SamplingCellsName = samplings,
                                     Cells = 'AllCalo')
    topSequence += alg 

    # Include all TileGap3 cells.
    from CaloRec.CaloRecConf import CaloThinCellsBySamplingAlg
    salg = CaloThinCellsBySamplingAlg ('CaloThinCellsBySamplingAlg_TileGap3',
                                       StreamName = 'StreamAOD',
                                       SamplingCellsName = ['TileGap3'],
                                       Cells = 'AllCalo')
    if salg not in topSequence:
        topSequence += salg

    # Originally, calo cell thinning worked by writing out a new cell container
    # and mangling links to point to it.  Now we use standard MT thinning,
    # so we just write out the default cell container with thinning.
    # Make an alias to the name we used to use for the cells in case
    # downstream code is using it.
    from CaloRec.CaloRecConf import CaloCellContainerAliasAlg
    aalg = CaloCellContainerAliasAlg ('CaloCellContainerAliasAlg_AOD',
                                      Alias = 'AODCellContainer',
                                      Cells = 'AllCalo')
    if aalg not in topSequence:
        topSequence += aalg

    return

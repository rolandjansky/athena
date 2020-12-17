# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#==========================================================================================
#This is basically based in the  Calorimeter/CaloRec/python/CaloCellAODGetter.py (used for ESD to AOD)
#It is  modified as  to be usable in derivation.
#
# The understanding is that in derivation we will run multiple "instances"
# of this algorithm for the different derivations. These could be scheduled in a "train"
# one after the other in the same AlgSequence. This is unlike ESD->AOD when we 
# need only one instance.
#
# Usage : 
#
#from DerivationFrameworkCalo.CaloCellDFGetter import thinCaloCellsForDF
#thinCaloCellsForDF (inputClusterKeys=["egammaClusters"],
#                    streamName = Stream.Name,
#                    outputCellKey="DFEGAM1CellContainer")
#
# The output cell key is used only to create an alias for the thinned cells,
# and may be omitted if this is not required.
#
#Using a custom sequence
#
#DESDM_EGAMMA_Seq = CfgMgr.AthSequencer("DESDM_EGAMMA_Seq")
#from DerivationFrameworkCalo.CaloCellDFGetter import thinCaloCellsForDF
#thinCaloCellsForDF (inputClusterKeys=["egammaClusters"],
#                    streamName = Stream.Name,
#                    outputCellKey="DFEGAM1CellContainer",
#                    sequence=DESDM_EGAMMA_Seq)
#
#
# Do not forget to write out the _links
#
#EGAM1Stream.AddItem("CaloClusterCellLinkContainer#egammaClusters_links")

# Also in principle everything has to belong to a cluster. 
# For example in ESD to AOD we create 7x11 cluster
# to write extra cell for e/gamma
#==========================================================================================

from AthenaCommon.AlgSequence import AlgSequence
topSequence=AlgSequence()

def thinCaloCellsForDF (inputClusterKeys,
                        streamName,
                        inputCellKey = 'AllCalo',
                        outputCellKey = None,
                        sequence = topSequence):

    from CaloRec.CaloRecConf import CaloThinCellsByClusterAlg

    samplings = ["TileGap1", "TileGap2", "TileGap3", "TileBar0","TileExt0", "HEC0"]
    for clkey in inputClusterKeys:
        alg = CaloThinCellsByClusterAlg ('CaloThinCellsByClusterAlg_' + clkey + '_' + streamName,
                                         StreamName = streamName,
                                         Clusters = clkey,
                                         SamplingCellsName = samplings,
                                         Cells = inputCellKey)
        if alg not in sequence:
            sequence += alg

    from CaloRec.CaloRecConf import CaloThinCellsBySamplingAlg
    salg = CaloThinCellsBySamplingAlg ('CaloThinCellsBySamplingAlg_TileGap3_' + streamName,
                                       StreamName = streamName,
                                       SamplingCellsName = ['TileGap3'],
                                       Cells = inputCellKey)
    if salg not in sequence:
        sequence += salg

    # Originally, calo cell thinning worked by writing out a new cell container
    # and mangling links to point to it.  Now we use standard MT thinning,
    # so we just write out the default cell container with thinning.
    # Make an alias to the name we used to use for the cells in case
    # downstream code is using it.
    if outputCellKey is not None:
        from CaloRec.CaloRecConf import CaloCellContainerAliasAlg
        aalg = CaloCellContainerAliasAlg ('CaloCellContainerAliasAlg_' + outputCellKey,
                                          Alias = outputCellKey,
                                          Cells = inputCellKey)
        if aalg not in sequence:
            sequence += aalg

    return

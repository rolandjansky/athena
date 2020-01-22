# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#Helper function

def CellCommonThinning (Stream) :
    from DerivationFrameworkCalo.CaloCellDFGetter import thinCaloCellsForDF
    thinCaloCellsForDF (streamName = Stream.Name,
                        inputClusterKeys=["egammaClusters"],
                        outputCellKey="DFEGAMCellContainer")

    Stream.AddItem("CaloClusterCellLinkContainer#egammaClusters_links")

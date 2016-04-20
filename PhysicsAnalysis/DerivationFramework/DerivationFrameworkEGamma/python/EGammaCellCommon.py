# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Helper function

def CellCommonThinning (Stream) :
    from DerivationFrameworkCalo.CaloCellDFGetter import CaloCellDFGetter
    theCaloCellDFGetter = CaloCellDFGetter(inputClusterKeys=["egammaClusters"],
                                           outputCellKey="DFEGAMCellContainer")

    Stream.AddItem("CaloClusterCellLinkContainer#egammaClusters_links")
    Stream.AddItem("CaloCellContainer#DFEGAMCellContainer")

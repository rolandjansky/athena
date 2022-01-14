#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

""" ComponentAccumulator-style configuration for common calorimeter sequences

These sequences must be common between the jet and MET slices. For this purpose
I have given the configuration functions very few arguments so that they can be
easily merged, but we can revisit this design choice if necessary.
"""

from .FullScanDefs import caloFSRoI, fs_cells, em_clusters, lc_clusters
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from AthenaConfiguration.AccumulatorCache import AccumulatorCache

@AccumulatorCache
def FSCaloCellCfg(flags):
    """ Create the full-scan cells """
    from TrigCaloRec.TrigCaloRecConfig import hltCaloCellMakerCfg

    return hltCaloCellMakerCfg(flags, name="HLTCaloCellMaker_FS", roisKey=caloFSRoI)

@AccumulatorCache
def CaloClusterCfg(flags, doLCCalib=False):
    """ Create the clusters """
    acc = ComponentAccumulator()
    if doLCCalib:
        # First get the uncalibrated clusters
        acc.merge(CaloClusterCfg(flags, doLCCalib=False))
        from TrigCaloRec.TrigCaloRecConfig import hltCaloTopoClusterCalibratorCfg

        # Then calibrate them
        acc.merge(
            hltCaloTopoClusterCalibratorCfg(
                flags,
                "HLTCaloClusterCalibratorLCFS",
                clustersin=em_clusters,
                clustersout=lc_clusters,
                OutputCellLinks=lc_clusters + "_cellLinks",
            )
        )
    else:
        # First merge in the cells
        acc.merge(FSCaloCellCfg(flags))
        from TrigCaloRec.TrigCaloRecConfig import hltTopoClusterMakerCfg

        # Then build the clusters
        acc.merge(
            hltTopoClusterMakerCfg(
                flags,
                "HLTCaloClusterMakerFS",
                clustersKey=em_clusters,
                cellsKey=fs_cells,
            )
        )
    return acc

def HICaloTowerCfg(flags):
    """ Create the towers for heavy ion """
    acc = ComponentAccumulator()

    acc.merge(FSCaloCellCfg(flags))

    from TrigCaloRec.TrigCaloRecConfig import hltHICaloTowerMakerCfg
    # Then build the clusters
    acc.merge(
        hltHICaloTowerMakerCfg(
            flags,
            "HLTHICaloTowerMakerFS",
            clustersKey=caloFSRoI,
            cellsKey=fs_cells,
        )
    )
    return acc

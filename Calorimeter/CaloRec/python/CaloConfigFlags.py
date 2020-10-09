# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createCaloConfigFlags(): 
    ccf=AthConfigFlags()

    #CaloNoise Flags     
    ccf.addFlag("Calo.Noise.fixedLumiForNoise",-1)     
    ccf.addFlag("Calo.Noise.useCaloNoiseLumi",True) 

    #CaloCell flags     
    ccf.addFlag("Calo.Cell.doPileupOffsetBCIDCorr", True)     
    ccf.addFlag("Calo.Cell.doDeadCellCorr",True)     
    ccf.addFlag("Calo.Cell.doPedestaCorr", lambda prevFlags: not prevFlags.Input.isMC)
    ccf.addFlag("Calo.Cell.doEnergyCorr",lambda prevFlags: not prevFlags.Input.isMC and not prevFlags.Common.isOnline)
    ccf.addFlag("Calo.Cell.doTimeCorr", lambda prevFlags: not prevFlags.Input.isMC and not prevFlags.Common.isOnline)

    #TopoCluster Flags:     
    ccf.addFlag("Calo.TopoCluster.doTwoGaussianNoise",True)     
    ccf.addFlag("Calo.TopoCluster.doTreatEnergyCutAsAbsolute",False)     
    ccf.addFlag("Calo.TopoCluster.doTopoClusterLocalCalib",True)

    #### Cluster correction flags:
    # If true, then reweight cells to prevent double-counting between clusters.
    ccf.addFlag ('Calo.ClusterCorrection.doSlidingWindowCellWeights', False)

    # If true, then among groups of clusters very close to each other,
    # remove all clusters but one.
    ccf.addFlag ('Calo.ClusterCorrection.doSlidingWindowRemoveDuplicates', False)

    # If true, remove clusters with energy below a threshold.
    ccf.addFlag ('Calo.ClusterCorrection.doSlidingWindowRemoveBad', True)

    # Defines from where we should try to construct the tools.
    # May be one of 'jo', 'pool', 'cool'.
    # May also be a list of them, to try in order.
    # It may also be set to a string defined in poolfiles to try to
    # read all tools from that file (except those marked as nopool).
    ccf.addFlag ('Calo.ClusterCorrection.defaultSource',
                 ['cool', 'pool', 'jo'])

    # Override the default correction version for sliding window clusters.
    ccf.addFlag ('Calo.ClusterCorrection.caloSwWhichCorrection', '')

    # Override the default correction version for EM topo clusters.
    ccf.addFlag ('Calo.ClusterCorrection.caloTopoEMWhichCorrection', '')

    # This flag allows setting additional correction arguments for
    # sliding window clusters.  It should normally be empty, but it may be
    # set for testing new correction versions.
    ccf.addFlag ('Calo.ClusterCorrection.caloSwCorrectionArgs', {})

    # This flag allows setting additional correction arguments for
    # EM topo clusters.  It should normally be empty, but it may be
    # set for testing new correction versions.
    ccf.addFlag ('Calo.ClusterCorrection.caloTopoEMCorrectionArgs', {})

    # Correction generation string that's embedded in database tags.
    # This string gets changed every time the database is updated.
    ccf.addFlag ('Calo.ClusterCorrection.caloSwGeneration', '')

    # Correction generation string that's embedded in database tags.
    # This string gets changed every time the database is updated.
    ccf.addFlag ('Calo.ClusterCorrection.caloTopoEMGeneration', '')

    # This flag allows overriding the subdetector name used for
    # database connections.  It's a dictionary, the keys of which are the
    # correction classes (normally either `CaloSwClusterCorrections'
    # or `EMTopoClusterCorrections').  If the correction class isn't
    # found, we use a key of None instead as a default.
    #
    # The subdetector name is normally `CALO'.  However, it can be set to the
    # name of a local sqlite file to force reading COOL information from there.
    ccf.addFlag ('Calo.ClusterCorrection.dbSubdetName', {None : 'CALO'})

    return ccf

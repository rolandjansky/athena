# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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

    return ccf

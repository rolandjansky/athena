# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createPFConfigFlags():
    pfConfigFlags=AthConfigFlags()

    pfConfigFlags.addFlag("PF.EOverPMode",False) #This defines whether we use the standard reconstruction or dedicated e/p mode used to measure e/p reference values
    pfConfigFlags.addFlag("PF.addClusterMoments",True) #This defines whether we add the cluster moments to neutral PFO
    pfConfigFlags.addFlag("PF.useClusterMoments",True) #This defines whether we add a full list of cluster moments or not. Currently only the HLT config makes use of this flag, it is not used for offline config.
    pfConfigFlags.addFlag("PF.useCalibHitTruthClusterMoments",False) #This defines whether we calculate the calibration hit moments - only possible on if running from special calibraiton hit ESD samples.
    pfConfigFlags.addFlag("PF.recoverIsolatedTracks",False) #Defines whether we should apply the split shower recovery algorithm on isolated tracks
    pfConfigFlags.addFlag("PF.useUpdated2015ChargedShowerSubtraction",True) #Toggle whether to use updated 2015 charged shower subtraction, which disables the shower subtraction in high calorimeter energy density regions  
    pfConfigFlags.addFlag("PF.useElPhotLinks",True)
    pfConfigFlags.addFlag("PF.useMuLinks",True)
    pfConfigFlags.addFlag("PF.useOldPFO",False)
    pfConfigFlags.addFlag("PF.useRecExCommon",False) #Toggle whether we are in the RecExCommon config or not.
    
    return pfConfigFlags
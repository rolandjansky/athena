# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createPFConfigFlags():
    pfConfigFlags=AthConfigFlags()

    pfConfigFlags.addFlag("PF.EOverPMode",False) #This defines whether we use the standard reconstruction or dedicated e/p mode used to measure e/p reference values
    pfConfigFlags.addFlag("PF.useCalibHitTruthMoments",False) #This defines whether we calculate the calibraiton hit moments - only possible on if running from special calibraiton hit ESD samples.
    pfConfigFlags.addFlag("PF.recoverIsolatedTracks",False) #Defines whether we should apply the split shower recovery algorithm on isolated tracks
    
    return pfConfigFlags

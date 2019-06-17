# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createTileConfigFlags():

     tcf = AthConfigFlags()

     tcf.addFlag('Tile.doFlat', False)
     tcf.addFlag('Tile.doFit', False)
     tcf.addFlag('Tile.doFitCOOL', False)
     tcf.addFlag('Tile.doOpt', False)
     tcf.addFlag('Tile.doOpt2', False)
     tcf.addFlag('Tile.doQIE', False)
     tcf.addFlag('Tile.doOF1', False)
     tcf.addFlag('Tile.doOptATLAS', True)
     tcf.addFlag('Tile.doManyApms', False)
     tcf.addFlag('Tile.doMF', False)
     tcf.addFlag('Tile.NoiseFilter', -1)
     tcf.addFlag('Tile.RunType', 1) # 0 = Unknown, 1 = Physics, 2 = Laser, 4 = Pedestal, 8 = CIS)
     tcf.addFlag('Tile.calibrateEnergy', False)
     tcf.addFlag('Tile.correctTime', False)
     tcf.addFlag('Tile.correctTimeNI', True)
     tcf.addFlag('Tile.correctAmplitude', True)
     tcf.addFlag('Tile.AmpMinForAmpCorrection', 15.0)
     tcf.addFlag('Tile.TimeMinForAmpCorrection', lambda prevFlags : (prevFlags.Beam.BunchSpacing / -2.))
     tcf.addFlag('Tile.TimeMaxForAmpCorrection', lambda prevFlags : (prevFlags.Beam.BunchSpacing / 2.))
     tcf.addFlag('Tile.OfcFromCOOL', True)
     tcf.addFlag('Tile.BestPhaseFromCOOL', True)
     tcf.addFlag('Tile.readDigits', True)
     tcf.addFlag('Tile.doOverflowFit', True)
     tcf.addFlag('Tile.zeroAmplitudeWithoutDigits', True)
     tcf.addFlag('Tile.correctPedestalDifference', True)
     tcf.addFlag('Tile.RawChannelContainer', lambda prevFlags : ('TileRawChannelCnt' if prevFlags.Input.isMC else 'TileRawChannelFixed'))

     return tcf

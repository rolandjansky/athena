# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createTileConfigFlags():

     tcf = AthConfigFlags()

     tcf.addFlag('Tile.doQIE', False)
     tcf.addFlag('Tile.doManyAmps', False)
     tcf.addFlag('Tile.doFlat', False)
     tcf.addFlag('Tile.doFit', False)
     tcf.addFlag('Tile.doFitCOOL', False)
     tcf.addFlag('Tile.doMF', False)
     tcf.addFlag('Tile.doOF1', False)
     tcf.addFlag('Tile.doOpt2', _doOpt2)
     tcf.addFlag('Tile.doOptATLAS', _doOptATLAS)
     tcf.addFlag('Tile.NoiseFilter', lambda prevFlags : -1 if prevFlags.Input.isMC else 1)
     tcf.addFlag('Tile.RunType', _getRunType)
     tcf.addFlag('Tile.correctTime', lambda prevFlags : ('collisions' in prevFlags.Beam.Type))
     tcf.addFlag('Tile.correctTimeNI', True)
     tcf.addFlag('Tile.correctAmplitude', True)
     tcf.addFlag('Tile.AmpMinForAmpCorrection', 15.0)
     tcf.addFlag('Tile.TimeMinForAmpCorrection', lambda prevFlags : (prevFlags.Beam.BunchSpacing / -2.))
     tcf.addFlag('Tile.TimeMaxForAmpCorrection', lambda prevFlags : (prevFlags.Beam.BunchSpacing / 2.))
     tcf.addFlag('Tile.OfcFromCOOL', True)
     tcf.addFlag('Tile.BestPhaseFromCOOL', lambda prevFlags : ('collisions' in prevFlags.Beam.Type))
     tcf.addFlag('Tile.readDigits', lambda prevFlags : not prevFlags.Input.isMC)
     tcf.addFlag('Tile.doOverflowFit', True)
     tcf.addFlag('Tile.zeroAmplitudeWithoutDigits', _zeroAmplitudeWithouDigits)
     tcf.addFlag('Tile.correctPedestalDifference', _correctPedestalDifference)
     tcf.addFlag('Tile.correctTimeJumps', _correctTimeJumps)
     tcf.addFlag('Tile.RawChannelContainer', _getRawChannelContainer)
     tcf.addFlag('Tile.useDCS', _useDCS)
     tcf.addFlag('Tile.TimingType', _getTimingType)

     return tcf



def _doOpt2ByDefault(prevFlags):
     runNumber = prevFlags.Input.RunNumber[0]
     # Run Optimal Filter with iterations (Opt2) by default,
     # both for cosmics and collisions data before 2011
     if not prevFlags.Input.isMC and runNumber > 0 and runNumber < 171194:
          return True
     # Run Optimal Filter without iterations (OptATLAS)
     # for collisions (data in 2011 and later and MC)
     elif 'collisions' in prevFlags.Beam.Type:
          return False  # Use OF without iterations for collisions
     else:
          return True


def _doExtraMethods(prevFlags):
     # Check if any Optimal Filter, but Opt2 and OptATLAS, is already scheduled
     if (prevFlags.Tile.doQIE or prevFlags.Tile.doManyAmps or prevFlags.Tile.doFlat
         or prevFlags.Tile.doFit or prevFlags.Tile.doFitCOOL or prevFlags.Tile.doMF
         or prevFlags.Tile.doOF1):
          return True
     else:
          return False

def _doOpt2(prevFlags):
     # Do not run Opt2 method if any Optimal Filter is already scheduled
     if _doExtraMethods(prevFlags) :
          return False
     # If no extra Optimal Filters are scheduled yet check if Opt2 should be used by default
     else:
          if _doOpt2ByDefault(prevFlags):
               _flags = prevFlags.clone()
               _flags.Tile.doOpt2 = True
               # Opt2 should be run by default, but check if OptATLAS is already scheduled
               return not _flags.Tile.doOptATLAS
          else:
               return False


def _doOptATLAS(prevFlags):
     # Do not run OptATLAS method if any Optimal Filter is alredy scheduled
     if _doExtraMethods(prevFlags) :
          return False
     # If no Optimal Filters are scheduled yet check if OptATLAS should be used by default
     else:
          if not _doOpt2ByDefault(prevFlags):
               # OptATLAS should be run by default, but check if Opt2 is already scheduled
               _flags = prevFlags.clone()
               _flags.Tile.doOptATLAS = True
               return not _flags.Tile.doOpt2
          else:
               return False


def _zeroAmplitudeWithouDigits(prevFlags):
     if not prevFlags.Input.isMC:
          runNumber = prevFlags.Input.RunNumber[0]
          # Use OF1 corrections only for years 2015 - 2016
          return runNumber > 269101 and runNumber < 324320
     else:
          return False


def _correctPedestalDifference(prevFlags):
     if not prevFlags.Common.isOnline:
          return _zeroAmplitudeWithouDigits(prevFlags)
     else:
          return False


def _correctTimeJumps(prevFlags):
     if not (prevFlags.Input.isMC or prevFlags.Overlay.DataOverlay) and prevFlags.Input.Format == 'BS':
          return True
     else:
          return False


def _getRunType(prevFlags):
     # Tile run types: UNDEFINED, PHY, PED, LAS, BILAS, CIS, MONOCIS
     from AthenaConfiguration.AutoConfigFlags import GetFileMD
     if not prevFlags.Input.isMC and 'calibration_Tile' in GetFileMD(prevFlags.Input.Files).get('triggerStreamOfFile', ''):
          return 'UNDEFINED'
     else:
          return 'PHY'


def _useDCS(prevFlags):
     if not (prevFlags.Common.isOnline or prevFlags.Input.isMC):
          runNumber = prevFlags.Input.RunNumber[0]
          # Use Tile DCS only for 2011 data and later, excluding shutdown period
          return (runNumber > 171194 and runNumber < 222222) or runNumber > 232498
     else:
          return False


def _getRawChannelContainer(prevFlags):

     rawChannelContainer = 'UNDEFINED'

     if prevFlags.Tile.doQIE:
          rawChannelContainer = 'TileRawChannelQIE'
     if prevFlags.Tile.doManyAmps:
          rawChannelContainer = 'TileRawChannelManyAmp'
     if prevFlags.Tile.doFlat:
          rawChannelContainer = 'TileRawChannelFlat'
     if prevFlags.Tile.doFit:
          rawChannelContainer = 'TileRawChannelFit'
     if prevFlags.Tile.doFitCOOL:
          rawChannelContainer = 'TileRawChannelFitCool'
     if prevFlags.Tile.doMF:
          rawChannelContainer = 'TileRawChannelMF'
     if prevFlags.Tile.doOF1:
          rawChannelContainer = 'TileRawChannelOF1'
     if prevFlags.Tile.doOpt2:
          rawChannelContainer = 'TileRawChannelOpt2'
     if prevFlags.Tile.doOptATLAS:
          if not (prevFlags.Input.isMC or prevFlags.Overlay.DataOverlay) and prevFlags.Input.Format == 'BS':
               rawChannelContainer = 'TileRawChannelFixed'                                                   
          else:                               
               rawChannelContainer = 'TileRawChannelCnt'

     return rawChannelContainer


def _getTimingType(prevFlags):
     # Tile timing types: PHY, LAS, GAP/LAS, CIS
     timingType = {'PHY' : 'PHY', 'PED' : 'PHY',
                   'LAS' : 'LAS', 'BILAS' : 'LAS', 'GAPLAS' : 'GAP/LAS',
                   'CIS' : 'CIS', 'MONOCIS' : 'CIS'}

     return timingType.get(prevFlags.Tile.RunType, 'UNDEFINED')

if __name__=="__main__":
     import sys
     from AthenaConfiguration.AllConfigFlags import ConfigFlags

     from AthenaConfiguration.TestDefaults import defaultTestFiles
     ConfigFlags.Input.Files = defaultTestFiles.RAW

     if len(sys.argv) > 1:
          ConfigFlags.fillFromArgs()

     ConfigFlags.lock()

     ConfigFlags.needFlagsCategory('Tile')
     ConfigFlags.initAll()
     ConfigFlags.dump()

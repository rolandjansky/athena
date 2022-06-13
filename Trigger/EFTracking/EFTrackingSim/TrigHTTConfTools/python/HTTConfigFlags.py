#Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def createHTTConfigFlags():

    from AthenaConfiguration.AthConfigFlags import AthConfigFlags
    cf = AthConfigFlags()

    # common
    cf.addFlag('name', '')
    cf.addFlag('wrapperFileName', 'HTTWrapper.root')

    # input
    cf.addFlag('firstInputToolN', 1)
    cf.addFlag('realHitsOverlay', False)
    cf.addFlag('hitFiltering', False)
    cf.addFlag('spacePoints', False)
    cf.addFlag('spacePointFiltering', False)
    cf.addFlag('writeOutputData', True)

    # road finding selection
    cf.addFlag('houghRootoutput', False)
    cf.addFlag('hough', False)
    cf.addFlag('hough1D', False)

    # NN filtering
    cf.addFlag('trackNNAnalysis', False)

    # overlap removal
    cf.addFlag('doFastOR', False)

    # hough
    cf.addFlag('xVar', 'phi')
    cf.addFlag('yVar', 'q/pt')
    cf.addFlag('qptMin', -1.0)
    cf.addFlag('qptMax', 1.0)
    cf.addFlag('d0Slices', [])
    cf.addFlag('slicing', True)
    cf.addFlag('localMaxWindowSize', 0)
    cf.addFlag('roadSliceOR', False)
    cf.addFlag('fieldCorrection', True)
    cf.addFlag('phiMin', 0.0)
    cf.addFlag('phiMax', 0.0)
    cf.addFlag('xBins', 0)
    cf.addFlag('yBins', 0)
    cf.addFlag('xBufferBins', 0)
    cf.addFlag('yBufferBins', 0)
    cf.addFlag('threshold', [7])
    cf.addFlag('convolution', [])
    cf.addFlag('convSizeX', 0)
    cf.addFlag('convSizeY', 0)
    cf.addFlag('hitExtendX', [])

    cf.addFlag('doMajority', 1)

    cf.addFlag('doTracking', False)
    cf.addFlag('outputHitTxt', False)

    # performance monitoring
    cf.addFlag('barcodeFracMatch', 0.5)

    # track Fitter
    cf.addFlag('chi2DoFRecoveryMin', 40)
    cf.addFlag('chi2DoFRecoveryMax', 100000000000)
    cf.addFlag('nHitsNoRecovery', -1)
    cf.addFlag('guessHits', True)
    cf.addFlag('doMissingHitsChecks', False)
    cf.addFlag('idealCoordFitType', 0)
    cf.addFlag('doDeltaGPhis', False)

    # second stage fitting
    cf.addFlag('secondStage', False)

    # fast monitoring
    cf.addFlag('fastMon', False)
    cf.addFlag('canExtendHistRanges', False)
    cf.addFlag('dataClass', 2)

    # lrt settings
    cf.addFlag('lrt', False)
    cf.addFlag('lrtUseBasicHitFilter', False)
    cf.addFlag('lrtUseMlHitFilter', False)
    cf.addFlag('lrtUseStraightTrackHT', False)
    cf.addFlag('lrtUseDoubletHT', False)
    cf.addFlag('lrtDoubletD0Range', 120.0)
    cf.addFlag('lrtDoubletD0Bins', 216)
    cf.addFlag('lrtDoubletQptRange', 0.02)
    cf.addFlag('lrtDoubletQptBins', 216)
    cf.addFlag('lrtMonPhiRange', (0.2, 0.5))
    cf.addFlag('lrtMonD0Range', (-100,100))
    cf.addFlag('lrtMonZ0Range', (-300,300))

    return cf


def createHough1dHTTConfigFlags():
    cf = createHTTConfigFlags()

    cf.name = 'hough_1d'

    cf.hough1D =  True

    cf.phiMax = 0.8
    cf.xBins = 200
    cf.hitExtendX = [1] * 8

    cf.addFlag('phiRangeCut', True)
    cf.addFlag('splitpt', 1)

    cf.outputHitTxt = None

    return cf


def createHoughHTTConfigFlags():
    cf = createHTTConfigFlags()

    cf.name = 'hough'
    cf.hough = True

    cf.phiMin = 0.3
    cf.phiMax = 0.5
    cf.xBins = 216
    cf.yBins = 216
    cf.xBufferBins = 6
    cf.yBufferBins = 2
    cf.addFlag('combineLayers', [])
    cf.addFlag('scale', [])
    cf.hitExtendX = [2,1,0,0,0,0,0,0]

    cf.addFlag('lrtSkipHitFiltering', False)
    cf.addFlag('lrtPtmin', 5)
    cf.addFlag('allowHighTruthBarcode', False)
    cf.addFlag('mLRTpdgID', 0)
    cf.addFlag('lrtStraighttrackXVar', 'phi')
    cf.addFlag('lrtStraighttrackYVar', 'd0')
    cf.addFlag('lrtStraighttrackPhiMin', 0.3)
    cf.addFlag('lrtStraighttrackPhiMax', 0.5)
    cf.addFlag('lrtStraighttrackD0Min', -300.0)
    cf.addFlag('lrtStraighttrackD0Max', 300.0)
    cf.addFlag('lrtStraighttrackXBins', 216)
    cf.addFlag('lrtStraighttrackYBins', 216)
    cf.addFlag('lrtStraighttrackXBufferBins', 6)
    cf.addFlag('lrtStraighttrackYBufferBins', 2)
    cf.addFlag('lrtStraighttrackSlicing', True)
    cf.addFlag('lrtStraighttrackThreshold', [7])
    cf.addFlag('lrtStraighttrackConvolution', [])
    cf.addFlag('lrtStraighttrackCombineLayers', [0,1,2,3,4,5,6,7])
    cf.addFlag('lrtStraighttrackScale', [1,1,1,1,1,1,1,1])
    cf.addFlag('lrtStraighttrackConvSizeX', 0)
    cf.addFlag('lrtStraighttrackConvSizeY', 0)
    cf.addFlag('lrtStraighttrackHitExtendX', [2,1,0,0,0,0,0,0])
    cf.addFlag('lrtStraighttrackStereo', False)
    cf.addFlag('lrtStraighttrackLocalMaxWindowSize', 0) 

    return cf 


def createDev21_02_15_HTTConfigFlags():
    cf = createHTTConfigFlags()

    cf.name = 'dev_21-02-15'
    cf.phiMin = 0.3
    cf.phiMax = 0.5
    cf.xBins = 64
    cf.yBins = 216
    cf.xBufferBins = 2
    cf.yBufferBins = 2
    cf.threshold = [70]
    cf.convolution = [1, 10, 1]
    cf.addFlag('combineLayers', [])
    cf.addFlag('scale', [])
    cf.convSizeX = 3
    cf.convSizeY = 1

    cf.doMajority = 0
    cf.doTracking = True

    return cf


#####################################################################
#####################################################################
#####################################################################

if __name__ == "__main__":

  flags = createHTTConfigFlags()

  assert flags.firstInputToolN == 1 , "default firstInputToolN  is wrong"
  assert flags.barcodeFracMatch == 0.5, "default barcodeFracMatch  is wrong"
  assert flags.fastMon is False , "default fastMon is wrong"
  assert flags.lrtMonZ0Range ==  (-300,300), "default lrtMonZ0Rang is wrong"

  print( "allok" )   








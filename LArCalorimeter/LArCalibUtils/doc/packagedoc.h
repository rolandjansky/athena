/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page LArCalibUtils_page LArCalibUtils
@author Marco.Delmastro@cern.ch

@section LArCalibUtils_LArCalibUtilsIntro Introduction

This package containes the algorithms to reconstruct the LAr calibration runs
(pedestal, noise autocorrelation, ramp, delay), fill the relative electronic
calibration condition objects, and to manipulate and compute the derived
constants (e.g. ionization pulse, optimal filtering coefficients).

@todo

- This package still contains some tools directly used in physics data
  reconstruction (LArADC2MeVTool, LArAutoCorrNoiseTool, LArOFCTool) that  should
  be moved elsewhere (e.g. <a href=http://isscvs.cern.ch/cgi-bin/viewcvs-all.cgi/offline/LArCalorimeter/LArCondUtils/?cvsroot=atlas>LArCalorimeter/LArCondUtils</a>). 
  The package itself should probably be moved from the AtlasEvent project to AtlasReconstruction.
- A serious cleanup/update of the exaple jobOptions is needed.
- All the algorithms need to be migrated to configurable.
- NovaIOV support should be removed.
- Doxygen description of classes and methods is missing.

@section LArCalibUtils_LArCalibUtilsOverview Class Overview
  
  The LArCalibUtils package contains of following classes:

  - LArADC2MeVTool
  - LArAutoCorr
  - LArAutoCorrMaker
  - LArAutoCorrNoiseTool
  - LArAutoCorrTotalTool
  - LArCalibDigitMaker
  - LArCaliWaveAverage
  - LArCaliWaveBuilder
  - LArCaliWaveBuilderXtalk
  - LArCaliWaveMerger
  - LArDeltaRespPredictor
  - LArDeltaRespTool
  - LArDigitOscillationCorrTool
  - LArDumpShapes
  - LArFill
  - LArHVCorrTool
  - LArMasterWaveBuilder
  - LArOFCAlg
  - LArOFCTool
  - LArPedestal
  - LArPedestalInPhysicsMaker
  - LArPedestalMaker
  - LArPhaseToolConst
  - LArPhaseToolMC
  - LArPhaseToolTB
  - LArPhysWaveBuilderFillDB
  - LArPhysWaveBuilder
  - LArPhysWavePredictor
  - LArPhysWaveShifter
  - LArPhysWaveTool
  - LArRampBuilder
  - LArRampCorr
  - LArRTMParamExtractor
  - LArStripsCrossTalkCorrector
  - LArTCMFitter
  - LArTCMPhysWavePredictor
  - LArTCMPredictor
  - LArTimeTuning
  - LArTimeTuningNtuple
  - LArWFParams2DB
  - LArWFParamTool



*/

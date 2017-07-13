/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page CalibNtupleUtils_page 
@author Zdenko.Van.Kesteren@cern.ch, Niels.Van.Eldik@cern.ch

@section CalibNtupleUtils_CalibNtupleUtilsIntro Introduction

Helper packages for the MuonCalibStandAlone packages.

@section CalibNtupleUtilsOverview Class Overview
This set of classes are usefull whilst analysing/handling CalibrationNtuples. It contains the following packages:
- HistHandler : a class reducing the amount of code in order to make sets of TH1F/Th2F histograms which similar features. For example, if one choses to store station-level TDC spectra, only one HistHandler should be declared. The class sorts out which station-histogram will be filled (given a MuonFixedId).
- HistSetting : Helper class in order to customize HistHandler with your favorite LabelSize/Offset, TitleSize/Offset etc.
- RootDisplay : class used to view MuonCalibSegment s, MuonCalibPattern s, MuonCalibTruth s and MdtCalibHitBase s. Usefull for debugging. Consists of the classes Display and ShapeCreator
- SegmentCombi : class to collect MuonCalibSegment s in, constructing combinations
- MuonCalib::SegmentInfo : class holding (quality) information of the Segment
- MuonCalib::SegmentNtWriter : class writing MuonSegmentInfo and MuonCalibSegment variables to an ntuple
- MuonCalib::T0Fitter : class refitting MuonCalibSegments of sufficient quality (enough hits). In this refit, the t0 variable is set loose as a variable. In cases that the timing of the muonSpectrometer is not well understood, this procedure reduces residuals significantly.
- MuonCalib::TimedMdtCalibHit : class used by the T0Fitter, since each MdtCalibHitBase owns a t0.
- MuonCalib::TimedSegment : class used by the T0fitter, since each MuonCalibSegment will own a t0.


*/

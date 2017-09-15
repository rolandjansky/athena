/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MuonCalibExtraNtuple_page MuonCalibExtraNtuple Package
@author Zdenko.Van.Kesteren@cern.ch

@section MuonCalibExtraNtuple_MuonCalibExtraNtupleIntro Introduction

MuonCalibExtraNtuple is the package where one builds its analysis based on the extended CalibrationNtuple. 

@section MuonCalibExtraNtuple_MuonCalibExtraNtupleOverview Class Overview

MuonCalibExtraNtuple contains several classes:

- NTReaderBase_E :  Extended version of NTReaderBase, making it capable to hold variables of the extended Branches too.

- MuonCalib::NTReader_E : Class storing the variables of the extended CalibrationNtuple into extended Calib-EDM classes (as described in MuonCalib::MuonCalibEvent_E)
   
And some analysis programmes:

- MuonCalib::ExtraAnalysis : Empty Analysis routine, to be used as skeleton analysis. It has an example of how to loop over the 'extended' classes available from MuonCalibEvent_E.

- MuonCalib::TrackAnalysis : Analysis program for Tracks on Calibration Ntuple. This class performs checks and makes histograms for tracks, tue tracks, holes. Efficiencies, fake rates, outliers, hit distributions, angle distributions can be calculated and plotted 

- MuonCalib::MultipleAnalysis : Analysis program for MuonSegment (MDT or CSC). This analysis routine performs checks for segments and patterns, and using hit truth information. Additional, the programme includes extended pattern recognition.


                                                                                
                                                                                
*/

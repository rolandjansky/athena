/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage TrkDetElementBase Package

@authors Edward Moyse, Grant Gorfine
 
@section TrkDetElementBaseIntro Introduction

This package contains the base class Trk::TrkDetElementBase for the tracking detector elements. 
This allows for common usage of the detector elements for all 
tracking detectors including both the Inner Detector and the Muon Spectrometer

@section TrkDetElementBaseUsage Usage Notes

Most methods have two forms. One with no parameters and one which take
an Identifier as an argument.  The first form corresponds to the whole
detector element. For example the silicon wafer in the Pixel and SCT
detectors or a whole straw layer in the TRT.  The second form is for
detector elements that contain multiple surfaces (such as the MDT and
TRT) and allow access to the individual straws or tubes.

@include requirements

*/

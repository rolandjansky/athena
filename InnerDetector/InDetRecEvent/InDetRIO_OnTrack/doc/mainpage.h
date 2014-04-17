/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
\mainpage The InDetRIO_OnTrack class 

\section introductionInDetRIO_OnTrack Introduction

This package contains InnerDetector-specific  implementations of the Trk::RIO_OnTrack class. In other words, it provides versions of Trk::RIO_OnTrack classes for each of the Inner Detector sub-detectors.
   
As is described in the documentation of Trk::RIO_OnTrack, these classes represent 'calibrated/corrected' measurements, and are stored on the tracks. The equivalent 'uncorrected' measurements are defined in InDet::InDetPrepRawData.
   
The provided classes are:

   - InDet::SiClusterOnTrack : a common silicon measurement
      - InDet::PixelClusterOnTrack : " extended for the Pixels
      - InDet::SCT_ClusterOnTrack  : " extended for the SCT
   - InDet::TRT_DriftCircleOnTrack : TRT measurement
   

Please let me know of any errors, or if anything is unclear.
Edward.Moyse@cern.ch

*/

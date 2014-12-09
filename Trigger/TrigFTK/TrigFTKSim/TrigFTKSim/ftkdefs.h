/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKDEFS_H
#define FTKDEFS_H

// leave it in if using split-arch pseudolayer flat-in-ctheta everywhere
// #define SPLIT_OLDCTHETA

namespace ftk {
  const int NHITTYPES(3);     // Number of coordinate types: PIXEL,SCT,SCTTrk

  const int NOASSIGNED(-999);
  const int PIXEL(1);
  const int SCT(0);
  const int SCTtrk(2);
  const int BARREL(0);
  const int ENDCAP(1);
  const int POSEC(2);
  const int NEGEC(-2);
  const double PI(3.14159265398);
  const double COTTMAX(10.0178749);
  const double CURVMAX(0.00065);

  // IBL and Pixel modules constants
  const float micrometer = 1E-6;
  const float millimiter = 1E-3;
  const float etaPitchPixel = 400*micrometer;
  const float etaPitchIbl = 250*micrometer;
  const float phiPitch = 50*micrometer;
  const float sensorThicknessPixel = 250*micrometer;
  const float sensorThicknessIbl = 200*micrometer;
  const float lengthOfPixelModuleIn400umPixels = 152.;
  const float lengthOfIblModuleIn250umPixels = 156.+2*2.+2*0.400/0.250;
  // 156 pixels of 250um, plus 2 pixels of (2*250um) + 2 pixels of 400um
  const float numberOfEtaPixelsInPixelModule = 144.; // Pixels modules
  const float numberOfEtaPixelsInIblModule = 160.; // Pixels in IBL modules
  const float numberOfPhiPixelsInPixelModule = 328.; // Pixels modules
  const float numberOfPhiPixelsInIblModule = 336.; // Pixels in IBL modules 

}

// TTree bufsize values for different ObjectStreams (used globally)
const int TREE_ROAD_BUFSIZE = 16000; // smaller than default root value (32000)
const int TREE_TRACK_BUFSIZE = 0;    // uses default ROOT value (32000)

#endif // FTKDEFS_H

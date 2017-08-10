/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKDEFS_H
#define FTKDEFS_H

#include <stdint.h> // for uint8_t/uint32_t
#include <stddef.h> // for size_t
#include <TString.h>

// leave it in if using split-arch pseudolayer flat-in-ctheta everywhere
// #define SPLIT_OLDCTHETA

/**
 * \namespace ftk 
 * \brief FTK default values.
 */
namespace ftk {
  const int NHITTYPES(3);     ///< Number of coordinate types: PIXEL, SCT, SCTTrk

  const int MAXPLANES(20);
  const int NOASSIGNED(-999);
  const int PIXEL(1);
  const int SCT(0);
  const int SCTtrk(2);
  const int BARREL(0);
  const int ENDCAP(1);
  const int POSEC(2);
  const int NEGEC(-2);
  const double PI(3.14159265398); ///<PI, duh
  const double COTTMAX(10.0178749);
  const double CURVMAX(0.00065);

  // New Hit Type definitions that separate different module types
  const int MODULETYPE_INVALID(0); // used for backward compatibility
  const int MODULETYPE_IBL3D(1);
  const int MODULETYPE_IBL_PLANAR(2);
  const int MODULETYPE_PIXEL(3);
  const int MODULETYPE_SCT(100);

  // IBL and Pixel modules constants
  const float micrometer = 1E-6; ///< Micrometer expressed in meters
  const float millimiter = 1E-3; ///< Millimiter expressed in meters
  const float etaPitchPixel = 400*micrometer; ///< Eta pitch for Pixel modules
  const float etaPitchIbl = 250*micrometer; ///< Eta pitch for IBL modules
  const float phiPitch = 50*micrometer; ///< Phi pitch
  const float sensorThicknessPixel = 250*micrometer; ///< Thickness of pixel module sensor expressed in mm
  const float sensorThicknessIbl = 200*micrometer; ///< Thickness of IBL module sensor expressed in mm
  const float lengthOfPixelModuleIn400umPixels = 152.; ///< 152 pixels of 400um
  const float lengthOfIblModuleIn250umPixels = 156.+2*2.+2*0.400/0.250; ///<156 pixels of 250um, plus 2 pixels of (2*250um) + 2 pixels of 400um
  const float numberOfEtaPixelsInPixelModule = 144.; ///< Number of columns in a pixel module
  const float numberOfEtaPixelsInIblModule = 160.; ///< Number of columns in an IBL module
  const float numberOfPhiPixelsInPixelModule = 328.; ///< Number of rows in a pixel module
  const float numberOfPhiPixelsInIblModule = 336.; ///< Number of rows in an IBL module

  /**
   * \namespace ftk::clustering 
   * \brief Default values used for the centroid calculation of a cluster.
   */
  namespace clustering {
      const float yScaleFactorPixel = 16.; ///<TODO
      const float xScaleFactorPixel = 8.; ///<TODO
      const float yScaleFactorIbl = 10.; ///<TODO
      const float xScaleFactorIbl = 8.; ///<TODO
      const float pixelEndCapRPhiCorrection = 25.4*micrometer/phiPitch; ///< TODO - Lorentz angle?
      const float pixelIblRPhiCorrection = 7*micrometer/phiPitch; ///< TODO - Lorentz angle?
      const float radiusIbl = 35.78; ///< IBL (from fig5 in IBL TDR: (33.25 + 38.31)/2)   
      const float radii[4] = {35.78, 50.5, 88.5, 122.5}; ///< Radii of the 4 layers, starting with IBL, B-Layer, etc.
      const int feChipsInRowPixel = 8; ///< Number of Front-End chips in each pixel module row
      const int rowsInFEChipPerPixelModuleRow = 164; ///<Number of rows in a FE chip of a pixel module
      const int colsInFEChipPerPixelModuleRow = 18;///<Number of columns in a FE chip of a pixel module
      const int rowsInFEChipPerIblModuleRow = 336; ///<Number of rows in a FE chip of a IBL module
      const int colsInFEChipPerIblModuleRow = 80;///<Number of columns in a FE chip of a IBL module

  }

  const int MOD_ADLER = 65521;
  uint32_t adler32chksum(const void *buf, size_t buflength);
  TString StripFileName(TString path);
}

// TTree bufsize values for different ObjectStreams (used globally)
const int TREE_ROAD_BUFSIZE = 16000; ///< smaller than default root value (32000)
const int TREE_TRACK_BUFSIZE = 0;    ///< uses default ROOT value (32000)

#endif // FTKDEFS_H

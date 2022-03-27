/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * author: Riccardo Maria BIANCHI <riccardo.maria.bianchi@cern.ch>, 2022 Jan
 *
 * The class offers different conversion tools and helpers to handle colors in different formats. 
 * In particular, it helps setting the Coin colors, which are defined in the [0.0, 1.0] range, 
 * from standard RGB values, which are defined in the [0, 255] range.
 *
 */

#ifndef VP1UTILSCOINSOQT_VP1COLORUTILS_H
#define VP1UTILSCOINSOQT_VP1COLORUTILS_H


#include <string>


// FWD declarations
class SoMaterial;
class SbColor;

class VP1ColorUtils {

  public:

// set SoMaterial color from RGB values
  static void setMatColorFromRGB(SoMaterial* mat, std::string type, const unsigned int r, const unsigned int g, const unsigned int b);
  static SbColor getSbColorFromRGB(const unsigned int r, const unsigned int g, const unsigned int b);

  // convert the usual range RGB [0,255] to Coin [0,1]
  static float getValFromRGB(const unsigned int rgb);
};

#endif

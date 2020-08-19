/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1Settings                         //
//                                                            //
//  Description: Common place to pick up settings, mainly     //
//               those controlled by environment variables.   //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: December 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1SETTINGS_H
#define VP1SETTINGS_H

// FIXME: need to include this before any QT header, because QT defines
// a macro `foreach' that collides with an identifier in AthenaKernel.
#include "xAODCore/BaseInfo.h"

#include <QString>

class VP1Settings {
public:

  static QString defaultFileSelectDirectory();

private:

  VP1Settings();
  ~VP1Settings();
};

#endif

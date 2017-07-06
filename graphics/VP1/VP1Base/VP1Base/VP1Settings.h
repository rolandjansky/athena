/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include <QString>

class VP1Settings {
public:

  static QString defaultFileSelectDirectory();

private:

  VP1Settings();
  ~VP1Settings();
};

#endif

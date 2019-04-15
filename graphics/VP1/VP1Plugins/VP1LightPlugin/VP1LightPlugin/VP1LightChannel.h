/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
//                                                             //
//  Header file for class VP1LightChannel                      //
//                                                         	   //
//  Author: Sebastian Merkt <sebastian.andreas.merkt@cern.ch>  //
//                                                             //
//  Initial version: December 2017                             //
//                                                             //
/////////////////////////////////////////////////////////////////

#ifndef VP1LIGHTCHANNEL_H
#define VP1LIGHTCHANNEL_H

#include "VP1Base/IVP13DStandardChannelWidget.h"

class VP1LightChannel : public IVP13DStandardChannelWidget {

  Q_OBJECT

public:

  VP1LightChannel();
  void init();
  virtual ~VP1LightChannel(){}

};

#endif // VP1LIGHTCHANNEL_H

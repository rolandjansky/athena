/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP13DCocktailChannel             //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: June 2007                             //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP13DCOCKTAILCHANNEL_H
#define VP13DCOCKTAILCHANNEL_H

#include "VP1Base/IVP13DStandardChannelWidget.h"

class VP13DCocktailChannel : public IVP13DStandardChannelWidget {

  Q_OBJECT

public:

  VP13DCocktailChannel();
  void init();
  virtual ~VP13DCocktailChannel(){}

};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1LEGOCHANNEL_H
#define VP1LEGOCHANNEL_H

#include "VP1Base/IVP13DStandardChannelWidget.h"

class VP1LegoChannel : public IVP13DStandardChannelWidget
{

  Q_OBJECT

public:

  VP1LegoChannel();
  void init();
  virtual ~VP1LegoChannel(){}

};

#endif


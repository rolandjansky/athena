/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1AODCHANNEL_H
#define VP1AODCHANNEL_H

#include "VP1Base/IVP13DStandardChannelWidget.h"

class VP1AODChannel : public IVP13DStandardChannelWidget {

  Q_OBJECT

public:

  VP1AODChannel();
  void init();
  virtual ~VP1AODChannel(){}

};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1TRACKCHANNEL_H
#define VP1TRACKCHANNEL_H

#include "VP1Base/IVP13DStandardChannelWidget.h"

class VP1TrackChannel : public IVP13DStandardChannelWidget {

  Q_OBJECT

public:

  VP1TrackChannel();
  void init();
  virtual ~VP1TrackChannel(){}

};

#endif

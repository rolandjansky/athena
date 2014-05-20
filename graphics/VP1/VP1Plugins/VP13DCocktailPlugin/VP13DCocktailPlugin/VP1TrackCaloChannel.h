/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1TRACKCALOCHANNEL_H
#define VP1TRACKCALOCHANNEL_H

#include "VP1Base/IVP13DStandardChannelWidget.h"

class VP1TrackCaloChannel : public IVP13DStandardChannelWidget {

  Q_OBJECT

public:

  VP1TrackCaloChannel();
  void init();
  virtual ~VP1TrackCaloChannel(){}

  //void systemRefreshed(IVP1System*);

};

#endif

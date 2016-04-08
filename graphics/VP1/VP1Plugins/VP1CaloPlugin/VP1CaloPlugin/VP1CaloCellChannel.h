/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1CALOCELLCHANNEL_H
#define VP1CALOCELLCHANNEL_H

#include "VP1Base/IVP13DStandardChannelWidget.h"

class VP1CaloCellChannel : public IVP13DStandardChannelWidget
{

  Q_OBJECT

public:

  VP1CaloCellChannel();
  void init();
  virtual ~VP1CaloCellChannel(){}

};

#endif


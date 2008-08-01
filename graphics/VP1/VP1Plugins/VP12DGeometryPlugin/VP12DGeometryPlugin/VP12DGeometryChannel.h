/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP12DGeometryChannel             //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: June 2007                             //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP12DGEOMETRYCHANNEL_H
#define VP12DGEOMETRYCHANNEL_H

#include "VP1Base/IVP12DDetViewsChannelWidget.h"

class VP12DGeometryChannel : public IVP12DDetViewsChannelWidget {

  Q_OBJECT

public:

  VP12DGeometryChannel();
  void init();
  virtual ~VP12DGeometryChannel(){}

};

#endif

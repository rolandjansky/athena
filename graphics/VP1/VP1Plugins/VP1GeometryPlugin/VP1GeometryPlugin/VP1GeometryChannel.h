/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1GeometryChannel               //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: May 2007                              //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1GEOMETRYCHANNEL_H
#define VP1GEOMETRYCHANNEL_H

#include "VP1Base/IVP13DStandardChannelWidget.h"

class VP1GeometryChannel : public IVP13DStandardChannelWidget {

  Q_OBJECT

public:

  VP1GeometryChannel();
  void init();
  virtual ~VP1GeometryChannel(){}

};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class Example3DChannel4                //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: June 2007                             //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef EXAMPLE3DCHANNEL4_H
#define EXAMPLE3DCHANNEL4_H

#include "VP1Base/IVP13DStandardChannelWidget.h"

class Example3DChannel4 : public IVP13DStandardChannelWidget {

  Q_OBJECT

public:

  Example3DChannel4();
  void init();
  virtual ~Example3DChannel4(){}

};

#endif

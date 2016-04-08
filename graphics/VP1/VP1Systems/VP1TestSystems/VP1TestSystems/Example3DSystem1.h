/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXAMPLE3DSYSTEM1_H
#define EXAMPLE3DSYSTEM1_H

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class Example3DSystem1                             //
//                                                                     //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial version: June 2007                                         //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1Base/IVP13DSystemSimple.h"

class Example3DSystem1 : public IVP13DSystemSimple {

  Q_OBJECT

public:
  Example3DSystem1();
  virtual ~Example3DSystem1(){};
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);
};

#endif

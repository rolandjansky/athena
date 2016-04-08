/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXAMPLE3DSYSTEM2_H
#define EXAMPLE3DSYSTEM2_H

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class Example3DSystem2                             //
//                                                                     //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial version: June 2007                                         //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1Base/IVP13DSystemSimple.h"

class Example3DSystem2 : public IVP13DSystemSimple {

  Q_OBJECT

public:
  Example3DSystem2();
  virtual ~Example3DSystem2(){};
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);
};

#endif

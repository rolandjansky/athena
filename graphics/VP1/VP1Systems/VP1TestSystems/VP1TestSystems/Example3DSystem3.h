/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXAMPLE3DSYSTEM3_H
#define EXAMPLE3DSYSTEM3_H

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class Example3DSystem3                             //
//                                                                     //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial version: June 2007                                         //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1Base/IVP13DSystemSimple.h"

#include <map>

namespace Trk {
  class Track;
}

class Example3DSystem3 : public IVP13DSystemSimple {

  Q_OBJECT

public:

  Example3DSystem3();
  virtual ~Example3DSystem3(){};
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);
  void userPickedNode(SoNode* pickedNode, SoPath * pickedPath);

private:
  std::map<SoNode*,const Trk::Track *> m_nodeToTrack;

};

#endif

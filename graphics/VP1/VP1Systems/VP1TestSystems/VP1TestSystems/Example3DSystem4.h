/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXAMPLE3DSYSTEM4_H
#define EXAMPLE3DSYSTEM4_H

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class Example3DSystem4                             //
//                                                                     //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial version: June 2007                                         //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1Base/IVP13DSystemSimple.h"

#include <map>

class SoSwitch;
namespace Trk {
  class Track;
}

class Example3DSystem4 : public IVP13DSystemSimple {

  Q_OBJECT

public:

  Example3DSystem4();
  virtual ~Example3DSystem4(){};
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);
  void userPickedNode(SoNode* pickedNode, SoPath * pickedPath);
  QWidget * buildController();

private slots:
  void ptCutChanged(double);

private:
  std::map<SoNode*,const Trk::Track *> m_nodeToTrack;
  std::map<SoSwitch*,double> m_switchToPt;
  double m_ptcut;
};

#endif

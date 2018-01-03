/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXAMPLE3DSYSTEM5_H
#define EXAMPLE3DSYSTEM5_H

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class Example3DSystem5                             //
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


class Example3DSystem5 : public IVP13DSystemSimple {

  Q_OBJECT

public:

  Example3DSystem5();
  virtual ~Example3DSystem5(){};
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);
  void userPickedNode(SoNode* pickedNode, SoPath * pickedPath);
  void userChangedSelection(SoCooperativeSelection*, QSet<SoNode*>, QSet<SoPath*>);
  QWidget * buildController();

private slots:
  void ptCutChanged(double);
  void updateSelectionMode( bool single );

private:
  std::map<SoNode*,const Trk::Track *> m_nodeToTrack;
  std::map<SoSwitch*,double> m_switchToPt;
  double m_ptcut;
  SoCooperativeSelection * m_multiselection;

};

#endif

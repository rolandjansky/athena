/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKINGSURFACESSYSTEM_H
#define TRACKINGSURFACESSYSTEM_H

#include "VP1Base/IVP13DSystemSimple.h"
#include "VP1Utils/SurfaceToSoNode.h"

#include <map>

class SoSwitch;
namespace Trk {
  class Surface;
}

class TrackingSurfacesSystem : public IVP13DSystemSimple {

  Q_OBJECT

public:

  TrackingSurfacesSystem();
  virtual ~TrackingSurfacesSystem(){};
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);
  void userPickedNode(SoNode* pickedNode, SoPath * pickedPath);
  void userChangedSelection(SoCooperativeSelection*, QSet<SoNode*>, QSet<SoPath*>);
  QWidget * buildController();

private slots:
  void updateSelectionMode( bool single );

private:
  std::map<SoNode*,const Trk::Surface *>  m_nodeToSurface;
  SoCooperativeSelection *          m_multiselection;
  SurfaceToSoNode                   m_surfConvertor;
};

#endif

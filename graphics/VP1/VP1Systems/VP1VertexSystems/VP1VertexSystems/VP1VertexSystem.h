/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1VERTEXSYSTEM_H
#define VP1VERTEXSYSTEM_H

#include "VP1Base/IVP13DSystemSimple.h"
#include <map>

class SoMaterial;
namespace Trk {
  class Track;
}

class VP1VertexSystem : public IVP13DSystemSimple {

  Q_OBJECT

public:
  VP1VertexSystem();
  virtual ~VP1VertexSystem();
  void systemerase();
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);
  void userPickedNode(SoNode* pickedNode, SoPath * pickedPath);
  QWidget * buildController();
  QByteArray saveState();
  void restoreFromState(QByteArray ba);

public slots:
  void updateVertexToTracks(QList<std::pair<const SoMaterial*,QList<const Trk::Track*> > >&);

//FIXME: We need something like this for the trackparticles too:
signals:
  void tracksFromVertexChanged(QList<std::pair<const SoMaterial*, QList<const Trk::Track*> > >&);

private:

  class Imp;
  Imp * m_d;

};

#endif

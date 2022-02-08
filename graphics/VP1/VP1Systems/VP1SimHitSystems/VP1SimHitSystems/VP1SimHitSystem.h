/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1SIMHITSYSTEM_H
#define VP1SIMHITSYSTEM_H

#include "VP1Base/IVP13DSystemSimple.h"

// fwd 
class CaloDetDescrElement;
class SoVertexProperty;

class VP1SimHitSystem: public IVP13DSystemSimple
{
  Q_OBJECT

 public:
  VP1SimHitSystem();
  ~VP1SimHitSystem();

  QWidget* buildController();

  void systemcreate(StoreGateSvc* detstore);
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);

  protected slots:
    void checkboxChanged();

 private:
  class Clockwork;
  Clockwork* m_clockwork;

  void buildHitTree(const QString& detector);
  void handleDetDescrElementHit(const CaloDetDescrElement *hitElement, SoVertexProperty* hitVtxProperty, unsigned int &hitCount);

};

#endif

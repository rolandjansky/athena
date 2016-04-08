/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1HVLEGOSYSTEM_H
#define VP1HVLEGOSYSTEM_H

#include "VP1Base/IVP13DSystemSimple.h"

class VP1HVLegoSystem: public IVP13DSystemSimple
{
  Q_OBJECT

 public:

  // Public type definitions:

  enum POSITION { FRONT, CENTER, BACK };

  VP1HVLegoSystem();
  ~VP1HVLegoSystem();

  QWidget* buildController();

  void systemcreate(StoreGateSvc* detstore);
  void buildPermanentSceneGraph(StoreGateSvc* detstore,SoSeparator *root);
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);

  void userPickedNode(SoNode* pickedNode, SoPath *pickedPath);
  QByteArray saveState();
  void restoreFromState(QByteArray ba);

  protected slots:
    void checkboxChanged();

 private:
  class Clockwork;
  Clockwork* _clockwork;


};

#endif

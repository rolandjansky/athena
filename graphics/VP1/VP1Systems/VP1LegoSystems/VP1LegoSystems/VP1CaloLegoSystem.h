/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1CALOLEGOSYSTEM_H
#define VP1CALOLEGOSYSTEM_H

#include "VP1Base/IVP13DSystemSimple.h"

class VP1CaloLegoSystem: public IVP13DSystemSimple
{
  Q_OBJECT

 public:

  // Public type definitions:

  enum POSITION { FRONT, CENTER, BACK };

  VP1CaloLegoSystem();
  ~VP1CaloLegoSystem();

  QWidget* buildController();

  void systemcreate(StoreGateSvc* detstore);
  void systemuncreate();
  void systemerase();
  void buildPermanentSceneGraph(StoreGateSvc* detstore,SoSeparator *root);
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);

  // In this system it is sometimes necessary to recreate the grid
  // eta-phi boundaries.
  void createEtaPhi();

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

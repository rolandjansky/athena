/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1CALOREADOUTSYSTEM_H
#define VP1CALOREADOUTSYSTEM_H

#include "VP1Base/IVP13DSystemSimple.h"

class QCheckBox;

class VP1CaloReadoutSystem: public IVP13DSystemSimple
{
  Q_OBJECT

 public:

  // Public type definitions:

  enum POSITION { FRONT, CENTER, BACK };

  VP1CaloReadoutSystem();
  ~VP1CaloReadoutSystem();

  QWidget* buildController();

  void systemcreate(StoreGateSvc* detstore);
  void buildPermanentSceneGraph(StoreGateSvc* detstore,SoSeparator *root);
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);

  // In this system it is sometimes necessary to recreate the grid
  // eta-phi boundaries.
  void createEtaPhi();
  void createHV();

  void userPickedNode(SoNode* pickedNode, SoPath *pickedPath);
  void userClickedOnBgd();

  QByteArray saveState();
  void restoreFromState(QByteArray ba);

  protected slots:
    void checkboxChanged();
    void checkboxChanged(QCheckBox*cb);
    void phiSectorTurnon(int);
    void phiSectorTurnoff(int);
    void positionOptionChanged();
    void setGeomSelectable(bool);
    void acceptHV();
    void enabledPhiSectorsChanged();
 private:

  class Clockwork;
  Clockwork* _clockwork;


};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1CALOCELLSYSTEM_H
#define VP1CALOCELLSYSTEM_H

#include "VP1Base/IVP13DSystemSimple.h"

class VP1CaloCellSystem: public IVP13DSystemSimple
{
  Q_OBJECT

public:
  VP1CaloCellSystem();
  ~VP1CaloCellSystem();

  QWidget* buildController();

  void systemcreate(StoreGateSvc* detstore);
  void systemerase();
  void systemuncreate();
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);

  void userPickedNode(SoNode* pickedNode, SoPath *pickedPath);
  QByteArray saveState();
  void restoreFromState(QByteArray ba);

protected slots:
//  void digitsOn(bool flag);
  void energyMode();

private:
  class Clockwork;
  Clockwork* _clockwork;

  // This method is called when either of the following conditins is met:
  //   1. At the beginning of every event
  //   2. When energy mode is changed (E/Et)
  //
  // It performes the following actions:
  //   1. Prepare Separator Helpers for large changes
  //   2. Build cell managers
  //   3. Fill cell managers
  //   4. Update scene with initial values of controller objects
  //   5. Inform Separator Helpers that large changes are over
  void setupEnergyMode();

};

#endif

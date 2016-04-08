/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1EXTRAINPUTSYSTEM_H
#define VP1EXTRAINPUTSYSTEM_H

#include "VP1Base/IVP13DSystemSimple.h"

class VP1ExtraInputSystem: public IVP13DSystemSimple
{
  Q_OBJECT

 public:


  VP1ExtraInputSystem();
  ~VP1ExtraInputSystem();

  QWidget* buildController();

  void buildPermanentSceneGraph(StoreGateSvc* detstore,SoSeparator *root);
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);
  void systemuncreate();

  QByteArray saveState();
  void restoreFromState(QByteArray ba);

  protected slots:

  void toggleVisibility(bool);
  void toggleBaseLightModel(bool);
  void newFile();
  void deleteFile();

 private:

  class Clockwork;
  Clockwork* c;


};

#endif

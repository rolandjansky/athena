/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1PARTSPECTSYSTEM_H
#define VP1PARTSPECTSYSTEM_H
 
/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class VP1PartSpectSystem                           //
//                                                                     //
//                                                                     //
//  Initial version: September 2009                                    //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1Base/IVP13DSystemSimple.h"

#include <QStack>
#include <QString>

class VP1PartSpectSystem : public IVP13DSystemSimple 
{
  Q_OBJECT

public:
  VP1PartSpectSystem();
  virtual ~VP1PartSpectSystem();

  QWidget* buildController();

  void buildPermanentSceneGraph(StoreGateSvc* detstore,SoSeparator *root);
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);

  QByteArray saveState();
  void restoreFromState(QByteArray);

public slots:
  void plotSpectrum(QStack<QString>&, int copyNumber=-1);
  void particleType();
  void fileUpdated(const QString&);

protected:
  class Imp;
  Imp * d;
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1GUIDELINESYSTEM_H
#define VP1GUIDELINESYSTEM_H

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class VP1GuideLineSystem                           //
//                                                                     //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial version: July 2007                                         //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1Base/IVP13DSystemSimple.h"
#include "VP1Utils/InDetProjFlags.h"
#include <QColor>//Fixme obsolete

class VP1GuideLineSystem : public IVP13DSystemSimple {

  Q_OBJECT

public:

  ///////////////////
  //  Constructor  //
  ///////////////////

  VP1GuideLineSystem();

  //////////////////////////////////////////
  //  Reimplementations from base class:  //
  //////////////////////////////////////////

  virtual ~VP1GuideLineSystem();
  void buildPermanentSceneGraph(StoreGateSvc* detstore, SoSeparator *root);
  void buildEventSceneGraph(StoreGateSvc*, SoSeparator *);
  void systemuncreate();
  QWidget * buildController();
  QByteArray saveState();
  void restoreFromState(QByteArray ba);

  ////////////////////////////////////////////
  //  Signals/slots for InDet Projections:  //
  ////////////////////////////////////////////

public slots:

  //To automatically show projection surfaces depending on data
  //projected to those (NB: Several senders can connect to this slot,
  //and the final result will be a union of the requested parts):
  //
  //As elsewhere, it is important that senders emit signals with
  //InDetProjFlags::NoDet in their systemerase().
  void setIDDetTypesUsingProjections( InDetProjFlags::DetTypeFlags );

  //Channel should explicitly call this after making all connections to other systems.
  void possiblyEmit_ApplicableProjectionsChanged();

signals:

  //So data systems can automatically project to the correct surfaces
  //(of course, when the surfaces are "off" in the guide line system,
  //they will not be rendered, but they will still be the ones used
  //for projections:
  void applicableProjectionsChanged( InDetProjFlags::InDetProjPartsFlags pixel,
				     InDetProjFlags::InDetProjPartsFlags sct,
				     InDetProjFlags::InDetProjPartsFlags trt );

private:
  class Imp;
  Imp * d;
};

#endif

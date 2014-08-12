/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////

//                                                            //
//  Header file for class IParticleCollHandle_Muon       //
//                                                            //
//  Description: Collection handle for track particles        //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef IPARTICLECOLLHANDLE_MUON_H
#define IPARTICLECOLLHANDLE_MUON_H

// FIXME! This is needed because of a Qt bug, which bizarrely manifests for Muons...
#ifndef Q_MOC_RUN
#include "IParticleCollHandleBase.h"
#include "xAODBase/ObjectType.h"
#include "xAODMuon/MuonContainer.h"
#endif 

class MuonCollectionSettingsButton;

class IParticleCollHandle_Muon : public IParticleCollHandleBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);//For the collection widget.

  IParticleCollHandle_Muon( AODSysCommonData *,
  const QString& name, xAOD::Type::ObjectType type );
  virtual ~IParticleCollHandle_Muon();

  virtual void init(VP1MaterialButtonBase* matBut=0);//reimplementations must start with a call to this.
  virtual void setupSettingsFromControllerSpecific(AODSystemController*);

  const MuonCollectionSettingsButton& collSettingsButton() const;

protected:	
  virtual bool load();
  virtual bool cut(IParticleHandleBase*);
  virtual QColor defaultColor() const { return QColor::fromRgbF(1.0f, 1.0f, 0.5f); }

private:

  class Imp;
  Imp * d;

};

#endif

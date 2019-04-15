/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class IParticleCollHandle_Electron        //
//                                                            //
//  Description: Collection handle for electron               //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef IPARTICLECOLLHANDLE_ELECTRON_H
#define IPARTICLECOLLHANDLE_ELECTRON_H

#include "IParticleCollHandleBase.h"
#include "VP1AODSystems/VP1AODSystem.h"
#include "xAODBase/ObjectType.h"
#include "xAODTracking/TrackingPrimitives.h"


class SoMaterial;
class ElectronCollectionSettingsButton;

class IParticleCollHandle_Electron : public IParticleCollHandleBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);//For the collection widget.

  /// shouldLoad is used to disable the call to load if the collhandle is for temporary (read: associated) electrons.
  IParticleCollHandle_Electron( AODSysCommonData *,
           const QString& name, xAOD::Type::ObjectType type, bool shouldLoad=true );

  virtual ~IParticleCollHandle_Electron();

  virtual void init(VP1MaterialButtonBase* matBut=0);//reimplementations must start with a call to this.
  virtual void setupSettingsFromControllerSpecific(AODSystemController*);
  virtual void dumpToJSON( std::ofstream& ) const ;

  const ElectronCollectionSettingsButton& collSettingsButton() const;

  // SoMaterial* defaultParameterMaterial() const; // Just forwards to CollSettingsButton, but avoids exposing it in interface.
  // SoMaterial* parameterMaterial( xAOD::ParameterPosition) const; // Just forwards to CollSettingsButton, but avoids exposing it in interface.


protected:	
  virtual bool load();
  virtual bool cut(AODHandleBase*);
  virtual QColor defaultColor() const { return QColor::fromRgbF(1.0f, 1.0f, 0.5f); }

private slots:
  void showParametersChanged(bool);
  void propagationOptionsChanged();
  void resetCachedValuesCuts();

private:

  class  Imp;
  Imp   *m_d;

};

#endif

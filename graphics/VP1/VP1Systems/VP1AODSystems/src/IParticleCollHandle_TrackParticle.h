/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////

//                                                            //
//  Header file for class IParticleCollHandle_TrackParticle       //
//                                                            //
//  Description: Collection handle for track particles        //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef IPARTICLECOLLHANDLE_TRACKPARTICLE_H
#define IPARTICLECOLLHANDLE_TRACKPARTICLE_H

#include "IParticleCollHandleBase.h"
#include "xAODBase/ObjectType.h"
#include "xAODTracking/TrackingPrimitives.h"

class SoMaterial;
class TrackParticleCollectionSettingsButton;

class IParticleCollHandle_TrackParticle : public IParticleCollHandleBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);//For the collection widget.

  /// shouldLoad is used to disable the call to load if the collhandle is for temporary (read: associated) trackparticles.
  IParticleCollHandle_TrackParticle( AODSysCommonData *,
         const QString& name, xAOD::Type::ObjectType type, bool shouldLoad=true );
  virtual ~IParticleCollHandle_TrackParticle();

  virtual void init(VP1MaterialButtonBase* matBut=0);//reimplementations must start with a call to this.
  virtual void setupSettingsFromControllerSpecific(AODSystemController*);
  virtual void dumpToJSON( std::ofstream& ) const ;

  const TrackParticleCollectionSettingsButton& collSettingsButton() const;

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

  class Imp;
  Imp * d;

};

#endif

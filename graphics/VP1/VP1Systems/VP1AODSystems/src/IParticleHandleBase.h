/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class IParticleHandleBase                 //
//                                                            //
//  Description: Base class for IParticle xAOD.               //
//                                                            //
//  Author: E. Moyse                                          //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef IPARTICLEHANDLEBASE_H
#define IPARTICLEHANDLEBASE_H

#include "GeoPrimitives/GeoPrimitives.h"

#include <QStringList>
#include <QTreeWidgetItem>
#include <vector>
#include <set>
#include <QList>
#include <QFlags>
#include "AODHandleBase.h"

class AssocObjAttachmentHandle;
class AssociatedObjectHandleBase;
class IParticleCollHandleBase;
class TrackSysCommonData;
class SoMaterial;
class SoNode;

namespace xAOD {
	class IParticle;
}

class AODSysCommonData;

class IParticleHandleBase : public AODHandleBase {
public:
  IParticleHandleBase(IParticleCollHandleBase*);
  virtual ~IParticleHandleBase();
  
  // AODHandle base methods
	SoMaterial * determineMaterial();
  virtual QStringList baseInfo() const;

  // IParticle specific methods
  virtual Amg::Vector3D momentum() const { return Amg::Vector3D(0,0,0); }

  virtual const xAOD::IParticle& iParticle() const =0;

  virtual double charge() const =0 ;
  virtual double mass() const;//!< Returns unknown() in case of trouble.

  bool hasCharge() const { return charge()!=unknown(); }
  bool hasMass() const { return mass()!=unknown(); }
  
protected:

private:

  // It is illegal to copy/assign a IParticleHandleBase:
  IParticleHandleBase(const IParticleHandleBase & );
  IParticleHandleBase & operator= (const IParticleHandleBase & );

  class Imp;
  Imp * d;  
};


#endif

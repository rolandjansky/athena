/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class IParticleHandle_Muon       //
//                                                            //
//  Description: Handles for track particles.                 //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef IPARTICLEHANDLE_MUON_H
#define IPARTICLEHANDLE_MUON_H

#include "IParticleHandleBase.h"

#include "xAODMuon/Muon.h"
#include <QList>

class IParticleHandle_Muon : public IParticleHandleBase {
public:

  IParticleHandle_Muon(IParticleCollHandleBase*, const xAOD::Muon * );
  virtual ~IParticleHandle_Muon();

	virtual bool has3DObjects();
	virtual void clear3DObjects();
	virtual SoNode* nodes();

  virtual QStringList clicked() const;
  virtual Amg::Vector3D momentum() const;
  const xAOD::IParticle& iParticle() const;
  virtual double charge() const ;//!< Returns unknown() in case of trouble.

  virtual QString type() const { return QString("Muon"); } //!< return very short word with type (maybe link with collection type?)

	/// This returns the information shown about the object in the object browser
	QString shortInfo() const;
	
	void fillObjectBrowser( QList<QTreeWidgetItem *>& listOfItems) ;
	
  // Some specific stuff used by base collection to draw tracks
  const QList<std::pair<xAOD::ParameterPosition, Amg::Vector3D> >& parametersAndPositions() const;

protected:  

private:

  class Imp;
  Imp * d;
  void ensureInitSubSysHitInfo() const;

};

#endif

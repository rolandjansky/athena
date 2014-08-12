/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class IParticleHandle_TrackParticle       //
//                                                            //
//  Description: Handles for track particles.                 //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef IPARTICLEHANDLE_TRACKPARTICLE_H
#define IPARTICLEHANDLE_TRACKPARTICLE_H

#include "IParticleHandleBase.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include <QList>

class IParticleHandle_TrackParticle : public IParticleHandleBase {
public:

  IParticleHandle_TrackParticle(IParticleCollHandleBase*, const xAOD::TrackParticle * );
  virtual ~IParticleHandle_TrackParticle();

	virtual bool has3DObjects();
	virtual void clear3DObjects();
	virtual SoNode* nodes();

  virtual QStringList clicked() const;
  virtual Amg::Vector3D momentum() const;
  const xAOD::IParticle& iParticle() const;
  virtual double charge() const ;//!< Returns unknown() in case of trouble.
	
	virtual unsigned summaryValue(xAOD::SummaryType type) const;
  virtual unsigned getNPixelHits()   const;
  virtual unsigned getNSCTHits()     const;
  virtual unsigned getNTRTHits()     const;
  virtual unsigned getNMuonPrecisionLayers() const;
  virtual unsigned getNMuonPrecisionHoleLayers()     const;
  virtual unsigned getNMuonPhiLayers()     const;
  virtual unsigned getNMuonPhiHoleLayers()     const;

  virtual QString type() const { return QString("TrackParticle"); } //!< return very short word with type (maybe link with collection type?)

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

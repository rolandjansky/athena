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
#include <QList>
#include "xAODMuon/Muon.h"

class IParticleHandle_Muon : public IParticleHandleBase {
public:

  IParticleHandle_Muon(IParticleCollHandleBase*, const xAOD::Muon * );
  virtual ~IParticleHandle_Muon();

  virtual bool has3DObjects();
  virtual void clear3DObjects();
  virtual SoNode* nodes();
  
  // FIXME! The methods below were copied from TrackParticle and should be put into a helper function.
  void addParameterShapes();
  void addLine_FromTrackParticle();
  void linear(Amg::Vector3D& output, Amg::Vector3D& a, Amg::Vector3D& b, float t);
  void bezier(Amg::Vector3D& output, Amg::Vector3D& a, Amg::Vector3D& b, Amg::Vector3D& c, Amg::Vector3D& d, float t);
  void fillLineFromSplineFit(const std::vector<Amg::Vector3D>& positions, const std::vector<Amg::Vector3D>&momenta);

  virtual QStringList clicked() const;
  virtual Amg::Vector3D momentum() const;
  const xAOD::IParticle& iParticle() const;
  virtual double charge() const ;//!< Returns unknown() in case of trouble.

  // Muon specific
  virtual QString type() const { return QString("Muon"); } //!< return very short word with type (maybe link with collection type?)
  unsigned int quality() const;
  const xAOD::Muon& muon() const;
  QString muonTypeString() const;
  QString qualityString() const;
  
  
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

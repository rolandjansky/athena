/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class IParticleHandle_TrackParticle           //
//                                                            //
//  Description: Handles for track particles.                 //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef IPARTICLEHANDLE_JET_H
#define IPARTICLEHANDLE_JET_H

#include "IParticleHandleBase.h"

#include "xAODJet/Jet.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include <string>

// FWD declarations
class SoSwitch;


class IParticleHandle_Jet : public IParticleHandleBase {
public:

  IParticleHandle_Jet(IParticleCollHandleBase*, const xAOD::Jet * );
  virtual ~IParticleHandle_Jet();

  virtual bool has3DObjects();
  virtual void clear3DObjects();
  virtual SoNode* nodes();

  // This fills the Jet specific information, and is needed in addition to the parent method.
  void fillObjectBrowser( QList<QTreeWidgetItem *>& listOfItems) ;

  // Setters
  void setScale( const double& sc);
  void setMaxR(const double& maxR);

  void updateHeight();

  // set/update random material
  void rerandomiseMaterial();


  virtual QStringList clicked() const;
  //virtual Amg::Vector3D momentum() const;
  const xAOD::IParticle& iParticle() const;

  virtual double charge() const ;//!< Returns unknown() in case of trouble.
  double phi() const;
  double eta() const;
  double energy() const;
  double transverseEnergy() const;
  double energyForCuts() const;

  virtual QString type() const { return QString("Jet"); } //!< return very short word with type (maybe link with collection type?)

  /// This returns the information shown about the object in the object browser
  QString shortInfo() const;

  void updateMaterial(bool);
  void updateBTagging(const std::string& bTaggingTagger, const double& bTaggingCut);
  void updateBTaggingSwitch(SoSwitch *bTaggingSwitch);
  
  virtual void dumpToJSON( std::ofstream& ) const ;


protected:  

private:

  class Imp;
  Imp * d;
  void ensureInitSubSysHitInfo() const;
  double getBTaggingWeight(std::string tagger);

};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class IParticleHandle_CaloCluster         //
//                                                            //
//  Description: Handles for xAOD calorimeter clusters        //
//                                                            //
//  Author: Riccardo Maria BIANCHI (rbianchi@cern.ch)         //
//  Initial version: Jun 2015                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1AODSYSTEMS_IParticleHandle_CaloCluster_H
#define VP1AODSYSTEMS_IParticleHandle_CaloCluster_H

// local
#include "IParticleHandleBase.h"

// VP1
#include "VP1HEPVis/nodes/SoGenericBox.h"

// xAOD
#include "xAODJet/Jet.h"
#include "xAODCaloEvent/CaloCluster.h"

// ATLAS
#include "GeoPrimitives/GeoPrimitives.h"

// C++
#include <string>

// FWD declarations
class SoSwitch;
class IParticleCollHandle_CaloCluster;
//class SoGenericBox;


class IParticleHandle_CaloCluster : public IParticleHandleBase {
public:

  IParticleHandle_CaloCluster(IParticleCollHandleBase*, const xAOD::CaloCluster * );
  virtual ~IParticleHandle_CaloCluster();

  virtual bool has3DObjects();
  virtual void clear3DObjects();
  virtual SoNode* nodes();

  // This fills the Jet specific information, and is needed in addition to the parent method.
  void fillObjectBrowser( QList<QTreeWidgetItem *>& listOfItems) ;

  // Setters
  void updateShape(IParticleCollHandle_CaloCluster* coll);
  //	void setScale( const double& sc);
  //	void setMaxR(const double& maxR);


  // set/update random material
  void rerandomiseMaterial();


  virtual QStringList clicked() const;
  //virtual Amg::Vector3D momentum() const;
  const xAOD::IParticle& iParticle() const;

//<<<<<<< .working
//  SoGenericBox * genericBox() const;
//  virtual double charge() const ;//!< Returns unknown() in case of trouble.
//  double phi() const;
//  double eta() const;
//  double energy() const;
//  double transverseEnergy() const;
//  double energyForCuts() const;
//=======
	SoGenericBox * genericBox() const;
	virtual double charge() const ;//!< Returns unknown() in case of trouble.
	double phi() const;
	double eta() const;
	double e() const;
	double et() const;
	double energyForCuts() const;
//>>>>>>> .merge-right.r712981
	bool isConsiderTransverseEnergies() const;

  virtual QString type() const { return QString("CaloCluster"); } //!< return very short word with type (maybe link with collection type?)

  /// This returns the information shown about the object in the object browser
  QString shortInfo() const;

  //	void updateMaterial(bool);
  void updateMaterial();
  //	void updateBTagging(const std::string& bTaggingTagger, const double& bTaggingCut);
  //	void updateBTaggingSwitch(SoSwitch *bTaggingSwitch);
  virtual void dumpToJSON( std::ofstream& ) const ;



private:

  class Imp;
  Imp * d;
  void ensureInitSubSysHitInfo() const;
  //	double getBTaggingWeight(std::string tagger);



};

#endif

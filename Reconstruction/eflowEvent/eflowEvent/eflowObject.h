/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWEVENT_EFLOWOBJECT_H
#define EFLOWEVENT_EFLOWOBJECT_H
/********************************************************************

NAME:     eflowObject.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  D.R. Tovey
CREATED:  22nd November, 2001

UPDATED:  14th March 2003 (P Loch) implement navigation system

PURPOSE:  eflow object data class

********************************************************************/

// INCLUDE HEADER FILES:

 
#include "GaudiKernel/ContainedObject.h"

#include "AthLinks/ElementLink.h"

#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloRecoStatus.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"

#include "VxVertex/VxContainer.h"

#include "FourMom/P4EEtaPhiM.h"

#include "Navigation/NavigableVector.h"
#include "Navigation/Navigable.h"

#include "EventKernel/INavigable4Momentum.h"

#include <vector>

////////////////////////////////////////////////////////////////////
// temporary for navigation !!
#include <boost/any.hpp>

class VxCandidate;

// class eflowObject
//
// Navigation requires eflowObject to implement INavigable interface!

class eflowObject : public P4EEtaPhiM, virtual public INavigable4Momentum
{

 public:

// constructor
  eflowObject();

  //copy constructors

  eflowObject(eflowObject* eflowObj);
  eflowObject(eflowObject* eflowObj, bool useClus);

// destructor
  ~eflowObject();

  //new enum for object type
  enum ParticleType { ChargedPion = 0, Electron = 1, Photon = 2 };

  //initialize things for copy constructors
  void initialize(eflowObject* eflowObj, bool useClus);

  //  typedef std::vector<const Rec::TrackParticle*>	eflowTrack_type;
  typedef NavigableVector<Rec::TrackParticleContainer>  eflowTrack_type;

  typedef eflowTrack_type::const_iterator eflowTrack_iterator;

  // typedefs for vector eflowClus of pointers to clus objects and
  // corresponding iterators
  // 
  // general cluster typedef's
  typedef NavigableVector<CaloClusterContainer> cluster_type;
  typedef cluster_type::const_iterator          cluster_iterator;

  typedef cluster_type     eflowClus_type;
  typedef cluster_iterator eflowClus_iterator;

  // TrackParticle Accessor Methods:

  eflowTrack_iterator track_begin() const  { return m_eflowTrack.begin(); }
  eflowTrack_iterator track_end() const    { return m_eflowTrack.end(); }

  const Rec::TrackParticle* track(size_t i) const         { return m_eflowTrack[i]; }
  const ElementLink<Rec::TrackParticleContainer> trackLink(size_t i) const
  { return m_eflowTrack.linkAt(i); }

  void addTrack(const ElementLink<Rec::TrackParticleContainer>& trackElementLink);

  // clus Accessor Methods:

  eflowClus_iterator clus_begin() const  { return m_eflowClus.begin(); }
  eflowClus_iterator clus_end() const    { return m_eflowClus.end(); }

  const CaloCluster* clus(size_t i) const         { return m_eflowClus[i]; }
  const ElementLink<CaloClusterContainer> clusLink(size_t i) const
  { return m_eflowClus.linkAt(i); }

  void addClus(const ElementLink<CaloClusterContainer>& clusElementLink);
  void addClus(const CaloCluster* clus);

  // Other Methods
  
  int numTrack() const              { return m_eflowTrack.size(); }
  int numClus() const              { return m_eflowClus.size(); }
 
  const Analysis::Muon* muon() const ; 
  const ElementLink<Analysis::MuonContainer>& muonLink() const;
  void addMuon(const ElementLink<Analysis::MuonContainer>& muonElementLink) { m_muonElementLink = muonElementLink; }

  const Trk::VxCandidate* conversion() const         { return *m_convElementLink; }
  const ElementLink<VxContainer>& conversionLink() const;
  void addConversion(const ElementLink<VxContainer>& convElementLink) { m_convElementLink = convElementLink; }

  // Set/get parameters

  // most of these methods are in P4EEtaPhiM!
 
  void set_eta(double ceta)          { this->setEta(ceta); }

  void set_phi(double cphi)          { this->setPhi(cphi); } // phi convention

  double d0() const                  { return m_d0; }
  void set_d0(double cd0)            { m_d0 = cd0; }

  double z0() const                  { return m_z0; }
  void set_z0(double cz0)            { m_z0 = cz0; }

  double energy() const              { return this->e(); }
  void set_energy(double cenergy)    { this->setE(cenergy); }

  void set_m(double cm)              { this->setM(cm); }

  int eflowType() const              { return m_eflowType; }
  void set_type(int ctype)           { m_eflowType = ctype; }

  int charge() const                 { return m_charge; }
  void set_charge(int ccharge)       { m_charge = ccharge; }

  bool isValid() const               { return m_valid; }
  void set_notValid()                { m_valid = false; }
  void set_valid(bool flag=true)     { m_valid = flag; }

  double getPi0MVA() const { return m_pi0MVA;}
  void setPi0MVA(double Pi0MVA) {m_pi0MVA = Pi0MVA;}

  // new navigation system
  virtual void fillToken(INavigationToken& thisToken) const
    {
      this->fillToken(thisToken,double(1.0));
    }
  virtual void fillToken(INavigationToken& thisToken, 
			 const boost::any& aRelation ) const;

  std::vector<CaloClusterContainer*>::iterator firstClusContainer() { return  m_eflowClusContainers.begin(); }
  std::vector<CaloClusterContainer*>::iterator lastClusContainer() { return  m_eflowClusContainers.end(); }

  void setPassEOverPCheck(bool check) { m_passedEOverPCheck = check;}
  bool getPassEOverPCheck() const { return m_passedEOverPCheck;}

  void setIsSubtracted(bool isSubtracted) { m_isSubtracted = isSubtracted;}
  bool getIsSubtracted() const { return m_isSubtracted;}

  void setIsDuplicated(bool isDuplicated) { m_isDuplicated = isDuplicated;}
  bool getIsDuplicated() const { return m_isDuplicated;}

  void setCaloRecoStatus(CaloRecoStatus status) {m_recoStatus = status;}
  CaloRecoStatus getCaloRecoStatus() const  { return m_recoStatus;}

  void setCenterMag(double centerMag) { m_centerMag = centerMag;}
  double getCenterMag() const { return m_centerMag;}

  void setParticleType(ParticleType particleType) { m_eflowObjectType = particleType ;}
  bool checkParticleType(ParticleType particleType) const;

 protected:

  bool m_passedEOverPCheck;
  bool m_isSubtracted;
  bool m_isDuplicated;

  void navigateClusters(const cluster_type& theClusters,
			INavigationToken&   aToken,
			double              weight) const;

  bool navigateTrackParticles(INavigationToken& aToken, double weight) const;
  bool navigateMuons(       INavigationToken& aToken, double weight) const;
  bool navigateConversions( INavigationToken& aToken, double weight) const;

  template< typename CONT, typename TOKEN >
  void toToken(const CONT&  theClusters, 
	       TOKEN*       aToken, 
	       double       weight) const;

  template< typename CONT, typename TOKEN >
  void toToken(const CONT&  theClusters, 
	       TOKEN*       aToken) const;

 private:

  ParticleType m_eflowObjectType;

  std::vector<CaloClusterContainer*> m_eflowClusContainers;

  // Calorimeter private data members:
  eflowClus_type m_eflowClus;

  // Track private data member
  eflowTrack_type m_eflowTrack;

  //Cluster Reco Status
  CaloRecoStatus m_recoStatus;

  double m_d0, m_z0;

  int m_eflowType, m_charge;
  int m_nTrack, m_nClus;

  bool m_valid;

  ElementLink<Analysis::MuonContainer> m_muonElementLink;
  ElementLink<VxContainer> m_convElementLink;

  double m_pi0MVA;
  double m_centerMag;
  
inline double phicorr(double a)
{ 
  if (a <= -M_PI)   
    {     
      return a+(2*M_PI*floor(-(a-M_PI)/(2*M_PI)));
    }
  else if (a > M_PI) 
    {
      return a-(2*M_PI*floor((a+M_PI)/(2*M_PI)));
    }
  else
    {
      return a;
    }
}

};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowObject.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  D.R. Tovey
CREATED:  18th November, 2001

UPDATED:  14th March 2003 (P Loch) implement navigation system

UPDATED:   8th April 2004 (P Loch) implement new navigation scheme

********************************************************************/

#include "FourMom/P4EEtaPhiM.h"

#include "eflowEvent/eflowObject.h"
#include "eflowEvent/eflowCaloCluster.h"

#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

#include "VxVertex/VxCandidate.h"

#include <vector>
#include <math.h>

#include "Navigation/INavigationToken.h"
#include "Navigation/NavigationToken.h"
#include "Navigation/NavigableVector.h"

////////////////////////////////////////////////////////////////////
// temporary for navigation !!
#include <boost/any.hpp>

eflowObject::eflowObject() : P4EEtaPhiM(0.,0.,0.,0.)
{
  m_d0 = 0.0;
  m_z0 = 0.0;
  m_eflowType = 0;
  m_charge = 0;
  m_valid = true;
  m_passedEOverPCheck = true; //by default true
  m_isSubtracted = false;
  m_isDuplicated = false;
  m_recoStatus = CaloRecoStatus();
  m_nTrack = 0;
  m_nClus = 0;
  m_pi0MVA = 0.0;
  m_centerMag = 0.0;
  m_eflowObjectType = ChargedPion;
}

eflowObject::eflowObject(eflowObject* eflowObj) : P4EEtaPhiM(eflowObj->e(),eflowObj->eta(),eflowObj->phi(),eflowObj->m())
{
  this->initialize(eflowObj,true);
}

eflowObject::eflowObject(eflowObject* eflowObj, bool useClus) : P4EEtaPhiM(eflowObj->e(),eflowObj->eta(),eflowObj->phi(),eflowObj->m())
{
  this->initialize(eflowObj,useClus);
}


void eflowObject::initialize(eflowObject* eflowObj, bool useClus) 
{
  m_d0 = eflowObj->d0();
  m_z0 = eflowObj->z0();
  m_eflowType = eflowObj->eflowType();
  m_charge = eflowObj->charge();
  m_valid = eflowObj->isValid();
  m_passedEOverPCheck = eflowObj->getPassEOverPCheck();
  m_isSubtracted = eflowObj->getIsSubtracted();
  m_isDuplicated = eflowObj->getIsDuplicated();
  m_recoStatus = eflowObj->getCaloRecoStatus();
  m_nTrack = eflowObj->numTrack();
  m_nClus = eflowObj->numClus();
  m_pi0MVA = eflowObj->getPi0MVA();
  m_centerMag = eflowObj->getCenterMag();
  m_eflowObjectType = eflowObj->m_eflowObjectType;

  //add the conversion
  this->addConversion(eflowObj->m_convElementLink);
  //add the muon
  this->addMuon(eflowObj->m_muonElementLink);

  //add any tracks
  this->m_eflowTrack = eflowObj->m_eflowTrack;

  //*possibly* add some clusters
  if (true == useClus) this->m_eflowClus = eflowObj->m_eflowClus;


}


eflowObject::~eflowObject()
{

  std::vector<CaloClusterContainer*>::iterator firstContainer = m_eflowClusContainers.begin();
  std::vector<CaloClusterContainer*>::iterator lastContainer = m_eflowClusContainers.end();

  for (; firstContainer != lastContainer; firstContainer++) delete *firstContainer;
  m_eflowClusContainers.clear();

}

bool eflowObject::checkParticleType(ParticleType particleType) const{
  if (m_eflowObjectType == particleType) return true;
  else return false;
}

const Analysis::Muon* eflowObject::muon() const         { 
    if (m_muonElementLink.isValid()) return *m_muonElementLink;
    else{
      const Analysis::Muon* muon(0);
      return muon;
    }
  }


// new interface supports persistency
void eflowObject::addClus(const ElementLink<CaloClusterContainer>& clusElementLink)
{
  m_eflowClus.addElement(clusElementLink);
}

void eflowObject::addClus(const CaloCluster* clus)
{
  CaloClusterContainer* newContainer = new CaloClusterContainer(SG::VIEW_ELEMENTS);
  eflowCaloCluster* newClus = new eflowCaloCluster(clus);
  newContainer->push_back(newClus);
  m_eflowClus.addElement(newContainer,newClus);
  m_eflowClusContainers.push_back(newContainer);
}


void eflowObject::addTrack(const ElementLink<Rec::TrackParticleContainer>& trackElementLink)
{
  //const Rec::TrackParticleContainer* trackContainer = trackElementLink.getDataPtr();
  //const Rec::TrackParticle* track = *trackElementLink;
  //m_eflowTrack.addElement(trackContainer,track);
  m_eflowTrack.addElement(trackElementLink);
}

////////////////
// Navigation //
////////////////

void
eflowObject::fillToken(INavigationToken& thisToken,
		       const boost::any& aRelation) const
{
  //---------------------------------------------------------------------------
  // eflowObject can honor several queries:
  //
  // - CaloCluster
  // - TrackParticle
  // - Muon
  // - Conversion
  //
  // There are two potential objects to forward the query to, CaloCluster and
  // the egamma object. 
  //---------------------------------------------------------------------------
  
  // parameter type checking
  double weight;
  try        { weight = boost::any_cast<double>(aRelation); } 
  catch(...) { return; }

  //////////////////////////
  // Calorimeter Response //
  //////////////////////////

  if ( m_eflowClus.size() > 0 ) 
    {
      this->navigateClusters(m_eflowClus,thisToken,weight);
    }

  /////////////////
  // TrackParticle //
  /////////////////

  // called from within navigateClusters for performance optimization

  //////////////////
  // Muon //
  //////////////////

  // called from within navigateClusters for performance optimization

  ////////////////
  // Conversion //
  ////////////////

  // called from within navigateClusters for performance optimization

}

//////////////////////////////////////////////
// New Navigation: Temporary Implementation //
//////////////////////////////////////////////

// navigate CaloClusters
void eflowObject::navigateClusters(const cluster_type& theClusters,
			      INavigationToken&   thisToken,
			      double              weight) const
{
  // navigation with weights
  NavigationToken<CaloCluster,double>* weightedToken = 
    dynamic_cast< NavigationToken<CaloCluster,double>* >(&thisToken);
  NavigationToken<CaloCluster>* simpleToken = 
    weightedToken == 0
    ? dynamic_cast< NavigationToken<CaloCluster>* >(&thisToken)
    : 0;

  // query can not be honored, check on other types within eflowObject
  bool isHonored = weightedToken != 0 || simpleToken != 0;
  if ( ! isHonored )
    { 
      if ( m_eflowTrack.size() > 0 )
	{ 
	  isHonored = this->navigateTrackParticles(thisToken,weight);
	}
    }
  if ( ! isHonored )
    {
      isHonored = this->navigateMuons(thisToken,weight);
    }
  if ( ! isHonored )
    {
      isHonored = this->navigateConversions(thisToken,weight);
    }

  // forward query
  if ( ! isHonored )
    {
      cluster_iterator first = theClusters.begin();
      cluster_iterator last  = theClusters.end();
      for ( ; first != last; first++ )
	{
	  (*first)->fillToken(thisToken,weight);
	}
    }

  // fill token
  else
    {
      if ( weightedToken != 0 ) 
	{
	  this->
	    toToken< cluster_type, NavigationToken<CaloCluster,double> >
	    (theClusters,weightedToken,weight);
	}
      else if (simpleToken != 0)
	{
	  this->
	    toToken< cluster_type, NavigationToken<CaloCluster> >
	    (theClusters,simpleToken);
	}
    }
}

// navigate TrackParticles
bool
eflowObject::navigateTrackParticles(INavigationToken& thisToken, 
				  double weight) const
{
  NavigationToken<Rec::TrackParticle,double>* weightedToken = 
    dynamic_cast< NavigationToken<Rec::TrackParticle,double>* >(&thisToken);
  NavigationToken<Rec::TrackParticle>* simpleToken =
    weightedToken == 0 
    ? dynamic_cast< NavigationToken<Rec::TrackParticle>* >(&thisToken)
    : 0;
  bool isHonored = weightedToken != 0 || simpleToken != 0;
  
  if ( isHonored ) 
    {
      if ( weightedToken != 0 )
	{
	  this->toToken< eflowTrack_type, NavigationToken<Rec::TrackParticle,double> >
	    (m_eflowTrack,weightedToken,weight);
	}
      else
	{
	  this->toToken< eflowTrack_type, NavigationToken<Rec::TrackParticle> >
	    (m_eflowTrack,simpleToken);
	}
    }

  return isHonored;
}

// navigate Muons
bool
eflowObject::navigateMuons(INavigationToken& thisToken,
			   double weight) const
{
  NavigationToken<Analysis::Muon,double>* weightedToken = 
    dynamic_cast< NavigationToken<Analysis::Muon,double>* >(&thisToken);
  NavigationToken<Analysis::Muon>* simpleToken =
    weightedToken == 0
    ? dynamic_cast< NavigationToken<Analysis::Muon>* >(&thisToken)
    : 0;
  // honored
  bool isHonored = weightedToken != 0 || simpleToken != 0;

  if ( isHonored )
    {
      if ( weightedToken != 0 )
	{
	  weightedToken->setObject(*m_muonElementLink,weight);
	}
      else
	{
	  simpleToken->setObject(*m_muonElementLink);
	}
    }

  return isHonored;
}

// navigate conversions
bool
eflowObject::navigateConversions(INavigationToken& thisToken,
				 double weight) const
{
  NavigationToken<Trk::VxCandidate,double>* weightedToken =
    dynamic_cast< NavigationToken<Trk::VxCandidate,double>* >(&thisToken);
  NavigationToken<Trk::VxCandidate>* simpleToken =
    weightedToken == 0
    ? dynamic_cast< NavigationToken<Trk::VxCandidate>* >(&thisToken)
    : 0;

  bool isHonored = weightedToken != 0 || simpleToken != 0;
  
  if ( isHonored )
    {
      if ( weightedToken != 0 )
	{
	  weightedToken->setObject(*m_convElementLink,weight);
	}
      else
	{
	  simpleToken->setObject(*m_convElementLink);
	}
    }

  return isHonored;
}

template <typename CONT, typename TOKEN>
void 
eflowObject::toToken(const CONT&  theData,
		     TOKEN*       theToken,
		     double        weight) const
{
  typename CONT::const_iterator first = theData.begin();
  typename CONT::const_iterator last  = theData.end();
  for ( ; first != last; first++ )
    {
      theToken->setObject((*first),weight);
    }
}

template <typename CONT, typename TOKEN>
void 
eflowObject::toToken(const CONT&  theData,
		     TOKEN*       theToken) const
{
  typename CONT::const_iterator first = theData.begin();
  typename CONT::const_iterator last  = theData.end();
  for ( ; first != last; first++ )
    {
      theToken->setObject(*first);
    }
}

const ElementLink<Analysis::MuonContainer>& eflowObject::muonLink() const { return m_muonElementLink;}

const ElementLink<VxContainer>& eflowObject::conversionLink() const { return m_convElementLink;}
		     
	  

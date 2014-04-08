/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// implementation of TauPi0Details

#include "DataModel/ClassName.h"
#include "tauEvent/TauPi0Details.h"
#include "tauEvent/TauPi0Candidate.h"
#include "tauEvent/TauPi0Cluster.h"

using namespace CLHEP;

namespace Analysis {
  
  // default constructor
  TauPi0Details::TauPi0Details() :  
    TauDetails(),
    m_pi0CandContainer(0),
    m_visTau_hlv(HepLorentzVector(0.,0.,0.,0.))
  {
  }
  
  // copy constructor
  TauPi0Details::TauPi0Details(const TauPi0Details& rhs ) :  
    TauDetails(),
    m_pi0CandContainer()
  {
    std::vector<Analysis::TauPi0Candidate*>::const_iterator candIt    = rhs.m_pi0CandContainer.begin();
    std::vector<Analysis::TauPi0Candidate*>::const_iterator candItEnd = rhs.m_pi0CandContainer.end();
    // create new copies and take ownership
    for (; candIt!=candItEnd; ++candIt)  m_pi0CandContainer.push_back( new TauPi0Candidate( *(*candIt) ) );

    // Same for TauPi0Clusters
    std::vector<Analysis::TauPi0Cluster*>::const_iterator clusIt    = rhs.m_pi0ClusContainer.begin();
    std::vector<Analysis::TauPi0Cluster*>::const_iterator clusItEnd = rhs.m_pi0ClusContainer.end();
    // create new copies and take ownership
    for (; clusIt!=clusItEnd; ++clusIt)  m_pi0ClusContainer.push_back( new TauPi0Cluster( *(*clusIt) ) );

    // Same for TauShots
    std::vector<Analysis::TauShot*>::const_iterator shotIt    = rhs.m_shotContainer.begin();
    std::vector<Analysis::TauShot*>::const_iterator shotItEnd = rhs.m_shotContainer.end();
    // create new copies and take ownership
    for (; shotIt!=shotItEnd; ++shotIt)  m_shotContainer.push_back( new TauShot( *(*shotIt) ) );

    m_visTau_hlv = rhs.m_visTau_hlv;
  }

  // Assignment operator
  TauPi0Details& TauPi0Details::operator= ( const TauPi0Details& rhs )
  {
    if (this!=&rhs) {
      std::vector<Analysis::TauPi0Candidate*>::const_iterator candIt    = m_pi0CandContainer.begin();
      std::vector<Analysis::TauPi0Candidate*>::const_iterator candItEnd = m_pi0CandContainer.end();
      // delete existing objects
      for (; candIt!=candItEnd; ++candIt)  delete (*candIt);
      candIt    = rhs.m_pi0CandContainer.begin();
      candItEnd = rhs.m_pi0CandContainer.end();
      // create new copies and take ownership
      for (; candIt!=candItEnd; ++candIt)  m_pi0CandContainer.push_back( new TauPi0Candidate( *(*candIt) ) );

      // Same for TauPi0Clusters
      std::vector<Analysis::TauPi0Cluster*>::const_iterator clusIt    = m_pi0ClusContainer.begin();
      std::vector<Analysis::TauPi0Cluster*>::const_iterator clusItEnd = m_pi0ClusContainer.end();
      // delete existing objects
      for (; clusIt!=clusItEnd; ++clusIt)  delete (*clusIt);
      clusIt    = rhs.m_pi0ClusContainer.begin();
      clusItEnd = rhs.m_pi0ClusContainer.end();
      // create new copies and take ownership
      for (; clusIt!=clusItEnd; ++clusIt)  m_pi0ClusContainer.push_back( new TauPi0Cluster( *(*clusIt) ) );

      // Same for TauShots
      std::vector<Analysis::TauShot*>::const_iterator shotIt    = m_shotContainer.begin();
      std::vector<Analysis::TauShot*>::const_iterator shotItEnd = m_shotContainer.end();
      // delete existing objects
      for (; shotIt!=shotItEnd; ++shotIt)  delete (*shotIt);
      shotIt    = rhs.m_shotContainer.begin();
      shotItEnd = rhs.m_shotContainer.end();
      // create new copies and take ownership
      for (; shotIt!=shotItEnd; ++shotIt)  m_shotContainer.push_back( new TauShot( *(*shotIt) ) );

      m_visTau_hlv = rhs.m_visTau_hlv;
    }
    return *this;
  }

  // destructor
  TauPi0Details::~TauPi0Details() 
  {
    std::vector<Analysis::TauPi0Candidate*>::const_iterator candIt    = m_pi0CandContainer.begin();
    std::vector<Analysis::TauPi0Candidate*>::const_iterator candItEnd = m_pi0CandContainer.end();
    // delete existing objects
    for (; candIt!=candItEnd; ++candIt)  delete (*candIt);   

    // Same for TauPi0Clusters
    std::vector<Analysis::TauPi0Cluster*>::const_iterator clusIt    = m_pi0ClusContainer.begin();
    std::vector<Analysis::TauPi0Cluster*>::const_iterator clusItEnd = m_pi0ClusContainer.end();
    // delete existing objects
    for (; clusIt!=clusItEnd; ++clusIt)  delete (*clusIt);

    // Same for TauShots
    std::vector<Analysis::TauShot*>::const_iterator shotIt    = m_shotContainer.begin();
    std::vector<Analysis::TauShot*>::const_iterator shotItEnd = m_shotContainer.end();
    // delete existing objects
    for (; shotIt!=shotItEnd; ++shotIt)  delete (*shotIt);

  }

  //Class name
  std::string TauPi0Details::s_className = ClassName<TauPi0Details>::name();
  
  const std::string& TauPi0Details::className() const
  {
    return s_className;
  }

}  //end of namespace Analysis

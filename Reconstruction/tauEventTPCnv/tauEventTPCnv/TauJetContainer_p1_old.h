///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TauJetContainer_p1.h 
// Header file for class TauJetContainer_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
// Date:   December 2006
/////////////////////////////////////////////////////////////////// 
#ifndef tauEventTPCnv_TAUJETCONTAINER_P1_OLD_H 
#define tauEventTPCnv_TAUJETCONTAINER_P1_OLD_H 

// STL includes
#include <vector>

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLink_p1.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4EEtaPhiM_p1.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleBase_p1.h"

// forward declarations

class TauJetContainer_p1_old
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 


  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /** Default constructor: 
   */
  TauJetContainer_p1_old() {}

  /** Destructor: 
   */
  ~TauJetContainer_p1_old() {}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 


  /// the 4-mom part
  std::vector<P4EEtaPhiM_p1> m_momenta;

  /// the ParticleBase part
  std::vector<ParticleBase_p1> m_particles;

  // the TauJet part 

  /// links to clusters
  std::vector<ElementLinkInt_p1> m_clusters;
  
  /// links to tracks
  std::vector<ElementLinkIntVector_p1> m_tracks;

  /// links to decay vertex
  std::vector<ElementLinkInt_p1> m_decayVxs;

  /// links to (ESD) tau
  std::vector<ElementLinkInt_p1> m_taus;

  /// TauJet parameters
  std::vector<std::vector<double> > m_parameters;

  /// authors
  std::vector<unsigned int> m_authors;
  std::vector<bool> m_hasDecayVxs;

  std::vector<double> m_emPxs;
  std::vector<double> m_emPys;
  std::vector<double> m_emPzs;
  std::vector<double> m_emEnes;

}; 

#endif //> tauEventTPCnv_TAUJETCONTAINER_P1_OLD_H

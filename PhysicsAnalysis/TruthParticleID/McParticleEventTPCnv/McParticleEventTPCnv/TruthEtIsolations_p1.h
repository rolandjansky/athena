///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TruthEtIsolations_p1.h 
// Header file for class TruthEtIsolations_p1
// Author: S.Binet<binet@cern.ch>
// Date:   July 2007
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENTTPCNV_TRUTHETISOLATIONS_P1_H 
#define MCPARTICLEEVENTTPCNV_TRUTHETISOLATIONS_P1_H 

// STL includes
#include <vector>
#include <utility> // for std::pair

// Boost includes
#include "boost/array.hpp"

// Persistent ElementLink
#include "DataModelAthenaPool/ElementLink_p1.h"

// McParticleEvent includes
#include "McParticleEvent/TruthParticleParamDefs.h" // For enum

// forward declarations
class TruthEtIsolationsCnv_p1;

class TruthEtIsolations_p1 
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class TruthEtIsolationsCnv_p1;

  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
public: 

  typedef boost::array<float, 
		       TruthParticleParameters::NbrOfCones> EtIsolations_t;

  typedef std::pair<int, EtIsolations_t> EtIsolBc_t;
  typedef std::vector<EtIsolBc_t> EtIsolMap_t;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /** Default constructor: 
   */
  TruthEtIsolations_p1();

  /** Destructor: 
   */
  ~TruthEtIsolations_p1() = default;

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
private: 
  
  /** The persistent pointer toward the @c McEventCollection the (transient)
   *  @c TruthEtIsolations has been computed from (or its alias).
   */
  ElementLinkInt_p1 m_genEvent;

  /// The persistent representation of Et-isolations:
  /// a vector pairs (barcode, array-of-Et-isols)
  EtIsolMap_t m_etIsolations;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline 
TruthEtIsolations_p1::TruthEtIsolations_p1() :
  m_genEvent    ( ),
  m_etIsolations( )
{}

#endif //> MCPARTICLEEVENTTPCNV_TRUTHETISOLATIONS_P1_H

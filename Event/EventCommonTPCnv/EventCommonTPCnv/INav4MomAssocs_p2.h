///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INav4MomAssocs_p2.h 
// Header file for class INav4MomAssocs_p2
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_INAV4MOMASSOCS_P2_H 
#define EVENTCOMMONTPCNV_INAV4MOMASSOCS_P2_H 

// STL includes
#include <string>
#include <vector>
#include <utility>

// DataModelAthenaPool
#include "DataModelAthenaPool/DataLink_p1.h"
#include "DataModelAthenaPool/ElementLink_p2.h"
#include "DataModelAthenaPool/ElementLinkContNames_p2.h"

// HepMC / CLHEP includes

// Forward declaration
class INav4MomAssocsCnv_p2;

class INav4MomAssocs_p2
{ 

  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class INav4MomAssocsCnv_p2;

  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
 public:
  // some typedefs as a workaround for templated classes with a long name
  typedef std::vector<ElementLinkInt_p2> ElemLinkVect_t;
  typedef std::pair<ElementLinkInt_p2, 
                    ElemLinkVect_t>      AssocElem_t;
  typedef std::vector<AssocElem_t>       Assocs_t;
  typedef DataLinkVector_p1              INav4MomStores_t;

   /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  INav4MomAssocs_p2();

  /** Destructor: 
   */
  ~INav4MomAssocs_p2(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// container names
  ElementLinkContNames_p2 m_contNames;
  Assocs_t                m_assocs;
  INav4MomStores_t        m_assocStores;

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline 
INav4MomAssocs_p2::INav4MomAssocs_p2() :
  m_contNames(),
  m_assocs(),
  m_assocStores()
{}

inline INav4MomAssocs_p2::~INav4MomAssocs_p2()
{}

#endif //> EVENTCOMMONTPCNV_INAV4MOMASSOCS_P2_H

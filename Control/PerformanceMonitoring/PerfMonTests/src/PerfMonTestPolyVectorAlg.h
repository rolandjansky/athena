///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// PerfMonTestPolyVectorAlg.h 
/// Example for the memory optimization tutorial
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONTESTS_PERFMONTESTPOLYVECTORALG_H 
#define PERFMONTESTS_PERFMONTESTPOLYVECTORALG_H 

#include <map>
#include "AthenaBaseComps/AthAlgorithm.h"

namespace PerfMonTest {
class IHit;
class PolyVectorAlg : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Standard Gaudi Constructor:
  PolyVectorAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~PolyVectorAlg(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 
  /// Property to setup the size of the Hit container
  IntegerProperty m_vectorSize;
  /// Property to setup the amount of elements to reserve
  IntegerProperty m_2bReserved;
  /// Property to set DHIT/FHIT ratio
  IntegerProperty m_mixture;
  /// Property to introduce some fragmentation
  BooleanProperty m_mapIt;
  std::map<int,IHit*> m_mixMap; 



  /// no implementation
  PolyVectorAlg &operator=(const PolyVectorAlg &alg); 
  /// no implementation
  PolyVectorAlg();
}; 
} //> end namespace PerfMonTest

#endif //> PERFMONTESTS_PERFMONTESTPPOLYVECTORALG_H


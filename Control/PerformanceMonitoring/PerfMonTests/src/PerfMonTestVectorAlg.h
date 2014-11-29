///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// PerfMonTestVectorAlg.h 
/// Example for the memory optimization tutorial
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONTESTS_PERFMONTESTVECTORALG_H 
#define PERFMONTESTS_PERFMONTESTVECTORALG_H 

#include <map>
#include "AthenaBaseComps/AthAlgorithm.h"

namespace PerfMonTest {
  class IHit;
class VectorAlg : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Standard Gaudi Constructor:
  VectorAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~VectorAlg(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 
  /// Property to setup the size of the Hit container
  IntegerProperty m_vectorSize;
  /// Property to setup the amount of elements to reserve
  IntegerProperty m_2bReserved;
  /// Property to introduce some fragmentation
  BooleanProperty m_mapIt;
  std::map<int,IHit*> m_mixMap; 


  /// no implementation
  VectorAlg &operator=(const VectorAlg &alg); 
  /// no implementation
  VectorAlg();
}; 
} //> end namespace PerfMonTest

#endif //> PERFMONTESTS_PERFMONTESTPVECTORALG_H


///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// PerfMonTestPolyVectorAlgWithMemSvc.h 
/// Example for the memory optimization tutorial
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONTESTS_PERFMONTESTPOLYVECTORALGWITHMEMSVC_H 
#define PERFMONTESTS_PERFMONTESTPOLYVECTORALGWITHMEMSVC_H 

#include "AthenaBaseComps/AthAlgorithm.h"

namespace PerfMonTest {

class PolyVectorAlgWithMemSvc : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Standard Gaudi Constructor:
  PolyVectorAlgWithMemSvc( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~PolyVectorAlgWithMemSvc(); 

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


  /// no implementation
  PolyVectorAlgWithMemSvc &operator=(const PolyVectorAlgWithMemSvc &alg); 
  /// no implementation
  PolyVectorAlgWithMemSvc();
}; 
} //> end namespace PerfMonTest

#endif //> PERFMONTESTS_PERFMONTESTPPOLYVECTORALG_H


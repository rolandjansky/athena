///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthFilterAlgorithm.h 
// Header file for class AthFilterAlgorithm
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENABASECOMPS_ATHFILTERALGORITHM_H
#define ATHENABASECOMPS_ATHFILTERALGORITHM_H 1

// STL includes
#include <string>

// Framework includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/ICutFlowSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class AthFilterAlgorithm
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  AthFilterAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~AthFilterAlgorithm(); 

  // Assignment operator: 
  //AthFilterAlgorithm &operator=(const AthFilterAlgorithm &alg); 

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Initialization method invoked by the framework. This method is responsible
   *  for any bookkeeping of initialization required by the framework itself.
   *  It will in turn invoke the initialize() method of the derived algorithm,
   *  and of any sub-algorithms which it creates.
   */
  virtual StatusCode sysInitialize();
  
  /// Set the filter passed flag to the specified state
  virtual void setFilterPassed( bool state ) const;

  /// @brief helper method to ease the setting of this filter's description
  /// in derived classes
  /// Note that this method should be called in the c-tor of derived classes
  void setFilterDescription(const std::string& descr);

  /// @brief return a handle to an @c ICutFlowSvc instance
  ServiceHandle<ICutFlowSvc>& cutFlowSvc();

  /// return the @c CutIdentifier corresponding to the top-level cut of this filter algorithm
  CutIdentifier cutID();


  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  AthFilterAlgorithm();

  /// The CutIdentifier for this filter algorithm
  CutIdentifier m_cutID;

  typedef ServiceHandle<ICutFlowSvc> ICutFlowSvc_t;
  /// handle to the service holding tables of cut-flows for filtering algs.
  ICutFlowSvc_t m_cutFlowSvc;

  /// @brief filter description: describes what this filter does.
  /// this is usually pushed to the @c ICutFlowSvc
  std::string m_filterDescr;

  //special case: post-python default value of property FilterDescription sent to ICutFlowSvc only if not explicitly specified before
  bool m_resetSelfDescription;
  void doNotResetSelfDescription( Property& );

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

/** @brief return a handle to an @c ICutFlowSvc instance
 */
inline
ServiceHandle<ICutFlowSvc>& 
AthFilterAlgorithm::cutFlowSvc()
{
  return m_cutFlowSvc;
}


// /** @brief return the top-level @c EventBookkeeper of this filter algorithm
//  */
// inline
// EventBookkeeper* 
// AthFilterAlgorithm::ebk()
// {
//   return m_ebk;
// }


/// return the @c CutIdentifier corresponding to the top-level cut of this filter algorithm
inline
CutIdentifier AthFilterAlgorithm::cutID()
{
  return m_cutID;
}


inline 
void 
AthFilterAlgorithm::doNotResetSelfDescription( Property& )
{ 
  m_resetSelfDescription=false; 
}

#endif //> !ATHENABASECOMPS_ATHFILTERALGORITHM_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_CONDITIONSTESTSVC_H
#define TRT_CONDITIONSTESTSVC_H

/** @file TRT_ConditionsTestSvc.h
 *  @Service for example and test of TRT_ConditionsSummarySvc
 *  Inherits from ITRT_ConditionsSvc base class and returns InDet::TRT_GOOD.
 */

#include "TRT_ConditionsServices/ITRT_ConditionsTestSvc.h"
#include "TRT_ConditionsServices/ITRT_ConditionsSvc.h"
#include "AthenaBaseComps/AthService.h"

class Identifier;

class TRT_ConditionsTestSvc : public AthService,
  virtual public ITRT_ConditionsSvc,
  virtual public ITRT_ConditionsTestSvc
{

 public:

  TRT_ConditionsTestSvc( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TRT_ConditionsTestSvc();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

  /// @name Functions inherited from ITRT_ConditionsTestSvc
  //@{

  /// Test Method
  /** Function inherited from ITRT_ConditionsTestSvc.
   *  Simply prints "Hello World."
   */
  StatusCode test( const Identifier& );

  //@}

  /// @name Functions inherited from ITRT_ConditionsSvc
  //@{

  /// Evaluation for TRT_ConditionsSummarySvc
  /** Function inherited from ITRT_ConditionsSvc.
   *  Simply returns a InDet::TRT_GOOD.
   */
  InDet::TRT_CondFlag condSummaryStatus( const Identifier& );

  //@}

 private:

};

/// Query Interface
inline StatusCode TRT_ConditionsTestSvc::queryInterface( const InterfaceID& riid, void** ppvInterface ) {
  if ( ITRT_ConditionsSvc::interfaceID() == riid ) {
    ATH_MSG_DEBUG("TRT_ConditionsTestSvc interface queried as ITRT_ConditionsSvc.");
    *ppvInterface = dynamic_cast<ITRT_ConditionsSvc*>(this);
    addRef();
  } else if ( ITRT_ConditionsTestSvc::interfaceID() == riid ) {
    ATH_MSG_DEBUG("TRT_ConditionsTestSvc interface queried as ITRT_ConditionsTestSvc.");
    *ppvInterface = dynamic_cast<ITRT_ConditionsTestSvc*>(this);
    addRef();
  } else {
    ATH_MSG_DEBUG("TRT_ConditionsTestSvc interface queried as Service.");
    return AthService::queryInterface( riid, ppvInterface );
  }
  return StatusCode::SUCCESS;
}

inline StatusCode TRT_ConditionsTestSvc::test( const Identifier& ) {  
  ATH_MSG_INFO("Hello World! From TRT_ConditionsTestSvc.");
  return StatusCode::SUCCESS;
}

#endif // TRT_CONDITIONSTESTSVC_H

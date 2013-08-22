/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TRT_ConditionsSummarySvc.h
 * @author Christian.Schmitt@cern.ch, Denver.Whittington@cern.ch
**/
#ifndef TRT_ConditionsSummarySvc_h
#define TRT_ConditionsSummarySvc_h

//STL includes
#include <vector>
#include <string>
//Gaudi Includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"

//interface includes
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "TRT_ConditionsServices/ITRT_ConditionsSvc.h"

//forward declarations
class ISvcLocator;
class Identifier;
class IdentifierHash;
class StatusCode;
class TRT_ID;
namespace InDetDD {
  class TRT_DetectorManager;
}
namespace InDetConditions {
  enum Hierarchy;
}

/**
 * @class TRT_ConditionsSummarySvc
 * Service providing summary of status of an TRT detector element
 * Interface is IInDetConditionsSvc class
**/
class TRT_ConditionsSummarySvc :
  virtual public IInDetConditionsSvc,
  public AthService
{
 public:
  TRT_ConditionsSummarySvc( const std::string& name, ISvcLocator* svc );//!< Service constructor
  virtual ~TRT_ConditionsSummarySvc();
  //@name Gaudi Service Implementation
  //@{
  static const InterfaceID & interfaceID(); //!< reimplemented from Service->IService->IInterface inheritance
  virtual StatusCode initialize();          //!< Service init
  virtual StatusCode finalize();            //!< Service finalize
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  //@}
  
  //@name reimplemented from IInDetConditionsSvc
  //@{
  virtual bool isActive(const Identifier & elementId, const InDetConditions::Hierarchy h=InDetConditions::DEFAULT);
  virtual bool isActive(const IdentifierHash & elementHash);
  virtual bool isActive(const IdentifierHash & elementHash, const Identifier & elementId);
  virtual double activeFraction(const IdentifierHash & elementHash, const Identifier & idStart, const Identifier & idEnd);  
  virtual bool isGood(const Identifier & elementId, const InDetConditions::Hierarchy h=InDetConditions::DEFAULT);
  virtual bool isGood(const IdentifierHash & elementHash);
  virtual bool isGood(const IdentifierHash & elementHash, const Identifier & elementId);
  virtual double goodFraction(const IdentifierHash & elementHash, const Identifier & idStart, const Identifier & idEnd);
  //@}

private:
  ServiceHandleArray<ITRT_ConditionsSvc> m_svcCollection;
  InDet::TRT_CondFlag condSummaryStatus( const Identifier & ident);

  const InDetDD::TRT_DetectorManager* m_manager;
  const TRT_ID* m_trtid;

};

inline const InterfaceID & TRT_ConditionsSummarySvc::interfaceID(){
  return IInDetConditionsSvc::interfaceID();
}

// From s.binet
// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
inline StatusCode TRT_ConditionsSummarySvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IInDetConditionsSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IInDetConditionsSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

#endif // TRT_ConditionsSummarySvc_h

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsSummarySvc.h
 * @author shaun.roe@cern.ch
**/
#ifndef SCT_ConditionsSummarySvc_h
#define SCT_ConditionsSummarySvc_h

//STL includes
#include <vector>
#include <string>
//Gaudi Includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"

//local includes
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"

//forward declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;
class Identifier;
class IdentifierHash;
class ISCT_ConditionsSvc;
class StatusCode;

/**
 * @class SCT_ConditionsSummarySvc
 * Interface class for service providing summary of status of an SCT detector element
**/
class SCT_ConditionsSummarySvc: virtual public IInDetConditionsSvc, public AthService{
  friend class SvcFactory<SCT_ConditionsSummarySvc>;
public:
  SCT_ConditionsSummarySvc( const std::string& name, ISvcLocator* svc );//!< Service constructor
  virtual ~SCT_ConditionsSummarySvc();
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
  StringArrayProperty m_reportingServices; //!< list of services to be used
  typedef std::vector<ServiceHandle<ISCT_ConditionsSvc> > SvcVector_t;
  SvcVector_t m_svcHandles;
  bool m_noReports;
};

inline const InterfaceID & SCT_ConditionsSummarySvc::interfaceID(){
  return IInDetConditionsSvc::interfaceID();
}
#endif

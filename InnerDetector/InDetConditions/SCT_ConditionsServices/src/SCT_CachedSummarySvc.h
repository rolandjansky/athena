/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CachedSummarySvc.h
 * @author shaun.roe@cern.ch
**/
#ifndef SCT_CachedSummarySvc_h
#define SCT_CachedSummarySvc_h

//STL includes
#include <vector>
#include <string>
//Gaudi Includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "SCT_ConditionsCache.h"

//local includes
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "GaudiKernel/IIncidentListener.h"

//forward declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;
class Identifier;
class IdentifierHash;
class ISCT_ConditionsSvc;
class StatusCode;

/**
 * @class SCT_CachedSummarySvc
 * Interface class for service providing summary of status of an SCT detector element
**/
class SCT_CachedSummarySvc: virtual public IInDetConditionsSvc, virtual public IIncidentListener, public AthService{
  friend class SvcFactory<SCT_CachedSummarySvc>;
public:
  SCT_CachedSummarySvc( const std::string& name, ISvcLocator* svc );//!< Service constructor
  virtual ~SCT_CachedSummarySvc();
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
  //@name reimplemented from IIncidentListener
  //@{
    virtual void handle(const Incident& beginRunIncident);
  //@}
private:
  StringArrayProperty m_reportingServices; //!< list of services to be used
  typedef std::vector<ServiceHandle<ISCT_ConditionsSvc> > SvcVector_t;
  SvcVector_t m_svcHandles;
  bool m_noReports;
  SCT_ConditionsCache m_cache;
};

inline const InterfaceID & SCT_CachedSummarySvc::interfaceID(){
  return IInDetConditionsSvc::interfaceID();
}
#endif

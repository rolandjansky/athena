/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
class ISCT_ConditionsSvc;

/**
 * @class SCT_ConditionsSummarySvc
 * Interface class for service providing summary of status of an SCT detector element
**/
class SCT_ConditionsSummarySvc: virtual public IInDetConditionsSvc, public AthService {
  friend class SvcFactory<SCT_ConditionsSummarySvc>;
public:
  SCT_ConditionsSummarySvc(const std::string& name, ISvcLocator* svc); //!< Service constructor
  virtual ~SCT_ConditionsSummarySvc() = default;
  //@name Gaudi Service Implementation
  //@{
  static const InterfaceID& interfaceID(); //!< reimplemented from Service->IService->IInterface inheritance
  virtual StatusCode initialize() override;          //!< Service init
  virtual StatusCode finalize() override;            //!< Service finalize
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  //@}
  
  //@name reimplemented from IInDetConditionsSvc
  //@{
  virtual bool isActive(const Identifier& elementId, const InDetConditions::Hierarchy h=InDetConditions::DEFAULT) override;
  virtual bool isActive(const IdentifierHash& elementHash) override;
  virtual bool isActive(const IdentifierHash& elementHash, const Identifier& elementId) override;
  virtual double activeFraction(const IdentifierHash& elementHash, const Identifier& idStart, const Identifier& idEnd) override;
  virtual bool isGood(const Identifier& elementId, const InDetConditions::Hierarchy h=InDetConditions::DEFAULT) override;
  virtual bool isGood(const IdentifierHash& elementHash) override;
  virtual bool isGood(const IdentifierHash& elementHash, const Identifier& elementId) override;
  virtual double goodFraction(const IdentifierHash& elementHash, const Identifier& idStart, const Identifier& idEnd) override;
  //@}
private:
  StringArrayProperty m_reportingServices; //!< list of services to be used
  typedef ServiceHandle<ISCT_ConditionsSvc> Svc_t;
  typedef std::vector<Svc_t> SvcVector_t;
  SvcVector_t m_svcHandles;
  bool m_noReports;
};

inline const InterfaceID & SCT_ConditionsSummarySvc::interfaceID(){
  return IInDetConditionsSvc::interfaceID();
}
#endif // SCT_ConditionsSummarySvc_h

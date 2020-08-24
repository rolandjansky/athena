/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_TGCCONDSUMMARYSVC_H
#define MUONCONDSVC_TGCCONDSUMMARYSVC_H

#include <string>
#include <set>
#include <vector>

#include "MuonCondInterface/ITGCConditionsSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonCondSvc/MuonHierarchy.h"
#include "StoreGate/DataHandle.h"
#include "Identifier/Identifier.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

template <class TYPE> class SvcFactory;

class TGCCondSummarySvc: virtual public ITGCConditionsSvc, public AthService{
  friend class SvcFactory<TGCCondSummarySvc>;
public:

  TGCCondSummarySvc( const std::string & name, ISvcLocator* svc);
  virtual ~TGCCondSummarySvc()=default;
  virtual StatusCode initialize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID & interfaceID();

  virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys));
  StatusCode update_TGC(IOVSVC_CALLBACK_ARGS_P(I,keys));

  virtual bool isGoodChamber(const Identifier & Id);
  virtual const std::vector<Identifier> &deadStationsId();

private:

 ServiceHandleArray<ITGCConditionsSvc> m_reportingServices; //!< list of services to be used
  
 std::vector<Identifier> m_emptyId;
 
 ServiceHandle<StoreGateSvc> m_detStore;
 bool m_noReports; 

 //compare method for the binary search
 static bool Compare(const Identifier &a, const Identifier &b) {return (a>b);}

};



inline const InterfaceID & TGCCondSummarySvc::interfaceID(){
  return ITGCConditionsSvc::interfaceID(); 
}

#endif

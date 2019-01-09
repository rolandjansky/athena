/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_TGCCONDSUMMARYSVC_H
#define MUONCONDSVC_TGCCONDSUMMARYSVC_H
//STL includes
#include <string>
#include <set>

#include <vector>

//Gaudi Includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"

//local includes
#include "MuonCondSvc/MuonHierarchy.h"
//#include "MuonCondInterface/IMuonConditionsSvc.h"
#include "MuonCondInterface/ITGCConditionsSvc.h"
//Gaudi includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"

//Athena includes
#include "Identifier/Identifier.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

template <class TYPE> class SvcFactory;
class ISvcLocator;
class IdentifierHash;
class TgcIdHelper;
class StatusCode;

class Identifier;
class ITGCConditionsSvc;


class TGCCondSummarySvc: virtual public ITGCConditionsSvc, public AthService{
  friend class SvcFactory<TGCCondSummarySvc>;
public:

  TGCCondSummarySvc( const std::string & name, ISvcLocator* svc);
  virtual ~TGCCondSummarySvc(){}
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID & interfaceID();

  virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys));
  StatusCode update_TGC(IOVSVC_CALLBACK_ARGS_P(I,keys));

  // virtual bool isActive(const Identifier & elementId, const MuonConditions::Hierarchy h=MuonConditions::DEFAULT);
  //virtual bool isActive(const IdentifierHash & elementHash);
  virtual bool isGoodChamber(const Identifier & Id);
 
  
  virtual const std::vector<Identifier> &deadStationsId();
  
 
private:

 ServiceHandleArray<ITGCConditionsSvc> m_reportingServices; //!< list of services to be used
 
  
 std::vector<Identifier> m_emptyId;
 const TgcIdHelper * m_pHelper;
 
 ServiceHandle<StoreGateSvc> m_detStore;
 bool m_noReports; 

 //compare method for the binary search
 static bool Compare(const Identifier &a, const Identifier &b) {return (a>b);}

};



inline const InterfaceID & TGCCondSummarySvc::interfaceID(){
  return ITGCConditionsSvc::interfaceID(); 
}

#endif

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_CSCCONDSUMMARYSVC_H
#define MUONCONDSVC_CSCCONDSUMMARYSVC_H

#include <string>
#include <set>
#include <vector>

#include "MuonCondInterface/ICSCConditionsSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonCondSvc/MuonHierarchy.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

template <class TYPE> class SvcFactory;
class ISvcLocator;

class CSCCondSummarySvc: virtual public ICSCConditionsSvc, public AthService {
  friend class SvcFactory<CSCCondSummarySvc>;
public:

  CSCCondSummarySvc( const std::string & name, ISvcLocator* svc);
  virtual ~CSCCondSummarySvc()=default;
  virtual StatusCode initialize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID & interfaceID();

  virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys));
  StatusCode update_CSC(IOVSVC_CALLBACK_ARGS_P(I,keys));
 
  // virtual bool isActive(const Identifier & elementId, const MuonConditions::Hierarchy h=MuonConditions::DEFAULT);
  //virtual bool isActive(const IdentifierHash & elementHash);
 
  virtual bool isGoodChamber(const Identifier & Id) const;
  virtual bool isGoodWireLayer(const Identifier & Id) const;  
  virtual bool isGood(const Identifier & Id) const;
 
   
  virtual const std::vector<Identifier> &deadStationsId() const;
  virtual const std::vector<Identifier>& deadWireLayersId() const;

  virtual const std::vector<std::string> &deadStations() const;
  virtual const std::vector<std::string>& deadWireLayers() const;

 private:

  //StringArrayProperty m_reportingServices; //!< list of services to be used
 ServiceHandleArray<ICSCConditionsSvc> m_reportingServices; //!< list of services to be used
 
 ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  
 std::vector<Identifier> m_emptyId;
 std::vector<std::string> m_empty;
 
 ServiceHandle<StoreGateSvc> m_detStore;
 bool m_noReports; 
 bool m_usesimulation;
 //compare method for the binary search
 static bool Compare(const Identifier &a, const Identifier &b) {return (a>b);}
 
};



inline const InterfaceID & CSCCondSummarySvc::interfaceID(){
  return ICSCConditionsSvc::interfaceID(); 
}

#endif

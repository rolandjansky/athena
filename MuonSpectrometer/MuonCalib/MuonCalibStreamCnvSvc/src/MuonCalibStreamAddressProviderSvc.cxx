/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Include files
#include "MuonCalibStreamCnvSvc/MuonCalibStreamAddressProviderSvc.h"
#include "MuonCalibStreamCnvSvc/MuonCalibStreamAddress.h"

#include "StoreGate/StoreGate.h" 
#include "SGTools/TransientAddress.h"
#include "GaudiKernel/ListItem.h"

#include "AthenaKernel/IClassIDSvc.h"

/// Standard constructor
MuonCalibStreamAddressProviderSvc::MuonCalibStreamAddressProviderSvc(const std::string& name, ISvcLocator* svc)
: AthService( name, svc) {
  m_typeNames.push_back("Muon::MdtPrepDataContainer/MDT_DriftCircles");
  m_typeNames.push_back("RpcPadContainer/RPCPAD");
  m_typeNames.push_back("TgcRdoContainer/TGCRDO");
  m_typeNames.push_back("CscRawDataContainer/CSCRDO");
  declareProperty("TypeNames", m_typeNames);
}

/// Standard Destructor
MuonCalibStreamAddressProviderSvc::~MuonCalibStreamAddressProviderSvc() {
}

/// Initialize the service.
StatusCode MuonCalibStreamAddressProviderSvc::initialize() {

  ATH_CHECK( service("MuonCalibStreamDataProviderSvc", m_dataSvc) ); 

  ATH_MSG_INFO( " initialized " );
  return StatusCode::SUCCESS;
}

StatusCode MuonCalibStreamAddressProviderSvc::preLoadAddresses(StoreID::type idp, tadList& tlist) {
  ATH_MSG_DEBUG( " in preLoadAddress " ); 

  if(idp!=StoreID::EVENT_STORE) return StatusCode::SUCCESS ; 

  // only deal with event store. 
  std::vector<std::string>::const_iterator it   = m_typeNames.begin(); 
  std::vector<std::string>::const_iterator it_e = m_typeNames.end(); 

  IClassIDSvc* clidSvc ; 
  ATH_CHECK( service("ClassIDSvc",clidSvc ) ); 

  for(; it!=it_e;++it) {
    ListItem item(*it); 
    std::string t = item.type();
    std::string nm = item.name();
    
    CLID id ;
    StatusCode sc = clidSvc->getIDOfTypeName(t, id); 
    if(sc!=StatusCode::SUCCESS) {
      ATH_MSG_WARNING( " can not create TAD for (type,name)"<<
	" no CLID for " << t ); 
    } else { 
      SG::TransientAddress* tad = new SG::TransientAddress( id, nm );
      tlist.push_back(tad) ; 
      ATH_MSG_DEBUG( " created TAD for (type,clid,name)"<<t
	  <<" "<<id<<" "<<nm ); 

      // save the clid and key. 
      m_clidKey[id].insert(nm); 
    } 
  }
  return StatusCode::SUCCESS; 
}

// update an existing transient Address
StatusCode MuonCalibStreamAddressProviderSvc::updateAddress(StoreID::type /*tp*/, SG::TransientAddress* tad) {
  CLID clid = tad->clID(); 
  std::string nm = tad->name();
  std::map<CLID, std::set<std::string> >::const_iterator it = m_clidKey.find(clid); 
  
  if(it==m_clidKey.end()) return StatusCode::FAILURE ; 

  if((*it).second.count(nm) == 0) return StatusCode::FAILURE ; 

  ATH_MSG_DEBUG( " creating address for " << clid<<" "<< nm );

  MuonCalibStreamAddress* add = new MuonCalibStreamAddress(clid,nm,""); 
  tad->setAddress(add); 
  return StatusCode::SUCCESS; 
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBSTREAMCNVSVCBASE_MUONCALIBSTREAMADDRESSPROVIDERSVC_H
#define MUONCALIBSTREAMCNVSVCBASE_MUONCALIBSTREAMADDRESSPROVIDERSVC_H

#include "AthenaKernel/IAddressProvider.h" 
#include "AthenaBaseComps/AthService.h" 
#include "GaudiKernel/ClassID.h" 
#include "MuonCalibStreamCnvSvc/IMuonCalibStreamDataProviderSvc.h"
#include <map>
#include <set>

// Forward declarations

template <class TYPE> class SvcFactory;

class MuonCalibStreamAddressProviderSvc : public virtual AthService,
					 public virtual IAddressProvider
 {
  /// Allow the factory class access to the constructor
  friend class SvcFactory<MuonCalibStreamAddressProviderSvc>;
public:

  MuonCalibStreamAddressProviderSvc(const std::string& name, ISvcLocator* svcloc);
  virtual ~MuonCalibStreamAddressProviderSvc();

  // Service initialize 
  virtual StatusCode initialize();

  // IAddressProvider interface. 
  // preload the address 
  virtual StatusCode preLoadAddresses(StoreID::type id, tadList& tlist); 

  /// update an existing transient Address
  virtual StatusCode updateAddress(StoreID::type tp, SG::TransientAddress* tad) ;

private: 

  // type and name of the objects to create the address for.
  std::vector<std::string> m_typeNames; 

  IMuonCalibStreamDataProviderSvc* m_dataSvc; 

  std::map<CLID, std::set<std::string> > m_clidKey ; 

 }; 

#endif 


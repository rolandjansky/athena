/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARCALORIMETER_LARFLATCONDITIONSVC_H
#define LARCALORIMETER_LARFLATCONDITIONSVC_H

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IAddressProvider.h"
#include "GaudiKernel/IIncidentListener.h"
#include <string>

class IIOVSvc;
class StoreGateSvc;
class IClassIDSvc;
class CondAttrListCollection;


template <class TYPE> class SvcFactory;

class LArFlatConditionSvc : public virtual IAddressProvider,
			    public virtual IIncidentListener,
			    public AthService {

  friend class SvcFactory<LArFlatConditionSvc>;
  
 public:
  LArFlatConditionSvc( const std::string& name, ISvcLocator* svc );
  virtual ~LArFlatConditionSvc();
  
  /// Serice init
  virtual StatusCode initialize();

  /// Service finalize
  virtual StatusCode finalize();
  
  /// Query the interfaces.
  //virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  //static const InterfaceID& interfaceID();
  
  /// Service type.
  ///virtual const InterfaceID& type() const;

  /* IAddress provider interface */

  typedef IAddressProvider::tadList tadlist;
  typedef tadList::iterator tadListIterator;
 
  ///get all addresses from Provider : Called before Begin Event
  virtual StatusCode preLoadAddresses(StoreID::type /* storeID */,
				      tadList& /* list */);

  /// get all new addresses from Provider for this Event.
  virtual StatusCode loadAddresses(StoreID::type /* storeID */,
				   tadList& /* list */) 
  { return StatusCode::SUCCESS;}

  /// update a transient Address
  virtual StatusCode updateAddress(StoreID::type,
                                   SG::TransientAddress* tad,
                                   const EventContext& ctx);

  ///Incident handle
  virtual void handle( const Incident& incident );


 private:
  ServiceHandle<IIOVSvc>         m_IOVSvc;
  ServiceHandle<StoreGateSvc>    m_detStore;
  ServiceHandle<IClassIDSvc>     m_clidSvc;


  struct objInfo_t {
  public:
    std::string m_classname;
    std::string m_ifacename;
    std::string m_inputKey;
    std::string m_outputKey;
    CLID m_clid;
    CLID m_sclid;
  };

  std::vector<objInfo_t> m_objInfo;
  CLID m_attrListClid;
  bool m_initializing;

  bool m_doRegularCells;
  bool m_doSuperCells;
  bool m_printChecksum;

  template<class T> 
  T* createFlatObj(const CondAttrListCollection* attr,  SG::TransientAddress* tad);


  void printCheckSums(const objInfo_t& objInfo, const CondAttrListCollection* attr); 
};


#endif

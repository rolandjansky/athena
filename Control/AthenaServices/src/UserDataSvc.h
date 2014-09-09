/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef USERDATASTORESERVICE
#define USERDATASTORESERVICE

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthService.h"

#include "AthenaKernel/IUserDataSvc.h"
#include "boost/any.hpp"

//Forward declarations
class UserDataStore;
class StoreGateSvc;
class IAthenaBarCode;

template <class TYPE> class SvcFactory;


class UserDataSvc: virtual public IUserDataSvc,
		   public AthService,
		   public IIncidentListener 
 {

 protected:
   friend class SvcFactory<UserDataSvc>;

 public:

   UserDataSvc(const std::string& name, ISvcLocator* pSvcLocator );
  
  ~UserDataSvc();

   virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

   StatusCode initialize();
   StatusCode finalize();


   //For Element decoration:
   virtual StatusCode recordAny(const IAthenaBarCode& abc, const std::string& label, const boost::any& value);  
   virtual StatusCode retrieveAny(const IAthenaBarCode& abc, const std::string& label, const boost::any*& value);
   virtual bool contains(const IAthenaBarCode& obj, const std::string& label);
   virtual std::vector<const std::string*> getLabels(const IAthenaBarCode& obj);

   //For Event decoration:
   virtual StatusCode recordAny(const std::string& label, const boost::any& value);  
   virtual StatusCode retrieveAny(const std::string& label, const boost::any*& value);
   virtual bool contains(const std::string& label);
   virtual std::vector<const std::string*> getLabels();



   //Legacy interface
   virtual int vdecorateElement(const IAthenaBarCode &abc, const std::string& label,
				const std::type_info &decoinfo, void* & deco);

   virtual int vgetElementDecoration(const IAthenaBarCode &abc, const std::string& label,
				     const std::type_info &decoinfo, void *&deco);


 private:

   class DummyABC : virtual public IAthenaBarCode {
   public:
     DummyABC(const AthenaBarCode_t in) : m_abc(in) {}; 
     AthenaBarCode_t getAthenaBarCode() const { return m_abc; }
     void setAthenaBarCode(const AthenaBarCode_t in) { m_abc=in;}
   private:
     AthenaBarCode_t m_abc;

   };

   
   void getUserDataStore();


   template<typename T> 
   bool AnyFromType(const std::type_info &decoinfo, void* & deco, boost::any& out);

   StatusCode typeIDtoAny(const std::type_info &decoinfo, void* & deco, boost::any& out);

   ServiceHandle<StoreGateSvc> m_storeGateSvc;
  
   /** incident service handle for {Begin,End}Event
    */
   void handle( const Incident& incident );
   
   std::string m_key;
   UserDataStore* m_userData;
   DummyABC m_eventABC;

};


template<typename T> 
bool UserDataSvc::AnyFromType(const std::type_info &decoinfo, void* & deco, boost::any& out) {
  if (decoinfo==typeid(T)) {
    const T* v=static_cast<const T*>(deco);
    out=(*v);
    return true;
  }
  else
    return false;
}

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISPROPERTYMGR_H
#define ISPROPERTYMGR_H

//// Gaudi includes
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/Auditor.h"

// to avoid compiler warning
#ifdef PACKAGE_VERSION
#undef PACKAGE_VERSION
#endif

/// ISProperty includes
#include "ISProperty/IISPropertySvc.h"
#include "ISProperty/ISProperty.h"

/** @class ISPropertyMgr
 
    @brief Generic base for gaudi classes using ISPropertySvc
    
    @author  Sebastian Boeser <sboeser@hep.ucl.ac.uk>
*/  

class ISPropertyMgr : 
  virtual public INamedInterface,
  virtual public IProperty
{ 

  public:
    /// publish the properties declared with declareISProperty
    StatusCode publishProperties();

    /// unpublish the properties declared with declareISProperty
    StatusCode unpublishProperties();

    /// return a list of the names of the properties (to be) published
    PropertyList getISProperties();

  protected:

    /// Declare an IS property (templated)
    template < class TYPE > Property* declareISProperty ( const std::string& pname  ,
            TYPE& value , const std::string& doc = "none" ) {
            
      //try to identify the type of this-pointer via its interface
      //WARNING: This is dangerous, since other classes than Service, Algorithm
      //and AlgTool may inherit from IService,IAlgorithm, IAlgTool
      //However, currently there is no other way as declareProperty is not
      //defined in any of the interfaces!

      //Currently four implementations of IProperty interface
      Algorithm* thisAlgorithm;
      AlgTool* thisAlgTool;
      Service* thisService;
      Auditor* thisAuditor;

      //Retain a pointer to the declared property
      Property* theProp = NULL ; 

      //Try query-interface to get proper interface
      //--Algorithm
      if (queryInterface(IAlgorithm::interfaceID(),(void**)&thisAlgorithm).isSuccess()){
        //Obtain log
        MsgStream log(thisAlgorithm->msgSvc(),name()+":ISPropertyMgr");
        log << MSG::VERBOSE << "identified this-pointer as Algorithm" << endreq;
        //declare Gaudi property
        theProp = thisAlgorithm->declareProperty(pname,value,doc);

      //--AlgTool
      } else if (queryInterface(IAlgTool::interfaceID(),(void**)&thisAlgTool).isSuccess()){
        //Obtain msgSvc
        MsgStream log(thisAlgTool->msgSvc(),name()+":ISPropertyMgr");
        log << MSG::VERBOSE << "identified this-pointer as AlgTool" << endreq;
        //declare Gaudi property
        theProp = thisAlgTool->declareProperty(pname,value,doc);
      
      //--Service
      } else if (queryInterface(IService::interfaceID(),(void**)&thisService).isSuccess()){
        //Obtain log
        MsgStream log(thisService->msgSvc(),name()+":ISPropertyMgr");
        log << MSG::VERBOSE << "identified this-pointer as Service" << endreq;
        //declare Gaudi property
        theProp = thisService->declareProperty(pname,value,doc);

      //--Auditor
      } else if (queryInterface(IAuditor::interfaceID(),(void**)&thisAuditor).isSuccess()){
        //Obtain log
        MsgStream log(thisAuditor->msgSvc(),name()+":ISPropertyMgr");
        log << MSG::VERBOSE << "identified this-pointer as Auditor" << endreq;
        //declare Gaudi property
        theProp = thisAuditor->declareProperty(pname,value,doc);
      //-- Unknown
      } else {
        //Obtain msgSvc - can not use service()
        IMessageSvc* msgSvc;
        StatusCode sc = m_pSvcLocator->service( "MessageSvc", msgSvc, true );
        if( sc.isFailure() )
           throw GaudiException("Service [MessageSvc] not found", name(), sc);
        //Obtain log
        MsgStream log(msgSvc,name()+":ISPropertyMgr");
        log << MSG::WARNING << "Unable to identify this-pointer type - IAlgorithm,IAlgTool,IService & IAuditor query failed!" << endreq;
        log << MSG::WARNING << "Property \"" << pname << "\" will not be published to IS!";
        return NULL ;
      }

      //If we have create a new property
      if (theProp != NULL){
        //Create a handle to it
        GaudiPropertyHandle propHandle(pname,this);
        //Add this property to the list of properties to be published
        m_ISProperties.push_back(propHandle);
      }

      return theProp ; 
    }

    /// Declare a property that will only be available in IS, but not as jobOption
    template <class TYPE>
    Property* declareISOnlyProperty ( const std::string& pname, TYPE& property,
                                      const std::string& doc = "none" ){

      //Create local propertyMgr if it does not exist
      if (! m_propertyMgr) m_propertyMgr = new PropertyMgr();
      
      //now add the property to the local propertyMgr
      Property* theProp = m_propertyMgr->declareProperty(pname, property, doc);

      //If we have create a new property
      if (theProp != NULL){
        //Create a handle to it using our local propertyMgr
        GaudiPropertyHandle propHandle(pname, m_propertyMgr) ;
        //Add this property to the list of properties to be published
        m_ISProperties.push_back(propHandle);
      }

      return theProp;
    }
   
    /// Declare a remove IS property, i.e. a property from another Service/Algorithm/AlgTool
    template <class TYPE>
    Property* declareISRemoteProperty ( const std::string& pname, GaudiHandle<TYPE>& remoteHandle ){

      //Obtain msgSvc - can not use service()
      IMessageSvc* msgSvc;
      StatusCode sc = m_pSvcLocator->service( "MessageSvc", msgSvc, true );
      if (sc.isFailure()) 
         throw GaudiException("Service [MessageSvc] not found", name(), sc);
      //Obtain log
      MsgStream log(msgSvc,name()+":ISPropertyMgr");

      //Obtain the remote Service/Algorithm/AlgTool
      if ( remoteHandle.retrieve().isFailure() ){
        log << MSG::WARNING << "Unable to retrieve object from GaudiHandle " << remoteHandle << endreq;
        log << MSG::WARNING << "Property \"" << pname << "\" will not be published to IS!";
        return NULL ;
      }

      //Check wether this is really an IProperty
      IProperty* remotePropertyMgr = dynamic_cast<IProperty*>(&(*remoteHandle));
      if (remotePropertyMgr == NULL ) {
        log << MSG::WARNING << remoteHandle << " does not reference an IProperty " << endreq;
        log << MSG::WARNING << "Property \"" << pname << "\" will not be published to IS!";
        return NULL ;
      }

      //Create local propertyMgr if it does not exist
      if (! m_propertyMgr) m_propertyMgr = new PropertyMgr();

      //now add the property to the local propertyMgr as a remote property
      Property* theProp = m_propertyMgr->declareRemoteProperty(pname, remotePropertyMgr, pname);

      //If we have create a new property
      if (theProp == NULL){
        log << MSG::WARNING << remoteHandle << " does not have the property " << pname << endreq;
        log << MSG::WARNING << "Property \"" << pname << "\" will not be published to IS!";
        return NULL ;
      }
      
      //Create a handle to it using the remote property manager so we can
      //acess it usign Gaudi::Utils::getProperty() (only local props)
      GaudiPropertyHandle propHandle(pname, remotePropertyMgr) ;
      //Add this property to the list of properties to be published
      m_ISProperties.push_back(propHandle);

      return theProp;
    }

    ///Construtor is protected so we can't explicitly generate this class
    ISPropertyMgr( const std::string& name , ISvcLocator* pSvcLocator );
 
    ///Destrutor is protected so we can't explicitly destroy this class
    virtual ~ISPropertyMgr() = 0;

    ///Get an ISOnly property (e.g. if its a remote property)
    const Property& getISOnlyProperty( const std::string& name ) const;
    
    ///Get the name of the IS object 
    const std::string ISName();

  protected:
    
    /** handle to the IS property service */
    ServiceHandle<IISPropertySvc> m_ISPropSvc;

  private:

    /** A list of properties to be published */
    PropertyList m_ISProperties;

    /** Service locator handle */
    ISvcLocator* m_pSvcLocator;

    /** Local property manager for IS-only properties */
    PropertyMgr* m_propertyMgr;

  protected:

    /** Minimum intervall inbetween two updates of an ISProperty (in seconds) */
    double m_updateTime;

    /** Wether to publish unique names with hostname and pid */
    bool m_uniqueName;

};

#endif

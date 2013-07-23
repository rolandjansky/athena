/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "ISProperty/ISPropertySvc.h"
#include "ISProperty/ISProperty.h"

#include <is/infoT.h>
#include <is/infoiterator.h>
#include <ipc/core.h>

#include <iterator>
#include <algorithm>
#include <ostream>


  /// Constructor
  ISPropertySvc::ISPropertySvc(const std::string& name, ISvcLocator* svcLoc) :
    Service(name,svcLoc),
    log(msgSvc(),name)
  {
     //mp_PropMap = &m_PropertyMap;
     declareProperty("PartitionName",m_PartitionName = "ATLAS","Name of the partition in which the IS server is running");
     declareProperty("ServerName", m_ServerName = "Athena","Name of the IS server to which the information is published");
     declareProperty("ForcePublish", m_forcePublish = false,"Delete already published object with same name");
  }

  // Destructor.
  ISPropertySvc::~ISPropertySvc()
  {
    //be verbose
    log << MSG::VERBOSE << "destructor()" << endreq;
    //Undeclare pseudo-singleton
    m_this = NULL ;
  }
 
  /// Return singleton instance
  ISPropertySvc* ISPropertySvc::self(){
    return m_this;
  }

  /// Initialize
  StatusCode ISPropertySvc::initialize() {
    
    //Setup service - needed to update jo
    Service::initialize().ignore();
    
    //setup logging
    log.setLevel(outputLevel());
    log << MSG::VERBOSE << "initialize()" << endreq;

    /// Setup pseudo-singleton
    /// - check for existance
    if (m_this != NULL ) {
      log << MSG::FATAL << "Another instance of ISPropertySvc already exists!" << endreq;
      return StatusCode::FAILURE;
    }
    /// now set singleton pointer
    m_this = this ;

    // Try to initialize IPC core
    try {
      int nIPCOpts =0;
      if (!IPCCore::isInitialised()) IPCCore::init(nIPCOpts,NULL);
    } catch ( daq::ipc::Exception & ex ) {
      log << MSG::FATAL << "Unable to initialize IPC core : " << ex.what() << endreq;
      return StatusCode::FAILURE;
    } 

    // Create the instance of partition handle
    m_partition = IPCPartition(m_PartitionName);
    if (! m_partition.isValid() ) {
      log << MSG::FATAL << "Could not obtain valid partition with name " << m_PartitionName << endreq;
      return StatusCode::FAILURE;
    }

    // Create the IS receiver instance in the specific partitition
    m_rec = new ISInfoReceiver(m_partition);

    return StatusCode::SUCCESS;
  }

  /// Finalize
  StatusCode ISPropertySvc::finalize() {
    
    //be verbose
    log << MSG::VERBOSE << "finalize()" << endreq;

    //Unpublish all remaining objects
    ISPropertyMap::const_iterator propItr = m_PropertyMap.begin();
    while ( propItr != m_PropertyMap.end()){
      //be a bit verbose
      log << MSG::INFO << "ISInfo object " << (*propItr).first << " still published - forcing unpublish" << endreq;
      //now unpublish
      unpublish((*propItr).first).ignore();
      //property will be erased - reget first object
      propItr = m_PropertyMap.begin();
    }

    //Now delete 
    delete m_rec;

    return StatusCode::SUCCESS;
  }

  /**
   * @brief Custom callback handler from IS side
   *   - notify gaudi property callbacks if update was not triggered from our side
   *   - recreate property in case it has been deleted
   **/
  void ISPropertySvc::ISCallbackHandler(ISCallbackInfo * isc){

    self()->log << MSG::DEBUG << "Callback for " << isc->name();
    self()->log << ", reason is  " << isc->reason() << endreq;
    
    //Remove server name from string
    std::string propName = std::string(isc->name()).substr(std::string(isc->name()).find('.')+1);
    
    //check if we have that property
    ISPropertyMap::iterator propItr = self()->m_PropertyMap.find(propName);
    if ( propItr == self()->m_PropertyMap.end()) {
      self()->log << MSG::WARNING << "Received callback for property " << propName;
      self()->log << " which is not handled by this ISPropertySvc" << endreq;
      return ;
    }
 
    //On update, check if we need to notify athena handler
    if ( isc->reason() ==  is::Updated ){
      //get the updated ISObject
      ISInfoAny is; isc->value(is);
      ISInfoAny* ours = (ISInfoAny*)(self()->m_PropertyMap[propName]);

      self()->log << MSG::VERBOSE << "Update object: " << is << endreq;
      self()->log << MSG::VERBOSE << "Our object: " << (*ours) << endreq;

      ///Check if the IS event is newer than ours
      if ( ours->time() < isc->time() ){
        self()->log << MSG::DEBUG << "Update from IS-side!" << endreq;
        //call IS update handler
        self()->m_PropertyMap[propName]->updateFromISHandler(is);
      }
    }
    //On delete, check if we still have that property.
    if ( isc->reason() ==  is::Deleted ){
      //Make sure ISProperty object exist
      if (self()->m_PropertyMap[propName]){
        self()->log << MSG::WARNING << "Property " << propName << " has been deleted in IS - will create again!" << endreq;
        //Re-publish this propery to IS
        self()->m_PropertyMap[propName]->checkin();
      }
    }
  }
  
  /**
   * @brief Publish a property set to IS
   *  - make sure it doesn't exist
   *  - create ISProperty and add to local map
   * @param name the name of the IS object
   * @param type_name the type name of the IS object
   * @param props a list of GaudiProprtyHandles
   * @param updateTime the minimum interval in seconds inbetween to updates of this IS object
   */
  StatusCode ISPropertySvc::publish( const std::string name, std::string type_name, const PropertyList props, double updateTime ) {

    //Make sure property does not exist yet in out list
    if (m_PropertyMap.find(name) != m_PropertyMap.end()){
      log << MSG::ERROR << "ISProperty object with name " << name << " is already published" << endreq;
      return StatusCode::FAILURE;
    }

    //Create the IS name from the server name and object name
    std::string ISObjName = m_ServerName+"."+name;
    //IS class name is simply the given type name
    std::string ISClassName = type_name;

    //Create new ISProperty from property
    m_PropertyMap[name] = new ISProperty(m_partition, ISObjName, ISClassName, props, updateTime);

    // Also Make sure property does not exist yet  at the is-server
    if (m_PropertyMap[name]->isExist()){
      //In forcePublish mode, just remove it and replace by our own
      if (m_forcePublish){
        log << MSG::WARNING << "Removing existing ISInfo objects with name " << name << " from ISServer " << m_ServerName << endreq;
        m_PropertyMap[name]->remove();
      } else {
      // otherwise return a failure
        log << MSG::ERROR << "ISInfo objects with name  " << name << " already exists at ISServer " << m_ServerName << endreq;
        return StatusCode::FAILURE;
      }
    }

    // Subscribe for changes of that object
    m_rec->subscribe( ISObjName, &ISPropertySvc::ISCallbackHandler );

    //Finally, update the property in IS
    m_PropertyMap[name]->checkin();

    //be a bit verbose
    log << MSG::DEBUG << "Created ISProperty: " << (*m_PropertyMap[name]) << endreq;


    return StatusCode::SUCCESS;
  }

  /**
   * @brief Unpublish a property set
   *  - remove ISProperty from local map
   */
  StatusCode ISPropertySvc::unpublish( const std::string name ){

    // Unsubscribe for changes of that object
    // Waiting for completion to make sure we don't remove before unsubscribe
    log << MSG::DEBUG << "Unsubscribing callback handler for " << name << endreq;
    try {
      m_rec->unsubscribe(m_ServerName+"."+name, true);
    } catch (std::exception& ex){
      log << MSG::ERROR << "Failed to unsubscribe callback handler: " << ex.what() << endreq;
    }

    //Make sure object exists in our map
    if (m_PropertyMap.count(name) == 0){
      log << MSG::ERROR << "ISProperty object with name " << name << " does not exist" << endreq;
      return StatusCode::SUCCESS;
    }
    log << MSG::VERBOSE << "Deleting ISProperty with name " << name << endreq;
    //Delete the ISProperty object
    delete m_PropertyMap[name];
    //Erase from map
    m_PropertyMap.erase(m_PropertyMap.find(name));

    //show remaining properites
    log << MSG::VERBOSE << "Remaining properties: [";
    for(std::map<std::string, ISProperty*>::const_iterator it = m_PropertyMap.begin(); it != m_PropertyMap.end(); ++it) 
      log << (*it).first << " ";
    log << "]" << endreq; 

    return StatusCode::SUCCESS;
  }



  /**
   * Return proper interface
   */
  StatusCode ISPropertySvc::queryInterface( const InterfaceID& riid, void** ppvIF) {
    // if this is a request to the IISPropertySvc, cast to IISPropertySvc
    if ( IISPropertySvc::interfaceID().versionMatch(riid) ) {
      *ppvIF = dynamic_cast<IISPropertySvc*> (this);
    // if this is about handling properties, cast to IProperty
    } else if ( IProperty::interfaceID().versionMatch(riid) ) {
      *ppvIF = dynamic_cast<IProperty*>(this);
    // else handle by base class
    } else {
      return Service::queryInterface(riid, ppvIF);
    }
    //Check if casting was successfull
    if (*ppvIF == NULL) return StatusCode::FAILURE;

    //Add reference counter
    addRef();
    return StatusCode::SUCCESS;
  }


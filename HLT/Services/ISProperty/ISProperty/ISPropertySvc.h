// this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISPROPERTY_ISPROPERTYSVC_H 
#define ISPROPERTY_ISPROPERTYSVC_H 

#include <GaudiKernel/Service.h>
#include <GaudiKernel/MsgStream.h>

#include <ISProperty/IISPropertySvc.h>
#include <ISProperty/ISProperty.h>

#include <ipc/partition.h>
#include <is/infodictionary.h>
#include <is/inforeceiver.h>

/**
 *  @class ISPropertySvc
 *  @brief Publish properties of a Gaudi-Component to IS Service and listen for
 *  updates. This is a pseudo-singleton with a public constructor so it can be
 *  created by Gaudi. Will return FAILURE on double-create in initialization.
 * 
 *  @author Sebastian Boeser
 * 
 */

class ISPropertySvc : public Service, virtual public IISPropertySvc {

  public:

     // Constructors:
     ISPropertySvc(const std::string& name, ISvcLocator* svcloc);

     // Destructor.
     virtual ~ISPropertySvc();
      
     // Gaudi default methods
     virtual StatusCode initialize();
     virtual StatusCode finalize();

     // Publish a list of properties to IS
     virtual StatusCode publish( const std::string name, const std::string type_name, const PropertyList props, double updateTime ) ;

     // Unpublish a property set
     virtual StatusCode unpublish( const std::string name );

    /** Return proper interface */
    virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIF);

    // Handling changes of object status in IS
    static void ISCallbackHandler(ISCallbackInfo* isc);

    // Return singleton selfpointer
    static ISPropertySvc* self();

  private:

     //@defgroup Gaudi properties
     //@{
     std::string m_PartitionName;       //!< Name of the partition in which the IS server is running
     std::string m_ServerName;          //!< Name of the IS server to which the information is published
     bool m_forcePublish;               //!< Delete already published information with same name
     //@}

     IPCPartition m_partition;          //!< A handle to the partition
     ISPropertyMap m_PropertyMap;       //!< A list of the published properties  
     ISInfoReceiver* m_rec;             //!< The object for receiving status changes from IS


     //logging 
     mutable MsgStream log;

     //Singleton selfpointer
     static ISPropertySvc* m_this;

};

//Pseudo-singleton instanciation
ISPropertySvc* ISPropertySvc::m_this = NULL ;

#endif  

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file IOVRegistrationSvc.h 
 * 
 * @brief This is an interface to a tool used to register conditions
 * objects in the Interval of Validity (IOV) database
 * 
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Antoine Pérus <perus@lal.in2p3.fr>
 * 
 * $Id: IOVRegistrationSvc.h,v 1.19 2009-01-29 16:38:27 hawkings Exp $
 */

#ifndef REGISTRATIONSERVICES_IOVREGISTRATIONSVC_H
# define REGISTRATIONSERVICES_IOVREGISTRATIONSVC_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "RegistrationServices/IIOVRegistrationSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ClassID.h"
//#include "GaudiKernel/IProperty.h"
#include "CoolKernel/StorageType.h"

//<<<<<< FORWARD DECLARATIONS                                             >>>>>>

class IIOVCondDbSvc;
class StoreGateSvc;
class IAddressCreator;   
class IClassIDSvc;   


//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 ** @class IOVRegistrationSvc 
 ** 
 **    Properties:
 **
 **    - RecreateFolder:<pre>  flag to force the recreation of the requested folder</pre>
 **    - BeginRun:<pre>        Begin run number</pre>
 **    - EndRun:<pre>          End run number</pre>
 **    - BeginLB:<pre>      Begin LB number</pre>
 **    - EndLB:<pre>        End LB number</pre>
 **    - BeginTime:<pre>       Begin time (ns)</pre>
 **    - EndTime:<pre>         End time (ns)</pre>
 **    - IOVDbTag:<pre>        the tag to be used</pre>
 **    - IOVDbTimeStamp:<pre>  the flag for time versus run-LB use</pre>
 **/
class IOVRegistrationSvc : public AthService,
                           public virtual IIOVRegistrationSvc
{
  // Forward declarations
  template <class TYPE> class SvcFactory;
  
public:    
  
    IOVRegistrationSvc( const std::string& name, ISvcLocator* svc );

    virtual ~IOVRegistrationSvc();

    /// Query the interfaces.
    virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface);

    /// Service type.
    virtual const InterfaceID& type() const;

    /// Register IOV DB for an object given its typeName - run/LB numbers
    ///   interval or times interval  and tag are taken from JobOptions
    ///   Choice between run/LB and timestamp given in JobOptions
    virtual StatusCode registerIOV( const std::string& typeName ) const;
    
    /// Register IOV DB for an object given its typeName - run/LB numbers
    ///   interval or times interval taken from JobOptions
    ///   tag is specified
    ///   Choice between run/LB and timestamp given in JobOptions
    virtual StatusCode registerIOV( const std::string& typeName, const std::string& tag ) const;
    
    /// Register IOV DB for an object given its typeName and its key
    ///   run/LB numbers interval or times interval  and tag are taken
    ///   from JobOptions
    ///   Choice between run/LB and timestamp given in JobOptions
    virtual StatusCode registerIOV( const std::string& typeName, const std::string& key,
				    const std::string& tag ) const;
    
    /// Register IOV DB for an object given its typeName, tag and run/LB
    /// numbers interval
    virtual StatusCode registerIOV( const std::string& typeName, 
				    const std::string& tag,
				    unsigned int beginRun, 
				    unsigned int endRun, 
				    unsigned int beginLB, 
				    unsigned int endLB ) const;
    

    /// Register IOV DB for an object given its typeName, tag and
    /// times interval
    virtual StatusCode registerIOV( const std::string& typeName, 
				    const std::string& tag,
				    uint64_t beginTime, 
				    uint64_t endTime ) const;

    /// Register IOV DB for an object given its typeName, key, tag and run/LB
    /// numbers interval
    virtual StatusCode registerIOV( const std::string& typeName,
				    const std::string& key,
				    const std::string& tag,
				    unsigned int beginRun, 
				    unsigned int endRun, 
				    unsigned int beginLB, 
				    unsigned int endLB ) const;
    
    /// Register IOV DB for an object given its typeName, key, tag and
    /// times interval
    virtual StatusCode registerIOV( const std::string& typeName,
				    const std::string& key,
				    const std::string& tag,
				    uint64_t beginTime, 
				    uint64_t endTime ) const;

    /// Register IOV DB for an object given its typeName, key, folder, 
    /// tag and run/LB  numbers interval
    virtual StatusCode registerIOV( const std::string& typeName,
				    const std::string& key,
				    const std::string& folder,
				    const std::string& tag,
				    unsigned int beginRun, 
				    unsigned int endRun, 
				    unsigned int beginLB, 
				    unsigned int endLB ) const;

    /// Register IOV DB for an object given its typeName, key, folder, tag and
    /// times interval
    virtual StatusCode registerIOV( const std::string& typeName,
				    const std::string& key,
				    const std::string& folder,
				    const std::string& tag,
				    uint64_t beginTime, 
				    uint64_t endTime ) const;

    /// Initialize AlgTool
    virtual StatusCode initialize();

    /// Finalize AlgTool
    virtual StatusCode finalize();


private:
    
    // Allow SvcFactory to instantiate the service.
    friend class SvcFactory<IOVRegistrationSvc>;

    StatusCode registerIOV(const std::string& typeName,
			   const std::string& key,
			   const std::string& folder,
			   const std::string& tag,
			   const IOVTime&     begin,
			   const IOVTime&     end) const;
    
    StatusCode registerIOVCOOL(const std::string& typeName,
			       const std::string& key,
			       const std::string& folderName,
			       const std::string& tag,
			       const IOVTime&     begin,
			       const IOVTime&     end) const;

    cool::StorageType::TypeId coralToCoolType(const std::string& parname,
				   const std::string& coralName) const;

    StatusCode getStringAddress(const CLID& clid,
				const std::string& key,
				std::string& saddr) const;

    uint64_t timeToNano(const unsigned long int timesec) const;

    /// Build the folder description field
    ///  add in front of the description the value with identifier-markups
    virtual StatusCode buildDescription(const std::string&  identifier,
					const std::string&  value,
					std::string& description) const;

    /// Split address in its header and data parts
    StatusCode splitAddress(const std::string& address,
			    std::string& address_header,
			    std::string& address_data) const;




    BooleanProperty         m_recreateFolders;
    UnsignedIntegerProperty m_beginRun;
    UnsignedIntegerProperty m_endRun;
    UnsignedIntegerProperty m_beginLB;
    UnsignedIntegerProperty m_endLB;
    
    UnsignedLongProperty m_beginTime;
    UnsignedLongProperty m_endTime;

    StringProperty          m_tag;
    BooleanProperty         m_timeStamp;
    StringProperty          m_tagDescription;
    BooleanProperty         m_writeKeyInfo;
    BooleanProperty         m_userTags;
    BooleanProperty         m_userTagsUH;
    BooleanProperty         m_svFolder;
    BooleanProperty         m_payloadTable;
    BooleanProperty         m_forceGlobalIOV;
    std::vector<std::string> m_overrideName;
    std::vector<std::string> m_overrideType;

    mutable ServiceHandle<IIOVCondDbSvc> m_iov_db;
    ServiceHandle<StoreGateSvc>    m_detStore;
    mutable ServiceHandle<IAddressCreator> m_persSvc;   
    ServiceHandle<IClassIDSvc>     m_clidSvc;   
    
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif //  REGISTRATIONSERVICES_IOVREGISTRATIONSVC_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RegistrationStream.h
 *
 * @brief Algorithm functioning like an OutputStream which registers a
 * reference to an event, i.e. a POOL Token, in an event
 * collection. Selection criteria may be applied to accept or reject
 * events.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Jack Cranshaw <Jack.Cranshaw@cern.ch>
 *
 * $Id: RegistrationStream.h,v 1.30 2009-04-28 19:04:06 cranshaw Exp $
 *
 */

#ifndef ATHENAPOOLTEST_REGISTRATIONSVC_H
# define ATHENAPOOLTEST_REGISTRATIONSVC_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIoComponent.h"

#include "AthenaBaseComps/FilteredAlgorithm.h"

#include <map>

//<<<<<< PUBLIC TYPES                                                   >>>>>>

template <class ConcreteAlgorithm> class AlgFactory;
class IAddressCreator;
class IRegistrationStreamTool;
class StoreGateSvc;
namespace SG 
{
    class DataProxy;
    class IFolder;
}

/**
 * @class RegistrationStream
 *
 * @brief Algorithm functioning like an OutputStream which registers a
 * reference to an event, i.e. a POOL Token, in an event
 * collection. Selection criteria may be applied to accept or reject
 * events.
 *
 */

class RegistrationStream : public FilteredAlgorithm,
                           virtual public IIncidentListener,
                           virtual public IIoComponent 
{
  friend class AlgFactory<RegistrationStream>;
public:
    
    /// Initialize RegistrationStream
    virtual StatusCode initialize();

    /// Terminate RegistrationStream
    virtual StatusCode finalize();

    virtual void handle(const Incident& incident);

    /// Working entry point
    virtual StatusCode execute();

    /// Callback method to reinitialize the internal state of the component for I/O purposes (e.g. upon @c fork(2))
    virtual StatusCode io_reinit();
    virtual StatusCode io_finalize();

    /// Standard RegistrationStream Constructor
    RegistrationStream(const std::string& name, ISvcLocator* pSvcLocator); 

    /// Standard Destructor
    virtual ~RegistrationStream();

private:

    /// PRIVATE METHODS
    /// Handler for ItemNames Property
    void itemListHandler( Property& );

    /// Handler for ProvNames Property
    void ProvIncludeHandler( Property& );

    /// Handler for ItemNames Property
    void ProvExcludeHandler( Property& );

    /// NOTE that the Tools assume this list will be ordered.
    ///  begin = newest; end = oldest
    ///  duplicate elimination is left to the Tools
    StatusCode getRefs(std::vector<std::pair<std::string,std::string> >& refs);

    std::string getAttListKey();

    std::vector<std::string> getCollMetadataKeys();

    /// PRIVATE DATA

    /// name of store:
    StringProperty           m_storeName;

    /// Vector of item names to register
    StringArrayProperty      m_itemList;

    /// If this flag is set then ProvInclude is disregarded, but ProvExclude applies
    BooleanProperty          m_writeAllProvFlag;
    /// List of process tags to include in output
    StringArrayProperty      m_provIncludeList;
    /// List of process tags to include in output
    StringArrayProperty      m_provExcludeList;
    /// Storage object for m_provIncludeList
    std::set<std::string>    m_provInclude;
    /// Storage object for m_provExcludeList
    std::set<std::string>    m_provExclude;

    /// the top-level folder with headers to be registered
    ToolHandle<SG::IFolder>  m_2BRegistered;

    /// Flag to signal whether or not to write input data header
    BooleanProperty          m_writeInputDH;

    /// Compatibility data actually used by tools not RegistrationStream
    /// type of the collection
    StringProperty           m_collectionType; 
    
    /// DB connection
    StringProperty           m_connection; 

    /// Name of the output collection
    StringProperty           m_outputCollection;

    /// Collection open mode
    StringProperty           m_openMode; 

    /// Tools for handling the metadata technology
    ToolHandle<IRegistrationStreamTool> m_regTool;

};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // ATHENAPOOLTEST_REGISTRATIONSVC_H

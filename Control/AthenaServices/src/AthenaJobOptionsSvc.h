///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JobOptionsSvc.h 
// Header file for class JobOptionsSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENASERVICES_ATHENAJOBOPTIONSSVC_H
#define ATHENASERVICES_ATHENAJOBOPTIONSSVC_H 1

// C includes
#include <stdio.h>

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthService.h"

// GaudiKernel
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ServiceHandle.h"

// Forward declaration
class ISvcLocator;
template <class TYPE> class SvcFactory;

namespace Athena {

class JobOptionsSvc
  : virtual public ::IJobOptionsSvc,
            public ::AthService
{ 
  friend class SvcFactory<JobOptionsSvc>;
  using AthService::getProperties;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  JobOptionsSvc( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~JobOptionsSvc(); 

  // Assignment operator: 
  //JobOptionsSvc &operator=(const JobOptionsSvc &alg); 

  /// Gaudi Service Implementation
  //@{
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface );
  //@}

  /// @c IJobOptionsSvc interface
  //@{
  /** Override default properties of the calling client
      @param client Name of the client algorithm or service
      @param me Address of the interface IProperty of the client
  */
  virtual StatusCode setMyProperties 
  ( const std::string& client, IProperty* me );  
  
  /// Add a property into the JobOptions catalog
  virtual StatusCode addPropertyToCatalogue( const std::string& client, 
                                             const Gaudi::Details::PropertyBase& property );
  /// Remove a property from the JobOptions catalog
  virtual StatusCode removePropertyFromCatalogue( const std::string& client, 
                                                  const std::string& name );
  /// Get the properties associated to a given client
  virtual const std::vector<const Gaudi::Details::PropertyBase*>*
  getProperties( const std::string& client) const; 

  /// Get a property for a client
  virtual const Gaudi::Details::PropertyBase*
  getClientProperty( const std::string& client,
		     const std::string& name ) const;

  /// Get the list of clients
  virtual std::vector<std::string> getClients() const;
  
  /** look for file 'File' into search path 'Path' 
   *  and read it to update existing JobOptionsCatalogue 
   *  @param File file   name 
   *  @param Path search path 
   *  @return status code 
   */
  virtual StatusCode readOptions 
  ( const std::string& file      , 
    const std::string& path );
  //@}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  static const InterfaceID& interfaceID();

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  JobOptionsSvc();

  /// Property handler
  void propertyHandler(Gaudi::Details::PropertyBase& p);

  /// handle to the "real" @c IJobOptionsSvc we are proxying
  ServiceHandle<IJobOptionsSvc> m_jobosvc;
  
  std::string         m_source_path;
  std::string         m_source_type;
  std::string         m_dirSearchPath;
  // optional output file to dump all properties 
  std::string         m_dump   ; ///< optional output file to dump all properties 

  /// switch to dump the full python-like configuration table
  ///  if empty string (default) no dump
  std::string m_cat_fname;

  /// file handle for the configuration table dump
  FILE *m_cat_fd;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& JobOptionsSvc::interfaceID() 
{ 
  return IJobOptionsSvc::interfaceID(); 
}

} //> end namespace Athena

#endif //> !ATHENASERVICES_ATHENAJOBOPTIONSSVC_H

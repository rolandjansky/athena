///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// PyJobOptionsSvc.h 
// Header file for class PyJobOptionsSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAPYTHON_PYJOBOPTIONSSVC_H
#define ATHENAPYTHON_PYJOBOPTIONSSVC_H 1

// STL includes
#include <string>
#include <vector>
#include <list>

// FrameWork includes
#include "AthenaBaseComps/AthService.h"

// GaudiKernel
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/PropertyHolder.h"
//#include "GaudiKernel/PropertyMgr.h"

// AthenaPython includes
#include "PyJobOptionsCatalogue.h"

#include "CxxUtils/checker_macros.h"

// Forward declaration
class ISvcLocator;
template <class TYPE> class SvcFactory;
class IProperty;
//class  Property;
struct _object; 
typedef _object PyObject;

class ATLAS_NOT_THREAD_SAFE PyJobOptionsSvc
  : virtual public ::IJobOptionsSvc,
    virtual public ::IProperty,
            public ::AthService
{ 
  friend class SvcFactory<PyJobOptionsSvc>;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // unhides some of AthService's methods
  using AthService::setProperty;
  using AthService::getProperty;
  using AthService::getProperties;

  // Copy constructor: 

  /// Constructor with parameters: 
  PyJobOptionsSvc( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~PyJobOptionsSvc(); 

  // Assignment operator: 
  //PyJobOptionsSvc &operator=(const PyJobOptionsSvc &alg); 

  /// Gaudi Service Implementation
  //@{
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface ) override;
  //@}

  /// @c IJobOptionsSvc interface
  ///@{
  /** Override default properties of the calling client
      @param client Name of the client algorithm or service
      @param me Address of the interface IProperty of the client
  */
  virtual 
  StatusCode setMyProperties (const std::string& client, IProperty* me) override;
  
  /// Add a property into the JobOptions catalog
  virtual 
  StatusCode addPropertyToCatalogue (const std::string& client, 
				     const Property& property ) override;
  /// Remove a property from the JobOptions catalog
  virtual 
  StatusCode removePropertyFromCatalogue (const std::string& client, 
					  const std::string& name ) override;

  /// Get the properties associated to a given client
  virtual 
  //  const std::vector<const Property*>* 
  const std::vector<const Gaudi::Details::PropertyBase*>* 
  getProperties (const std::string& client) const override;

  /// Get a property for a client
  const Gaudi::Details::PropertyBase* 
  getClientProperty( const std::string& client,
                     const std::string& name ) const override;

  /// Get the list of clients
  virtual 
  std::vector<std::string> getClients() const override;
  
  /** look for file 'File' into search path 'Path' 
   *  and read it to update existing JobOptionsCatalogue 
   *  @param File file   name 
   *  @param Path search path 
   *  @return status code 
   */
  virtual 
  StatusCode readOptions (const std::string& file,
			  const std::string& path) override;

  ///@}

  /// IProperty implementation (needed for initialisation)
  // StatusCode setProperty(const Property& p);
  // StatusCode getProperty(Property *p) const;

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
  PyJobOptionsSvc();

  // PropertyMgr m_pmgr;

  // /// path to joboption file
  // std::string m_source_path;
  // /// source type (old-txt, py, pickle?)
  // std::string m_source_type;
  // /// list of paths to directories to look for joboptions
  // std::string m_dir_search_path;

  // /// optional output file to dump all properties 
  // std::string m_dump;

  Gaudi::Property<std::string> m_source_type{this, "TYPE"};
  Gaudi::Property<std::string> m_source_path{this, "PATH"};
  Gaudi::Property<std::string> m_dir_search_path{this, "SEARCHPATH"};
  Gaudi::Property<std::string> m_dump{this, "DUMPFILE"};


  /// catalogue holding the properties
  PyJobOptionsCatalogue m_catalogue;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline 
const InterfaceID& 
PyJobOptionsSvc::interfaceID() 
{ 
  return IJobOptionsSvc::interfaceID(); 
}



#endif //> !ATHENAPYTHON_PYJOBOPTIONSSVC_H

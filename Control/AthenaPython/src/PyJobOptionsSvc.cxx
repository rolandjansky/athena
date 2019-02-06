///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// PyJobOptionsSvc.cxx 
// Implementation file for class PyJobOptionsSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "Python.h"

// AthenaPython includes
#include "PyJobOptionsSvc.h"
#include "AthenaPython/PyProperty.h"
#include "AthenaPython/PyAthenaUtils.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "GaudiKernel/System.h"

// PyRoot
#include "TPython.h"

typedef PyJobOptionsCatalogue::Property_t   Property_t;
typedef PyJobOptionsCatalogue::Properties_t Properties_t;
typedef PyJobOptionsCatalogue::Objects_t    Objects_t;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
PyJobOptionsSvc::PyJobOptionsSvc (const std::string& name, 
				  ISvcLocator* pSvcLocator)
  : base_class (name, pSvcLocator),
  // m_pmgr       (),
  // m_source_path(),
  // m_source_type(),
  // m_dir_search_path(),
  m_catalogue()
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  { // bwd-compat with Gaudi joboptionssvc
    std::string tmp =System::getEnv ("JOBOPTSEARCHPATH");
    if (!tmp.empty() && ("UNKNOWN" != tmp)) {
      m_dir_search_path = tmp;
    }
  }

  { // bwd-compat with Gaudi joboptionssvc
    std::string tmp =System::getEnv ("JOBOPTSDUMPFILE");
    if (!tmp.empty() && ("UNKNOWN" != tmp)) {
      m_dump = tmp;
    }
  }

  // m_pmgr.declareProperty ("TYPE", 
  //       		  m_source_type = "NONE",
  //       		  "type of the joboptions to parse (txt,py,pickle)");
  // m_pmgr.declareProperty ("PATH", 
  //       		  m_source_path,
  //       		  "path to the joboptions to parse");
  // m_pmgr.declareProperty ("SEARCHPATH", 
  //       		  m_dir_search_path,
  //       		  "list of directory-paths to look for joboptions");
  // m_pmgr.declareProperty ("DUMPFILE", 
  //       		  m_dump,
  //       		  "name of a file where to dump the list of options");

}

// Athena Service's Hooks
////////////////////////////
StatusCode PyJobOptionsSvc::initialize()
{
  StatusCode sc = StatusCode::SUCCESS;
  ATH_MSG_INFO ("Initializing " << name() << "...");
  
  // read the job options if needed
  if (m_source_type != "NONE") {
    sc = this->readOptions (m_source_path, m_dir_search_path);
    if (!sc.isSuccess()) {
      ATH_MSG_WARNING ("problem while reading options !");
    }
  }

  return sc;
}

StatusCode PyJobOptionsSvc::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

/** Override default properties of the calling client
 *  @param client Name of the client algorithm or service
 *  @param me Address of the interface IProperty of the client
 */
StatusCode
PyJobOptionsSvc::setMyProperties (const std::string& client, 
				  IProperty* component)
{
  const Properties_t* props = m_catalogue.properties (client);
  if (!props || !component) {
    return StatusCode::SUCCESS;
  }

  bool allgood = true;
  for (Properties_t::const_iterator
	 iprop = props->begin(),
	 iend  = props->end();
       iprop != iend;
       ++iprop) {
    if (!component->setProperty (**iprop).isSuccess()) {
      ATH_MSG_ERROR ("Unable to set the property '" 
		     << (*iprop)->name() << "'"
		     << " of '" << client << "'."
		     << endmsg
		     << "Check option and algorithm names, type and bounds.");
      allgood = false;
    }
  }

  return allgood 
    ? StatusCode::SUCCESS 
    : StatusCode::FAILURE;
}

/// Add a property into the JobOptions catalog
StatusCode
PyJobOptionsSvc::addPropertyToCatalogue (const std::string& client, 
					 const Property& property)
{
  ATH_MSG_VERBOSE ("::addPropToCat (client=[" << client << "], prop=["
		   << property.name() << ", "
		   << System::typeinfoName(*property.type_info()) << "], "
		   << "prop_val=["
		   << property.toString() << "])...");

  const PyProperty* pyprop = dynamic_cast<const PyProperty*>(&property);
  if (pyprop) {
    return m_catalogue.add_property (client, pyprop->clone());
  }
  
  Property *p = new StringProperty (property.name(), "");
  if (!property.load (*p)) {
    delete p; p = 0;
    return StatusCode::FAILURE;
  }
  return m_catalogue.add_property (client, p);
}

/// Remove a property from the JobOptions catalog
StatusCode 
PyJobOptionsSvc::removePropertyFromCatalogue (const std::string& client, 
					      const std::string& name)
{
  return m_catalogue.remove_property (client, name);
}

/// Get the properties associated to a given client
const Properties_t* 
PyJobOptionsSvc::getProperties (const std::string& client) const
{
  return m_catalogue.properties (client);
}

const Property_t*
PyJobOptionsSvc::getClientProperty( const std::string& client,
                                    const std::string& name ) const 
  {
    return m_catalogue.getProperty( client, name );
  }


/// Get the list of clients
std::vector<std::string> 
PyJobOptionsSvc::getClients() const
{
  return m_catalogue.clients();
}
  
/** look for file 'File' into search path 'Path' 
 *  and read it to update existing JobOptionsCatalogue 
 *  @param File file   name 
 *  @param Path search path 
 *  @return status code 
 */
StatusCode 
PyJobOptionsSvc::readOptions (const std::string& file,
			      const std::string& path)
{
  // use the default non-empty path (if any) if no path specified
  if (path.empty() && !m_dir_search_path.empty()) {
    return readOptions (file, m_dir_search_path);
  }

  ATH_MSG_DEBUG ("Job-options read from the file [" << file << "]");
  // do the parsing --> invoke python
  TPython::LoadMacro (file.c_str());
  if ( PyErr_Occurred() ) {
    PyAthena::throw_py_exception();
  }
  
  ATH_MSG_INFO ("Job options successfully read in from [" << file <<"]");
  // fillServiceCatalogue();

  return StatusCode::SUCCESS;
}

// /// IProperty implementation (needed for initialisation)
// StatusCode 
// PyJobOptionsSvc::setProperty(const Property& p)
// {
//   return m_pmgr.setProperty (p);
// }

// /// IProperty implementation (needed for initialisation)
// StatusCode
// PyJobOptionsSvc::getProperty(Property *p) const
// {
//   return m_pmgr.getProperty (p);
// }

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 



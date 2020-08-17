///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthenaJobOptionsSvc.cxx 
// Implementation file for class JobOptionsSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AthenaServices includes
#include "AthenaJobOptionsSvc.h"

// C includes
#include <stdio.h>

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IAuditor.h"

namespace Athena {

  std::string pythonize_value(const Gaudi::Details::PropertyBase* prop);

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
JobOptionsSvc::JobOptionsSvc( const std::string& name, 
		      ISvcLocator* pSvcLocator ) : 
  ::AthService( name, pSvcLocator ),
  m_jobosvc("JobOptionsSvc/AthWrapped_JobOptionsSvc", name),
  m_cat_fd (nullptr)
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  std::string tmp ;
  tmp = System::getEnv ( "JOBOPTSEARCHPATH" ) ;
  if ( !tmp.empty() && ("UNKNOWN" != tmp) ) { m_dirSearchPath = tmp ; }
  tmp = System::getEnv ( "JOBOPTSDUMPFILE"  ) ;
  if ( !tmp.empty() && ("UNKNOWN" != tmp) ) { m_dump          = tmp ; }
  
  declareProperty( "TYPE"       , m_source_type   )
    ->declareUpdateHandler(&Athena::JobOptionsSvc::propertyHandler, this);
                                                                                
  declareProperty( "PATH"       , m_source_path   )
    ->declareUpdateHandler(&Athena::JobOptionsSvc::propertyHandler, this);

  declareProperty( "SEARCHPATH" , m_dirSearchPath )
    ->declareUpdateHandler(&Athena::JobOptionsSvc::propertyHandler, this);

  declareProperty( "DUMPFILE"   , m_dump          )
    ->declareUpdateHandler(&Athena::JobOptionsSvc::propertyHandler, this);

  declareProperty( "PythonizedCatalogue", 
                   m_cat_fname = "jobo.catalogue.ascii.py",
                   "switch to dump the full python-like configuration table "
                   "if empty string (default) no dump" )
    ->declareUpdateHandler(&Athena::JobOptionsSvc::propertyHandler, this);
}

// Destructor
///////////////
JobOptionsSvc::~JobOptionsSvc()
{}

// Athena Service's Hooks
////////////////////////////
StatusCode JobOptionsSvc::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  if (!m_jobosvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR("could not retrieve [" << m_jobosvc.typeAndName() << "] !");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("retrieved [" << m_jobosvc.typeAndName() << "]");
  }

  ATH_MSG_INFO("PythonizedCatalogue: [" << m_cat_fname << "]");
  if (!m_cat_fname.empty()) {
    ATH_MSG_INFO("opening [" << m_cat_fname << "] to dump properties'"
                 " of all components");
    m_cat_fd = fopen(m_cat_fname.c_str(), "w+");
    if (m_cat_fd == nullptr) {
      ATH_MSG_ERROR("could not open the pythonized catalogue output file ["
                    << m_cat_fname << "] !");
      return StatusCode::FAILURE;
    } else {
      // write header...
      fprintf(m_cat_fd, 
              "# %s\n# %s\n",
              "Content of the (Athena) JobOptionsSvc's pythonized catalog",
              this->name().c_str());
      fprintf(m_cat_fd,
              "%s",
              "d = {\n");
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode JobOptionsSvc::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  // if (!m_jobosvc->finalize().isSuccess()) {
  //   ATH_MSG_ERROR("could not finalize [" << m_jobosvc.typeAndName() << "] !");
  //   return StatusCode::FAILURE;
  // }

  if (m_cat_fd != nullptr) {
    fprintf(m_cat_fd,
            "%s",
            "\n"
            "}\n"
            "# EOF #\n");
    fflush(m_cat_fd);
    fclose(m_cat_fd);
  }

  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
JobOptionsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IJobOptionsSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IJobOptionsSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return ::AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

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

void
JobOptionsSvc::propertyHandler(Gaudi::Details::PropertyBase& p)
{
  if (!m_jobosvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR("could not retrieve service [" 
                  << m_jobosvc.typeAndName() << "]");
    return;
  }

  SmartIF<IProperty> proxy = &*m_jobosvc;
  if (p.name () != "PythonizedCatalogue") {
    if (!proxy->setProperty(p).isSuccess()) {
      ATH_MSG_ERROR("could not transfer property [" << p.name() << "] "
                    << "with value [" << p.toString() << "] to svc ["
                    << m_jobosvc.typeAndName() << "] !");
      return;
    }
  } else {
    m_cat_fname = p.name();
  }
}

/** Override default properties of the calling client
    @param client Name of the client algorithm or service
    @param me Address of the interface IProperty of the client
*/
StatusCode
JobOptionsSvc::setMyProperties( const std::string& client, IProperty* me )
{
  //ATH_MSG_INFO("::setMyProperties(" << client << ")...");
  StatusCode sc = m_jobosvc->setMyProperties(client, me);
  //ATH_MSG_INFO("::setMyProperties(" << client << ")... [done]");

  // no dumping was requested...
  if (m_cat_fd == nullptr) {
    return sc;
  }

#define _INDENT "    "

  // first, fill the catalogue with *all* the prop-values of the component
  fprintf(m_cat_fd, "\n%s'%s' : {\n", _INDENT, client.c_str());
  fprintf(m_cat_fd, "%s'props': {\n", _INDENT _INDENT);

  const std::vector<Gaudi::Details::PropertyBase*>& props = me->getProperties();
  for ( std::vector<Gaudi::Details::PropertyBase*>::const_iterator 
          iprop = props.begin(),
          iend  = props.end();
        iprop != iend;
        ++iprop ) {
    const std::string value = Athena::pythonize_value(*iprop);
    fprintf(m_cat_fd, "%s'%s' : %s,\n", 
            _INDENT _INDENT _INDENT,
            (*iprop)->name().c_str(),
            value.c_str());
  }
  fprintf(m_cat_fd, "%s%s", _INDENT _INDENT, "},\n");
  fflush(m_cat_fd);

  std::string cxx_type = "N/A";
  std::string comp_type = "N/A";

  {
    // try IAlgTool
    SmartIF<IAlgTool> iface(me);
    if (iface.isValid()) {
      cxx_type = iface->type();
      SmartIF<INamedInterface> parent((IInterface*)iface->parent());
      if (parent->name() == "ToolSvc") {
        comp_type = "public_tool";
      } else {
        comp_type = "private_tool";
      }
      goto end;
    }
  }
  {
    // try IAlgorithm
    SmartIF<IAlgorithm> iface(me);
    if (iface.isValid()) {
      //cxx_type = iface->type();
      comp_type= "alg";
      goto end;
    }
  }
  {
    // try IService
    SmartIF<IService> iface(me);
    if (iface.isValid()) {
      //cxx_type = iface->type();
      comp_type= "svc";
      goto end;
    }
  }
  {
    // try IAuditor
    SmartIF<IAuditor> iface(me);
    if (iface.isValid()) {
      //cxx_type = iface->type();
      comp_type= "aud";
      goto end;
    }
  }
  {
    // still there ?
    // then we don't know how to handle that IProperty...
    ATH_MSG_INFO("==> name=[" << client << "] is of unhandled type !");
  }

 end:

  // ATH_MSG_INFO("==> name=[" << client <<"] type=["
  //              << cxx_type << "] (" << comp_type << ")");

  fprintf(m_cat_fd, 
          "%s'cxx_type':  '%s',\n"
          "%s'comp_type': '%s',\n"
          "%s},\n",
          _INDENT _INDENT, cxx_type.c_str(),
          _INDENT _INDENT, comp_type.c_str(),
          _INDENT);
  fflush(m_cat_fd);
#undef _INDENT

  return sc;
}
  
/// Add a property into the JobOptions catalog
StatusCode 
JobOptionsSvc::addPropertyToCatalogue( const std::string& client, 
                                    const Gaudi::Details::PropertyBase& property )
{
  return m_jobosvc->addPropertyToCatalogue(client, property);
}

/// Remove a property from the JobOptions catalog
StatusCode
JobOptionsSvc::removePropertyFromCatalogue( const std::string& client, 
                                            const std::string& name )
{
  return m_jobosvc->removePropertyFromCatalogue(client, name);
}

/// Get the properties associated to a given client
const std::vector<const Gaudi::Details::PropertyBase*>*
JobOptionsSvc::getProperties( const std::string& client) const
{
  return m_jobosvc->getProperties(client);
}

const Gaudi::Details::PropertyBase*
JobOptionsSvc::getClientProperty( const std::string& client,
                                  const std::string& name ) const 
{
  return m_jobosvc->getClientProperty(client, name);
}

/// Get the list of clients
std::vector<std::string> 
JobOptionsSvc::getClients() const
{
  return m_jobosvc->getClients();
}
  
/** look for file 'File' into search path 'Path' 
 *  and read it to update existing JobOptionsCatalogue 
 *  @param File file   name 
 *  @param Path search path 
 *  @return status code 
 */
StatusCode
JobOptionsSvc::readOptions( const std::string& file      , 
                            const std::string& path )
{
  return m_jobosvc->readOptions(file, path);
}


std::string
pythonize_value(const Gaudi::Details::PropertyBase* p)
{
  std::string pvalue = p->toString();
  const std::type_info& ti = *p->type_info();
  if ( ti == typeid(bool) ) {
    pvalue = ( pvalue == "0" || pvalue == "False" || pvalue == "false" )
           ? "False" 
           : "True";
  }
  else if ( ti == typeid(char)  || ti == typeid(signed char)    
            || ti == typeid(unsigned char)  ||
            ti == typeid(short) || ti == typeid(unsigned short) ||
            ti == typeid(int)   || ti == typeid(unsigned int)   ||
            ti == typeid(long)  || ti == typeid(unsigned long) ) {
    /*pvalue = pvalue;*/
  }
  else if ( ti == typeid(long long) || ti == typeid(unsigned long long) ) {
    pvalue = pvalue + "L";
  }
  else if ( ti == typeid(float) || ti == typeid(double) ) {
    // forces python to handle this as a float: put a dot in there...
    // pvalue = boost::to_lower_copy(cvalue);
    // if ( pvalue == "nan" ) {
    //   pvalue = "float('nan')";
    //   std::cout << "WARNING: default value for ["
    //             << p->name() << "] is NaN !!"
    //             << std::endl;
    // } else if ( std::string::npos == pvalue.find(".") &&
    //             std::string::npos == pvalue.find("e")  ) {
    //   pvalue = cvalue + ".0";
    // }
    // ptype  = "float";
  }
  else if ( ti == typeid(std::string) ) {
    pvalue = "'"+pvalue+"'";
  }
  else if ( ti == typeid(GaudiHandleBase) ) {
    pvalue = "'"+pvalue+"'";
    //
  }
  /*
  else if ( ti == typeid(GaudiHandleArrayBase) ) {
    m_importGaudiHandles = true;
    const GaudiHandleArrayProperty& hdl 
      = dynamic_cast<const GaudiHandleArrayProperty&>(*p);
    const GaudiHandleArrayBase&     base = hdl.value();
    
    pvalue = base.pythonRepr();
    ptype  = "GaudiHandleArray";
  }
  */
  else {
    /*pvalue = cvalue;*/
  }
  return pvalue;
}

} //> end namespace Athena

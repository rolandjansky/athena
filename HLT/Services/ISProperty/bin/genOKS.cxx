/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Include files----------------------------------------------------------------
#include "boost/program_options.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/exception.hpp"
#include "boost/filesystem/convenience.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/case_conv.hpp"
#include "boost/format.hpp"

#include "GaudiKernel/System.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IAuditor.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/HashMap.h"
#include "GaudiKernel/GaudiHandle.h"

#include "oks/class.h"
#include "oks/kernel.h"

#include "ISProperty/ISPropertyMgr.h"
#include "ISProperty/ISProperty.h"

#include "Reflex/PluginService.h"
#include "Reflex/Reflex.h"
#include "Reflex/SharedLibrary.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <ctime>
#include <set>
#include <vector>


namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace std;
using namespace ROOT::Reflex;

// useful typedefs
typedef std::vector<std::string> Strings_t;
typedef std::vector<fs::path>    LibPathNames_t;

class OKSGenerator 
{
  private:
    /// name of the package we are processing
    string  m_pkgName;

    /// absolute path to the directory where genOKS will store generated files
    string m_outputDirName;

    /// counting generated components
    int m_NumOfComponents;

  public:
    OKSGenerator( const string& pkgName,
                  const string& outputDirName ) : 
      m_pkgName           ( pkgName ),
      m_outputDirName     ( outputDirName ),
      m_NumOfComponents        ( 0 )
    {}

    /// main entry point of this class:
    ///  - iterate over all the modules (ie: library names)
    ///  - for each module extract component informations
    ///  - eventually generate the OKS Schema
    int genSchema( const Strings_t& modules );

  private:
    /// generate oks schema for a component
    void genComponent( OksKernel& kernel,
                       const std::string& componentName, 
                       const std::string& componentType,
                       const std::vector<GaudiPropertyHandle>& props );

    const char* getType( const Property* p);
};

int createAppMgr();

//--- Command main program-----------------------------------------------------
int main ( int argc, char** argv )
//-----------------------------------------------------------------------------
{
  fs::path pwd = fs::initial_path(); //current path
  fs::path out;  //output path
  Strings_t libs; //List of libraries
  std::string pkgName;  //Package name

  // declare a group of options that will be allowed only on command line
  po::options_description cmdline_opts("Generic options");
  cmdline_opts.add_options() 
    ("help,h",            
     "produce this help message")
    ("package-name,p",
     po::value<string>(),
     "name of the package for which we create the OKS schema file")
    ("input-libraries,i",
     po::value<string>(),
     "libraries to extract the components from")
    ("output-dir,o",
     po::value<string>()->default_value("../genOKS"),
     "output directory for generated OKS schema files.")
    ("debug-level,d",
     po::value<int>()->default_value(0),
     "debug level")
//    ("load-library,l",
//     po::value< Strings_t >()->composing(),
//     "preloading library")
    ;
  
  //--- Parse command line options ---------------------------------------------
  
  po::variables_map opt_map;
  
  try {
    po::store( po::command_line_parser(argc, argv).
               options(cmdline_opts).run(), 
               opt_map );

    po::notify(opt_map);
  }
  catch ( po::error& err ) {
    cout << "ERR0R: error detected while parsing command options: "<< err.what() << endl;
    return EXIT_FAILURE;
  }

  //--- Process command options -----------------------------------------------
  if( opt_map.count("help")) { 
    cout << cmdline_opts << endl; 
    return EXIT_FAILURE; 
  }

  if( opt_map.count("package-name") ) {
    pkgName = opt_map["package-name"].as<string>();
  }
  else {
    cout << "ERROR: 'package-name' required" << endl;
    cout << cmdline_opts << endl;
    return EXIT_FAILURE;
  }

  if( opt_map.count("input-libraries") ) {
    // re-shape the input arguments: 
    //  - removes spurious spaces, 
    //  - split into tokens.
    Strings_t inputLibs;
    {
      string tmp = opt_map["input-libraries"].as<string>();
      boost::trim(tmp);
      boost::split( inputLibs, tmp, 
                    boost::is_any_of(std::string(" ")), 
                    boost::token_compress_on );
    }

    //
    libs.reserve( inputLibs.size() );
    for ( Strings_t::const_iterator iLib = inputLibs.begin();
          iLib != inputLibs.end();
          ++iLib ) {
      fs::path libPath = fs::path( *iLib );
      std::string lib = libPath.leaf().string().substr(0, libPath.leaf().string().find('.') );
      if ( 0 == lib.find("lib") ) {
        lib = lib.substr(3); // For *NIX remove "lib"
      }
      // remove duplicates
      if ( !lib.empty() &&
          std::find( libs.begin(), libs.end(), lib ) == libs.end() ) {
        libs.push_back( lib );
      }
    } //> end loop over input-libraries
    if ( libs.empty() ) {
      cout << "ERROR: input component library(ies) required !\n" 
           << "ERROR: 'input-libraries' argument was [" 
           << opt_map["input-libraries"].as<string>()
           << "]"
           << endl;
      return EXIT_FAILURE;
    }
  }
  else {
    cout << "ERROR: input component library(ies) required" << endl;
    cout << cmdline_opts << endl;
    return EXIT_FAILURE;
  }
  
  if( opt_map.count("output-dir") ) {
    out = fs::absolute( fs::path( opt_map["output-dir"].as<string>()) );
  }
  
  if ( opt_map.count("debug-level") ) {
    PluginService::SetDebug( opt_map["debug-level"].as<int>() );
  }
  
//  if ( opt_map.count("load-library") ) {
//    Strings_t lLib_list = opt_map["load-library"].as< Strings_t >();
//    for (Strings_t::const_iterator lLib=lLib_list.begin(); 
//         lLib != lLib_list.end(); 
//         ++lLib) {
//      // load done through ROOT::Reflex helper class
//      SharedLibrary tmplib(*lLib) ;
//      tmplib.Load() ;
//    }
//  }

  // --- Create output path -------------------------------
  if ( !fs::exists( out ) ) {
    try {
      fs::create_directory(out);
    }
    catch ( fs::filesystem_error err ) {
      cout << "ERR0R: error creating directory: "<< err.what() << endl;
      return EXIT_FAILURE;
    }
  }
  
  cout << ":::::: libraries : [ ";
  copy( libs.begin(), libs.end(), ostream_iterator<string>(cout, " ") );
  cout << "] ::::::" << endl;

  // --- Create schema ------------------------------------------------
  OKSGenerator oks( pkgName, out.string() );
//  py.setConfigurableModule     (opt_map["configurable-module"].as<string>());
//  py.setConfigurableDefaultName(opt_map["configurable-default-name"].as<string>());
//  py.setConfigurableAlgorithm  (opt_map["configurable-algorithm"].as<string>());
//  py.setConfigurableAlgTool    (opt_map["configurable-algtool"].as<string>());
//  py.setConfigurableAuditor    (opt_map["configurable-auditor"].as<string>());
//  py.setConfigurableService    (opt_map["configurable-service"].as<string>());

  int sc = EXIT_FAILURE;
  try {
    sc = oks.genSchema( libs );
  }
  catch ( exception& e ) {
    cout << "ERROR: Could not generate OKS Schema!\n" 
         << "ERROR: Got exception: " << e.what() << endl;
    return EXIT_FAILURE;
  }

  cout << ":::::: libraries : [ ";
  copy( libs.begin(), libs.end(), ostream_iterator<string>(cout, " ") );
  cout << "] :::::: [DONE]" << endl;

  return sc;
}


//-----------------------------------------------------------------------------
int OKSGenerator::genSchema( const Strings_t& libs )
//-----------------------------------------------------------------------------
{
  //--- Disable checking StatusCode -------------------------------------------
  StatusCode::disableChecking();
  
  const Strings_t::const_iterator endLib = libs.end();
  
  //--- Instantiate ApplicationMgr --------------------------------------------
  if ( createAppMgr() ) {
    cout << "ERROR: ApplicationMgr can not be created. Check environment" << endl;
    return EXIT_FAILURE;
  }
  
  //--- Iterate over component factories --------------------------------------
  Scope factories = Scope::ByName(PLUGINSVC_FACTORY_NS);
  if ( !factories ) {
    cout << "ERROR: No PluginSvc factory namespace could be found" << endl;
    return EXIT_FAILURE;
  }
  
  //Create a service locator
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  //Create a dummy service as AlgTool parent
  IService*    dummySvc = new Service( "DummySvc", svcLoc );
  dummySvc->addRef();

  bool allGood = true;

  // iterate over all the requested libraries
  for ( Strings_t::const_iterator iLib=libs.begin(); iLib != endLib; ++iLib ) {
    
    std::cout << ":::: processing library: " << *iLib << "..." << std::endl;
    
    // Scan the pluginSvc namespace and store the "background" of already
    // alived components, so we can extract our signal later on
    set<string> bkgNames;
    for ( Member_Iterator it = factories.FunctionMember_Begin(); 
          it != factories.FunctionMember_End(); ++it ) {
      string ident = it->Properties().PropertyAsString("name");
      if ( PluginService::Debug() > 2 ) {
        cout << "::: " << ident << endl;
      }
      bkgNames.insert( ident );
    }
    const set<string>::const_iterator bkgNamesEnd = bkgNames.end();
    
    //--- Load component library ----------------------------------------------
    System::ImageHandle handle;
    unsigned long err = System::loadDynamicLib( *iLib, &handle );
    if ( err != 1 ) {
      cout << "ERROR: " << System::getLastErrorString() << endl;
      allGood = false;
      continue;
    }

    //--- Generate Oks schema file
    /// generate an Oks Kernel
    OksKernel k;
    /// set to silence mode
    k.set_silence_mode(PluginService::Debug() < 1);
    /// load the base info class
    k.load_file("is/is.xml");
    /// generate a new schema file
    OksFile *f = k.new_schema(m_outputDirName+"/"+(*iLib)+".schema.xml");
    
    //--- Loop over factory members -----------------------------------------
    for ( Member_Iterator it = factories.FunctionMember_Begin(); 
          it != factories.FunctionMember_End(); 
          ++it ) {
      const string ident = it->Properties().PropertyAsString("name");
      if ( bkgNames.find(ident) != bkgNamesEnd ) {
        if ( PluginService::Debug() > 2 ) {
          cout << "\t==> skipping [" << ident << "]..." << endl;
        }
        continue;
      }
      
      //Check type of member
      const string rtype = it->TypeOf().ReturnType().Name();
      string type;
      bool known = true;
      if      ( ident == "ApplicationMgr" ) type = "ApplicationMgr";
      else if ( rtype == "IInterface*" )    type = "IInterface";
      else if ( rtype == "IAlgorithm*" )    type = "algorithm";
      else if ( rtype == "IService*" )      type = "service";
      else if ( rtype == "IAlgTool*" )      type = "algtool";
      else if ( rtype == "IAuditor*" )      type = "auditor";
      else if ( rtype == "IConverter*" )    type = "Converter";
      else if ( rtype == "DataObject*" )    type = "DataObject";
      else                                  type = "Unknown", known = false;
      string name = ident;
      // handle possible problems with templated components
      boost::trim(name);


      if ( type == "IInterface" ) {
        /// not enough information... - skip it
        continue;
      }
      
      if ( type == "Converter" || type == "DataObject" ) {
        /// no Properties, so don't bother create Configurables...
        continue;
      }
      
      if ( type == "ApplicationMgr" ) {
        /// no IS properties for this component. yet...
        continue;
      }


      if ( !known ) {
        cout << "WARNING: Unknown (return) type [" << rtype << "] !!\n"
             << "WARNING: component [" << ident << "] is skipped !"
             << endl;
        allGood = false;
        continue ;
      }
      
      // --- Create an instance of that object --------------------------------
      string cname = "DefaultName";
      Object dummy;
      vector<void*>  args; 
      args.reserve( 3 );
      if ( type == "algtool" ) {
        args.resize( 3 );
        args[0] = &cname;
        args[1] = &type;
        args[2] = dummySvc;
      }
      else {
        args.resize( 2 );
        args[0] = &cname;
        args[1] = svcLoc;
      }

      //Create instance and dynamic cast to ISPropertyMgr
      ISPropertyMgr* ISPropMgr = 0;
      try {
        if ( type == "algorithm" ) {
          IAlgorithm* obj; 
          it->Invoke(dummy,obj,args);
          ISPropMgr = dynamic_cast<ISPropertyMgr*>(obj);
        }
        else if ( type == "service") {
          IService* obj;
          it->Invoke(dummy,obj,args);
          ISPropMgr = dynamic_cast<ISPropertyMgr*>(obj);
        }
        else if ( type == "algtool") {
          IAlgTool* obj;
          it->Invoke(dummy,obj,args);
          ISPropMgr = dynamic_cast<ISPropertyMgr*>(obj);
        }
        else if ( type == "auditor") {
          IAuditor* obj;
          it->Invoke(dummy,obj,args);
          ISPropMgr = dynamic_cast<ISPropertyMgr*>(obj);
        }
        else {
          // Should never reach here
          cout << "ERROR: Unhandled (return) type [" << rtype << "] was identified as [" 
               << type <<"] !!" << endl;
        }
      }
      catch ( exception& e ) {
        cout << "ERROR: Error instantiating " << name 
             << " from " << *iLib << endl;
        cout << "ERROR: Got exception: " << e.what() << endl;
        allGood = false;
        continue ;
      }
      catch ( ... ) {
        cout << "ERROR: Error instantiating " << name 
             << " from " << *iLib << endl;
        allGood = false;
        continue ;
      }
      if( ! ISPropMgr ) {
        if (PluginService::Debug() > 1){
          cout << "INFO: could not cast [" << type << "] object to an ISPropertyMgr* !\n"
               << "INFO: No OKS schema will be generated for [" << name << "] !"
               << endl;
        }
        continue ;
      }

      cout << " - component: " << name << endl;
      // Generate schema for this component
      genComponent( k, name, type, ISPropMgr->getISProperties() );
      //Count components
      m_NumOfComponents++;
      //Release the instance
      ISPropMgr->release();
 
    } //> end loop over factories

    // ---- Saving data ------------
    if ( m_NumOfComponents > 0 ) {
      //Add a comment
      f->add_comment("Automatically generated by genOKS - please do not edit","genOKS.exe");
      //save the schema
      k.save_schema(f);
      //reset counter
      m_NumOfComponents = 0;
    }
      
  } //> end loop over libraries
  
  dummySvc->release();
  dummySvc = 0;
  
  return allGood ? EXIT_SUCCESS : EXIT_FAILURE;

}

/**
 * Get the type for a given property
 **/
const char* OKSGenerator::getType(const Property* p){

  //Check each of the basic types we support
  if (*(p->type_info())==typeid(bool)) return OksAttribute::bool_type;
  if (*(p->type_info())==typeid(char)) return OksAttribute::s8_int_type;
  if (*(p->type_info())==typeid(unsigned char)) return OksAttribute::u8_int_type;
  if (*(p->type_info())==typeid(short)) return OksAttribute::s16_int_type;
  if (*(p->type_info())==typeid(unsigned short)) return OksAttribute::u16_int_type;
  if (*(p->type_info())==typeid(int)) return OksAttribute::s32_int_type;
  if (*(p->type_info())==typeid(long)) return OksAttribute::s32_int_type;
  if (*(p->type_info())==typeid(unsigned int)) return OksAttribute::u32_int_type;
  if (*(p->type_info())==typeid(unsigned long)) return OksAttribute::u32_int_type;
  if (*(p->type_info())==typeid(int64_t)) return OksAttribute::s64_int_type;
  if (*(p->type_info())==typeid(uint64_t)) return OksAttribute::u64_int_type;
  if (*(p->type_info())==typeid(float)) return OksAttribute::float_type;
  if (*(p->type_info())==typeid(double)) return OksAttribute::double_type;
  //If none of them matches, use string
  return OksAttribute::string_type;
}


//-----------------------------------------------------------------------------
void OKSGenerator::genComponent( OksKernel& kernel,
                                    const std::string& componentName, 
                                    const std::string& componentType, 
                                    const std::vector<GaudiPropertyHandle>& properties )
//-----------------------------------------------------------------------------
{

  
  /// Create an non-abstract OKS Class w/o kernel
  OksClass* c = new OksClass( componentName, componentName+" "+componentType, false,  &kernel );
  /// Make this a subclass of Info
  c->add_super_class("Info");

  /// Now loop over properties and add each as an attribute
  std::vector<GaudiPropertyHandle>::const_iterator propItr = properties.begin();
  for (; propItr != properties.end();++propItr){
    //get the property object
    const Property* p = (*propItr).getProperty();
    if (p == NULL) {
      std::cout << "Unable to obtain property " << (*propItr).getName() << " - will not be put in the schema" << endl;
      continue ;
    }
    //generate a non-multi-value, no-range string attribute
    OksAttribute * a = new OksAttribute( p->name(), getType(p),
      false, "", p->toString(), p->documentation(), true
    );
    //Fix writing out bool values as 0s and 1s (p->toString() will deliever "True" and "False"
    if (a->get_type() == OksAttribute::bool_type ) a->set_init_value((p->toString() == "True") ? "1" : "0"); 
    //add it to our class
    c->add(a);
 }

 //be verbose
 if ( PluginService::Debug() > 1 )
   //print class to std::cout
   std::cout << *c;
}

//-----------------------------------------------------------------------------
int createAppMgr()
//-----------------------------------------------------------------------------
{
  IInterface* iface = Gaudi::createApplicationMgr();
#ifdef ATLAS_GAUDI_V21
  SmartIF<IProperty> propMgr ( iface );
  SmartIF<IAppMgrUI> appUI  ( iface );
#else
  SmartIF<IProperty> propMgr ( IID_IProperty, iface );
  SmartIF<IAppMgrUI> appUI  ( IID_IAppMgrUI, iface );
#endif

  if ( propMgr.isValid() && appUI.isValid() ) {
    propMgr->setProperty( "JobOptionsType", "NONE" );  // No joboptions
    propMgr->setProperty( "AppName", "");              // No initial printout message
    propMgr->setProperty( "OutputLevel", "7");         // No other printout messages
    appUI->configure();
    return EXIT_SUCCESS;
  }
  else {
    return EXIT_FAILURE;
  }
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <exception>
#include <vector>
#include <string>
#include <fstream>

#include "GaudiKernel/Property.h"
#include "PathResolver/PathResolver.h"

#include "boost/lexical_cast.hpp"

// Local includes:
#include "TrigConfigSvc/HLTJobOptionsSvc.h"
#include "TrigConfBase/TrigDBConnectionConfig.h"
#include "TrigConfL1Data/HelperFunctions.h"
#include "TrigConfJobOptData/JobOptionTable.h"
#include "TrigConfStorage/IJobOptionTableLoader.h"
#include "TrigConfStorage/SessionMgr.h"
#include "TrigConfStorage/StorageMgr.h"

using namespace std;
using namespace boost;

TrigConf::HLTJobOptionsSvc::HLTJobOptionsSvc( const std::string& name, ISvcLocator* pSvcLocator ) :
   base_class( name, pSvcLocator ),
   m_sourcePath(""),
   m_sourceType("DB") // DB, FILE
{
   declareProperty( "TYPE", m_sourceType );
   declareProperty( "PATH", m_sourcePath );
   m_outputLevel = MSG::INFO;
}


TrigConf::HLTJobOptionsSvc::~HLTJobOptionsSvc() 
{}


//----------------------------------------------------------------------------
StatusCode
TrigConf::HLTJobOptionsSvc::readEnv()
{
  // reads the setup from a file
  // depreciated
  return StatusCode::SUCCESS;
}

StatusCode
TrigConf::HLTJobOptionsSvc::sysInitialize() {

   //We specifically do not call the base class sysInitialize here (ATR-15094)
   //StatusCode sc = base_class::sysInitialize(); 
   
   ATH_CHECK(initialize()); 
   ATH_CHECK(setMyProperties(name(), this));

   return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------
StatusCode
TrigConf::HLTJobOptionsSvc::initialize() {

   // CHECK(ConfigSvcBase::initialize());

   ATH_MSG_INFO("Initializing the HLTJobOptionsSvc");

   decltype(m_dbconfig->m_smkey) oldsmk{0};
   if(m_dbconfig) {
      ATH_MSG_INFO("TrigDBConnectionConfig before initialization: " << m_dbconfig->toString());
      oldsmk = m_dbconfig->m_smkey;
   }

   if ( m_sourceType == "FILE" ) {
      // Read job options from a text file
      if ( readOptions( m_sourcePath, m_dirSearchPath ).isFailure() ) {
         ATH_MSG_ERROR("error reading options");
         return StatusCode::FAILURE;
      }
   } else if( m_sourceType == "DB" ) {

      // this fills in all members of TrigDBConnectionConfig that are specified
      // in the path string
      m_dbconfig = std::unique_ptr<TrigDBConnectionConfig>{
         new TrigDBConnectionConfig(m_sourcePath)};

      ATH_MSG_INFO("TrigDBConnectionConfig for initialization: " << m_dbconfig->toString());

      if (m_dbconfig->m_type == TrigDBConnectionConfig::DBLookup)
         m_dbconfig->m_user = m_dbconfig->m_password = "";
      
      if(!oldsmk || oldsmk != m_dbconfig->m_smkey) {
         if ( readDbWithMasterKey().isFailure() ) {
            ATH_MSG_FATAL("Could not read DB with keys: " << m_dbconfig->m_smkey << "-" << m_dbconfig->m_hltkey << " - " << m_dbconfig->m_lvl1key);
            return StatusCode::FAILURE;
         }
      } else {
         ATH_MSG_INFO("HLTJobOptionsSvc holds already the configuration of SMK: " << m_dbconfig->m_smkey);
      }

   }

   ATH_MSG_INFO("TrigDBConnectionConfig after initialization: " << m_dbconfig->toString());

   return StatusCode::SUCCESS;
}



//----------------------------------------------------------------------------
StatusCode
TrigConf::HLTJobOptionsSvc::readDbWithMasterKey()
//----------------------------------------------------------------------------
{

   if(m_sourceType=="FILE") {
      ATH_MSG_ERROR("HLTJobOptionsSvc is configured to run from a file, cannot connect to database!");
      return StatusCode::FAILURE;
   }

   CHECK(initStorageMgr());

   SessionMgr & sessionMgr = dynamic_cast<TrigConf::StorageMgr*>(m_storageMgr)->sessionMgr();

   sessionMgr.setRetrialPeriod(m_dbconfig->m_retrialPeriod);
   sessionMgr.setRetrialTimeout( m_dbconfig->m_retrialPeriod * (m_dbconfig->m_maxRetrials + 1) );
   sessionMgr.setConnectionTimeout( 0 );
   sessionMgr.createSession();
   
   TrigConf::JobOptionTable jot;
   jot.setSMK(m_dbconfig->m_smkey);
   if(!m_storageMgr->jobOptionTableLoader().load( jot ))
     return StatusCode::FAILURE;

   CHECK(freeStorageMgr());

   //jot.print();

   if( FillCatalogue(jot).isSuccess() ) {

      //PrintCatalogue(msg().stream());

      // Now we have to resolve still all the entries which are
      // references to other Properties
      if ( m_catalogue.resolveOptions(msg()).isSuccess() ) {
         ATH_MSG_INFO("Job options successfully read in from " << m_dbconfig->toString());
      } else {
         ATH_MSG_FATAL("Job options references cannot be resolved.");
         m_catalogue.printUnresolvedOptions(msg());
         return StatusCode::FAILURE;
      }
   } else {
      ATH_MSG_FATAL("Failed to read in job options");
      return StatusCode::FAILURE;
   }

   return StatusCode::SUCCESS;
}



//----------------------------------------------------------------------------
void
TrigConf::HLTJobOptionsSvc::PrintCatalogue(ostream & os) const {

   os << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
   os << "Job options catalogue" << endl;

   uint nc(0), np(0);

   for(const string& algName: m_catalogue.getClients() ) {
      nc++;
      os << "Client: " << algName << endl;

      vector<const Property*>* pl;

      m_catalogue.optionsOf( algName, pl);

      for(const Property* prop: *pl) {
         np++;
         os << "      property ";
         prop->fillStream(os) << endl; // for Gaudi_V19
         //(*plIt)->nameAndValueAsStream(os) << std::endl;
      }
   }
   ATH_MSG_INFO("Job options catalogue contains " << nc << " clients with a total of " << np << " properties." << endl);
}



//----------------------------------------------------------------------------
StatusCode
TrigConf::HLTJobOptionsSvc::FillCatalogue(const JobOptionTable & jot) {

   for(const JobOption& jo : jot.jobOptionVector() ) {

      std::string client   = jo.component_alias();
      std::string propName = jo.name();
      std::string propVal  = jo.value();
      std::vector<std::string> valArr;

      bool isArrayProp = false;
      if(propVal[0]=='[') { // indicates an array, start parsing
         isArrayProp = true;
         std::string::size_type pos = 1;
         bool isStringtypeA = propVal.find('\"',pos) != std::string::npos;
         bool isStringtypeB = propVal.find('\'',pos) != std::string::npos;
         if(isStringtypeA||isStringtypeB) {
            if(isStringtypeA){
               while( (pos = propVal.find('\"',pos))!=std::string::npos) {
                  int posEnd = propVal.find('\"',pos+1);
                  std::string arrS = propVal.substr(pos,posEnd-pos+1);
                  pos=posEnd+1;
                  valArr.push_back(arrS);
               }
            }
            if(isStringtypeB){
               while( (pos = propVal.find('\'',pos))!=std::string::npos) {
                  int posEnd = propVal.find('\'',pos+1);
                  std::string arrS = propVal.substr(pos,posEnd-pos+1);
                  pos=posEnd+1;
                  valArr.push_back(arrS);
               }
            }
         } else {
            TrigConf::removeAllSpaces(propVal);
            std::string propValNoBrackets = propVal.substr(1,propVal.rfind("]")-1);
            valArr = split(propValNoBrackets, ",");
         }
      }
      if( isArrayProp ) {
         m_catalogue.addOption(client, new StringArrayProperty(propName, valArr));
      } else  {
         m_catalogue.addOption(client, new StringProperty(propName, propVal));
      }
   }
   return StatusCode::SUCCESS;
}



//----------------------------------------------------------------------------
StatusCode
TrigConf::HLTJobOptionsSvc::finalize() {
  ATH_MSG_DEBUG("Finalizing");
  CHECK(Service::finalize());
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------
StatusCode
TrigConf::HLTJobOptionsSvc::queryInterface( const InterfaceID& riid, void** ppvIF )
//----------------------------------------------------------------------------
{
   if ( IJobOptionsSvc::interfaceID() == riid )  {
      *ppvIF = (IJobOptionsSvc*) this;
   } else if ( IProperty::interfaceID() == riid ) {
      *ppvIF = (IProperty*) this;
   } else  {
      return Service::queryInterface(riid, ppvIF);
   }
   addRef();
   return StatusCode::SUCCESS;
}






//----------------------------------------------------------------------------
StatusCode
TrigConf::HLTJobOptionsSvc::setMyProperties( const std::string& client, IProperty* myInt ) {

   ATH_MSG_DEBUG("Client " << client << " has requested its options from HLTJobOptionsSvc");

   StatusCode sc;

   // now the list of job options for the client is sorted as it appears in the database
   // but we need it in the order as it is defined in the client (myInt->getProperties())
   // that is of particular importance for the ApplicationMgr, where the dll-jobOption needs to be set
   // before the services

   // define a sorted list of properties, which will be used from now on
   std::vector<const Property*> propertiesFromDBSorted;
   {

      // Get the list of available (in the DB) options for this client
      std::vector<const Property*>* propertiesFromDB;
      sc = m_catalogue.optionsOf( client, propertiesFromDB );
      if( !sc.isSuccess() ) return StatusCode::SUCCESS;  // Algorithm has no options

      // now bring those properties in the order that is specified by the client
      for( const Property * clientProp : myInt->getProperties() )   {

         for( const Property * dbProp : *propertiesFromDB ) {
            if( dbProp->name() == clientProp->name() ) {
               propertiesFromDBSorted.push_back( dbProp );
               break;
            }
         }

      }

      // check if the DB and the client agree in size
      if( propertiesFromDBSorted.size() != propertiesFromDB->size() && 
          client != "THistSvc") {
         //find the properties that differ between the two lists
         std::vector<const Property*> propertiesFromDBDiffer;
         for( const Property * dbProp : *propertiesFromDB ) {
            int gotAmatch(0);
            for(const Property* differ: propertiesFromDBSorted ) {

               if( dbProp->name() == differ->name() )
                  gotAmatch++;

            }
            if(gotAmatch!=1){
               propertiesFromDBDiffer.push_back(dbProp);
            }
         }
         ATH_MSG_ERROR( client << " asks for " << propertiesFromDBSorted.size() << " properties while the database knows about "
                          << propertiesFromDB->size() << ", properties that differ are: ");

         for(const Property*  differ : propertiesFromDBDiffer )
            ATH_MSG_ERROR( "   " << differ->name() );

      }

   }

   bool fail = false;

   // this is for the case of old configurations
   if(client=="HLTConfigSvc") {
      sc = myInt->setProperty( "DBHLTPSKeySet", m_dbconfig->hltKeysToString() );
      if( !sc.isSuccess() ) {
         ATH_MSG_ERROR("Unable to set StringProperty DBHLTPSKeySet to " << m_dbconfig->m_hltkeys);
         fail = true;
      } else {
         ATH_MSG_DEBUG("Set property DBHLTPSKeySet to " << m_dbconfig->m_hltkeys);
      }
   }

   // Iterate over the sorted list to set the options
   for( const Property * dbprop : propertiesFromDBSorted ) {

      // some exceptions
      // Go and Exit are used by LHCb
      // They are bound to an updateHandler which starts the
      // ApplicationMgr as soon as Go is set, and exits as soon as Exit
      // is modified -- we don't want that
      if(client=="ApplicationMgr" && dbprop->name()=="Go") continue;
      if(client=="ApplicationMgr" && dbprop->name()=="Exit") continue;

      if (dbprop->name()=="DBHLTPSKeySet")
        continue; /* special case of property that is wrongly interpreted an
                     empty array property (ATR-11877) */

      sc = StatusCode::FAILURE;

      const StringProperty* sp = dynamic_cast<const StringProperty*>(dbprop);
      if( 0 != sp ) {

         std::string spval = sp->value();

         ATH_MSG_DEBUG("Attempt to set property " << dbprop->name() << " to " << spval);
      
         // now we have to intervene to get the HLTMenu and the Level 1 menu
         // and thresholds from the database and not from the xml files
         if(client=="HLTConfigSvc") {
            if (dbprop->name()=="ConfigSource")  spval = m_dbconfig->typeToString();
            if (dbprop->name()=="DBServer")      spval = m_dbconfig->m_server;
            if (dbprop->name()=="DBUser")        spval = m_dbconfig->m_user;
            if (dbprop->name()=="DBPassword")    spval = m_dbconfig->m_password;
            if (dbprop->name()=="DBTable")       spval = m_dbconfig->m_schema;
            if (dbprop->name()=="DBSMKey")       spval = lexical_cast<string,int>(m_dbconfig->m_smkey);
            if (dbprop->name()=="DBHLTPSKey")    spval = lexical_cast<string,int>(m_dbconfig->m_hltkey);

         }

         if(m_dbconfig->m_lvl1key && client=="LVL1ConfigSvc") {
            if (dbprop->name()=="ConfigSource") spval = m_dbconfig->typeToString();
            if (dbprop->name()=="DBServer")     spval = m_dbconfig->m_server;
            if (dbprop->name()=="DBAccount")    spval = m_dbconfig->m_schema;
            if (dbprop->name()=="DBUser")       spval = m_dbconfig->m_user;
            if (dbprop->name()=="DBPass")       spval = m_dbconfig->m_password;
            if (dbprop->name()=="DBSMKey")      spval = lexical_cast<string,int>(m_dbconfig->m_smkey);
            if (dbprop->name()=="DBLVL1PSKey")  spval = lexical_cast<string,int>(m_dbconfig->m_lvl1key);
         }

         if(m_dbconfig->m_lvl1key && client=="L1TopoConfigSvc") {
            if (dbprop->name()=="ConfigSource") spval = m_dbconfig->typeToString();
            if (dbprop->name()=="DBServer")     spval = m_dbconfig->m_server;
            if (dbprop->name()=="DBAccount")    spval = m_dbconfig->m_schema;
            if (dbprop->name()=="DBUser")       spval = m_dbconfig->m_user;
            if (dbprop->name()=="DBPass")       spval = m_dbconfig->m_password;
            if (dbprop->name()=="DBSMKey")      spval = lexical_cast<string,int>(m_dbconfig->m_smkey);
         }


         sc = myInt->setProperty( dbprop->name(), spval );
         if( !sc.isSuccess() ) {
            ATH_MSG_ERROR("Unable to set StringProperty " << dbprop->name() << " to " << spval);
            fail = true;
         } else {
            ATH_MSG_DEBUG("Set property " << dbprop->name() << " to " << spval);
         }
         continue;
      }


      const StringArrayProperty* sap = dynamic_cast<const StringArrayProperty*>(dbprop);
      if( 0 != sap ) {
         std::string prop = "[";
         const std::vector<std::string>& vals = sap->value();
         for ( unsigned int i = 0; i < vals.size(); i++ )   {
            prop += vals[i];
            if ( i+1 < vals.size() ) prop += ",";
         }
         prop += "]";

         ATH_MSG_DEBUG("Attempt to set property " << dbprop->name() << " to " << prop);

         sc = myInt->setProperty( dbprop->name(), prop );
         if( !sc.isSuccess() ) {
            ATH_MSG_ERROR("Unable to set StringArrayProperty " << dbprop->name() << " of " << client << " to " << prop);
            fail = true;
         } else {
            ATH_MSG_DEBUG("Set Property " << dbprop->name() << " to :" << prop << ":");
         }
         continue;
      }

      ATH_MSG_ERROR("Unable to set property " << dbprop->name() << " of " << client << "\n Neither String nor StringArray");
      fail = true;
   }

   ATH_MSG_DEBUG("Finished setting properties for " << client);

   return fail? StatusCode::FAILURE : StatusCode::SUCCESS;
}





//----------------------------------------------------------------------------
StatusCode
TrigConf::HLTJobOptionsSvc::addPropertyToCatalogue( const std::string & client,
                                                    const Property& property )
//----------------------------------------------------------------------------
{
  //cout << "Start HLTJobOptionsSvc::addPropertyToCatalogue("<< client << ", " << property->name() << ")" << endl;
  StatusCode sc = StatusCode(StatusCode::SUCCESS,true);
  // remove property from catalogue if already exists
  removePropertyFromCatalogue( client, property.name() );

  // add property

  Property* p = new StringProperty ( property.name(), "" ) ;
  if ( !property.load( *p ) ) { delete p ; return StatusCode::FAILURE ; }

  m_catalogue.addOption( client , p );

  return sc;
}

//----------------------------------------------------------------------------
StatusCode
TrigConf::HLTJobOptionsSvc::removePropertyFromCatalogue(const std::string& client,
                                                        const std::string& name )
//----------------------------------------------------------------------------
{
  StatusCode sc = StatusCode(StatusCode::SUCCESS,true);
  std::vector<const Property*>* opts = 0;
  if ( m_catalogue.optionsOf( client, opts).isSuccess() ) {
    std::vector<const Property*>::iterator it;
    for ( it = opts->begin(); it != opts->end(); it++ ) {
      if ((*it)->name() == name )  {
        // delete existing property
        delete *it;
        opts->erase(it);
        return sc;
      }
    }
  }
  return sc;
}

//----------------------------------------------------------------------------
const std::vector<const Property*>*
TrigConf::HLTJobOptionsSvc::getProperties( const std::string& client) const
//----------------------------------------------------------------------------
{
  std::vector<const Property*>* opts = 0;
  if ( m_catalogue.optionsOf( client, opts).isSuccess() ) {
    return opts;
  }
  return 0;
}
//----------------------------------------------------------------------------
std::vector<std::string>
TrigConf::HLTJobOptionsSvc::getClients() const
//----------------------------------------------------------------------------
{
  return m_catalogue.getClients();
}


//----------------------------------------------------------------------------
StatusCode TrigConf::HLTJobOptionsSvc::readOptions( const std::string& /*file*/ ,
                                                    const std::string& /*path*/ )
//----------------------------------------------------------------------------
{
  return StatusCode::SUCCESS;
}

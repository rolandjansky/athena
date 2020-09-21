/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// class declaration
#include "./HLTConfigSvc.h"

#include <exception>
#include <vector>

// Athena/Gaudi includes:
#include "PathResolver/PathResolver.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Timing.h"

// Root incluces:
#include "TH1F.h"
#include "TH2I.h"

// Local includes:
#include "TrigConfIO/JsonFileLoader.h"
#include "TrigConfIO/TrigDBMenuLoader.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigConfBase/TrigDBConnectionConfig.h"
#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfStorage/IStorageMgr.h"
#include "TrigConfStorage/XMLStorageMgr.h"
#include "TrigConfStorage/IHLTPrescaleSetLoader.h"
#include "TrigConfStorage/IHLTPrescaleSetCollectionLoader.h"
#include "TrigConfL1Data/HelperFunctions.h"
#include "TrigConfL1Data/CTPConfig.h" 
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTSequenceList.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"
#include "TrigConfHLTData/HLTPrescaleSetCollection.h"
#include "AthenaMonitoringKernel/OHLockedHist.h"

#include "TrigConfData/HLTMenu.h"

#include "TrigConfInterfaces/IJobOptionsSvc.h"

#include "TrigConfInterfaces/IJobOptionsSvc.h"

#include "boost/algorithm/string/case_conv.hpp"
#include "boost/lexical_cast.hpp"

using namespace std;
using namespace TrigConf;

HLTConfigSvc::HLTConfigSvc( const string& name, ISvcLocator* pSvcLocator ) :
   base_class(name, pSvcLocator),
   m_eventStore( "StoreGateSvc/StoreGateSvc",  name ),
   m_histProp_timePrescaleUpdate(Gaudi::Histo1DDef("Time for prescale update",0,200,100))
{
   base_class::declareCommonProperties();

   declareProperty( "DBHLTPSKey",       m_dbHLTPSKey );
   declareProperty( "DBHLTPSKeySet",    m_dbHLTPSKeySet,
                    "List of HLT Prescale keys associated with start Lumiblocks LB1uPSK1cLB2uPSK2" );
   declareProperty( "HistTimePrescaleUpdate", m_histProp_timePrescaleUpdate,
                    "Histogram of time for prescale update");
   declareProperty( "doMergedHLT",      m_setMergedHLT,
                    "Set true to run the merged HLT processing");
   declareProperty( "doMonitoring",     m_doMon,
                    "Enable monitoring (mostly for online)");
}

HLTConfigSvc::~HLTConfigSvc()
{}


StatusCode
HLTConfigSvc::writeConfigToDetectorStore() {

    TrigConf::HLTMenu * hltmenu = new TrigConf::HLTMenu;

   if( m_inputType == "db" ) {

      // db menu loader
      TrigConf::TrigDBMenuLoader dbloader(m_dbConnection);
      dbloader.setLevel(TrigConf::MSGTC::WARNING);

      if( dbloader.loadHLTMenu( m_smk, *hltmenu ) ) {
         ATH_MSG_INFO( "Loaded HLT menu from DB " << m_dbConnection << " for SMK " << m_smk.value() );
      } else {
         ATH_MSG_WARNING( "Failed loading HLT menu from DB for SMK " << m_smk.value());
         return StatusCode::RECOVERABLE;
      }

   } else if ( m_inputType == "file" ) {

      // load the json file into TrigConf::HLTMenu
      TrigConf::JsonFileLoader fileLoader;
      fileLoader.setLevel(TrigConf::MSGTC::WARNING);

      if( fileLoader.loadFile( m_hltFileName, *hltmenu ) ) {
         ATH_MSG_INFO( "Loaded HLT menu file " << m_hltFileName.value() );
      } else {
         ATH_MSG_WARNING( "Failed loading HLT menu file " << m_hltFileName.value());
         return StatusCode::RECOVERABLE;
      }

   } else if( m_inputType == "cool" ) {
      ATH_MSG_FATAL( "Loading of HLT menu from COOL + DB not implemented");
      return StatusCode::FAILURE;
   }

   // To the reviewers: I will need to leave this commented, as one has to understand why this solution does not work
   // auto writeHandle = SG::makeHandle(m_hltMenuKey);
   // ATH_MSG_INFO("Recording HLT menu with " << m_hltMenuKey);
   // ATH_CHECK( writeHandle.record( std::unique_ptr<TrigConf::HLTMenu>(hltmenu) ));

   ServiceHandle<StoreGateSvc> detStore( "StoreGateSvc/DetectorStore", name() );
   ATH_CHECK( detStore.retrieve() );
   if( detStore->record(hltmenu,"HLTTriggerMenu").isSuccess() ) {
      ATH_MSG_INFO( "Recorded HLT menu with key 'HLTTriggerMenu' in the detector store" );
   }

   return StatusCode::SUCCESS;
} 


// Suppress warnings for two functions of this class marked as deprecated.
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
const HLTChainList*
HLTConfigSvc::chainList() const {
   return &m_HLTFrame.getHLTChainList();
}


const HLTSequenceList*
HLTConfigSvc::sequenceList() const {
   return &m_HLTFrame.getHLTSequenceList();
}


#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif


const HLTChainList&
HLTConfigSvc::chains() const {
   return m_HLTFrame.getHLTChainList();
}


const HLTSequenceList&
HLTConfigSvc::sequences() const {
   return m_HLTFrame.getHLTSequenceList();
}


StatusCode
HLTConfigSvc::initialize() {

   // ATH_CHECK( m_hltMenuKey.initialize() );

   {
      /// Handle to JobOptionsSvc used to retrieve the DataFlowConfig property
      if( auto joSvc = serviceLocator()->service<TrigConf::IJobOptionsSvc>( "JobOptionsSvc" ) ) {
         if( joSvc->superMasterKey()>0 ) {
            m_inputType = "db";
            m_smk = joSvc->superMasterKey();
            m_dbConnection = joSvc->server();
            m_configSourceString = "none";
         }
      } else {
         ATH_MSG_INFO("Did not locate TrigConf::JobOptionsSvc, not running athenaHLT");
      }

      StatusCode sc = writeConfigToDetectorStore();
      if( !sc.isSuccess() ) {
         ATH_MSG_INFO( "This previous WARNING message is being ignored in the current transition phase. Once we rely entirely on the new menu providing mechanism, this will become a reason to abort.");
      }
   }

   ATH_CHECK(ConfigSvcBase::initialize());

   std::string xmlFile(boost::to_lower_copy(m_xmlFile)); // lower case
   if( !fromDB() and (xmlFile=="none" or xmlFile == "")) {
      ATH_MSG_INFO("xml file set to NONE, will not load HLT Menu");
      return StatusCode::SUCCESS;
   }

   if( fromDB() ) {
      ATH_MSG_INFO("    DB HLT PrescaleKey  = " << m_dbHLTPSKey);
      ATH_MSG_INFO("    DB HLT PrescaleKeys = " << m_dbHLTPSKeySet);
   }
    ATH_MSG_INFO("    Run merged HLT      = " << m_setMergedHLT);


   if( fromDB() && m_dbHLTPSKey!=0 && !m_dbconfig->m_hltkeys.empty() )
      ATH_MSG_WARNING("DBHLTPSKey and DBHLTPSKeySet both specified, the DBHLTPSKeySet will be ignored");

   // Read the HLT configuration
   m_HLTFrame.setMergedHLT( m_setMergedHLT );
   if(fromDB())
   {
     m_HLTFrame.setSMK( m_dbSMKey );
     if(m_dbHLTPSKey) {
       m_HLTFrame.thePrescaleSetCollection().set_prescale_key_to_load(
           m_dbHLTPSKey);
     } else {
       m_HLTFrame.thePrescaleSetCollection().set_prescale_keys_to_load(
           m_dbconfig->m_hltkeys);
     }
   }

   CHECK(initStorageMgr());

   try {
      if( ! m_storageMgr->hltFrameLoader().load( m_HLTFrame ) ){
         ATH_MSG_ERROR("Selected keys could not be loaded");
         return StatusCode::FAILURE;
      }
      ATH_MSG_VERBOSE("HLTFrame configuration object loaded");
   }
   catch (exception & e) {
      ATH_MSG_ERROR("Standard exception caught: " << e.what());
      return StatusCode::FAILURE;
   }

   CHECK(freeStorageMgr());

   // populate the lower_chain_counter of the EF chains. 
   // to do so, take the EF chain's lower_chain_name and loop through 
   // all L2 chains to find the matching chain_counter 

   for( HLTChain* ch: m_HLTFrame.chains() ) {
      if( ch->level_enum() != EF ) continue;
      unsigned int lower_chain_counter = -1;
      if(ch->lower_chain_name() != "") {
         HLTChain* lowerChain = m_HLTFrame.chains().chain(ch->lower_chain_name());
         if(lowerChain) lower_chain_counter = lowerChain->chain_counter();
      }
      ch->set_lower_chain_counter( lower_chain_counter );
   }
   
   // print the menu after loading 
   if (msgSvc()->outputLevel() <= MSG::VERBOSE) {
      m_HLTFrame.print("  ", 5);
   } else if (msgSvc()->outputLevel() <= MSG::DEBUG) {
      m_HLTFrame.print("  ", 3);
   } else if (msgSvc()->outputLevel() <= MSG::INFO) {
      m_HLTFrame.print("  ",m_printMenuLevel);
   }
   m_HLTFrame.getPrescaleSetCollection().print();

   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",name());
   if (incSvc.retrieve().isFailure()) {
      ATH_MSG_WARNING("Cannot retrieve IncidentSvc");
   } else {
      string incname("HLTMenu HLTPS");
      incSvc->fireIncident( Incident(incname, "TrigConf") );
      ATH_MSG_INFO("Fired Incident 'TrigConf' - " << incname);
   }

   CHECK( m_eventStore.retrieve() );

   ATH_MSG_INFO("finish initialize");

   return StatusCode::SUCCESS;
}


/***********************************************************
 * start() is called by Gaudi::ServiceManager::addService()
 * which adds the  service to the active list
 *
 * Online this is called during prepareForRun() going from
 * from CONNECTED to PREPARED
 ***********************************************************/
StatusCode
TrigConf::HLTConfigSvc::start() {

   ATH_MSG_INFO("HLTConfigSvc::start");

   // Book histograms
   if (m_doMon) bookHistograms().ignore();
   m_currentLumiblock = 0;

   if( ! fromDB() ) // xml config
      return StatusCode::SUCCESS;

   // also load the prescale set for the initial HLT PSK into the PrescaleSetCollection
   if(m_dbHLTPSKey) {

      ATH_MSG_INFO("Loading PrescaleSet for initial HLT PSK: " << m_dbHLTPSKey);

      // Make sure we are starting with an empty prescale set collection
      m_HLTFrame.thePrescaleSetCollection().clear();
 
      HLTPrescaleSet* pss = new HLTPrescaleSet();
      pss->setId(m_dbHLTPSKey);

      CHECK(initStorageMgr());

      bool loadSuccess = dynamic_cast<TrigConf::StorageMgr*>(m_storageMgr)->hltPrescaleSetLoader().load( *pss );

      CHECK(freeStorageMgr());

      if(!loadSuccess) return StatusCode::FAILURE;

      m_HLTFrame.thePrescaleSetCollection().setPrescaleSet(pss); // assumes LB=0

      applyPrescaleSet(*pss);
    
      ATH_MSG_INFO( m_HLTFrame.getPrescaleSetCollection() );
   }
  
   return StatusCode::SUCCESS;
}


void
TrigConf::HLTConfigSvc::applyPrescaleSet(const TrigConf::HLTPrescaleSet& pss) {

  ATH_MSG_INFO("Applying PSK " << pss.id() << " to menu ");
  const EventInfo* pEvent(0);
  if ( m_eventStore->retrieve(pEvent).isSuccess() ) {
     ATH_MSG_INFO("on event " << *pEvent->event_ID());
  }

  m_HLTFrame.theHLTChainList().applyPrescaleSet(&pss);

  m_currentPSS = pss.id();

}


StatusCode
TrigConf::HLTConfigSvc::bookHistograms() {

   ServiceHandle<ITHistSvc> histSvc("THistSvc", name());

   CHECK(histSvc.retrieve());

   const std::string histPath = "/EXPERT/" + name() + "/";

   m_hist_timePrescaleUpdate = new TH1F("TimePrescaleUpdate",
                                        (m_histProp_timePrescaleUpdate.value().title() + ";time [ms]").c_str(),
                                        m_histProp_timePrescaleUpdate.value().bins(),
                                        m_histProp_timePrescaleUpdate.value().lowEdge(),
                                        m_histProp_timePrescaleUpdate.value().highEdge());
   const int nLB(1), nKey(1);
   m_hist_prescaleLB = new TH2I("PrescaleKey_LB","Prescale key used in LB;Lumiblock;Prescale key",
                                nLB, 0, nLB, nKey, 0, nKey);

   // try to register, delete if failure
   TH1** hists[] = { (TH1**)&m_hist_timePrescaleUpdate,
                     (TH1**)&m_hist_prescaleLB };
   for ( TH1** h: hists ) {
      if ( *h && histSvc->regHist(histPath + (*h)->GetName(), *h).isFailure() ) {
         ATH_MSG_WARNING("Cannot register histogram " << (*h)->GetName());
         delete *h;
         *h = 0;
      }
   }

   return StatusCode::SUCCESS;
}


StatusCode
TrigConf::HLTConfigSvc::finalize() {
   ATH_MSG_DEBUG("Finalizing");
   CHECK(AthService::finalize());
   return StatusCode::SUCCESS;
}


StatusCode
TrigConf::HLTConfigSvc::queryInterface( const InterfaceID& riid, void** ppvIF ) {
   StatusCode sc = StatusCode::FAILURE;

   if (ppvIF) {
      *ppvIF = 0;

      if (riid == IHLTConfigSvc::interfaceID()) {
         try {
            *ppvIF = dynamic_cast<IHLTConfigSvc*>( this );
         }
         catch( const std::bad_cast& ) {
            return StatusCode::FAILURE;
         }
         sc = StatusCode::SUCCESS;
      }
      else {
         sc = Service::queryInterface( riid, ppvIF );
      }

   }

   if ( sc.isSuccess() ) addRef();
   return sc;

}

uint32_t
TrigConf::HLTConfigSvc::masterKey() const {
   return m_HLTFrame.smk();
}

uint32_t
TrigConf::HLTConfigSvc::hltPrescaleKey() const {
   uint32_t key = m_HLTFrame.getPrescaleSet() ? m_HLTFrame.getPrescaleSet()->id() : 0;
   return key;
}

void
TrigConf::HLTConfigSvc::setL2LowerChainCounter(const CTPConfig* ctpcfg) {
   m_HLTFrame.theHLTChainList().setL2LowerChainCounter(ctpcfg);
}



// query the TriggerDB for the list of lumiblocks and corresponding prescalekeys
// will then load the and prescaleSets that have not yet been loaded
StatusCode
TrigConf::HLTConfigSvc::updatePrescaleSets(uint /*requestcount*/) {

   if( ! fromDB() ) { // xml 
      ATH_MSG_WARNING("Configured to not run from the database!");
      return StatusCode::SUCCESS;
   }

   ATH_MSG_ERROR("Running from DB is no longer supported in legacy trigger");
   return StatusCode::FAILURE;
}


// Assigns the prescales that are valid for a given lumiblock to the chains
// This method is called by TrigSteer on *every* event (keep it fast)
StatusCode
TrigConf::HLTConfigSvc::assignPrescalesToChains(uint /*lumiblock*/) {

   if(! fromDB() ) // xml
      return StatusCode::SUCCESS;

   ATH_MSG_ERROR("Running from DB is no longer supported in legacy trigger");
   return StatusCode::FAILURE;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class declaration
#include "TrigConfigSvc/HLTConfigSvc.h"

#include <exception>
#include <vector>

// Athena/Gaudi includes:
#include "PathResolver/PathResolver.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Timing.h"

// Root incluces:
#include "TH1F.h"
#include "TH2I.h"

// Local includes:
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
#include "TrigMonitorBase/TrigLockedHist.h"

#include "boost/algorithm/string/case_conv.hpp"
#include "boost/lexical_cast.hpp"

using namespace std;
using namespace TrigConf;

HLTConfigSvc::HLTConfigSvc( const string& name, ISvcLocator* pSvcLocator ) :
   base_class(name, pSvcLocator),
   m_eventStore(0),
   m_currentLumiblock(0),
   m_currentPSS(0),
   m_setMergedHLT(true),
   m_doMon(false),
   m_partition(""),
   m_histProp_timePrescaleUpdate(Gaudi::Histo1DDef("Time for prescale update",0,200,100)),
   m_hist_timePrescaleUpdate(0),
   m_hist_prescaleLB(0),
   m_PartitionName("")
{
   base_class::declareCommonProperties();

   declareProperty( "DBHLTPSKey",       m_dbHLTPSKey );
   declareProperty( "DBHLTPSKeySet",    m_dbHLTPSKeySet,
                    "List of HLT Prescale keys associated with start Lumiblocks LB1uPSK1cLB2uPSK2" );
   declareProperty( "HistTimePrescaleUpdate", m_histProp_timePrescaleUpdate,
                    "Histogram of time for prescale update");
   declareProperty( "PartitionName",    m_PartitionName,
                    "Name of the partition for the trigger prescale update mechanism");
   declareProperty( "doMergedHLT",      m_setMergedHLT,
                    "Set true to run the merged HLT processing");
   declareProperty( "doMonitoring",     m_doMon,
                    "Enable monitoring (mostly for online)");
}

HLTConfigSvc::~HLTConfigSvc()
{}


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

   CHECK(ConfigSvcBase::initialize());

   //////////////////////////////////////////////////////////////
   // BEGIN RUN-3 TESTING BLOCK - THIS SHOULD BE TEMPORARY
   ////////////////////////////////////////////////////////////// 
   string s(boost::to_lower_copy(m_configSourceString)); // lower case
   if (s == "run3_dummy") {

      std::map<std::string, std::string> dummyChains;
      dummyChains["HLT_e3_etcut1step"] = "L1_EM3";
      dummyChains["HLT_e3_etcut"] = "L1_EM3";
      dummyChains["HLT_e3_etcut_mu6"] = "L1_EM8I_MU10";
      dummyChains["HLT_e5_etcut"] = "L1_EM3";
      dummyChains["HLT_e7_etcut"] = "L1_EM3";
      dummyChains["HLT_g5_etcut"] = "L1_EM3";
      dummyChains["HLT_g10_etcut"] = "L1_EM7";
      dummyChains["HLT_g15_etcut"] = "L1_EM12";
      dummyChains["HLT_mu6"] = "L1_MU6";
      dummyChains["HLT_2mu6"] = "L1_MU6";
      dummyChains["HLT_mu6Comb"] = "L1_MU6";
      dummyChains["HLT_2mu6Comb"] = "L1_MU6";
      dummyChains["HLT_mu8"] = "L1_MU8";
      dummyChains["HLT_mu10"] = "L1_MU10";
      dummyChains["HLT_mu20"] = "L1_MU20";
      dummyChains["HLT_j85"] = "L1_J20";
      dummyChains["HLT_j100"] = "L1_J20";
      m_HLTFrame.setMergedHLT( m_setMergedHLT );
      for (const auto& mapPair : dummyChains) {
         const std::string& chainName = mapPair.first;
         const std::string& chainSeed = mapPair.second;
         const int chainCounter = std::distance(dummyChains.begin(), dummyChains.find(chainName));
         HLTChain* chain = new HLTChain( chainName, chainCounter, 1, "HLT", chainSeed, 0, vector<HLTSignature*>() );
         // Note: the ownership of chain is transfered to the frame, the frame will delete it on deconstruct.
         m_HLTFrame.theHLTChainList().addHLTChain( chain );
         ATH_MSG_INFO(" RUN 3 TESTING MODE! Adding dummy chain with hash:" << chain->chain_hash_id() << " : " << chainName << " [" << chainCounter << "] <- " << chainSeed); 
      }
      ATH_MSG_INFO(" RUN 3 TESTING MODE! Total number of chains: " << m_HLTFrame.chains().size()); 

      return StatusCode::SUCCESS;

   //////////////////////////////////////////////////////////////
   // END RUN-3 TESTING BLOCK - THIS SHOULD BE TEMPORARY
   //////////////////////////////////////////////////////////////
   } else if( !fromDB() and m_xmlFile=="NONE" ) {
      ATH_MSG_INFO("xml file set to NONE, will not load HLT Menu");
      return StatusCode::SUCCESS;
   }


   // get the partition name
   // non-empty job-property overwrite value from DataFlowConfig
   if(m_PartitionName.value() !="") {
      m_partition = m_PartitionName;
   } else {
      ServiceHandle<IJobOptionsSvc> jobOptionsSvc("JobOptionsSvc", name());
      if (jobOptionsSvc.retrieve().isFailure()) {
         ATH_MSG_WARNING("Cannot retrieve JobOptionsSvc");
      } else {
         const Property* p=Gaudi::Utils::getProperty( jobOptionsSvc->getProperties("DataFlowConfig"), "DF_PartitionName");   
         if (p != 0) m_partition = p->toString();
      }
   }


   if( fromDB() ) {
      ATH_MSG_INFO("    DB HLT PrescaleKey  = " << m_dbHLTPSKey);
      ATH_MSG_INFO("    DB HLT PrescaleKeys = " << m_dbHLTPSKeySet);
   }
   ATH_MSG_INFO("    Partition           = " << m_partition);
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

   CHECK(service("StoreGateSvc", m_eventStore, /*createIf=*/false));
   
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
  if (   m_eventStore && m_eventStore->retrieve(pEvent).isSuccess() ) {
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
TrigConf::HLTConfigSvc::updatePrescaleSets(uint requestcount) {

   if( ! fromDB() ) { // xml 
      ATH_MSG_WARNING("Configured to not run from the database!");
      return StatusCode::SUCCESS;
   }
   
   if( !m_dbconfig->m_hltkeys.empty() ) {
      ATH_MSG_WARNING("Has list of [(lb1,psk1), (lb2,psk2),...] defined!");
      return StatusCode::SUCCESS;
   }

   // Start timer
   longlong t1_ms = System::currentTime(System::milliSec);

   // Load prescale set
   CHECK(initStorageMgr());

   bool loadSuccess = dynamic_cast<TrigConf::StorageMgr*>
      (m_storageMgr)->hltPrescaleSetCollectionLoader().load( m_HLTFrame.thePrescaleSetCollection(), requestcount, m_partition );

   CHECK(freeStorageMgr());

   // Stop timer and fill histogram
   uint64_t t2_ms = System::currentTime(System::milliSec);
   if (m_hist_timePrescaleUpdate) m_hist_timePrescaleUpdate->Fill(t2_ms-t1_ms);

   if(!loadSuccess) {
      ATH_MSG_WARNING("HLTConfigSvc::updatePrescaleSets(): loading failed");
      return StatusCode::FAILURE;
   } else {
      ATH_MSG_INFO ( m_HLTFrame.thePrescaleSetCollection() );
   }
   return StatusCode::SUCCESS;
}


// Helper for assignPrescalesToChains
namespace {
   inline void fillPrescaleHist(TH2I* h, uint lb, int psk) {
      if (h==0) return;
      // Use alpha-numeric bin labels to ensure correct gathering
      char buf_psk[12], buf_lb[12];
      snprintf(buf_psk, sizeof(buf_psk), "%d", psk);  // faster than stringstream
      snprintf(buf_lb, sizeof(buf_lb), "%03d", lb);
      // Save number of bins and perform a locked Fill
      int xbins = h->GetNbinsX();    
      int ybins = h->GetNbinsY();
      lock_histogram_operation<TH2I> locked_hist(h);

      locked_hist->Fill(buf_lb, buf_psk, 1);

      // Need to make sure that all bins have labels (see Savannah #58243)    
      // Label additional bins on x-axis
      if ( h->GetNbinsX()!=xbins ) {
         const int N = h->GetNbinsX();
         for (int b=xbins+2; b<=N; ++b) {
            snprintf(buf_lb, sizeof(buf_lb), "%03d", ++lb);
            locked_hist->GetXaxis()->SetBinLabel(b, buf_lb);
         }
      }
      // Remove additional bins on y-axis
      if ( h->GetNbinsY()!=ybins ) locked_hist->LabelsDeflate("Y");
   }
}


// Assigns the prescales that are valid for a given lumiblock to the chains
// This method is called by TrigSteer on *every* event (keep it fast)
StatusCode
TrigConf::HLTConfigSvc::assignPrescalesToChains(uint lumiblock) {

   if(! fromDB() ) // xml
      return StatusCode::SUCCESS;

   if(lumiblock == m_currentLumiblock) {
      fillPrescaleHist(m_hist_prescaleLB, lumiblock, m_currentPSS);
      return StatusCode::SUCCESS;
   }

   m_currentLumiblock = lumiblock;
   
   // get the HLTPrescaleSet
   const HLTPrescaleSet* pss = m_HLTFrame.getPrescaleSetCollection().prescaleSet(lumiblock);
   if (pss == 0) {
      ATH_MSG_ERROR("Could not retrieve HLT prescale set for lumiblock = " << lumiblock);
      return StatusCode::FAILURE;
   }

   fillPrescaleHist(m_hist_prescaleLB, lumiblock, pss->id());
   
   // still the same HLTPSS -> nothing to do
   if(pss->id() ==  m_currentPSS) {
      return StatusCode::SUCCESS;
   }

   ATH_MSG_INFO("Changing PSK from " << m_currentPSS << " to " << pss->id()
                << " for lumiblock " << lumiblock);

   applyPrescaleSet(*pss);
   
   return StatusCode::SUCCESS;
}

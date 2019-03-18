/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "TrigT1NSWSimTools/StripTdsOfflineTool.h"
#include "TrigT1NSWSimTools/StripOfflineData.h"
#include "TrigT1NSWSimTools/PadOfflineData.h"


#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "MuonDigitContainer/sTgcDigit.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"
#include "MuonAGDDDescription/sTGCDetectorDescription.h"
#include "MuonAGDDDescription/sTGCDetectorHelper.h"

#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"

#include "TTree.h"
#include "TVector3.h"

#include <functional>
#include <algorithm>
#include <map>
#include <utility>


namespace NSWL1 {

    struct StripHits {
      Identifier      t_id;
      StripOfflineData* t_strip;
      int             t_cache_index;

      // constructor      
      StripHits(Identifier id, StripOfflineData* p, int c) { t_id = id; t_strip=p; t_cache_index=c; }
    };

    using STRIP_MAP=std::map < Identifier,std::vector<StripHits> >;
    using STRIP_MAP_IT=std::map < Identifier,std::vector<StripHits> >::iterator;
    using STRIP_MAP_ITEM=std::pair< Identifier,std::vector<StripHits> >;

    StripTdsOfflineTool::StripTdsOfflineTool( const std::string& type, const std::string& name, const IInterface* parent) :
      AthAlgTool(type,name,parent),
      m_incidentSvc("IncidentSvc",name),
      m_rndmSvc("AtRndmGenSvc",name),
      m_rndmEngine(0),
      m_detManager(0),
      m_sTgcIdHelper(0),
      m_strip_cache_runNumber(-1),
      m_strip_cache_eventNumber(-1),
      m_strip_cache_status(CLEARED),
      m_tree(0)

    {
      declareInterface<NSWL1::IStripTdsTool>(this);

      declareProperty("RndmEngineName", m_rndmEngineName = "StripTdsOfflineTool", "the name of the random engine");
      declareProperty("sTGC_DigitContainerName", m_sTgcDigitContainer = "sTGC_DIGITS", "the name of the sTGC digit container");
      declareProperty("sTGC_SdoContainerName", m_sTgcSdoContainer = "sTGC_SDO", "the name of the sTGC SDO container");
      declareProperty("DoNtuple", m_doNtuple = false, "input the StripTds branches into the analysis ntuple"); 

      // reserve enough slots for the trigger sectors and fills empty vectors
      // std::vector< std::vector<StripData*> >::iterator it = m_strip_cache.begin();
      //std::vector<std::unique_ptr<StripData>>::iterator it = m_strip_cache.begin();
    }

    StripTdsOfflineTool::~StripTdsOfflineTool() {
      // clear the internal cache
      this->clear_cache();
      // Clear Ntuple variables
      if(m_stripCharge) delete m_stripCharge;
      if(m_stripCharge_6bit) delete m_stripCharge_6bit;
      if(m_stripCharge_10bit) delete m_stripCharge_10bit;


    }

    void StripTdsOfflineTool::clear_cache() {
      ATH_MSG_INFO( "Clearing Strip Cache"); 
      for(unsigned int i = 0; i < m_strip_cache.size(); ++i)
	    //delete m_strip_cache[i];
      m_strip_cache.clear();     
    }
  
  StatusCode StripTdsOfflineTool::initialize() {
    ATH_MSG_INFO( "initializing " << name() ); 
    
    ATH_MSG_INFO( name() << " configuration:");
    ATH_MSG_INFO(" " << std::setw(32) << std::setfill('.') << std::setiosflags(std::ios::left) << m_rndmEngineName.name() << m_rndmEngineName.value());
    ATH_MSG_INFO(" " << std::setw(32) << std::setfill('.') << std::setiosflags(std::ios::left) << m_sTgcDigitContainer.name() << m_sTgcDigitContainer.value());
    ATH_MSG_INFO(" " << std::setw(32) << std::setfill('.') << std::setiosflags(std::ios::left) << m_sTgcSdoContainer.name() << m_sTgcSdoContainer.value());
    ATH_MSG_INFO(" " << std::setw(32) << std::setfill('.') << std::setiosflags(std::ios::left) << m_doNtuple.name() << ((m_doNtuple)? "[True]":"[False]")
                       << std::setfill(' ') << std::setiosflags(std::ios::right) );

 

      const IInterface* parent = this->parent();
      const INamedInterface* pnamed = dynamic_cast<const INamedInterface*>(parent);
      std::string algo_name = pnamed->name();
      
      if ( m_doNtuple && algo_name=="NSWL1Simulation" ) {
        ITHistSvc* tHistSvc;
        StatusCode sc = service("THistSvc", tHistSvc);
        if(sc.isFailure()) {
          ATH_MSG_FATAL("Unable to retrieve THistSvc");
          return sc;
        }

        char ntuple_name[40]={'\0'};
        //memset(ntuple_name,'\0',40*sizeof(char));
        sprintf(ntuple_name,"%sTree",algo_name.c_str());

        m_tree = 0;
        sc = tHistSvc->getTree(ntuple_name,m_tree);
        if (sc.isFailure()) {
          this->clear_ntuple_variables();
          ATH_MSG_FATAL("Could not retrieve the analysis ntuple from the THistSvc");
          return sc;
        } else {
          ATH_MSG_INFO("Analysis ntuple succesfully retrieved");
          sc = this->book_branches();
          if (sc.isFailure()) {
            ATH_MSG_ERROR("Cannot book the branches for the analysis ntuple");
          }
        }

      };

      // retrieve the Incident Service
      if( m_incidentSvc.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve the Incident Service");
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_INFO("Incident Service successfully retrieved");
      }
      m_incidentSvc->addListener(this,IncidentType::BeginEvent);

      // retrieve the Random Service
      if( m_rndmSvc.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve the Random Number Service");
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_INFO("Random Number Service successfully retrieved");
      }

      // retrieve the random engine
      m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
      if (m_rndmEngine==0) {
        ATH_MSG_FATAL("Could not retrieve the random engine " << m_rndmEngineName);
        return StatusCode::FAILURE;
      }

      //  retrieve the MuonDetectormanager
      if( detStore()->retrieve( m_detManager ).isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve the MuonDetectorManager");
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_INFO("MuonDetectorManager successfully retrieved");
      }

      //  retrieve the sTGC offline Id helper
      if( detStore()->retrieve( m_sTgcIdHelper ).isFailure() ){
        ATH_MSG_FATAL("Failed to retrieve sTgcIdHelper");
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_INFO("sTgcIdHelper successfully retrieved");
      }
 
      return StatusCode::SUCCESS;
    }

    void StripTdsOfflineTool::handle(const Incident& inc) {
      if( inc.type()==IncidentType::BeginEvent ) {
        this->clear_cache();  
        this->reset_ntuple_variables();

        m_strip_cache_status = CLEARED;
      }
    }


    StatusCode StripTdsOfflineTool::book_branches() {
      m_nStripHits = 0;
      m_stripCharge= new std::vector< float >();
      m_stripCharge_6bit= new std::vector< float >();
      m_stripCharge_10bit= new std::vector< float >();
      m_strip_global_X= new std::vector< float >();
      m_strip_global_Y= new std::vector< float >();
      m_strip_global_Z= new std::vector< float >();
      m_strip_local_X= new std::vector< float >();
      m_strip_local_Y= new std::vector< float >();
      m_strip_layer= new std::vector< float >();
      m_strip_isSmall= new std::vector< float >();
      m_strip_eta= new std::vector< float >();
      m_strip_phi= new std::vector< float >();
      m_strip_channel= new std::vector< int >();
      m_strip_BCID= new std::vector< int >();
      m_strip_wedge= new std::vector< int >();
      m_strip_time= new std::vector< float >();




      if (m_tree) {
	    std::string ToolName = name().substr(  name().find("::")+2,std::string::npos );
        const char* n = ToolName.c_str();
        m_tree->Branch(TString::Format("%s_nStripHits",n).Data(),&m_nStripHits,TString::Format("%s_nStripHits/i",n).Data());
        ATH_MSG_INFO("StripTdsOfflineTool Booking variable" << TString::Format("%s_nStripHits",n).Data());
        m_tree->Branch(TString::Format("%s_charge",n).Data(),&m_stripCharge);
        m_tree->Branch(TString::Format("%s_charge_6bit",n).Data(),&m_stripCharge_6bit);
        m_tree->Branch(TString::Format("%s_charge_10bit",n).Data(),&m_stripCharge_10bit);
        m_tree->Branch(TString::Format("%s_global_X",n).Data(),&m_strip_global_X);
        m_tree->Branch(TString::Format("%s_global_Y",n).Data(),&m_strip_global_Y);
        m_tree->Branch(TString::Format("%s_global_Z",n).Data(),&m_strip_global_Z);
        m_tree->Branch(TString::Format("%s_local_X",n).Data(),&m_strip_local_X);
        m_tree->Branch(TString::Format("%s_local_Y",n).Data(),&m_strip_local_Y);
        m_tree->Branch(TString::Format("%s_layer",n).Data(),&m_strip_layer);
        m_tree->Branch(TString::Format("%s_isSmall",n).Data(),&m_strip_isSmall);
        m_tree->Branch(TString::Format("%s_eta",n).Data(),&m_strip_eta);
        m_tree->Branch(TString::Format("%s_phi",n).Data(),&m_strip_phi);
        m_tree->Branch(TString::Format("%s_readStrip",n).Data(),&m_strip_readStrip);

        m_tree->Branch(TString::Format("%s_channel",n).Data(),&m_strip_channel);
        m_tree->Branch(TString::Format("%s_BCID",n).Data(),&m_strip_BCID);
        m_tree->Branch(TString::Format("%s_wedge",n).Data(),&m_strip_wedge);
        m_tree->Branch(TString::Format("%s_time",n).Data(),&m_strip_time);


    //	m_tree->Branch(TString::Format("%s_stripTruthHitGlobalX",n).Data(),&m_stripTruthHitGlobalX);
    //	m_tree->Branch(TString::Format("%s_stripTruthHitGlobalY",n).Data(),&m_stripTruthHitGlobalY);
    //	m_tree->Branch(TString::Format("%s_stripTruthHitGlobalZ",n).Data(),&m_stripTruthHitGlobalZ);

        

        //   m_tree->Branch(TString::Format("%s_stripGlobalY",n).Data(),&m_stripGlobalY);
        //   m_tree->Branch(TString::Format("%s_stripGlobalZ",n).Data(),&m_stripGlobalZ);
        //  m_tree->Branch(TString::Format("%s_stripTruthHitGlobal",n).Data(),&m_stripTruthHitGlobalZ);
        //   m_tree->Branch(TString::Format("%s_stripEtaIdFromOfflineId",n).Data(),&m_stripEtaIdFromOfflineId);
        //   m_tree->Branch(TString::Format("%s_stripPhiIdFromOfflineId",n).Data(),&m_stripPhiIdFromOfflineId);
        //   m_tree->Branch(TString::Format("%s_stripIsSmallIdFromOfflineId",n).Data(),&m_stripIsSmallFromOfflineId);
        //   m_tree->Branch(TString::Format("%s_stripLayerFromOfflineId",n).Data(),&m_stripLayerFromOfflineId);
        //   m_tree->Branch(TString::Format("%s_offlineIdStripEtaIdConverted",n).Data(),&m_offlineIdStripEtaConverted);
        //   m_tree->Branch(TString::Format("%s_offlineIdStripPhiIdConverted",n).Data(),&m_offlineIdStripPhiConverted);
        //   m_tree->Branch(TString::Format("%s_stripEtaIdFromOldSimu",n).Data(),&m_stripEtaIdFromOldSimu);
        //   m_tree->Branch(TString::Format("%s_stripPhiIdFromOldSimu",n).Data(),&m_stripPhiIdFromOldSimu);
      }
      else { 
         return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }


    void StripTdsOfflineTool::reset_ntuple_variables() {
      // if ntuple is not booked nothing to do
      if ( m_tree==0 ) return;
      this->clear_ntuple_variables();
      //reset the ntuple variables
      m_nStripHits = 0;
      return;
    }

    void StripTdsOfflineTool::clear_ntuple_variables() {
      //clear the ntuple variables
      m_nStripHits = 0;
      m_stripCharge->clear();
      m_stripCharge_6bit->clear();
      m_stripCharge_10bit->clear();
      m_stripCharge->clear();
      m_stripCharge_6bit->clear();
      m_stripCharge_10bit->clear();
      m_strip_global_X->clear();
      m_strip_global_Y->clear();
      m_strip_global_Z->clear();
      m_strip_local_X->clear();
      m_strip_local_Y->clear();
      m_strip_layer->clear();
      m_strip_isSmall->clear();
      m_strip_eta->clear();
      m_strip_phi->clear();
      m_strip_readStrip->clear();
      m_strip_channel->clear();
      m_strip_BCID->clear();
      m_strip_time->clear();
      m_strip_wedge->clear();
    
      return;
    }

  void StripTdsOfflineTool::fill_strip_validation_id() {
    
    for (unsigned int p=0; p<m_strip_cache.size(); p++) {
      m_nStripHits++;
      ATH_MSG_INFO("Hits :" << m_nStripHits << " index " <<  p << " Cache strip  " << m_strip_cache.at(p).get() << "  " << m_strip_cache.size() );	

      m_stripCharge->push_back(m_strip_cache.at(p)->strip_charge());
      m_stripCharge_6bit->push_back(m_strip_cache.at(p)->strip_charge_6bit());
      m_stripCharge_10bit->push_back(m_strip_cache.at(p)->strip_charge_10bit());
      m_strip_readStrip->push_back(m_strip_cache.at(p)->readStrip());
      m_strip_channel->push_back(m_strip_cache.at(p)->channelId());
      m_strip_BCID->push_back(m_strip_cache.at(p)->trig_BCID());
      m_strip_time->push_back(m_strip_cache.at(p)->time());
      m_strip_wedge->push_back(m_strip_cache.at(p)->wedge());
    }
    return;
  }


  StatusCode StripTdsOfflineTool::gather_strip_data(std::vector<std::unique_ptr<StripData>>& strips, const std::vector<std::unique_ptr<PadTrigger>>& padTriggers) {
      ATH_MSG_INFO( "gather_strip_data: start gathering all strip htis");

      // No sector implemented yet!!!
     

      // retrieve the current run number and event number
      const EventInfo* pevt = 0;
      StatusCode sc = evtStore()->retrieve(pevt);
      if ( !sc.isSuccess() ) {
        ATH_MSG_WARNING( "Could not retrieve the EventInfo, so cannot associate run and event number to the current STRIP cache" ); 
        m_strip_cache_runNumber   = -1;
        m_strip_cache_eventNumber = -1;
      } else {
        m_strip_cache_runNumber = pevt->event_ID()->run_number();
        m_strip_cache_eventNumber = pevt->event_ID()->event_number();
      }

      if (m_strip_cache_status==CLEARED) {
        // renew the STRIP cache if this is the next event
        m_strip_cache_status = fill_strip_cache(padTriggers);
      }


      // check the STRIP cache status
      if ( m_strip_cache_status!=OK ) {
        ATH_MSG_ERROR ( "STRIP cache is in a bad status!" );
        return StatusCode::FAILURE;
      }


      // delivering the required collection
      for (unsigned int i=0; i< m_strip_cache.size(); i++) { 
        // Check if a stip should be read according to pad triggers
        strips.push_back(std::move(m_strip_cache.at(i)));
	  }
      ATH_MSG_DEBUG( "delivered n. " << strips.size() << " STRIP hits." );

      return StatusCode::SUCCESS;
  }


    StripTdsOfflineTool::cStatus StripTdsOfflineTool::fill_strip_cache( const std::vector<std::unique_ptr<PadTrigger>>& padTriggers) {
      ATH_MSG_DEBUG( "fill_strip_cache: clearing existing STRIP hit cache" );
      this->clear_cache();

      ATH_MSG_DEBUG( "fill_strip_cache: start filling the cache for STRIP hits" );

      const MuonSimDataCollection* sdo_container = 0;
      StatusCode sc = evtStore()->retrieve( sdo_container, m_sTgcSdoContainer.value().c_str() );
      if ( !sc.isSuccess() ) {
        ATH_MSG_WARNING("could not retrieve the sTGC SDO container: it will not be possible to associate the MC truth");
      }

      const sTgcDigitContainer* digit_container = 0;
      sc = evtStore()->retrieve( digit_container, m_sTgcDigitContainer.value().c_str() );
      if ( !sc.isSuccess() ) {
        ATH_MSG_ERROR("could not retrieve the sTGC Digit container: cannot return the STRIP hits");
        return FILL_ERROR;
      }

      sTgcDigitContainer::const_iterator it   = digit_container->begin();
      sTgcDigitContainer::const_iterator it_e = digit_container->end();

      ATH_MSG_DEBUG( "retrieved sTGC Digit Container with " << digit_container->digit_size() << " collection" );
      int strip_hit_number = 0;
      //std::vector<StripHits> strip_hits;
      for(; it!=it_e; ++it) {
        const sTgcDigitCollection* coll = *it;
  
	    ATH_MSG_DEBUG( "processing collection with size " << coll->size() );
        
	     for (unsigned int item=0; item<coll->size(); item++) {
            const sTgcDigit* digit = coll->at(item);
            Identifier Id = digit->identify();

	        const MuonGM::sTgcReadoutElement* rdoEl = m_detManager->getsTgcReadoutElement(Id);
            int channel_type   = m_sTgcIdHelper->channelType(Id);
 	        // process only Strip data
	        if (channel_type!=1) continue;           
            
	        Amg::Vector2D  strip_lpos;
	        Amg::Vector3D strip_gpos;
	        rdoEl->stripPosition(Id,strip_lpos);
	        //Amg::Vector3D pos(strip_lpos.x(),strip_lpos.y(),0.0);
	  
	        //	  ATH_MSG_DEBUG( "Grabbed local pos" );
            auto stripSurface=rdoEl->surface(Id);
            
	        stripSurface.localToGlobal(strip_lpos, strip_gpos, strip_gpos);
	        //strip_gpos = rdoEl->localToGlobalCoords(pos,Id);


	        std::string stName = m_sTgcIdHelper->stationNameString(m_sTgcIdHelper->stationName(Id));
            int stationEta     = m_sTgcIdHelper->stationEta(Id);
            int stationPhi     = m_sTgcIdHelper->stationPhi(Id);
            int wedge      = m_sTgcIdHelper->multilayer(Id);
            int layer        = m_sTgcIdHelper->gasGap(Id);
          
            int channel        = m_sTgcIdHelper->channel(Id);
	        int bctag          = digit->bcTag();
	


	        strip_hit_number++;

            int strip_eta        = 0;// m_sTgcIdHelper->stripEta(Id);
            int strip_phi        = 0;// m_sTgcIdHelper->stripPhi(Id);

            ATH_MSG_DEBUG(     "sTGC Strip hit " << strip_hit_number << ":  Station Name [" << stName << "]"
			     << "  Station Eta ["  << stationEta             << "]"
			     << "  Station Phi ["  << stationPhi             << "]"
			     << "  Wedge ["    << wedge              << "]"
			     << "  Layer ["       << layer                << "]"
			     << "  Type ["         << channel_type           << "]"
			     << "  ChNr ["         << channel                << "]"
			     << "  Strip Eta ["      << strip_eta                << "]"
			     << "  Strip Phi ["      << strip_phi                << "]" 
			     << "  Strip bcTAg ["      << bctag                << "]" );

            int isSmall = stName[2] == 'S';

            int trigger_sector = (isSmall)? stationPhi*2-1 : stationPhi*2-2;
            int cache_index    = (stationEta>0)? trigger_sector + 16 : trigger_sector;

            ATH_MSG_DEBUG(     "sTGC Strip hit " << strip_hit_number << ":  Trigger Sector [" << trigger_sector << "]"
			     << "  Cache Index ["  << cache_index                         << "]" );


            // process STRIP hit time: apply the time delay, set the BC tag for the hit according to the trigger capture window               


            //ATH_MSG_INFO("Strip at GposZ=" << strip_gpos->z() << " belongs to multiplet ");
            if(m_doNtuple){
                ATH_MSG_DEBUG( "Fill Stuff" );  
                m_strip_global_X->push_back(strip_gpos.x());
                m_strip_global_Y->push_back(strip_gpos.y());
                m_strip_global_Z->push_back(strip_gpos.z());
                m_strip_local_X->push_back(strip_lpos.x());
                m_strip_local_Y->push_back(strip_lpos.y());
                m_strip_layer->push_back(layer);
                m_strip_isSmall->push_back(isSmall);
                m_strip_eta->push_back(stationEta);
                m_strip_phi->push_back(stationPhi);
                ATH_MSG_DEBUG( "Fill Stuff more" );
            }



            //S.I
            //StripOfflineData* strip = new StripOfflineData(Id,m_sTgcIdHelper,digit);
            auto strip=std::make_unique<StripOfflineData>(Id,m_sTgcIdHelper,digit);
            //S.I
            strip->set_locX(strip_lpos.x());
            strip->set_locY(strip_lpos.y());
        
        

            bool read_strip=readStrip(strip.get(),padTriggers);
            if (read_strip && (strip->bandId() ==-1 || strip->phiId()==-1 ) ){
                ATH_MSG_DEBUG("StripTdsOfflineTool:NO MATCH ALL \n" <<
                    "wedge:" << strip->wedge() << "\n"
                    <<"layer:"<< strip->layer() << "\n"
                    <<"loc_x:"<< strip->locX()<< "\n");
            }

            strip->set_readStrip(read_strip);

            strip->set_globX(strip_gpos.x());
            strip->set_globY(strip_gpos.y());
            strip->set_globZ(strip_gpos.z());

            strip->set_locX(strip_lpos.x());
            strip->set_locY(strip_lpos.y());
            strip->set_locZ(0             );
            m_strip_cache.push_back(std::move(strip));
	  }//collections

    }//items
      if (m_doNtuple) this->fill_strip_validation_id();
      ATH_MSG_DEBUG( "fill_strip_cache: end of processing" );
      return OK;
  }




  bool  StripTdsOfflineTool::readStrip(StripData* strip,const std::vector<std::unique_ptr<PadTrigger>>& padTriggers){
    /*!
     * ReadStrip(uint16_t bandID,StripData* strip): Simple function to return wether a fired strip should 
     *be readout base on a pad trigger bandID
     * Inputs: Pad bandID (uint16_t), and StripData strip.
     */

    
    //End test
    if (strip->bandId() !=-1){
      ATH_MSG_DEBUG("StripTdsOfflineTool:ReadStrip: BandId already set\n" <<"moduleID:"<< strip->moduleId() +1 << "\n"
		   <<"sectiorID:"<< strip->sectorId() + 1<< "\n" <<"layer:"<<strip->wedge()<< "\n");
    }
    if (strip->phiId() !=-1){
      ATH_MSG_DEBUG("StripTdsOfflineTool:ReadStrip: PhiId already set\n" <<"moduleID:"<< strip->moduleId() +1 << "\n"
		   <<"sectiorID:"<< strip->sectorId() + 1<< "\n" <<"layer:"<<strip->wedge()<< "\n");
    }

    char side= strip->sideId() ? 'A' : 'C';
    char type= strip->type();
    sTGCDetectorHelper sTGC_helper;
    // int layer_index=strip->layer() -1;

    sTGCDetectorDescription *sTGC=0;
    //Get_sTGCDetector's 2nd input value can range from eta=1 to eta=3
    //moduleID() provides the values -1 to -3 and 1 to 3, so we need abs value
    sTGC = sTGC_helper.Get_sTGCDetector(type,std::abs(strip->moduleId()),strip->sectorId(),strip->wedge(),side);
    //sTGCDetectorDescription *sTGC = sTGC_helper.Get_sTGCDetector(strip->stationName());

    if (!sTGC){
      ATH_MSG_WARNING("StripTdsOfflineTool:ReadStrip: Could not find detector with:\n" <<
		    "type:" << type << "\n"
 		     <<"moduleID:"<< strip->moduleId() +1 << "\n"
		     <<"sectiorID:"<< strip->sectorId() + 1<< "\n"
		    <<"layer:"<<strip->wedge()<< "\n"
		     <<"side:"<<side<< "\n");
      return false;
    }

    ATH_MSG_DEBUG("StripTdsOfflineTool:Strip: \n" <<
		 "wedge:" << strip->wedge() << "\n"
		 <<"layer:"<< strip->layer() << "\n"
		 <<"loc_x:"<< strip->locX()<< "\n");

    for(const auto& trig :padTriggers){
        
        const auto& innerbandsLocMinY=trig->m_trglocalminYInner;
        const auto& innerbandsLocMaxY=trig->m_trglocalmaxYInner;
        const auto& outerbandsLocMinY=trig->m_trglocalminYOuter;
        const auto& outerbandsLocMaxY=trig->m_trglocalmaxYOuter;        
        const auto& innerLayers=trig->m_trgSelectedLayersInner;
        const auto& outerLayers=trig->m_trgSelectedLayersOuter;
        int idxinner=0;
        for(const auto& lyr : innerLayers){
                  int loc_min_y = innerbandsLocMinY.at(idxinner);
                  int loc_max_y = innerbandsLocMaxY.at(idxinner);
                  auto pad=trig->m_padsInner.at(idxinner);
                  int trgwedge=pad->multipletId();
                  if (trgwedge != strip->wedge()||
                      lyr != strip->layer() ||
                      strip->sideId()!=pad->sideId() ||
                      strip->isSmall()==pad->sectorType() || //strip returns 1 pad returns 0 for small
                      strip->sectorId()!=pad->sectorId() ||
                      strip->moduleId() !=pad->moduleId() ||
                      strip->locX() > loc_max_y+3.2 ||
                      strip->locX() < loc_min_y-3.2 )
                      
                  {
                      idxinner++;
                      continue;
                  }
                  strip->setBandId(trig->m_bandid);
                  strip->setPhiId(trig->m_phi_id);
                  return true;
        }//inner layers loop
        
        int idxouter=0;
        for(const auto& lyr : outerLayers){
                  int loc_min_y = outerbandsLocMinY.at(idxouter);
                  int loc_max_y = outerbandsLocMaxY.at(idxouter);

                  auto pad=trig->m_padsOuter.at(idxouter);
                  int trgwedge=pad->multipletId();
                  
                  if (trgwedge != strip->wedge() ||
                      lyr != strip->layer() ||
                      strip->sideId()!=pad->sideId() ||
                      strip->isSmall()==pad->sectorType() || //strip returns 1 pad returns 0 for small
                      strip->sectorId()!=pad->sectorId() ||
                      strip->moduleId() !=pad->moduleId() ||
                       strip->locX() > loc_max_y+3.2 ||
                       strip->locX() < loc_min_y-3.2 )
                  {
                      idxouter++;
                      continue;
                  }
                  strip->setBandId(trig->m_bandid);
                  strip->setPhiId(trig->m_phi_id);
                  return true;
        }//outer layer loop
        
    }//padtriggers loop
    return false;
  } 



    /* Code for when pand id is fixed*/

  //   sTGCReadoutParameters  roParams=sTGC->GetReadoutParameters();
    

  //   if (layer_index < 0 || layer_index >= roParams.firstTriggerBand.size()+1){
  //     ATH_MSG_DEBUG("StripClusterTool:ReadStrip: Asked for layer that dosen't exist:" << layer_index+1);
  //     return false;
  //     }
  //   ATH_MSG_DEBUG("BandID" << "  " << unsigned(bandID) << "  ["<<roParams.firstTriggerBand.at(layer_index) <<","<< roParams.firstTriggerBand.at(layer_index)+ roParams.nTriggerBands.at(layer_index) << "]");

  //   if (unsigned(bandID) <= roParams.firstTriggerBand.at(layer_index)) {
  //     ATH_MSG_DEBUG("Module not in Trigger Band: low");
  //     return false;
  //   } 
  //   if (unsigned(bandID) >= roParams.firstTriggerBand.at(layer_index)+ roParams.nTriggerBands.at(layer_index)) {
  //     ATH_MSG_DEBUG("Module not in Trigger Band: high");
  // 	return false;    }

  //   int band_index=bandID-roParams.firstTriggerBand.at(layer_index);
  //   /* Strip where counting starts */
  //   int start_strip=roParams.firstStripInTrigger.at(layer_index);

  //   /* Grab correct layer */
  //   std::vector<int>* stripsInBands=0;
  //   if (layer_index==0) stripsInBands= &(roParams.StripsInBandsLayer1);
  //   else if (layer_index==1) stripsInBands= &(roParams.StripsInBandsLayer2);
  //   else if (layer_index==2) stripsInBands= &(roParams.StripsInBandsLayer3);
  //   else if (layer_index==3) stripsInBands= &(roParams.StripsInBandsLayer4);
  //   else{
  //     ATH_MSG_DEBUG("StripClusterTool:ReadStrip: Asked for layer that dosen't exist");
  //     return false;
  //   }
  //   /* Sum over all other strips to get the starting strip for this band id */

  //   if (band_index >= stripsInBands->size() or band_index<0) {
  //     ATH_MSG_DEBUG("StripClusterTool:ReadStrip: band_index is out of range:"<< band_index <<" of " << stripsInBands->size());
  //     return false;
  //   }

  //   for( int i=1; i <band_index; i++) start_strip+=stripsInBands->at(i-1);
    
  //   /* Check to see if this strip falls in this band Range */
  //   if (strip->channelId() < start_strip){
  //     ATH_MSG_DEBUG("Strip not in Trigger Band: low "<< strip->channelId() <<"  "<<start_strip);
  // 	     return false;}
  //   if (strip->channelId() > start_strip+stripsInBands->at(band_index)){
  //     ATH_MSG_DEBUG("Strip not in Trigger Band: high "<< strip->channelId() <<"  "<<start_strip+stripsInBands->at(band_index));
  // 	return false;
  //   }
  
  //   /* Some code here to cache this results */
  //   strip->setBandId((int) bandID);
  //   return true;
  
  // }


}

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1NSWSimTools/StripTdsOfflineTool.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "TrigT1NSWSimTools/StripOfflineData.h"
#include "TrigT1NSWSimTools/PadOfflineData.h"
#include "EventInfo/EventID.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "MuonDigitContainer/sTgcDigit.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"
#include "MuonAGDDDescription/sTGCDetectorDescription.h"
#include "MuonAGDDDescription/sTGCDetectorHelper.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "GaudiKernel/EventContext.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"

#include "TTree.h"
#include <functional>
#include <algorithm>
#include <map>
#include <utility>

namespace NSWL1 {

    class StripHits {
    public:
      Identifier      t_id;
      StripOfflineData* t_strip;
      int             t_cache_index;
      StripHits(Identifier id, StripOfflineData* p, int c) { t_id = id; t_strip=p; t_cache_index=c; }
    };

    using STRIP_MAP=std::map < Identifier,std::vector<StripHits> >;
    using STRIP_MAP_IT=std::map < Identifier,std::vector<StripHits> >::iterator;
    using STRIP_MAP_ITEM=std::pair< Identifier,std::vector<StripHits> >;

    StripTdsOfflineTool::StripTdsOfflineTool( const std::string& type, const std::string& name, const IInterface* parent) :
      AthAlgTool(type,name,parent),
      m_incidentSvc("IncidentSvc",name),
      m_rndmSvc("AtRndmGenSvc",name),
      m_rndmEngine(nullptr),
      m_detManager(nullptr),
      m_strip_cache_runNumber(-1),
      m_strip_cache_eventNumber(-1),
      m_strip_cache_status(CLEARED),
      m_tree(nullptr)

    {
      declareInterface<NSWL1::IStripTdsTool>(this);
      declareProperty("RndmEngineName", m_rndmEngineName = "StripTdsOfflineTool", "the name of the random engine");
      declareProperty("sTGC_DigitContainerName", m_sTgcDigitContainer = "sTGC_DIGITS", "the name of the sTGC digit container");
      declareProperty("sTGC_SdoContainerName", m_sTgcSdoContainer = "sTGC_SDO", "the name of the sTGC SDO container");
      declareProperty("DoNtuple", m_doNtuple = false, "input the StripTds branches into the analysis ntuple");
    }

    StripTdsOfflineTool::~StripTdsOfflineTool() {
      this->clear_cache();
      if(m_stripCharge) delete m_stripCharge;
      if(m_stripCharge_6bit) delete m_stripCharge_6bit;
      if(m_stripCharge_10bit) delete m_stripCharge_10bit;
    }

    void StripTdsOfflineTool::clear_cache() {
      ATH_MSG_INFO( "Clearing Strip Cache"); 
      for(unsigned int i = 0; i < m_strip_cache.size(); ++i)
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
      const std::string& algo_name = pnamed->name();
      if ( m_doNtuple && algo_name=="NSWL1Simulation" ) {
        ITHistSvc* tHistSvc;
        ATH_CHECK(service("THistSvc", tHistSvc));
        char ntuple_name[40]={'\0'};
        sprintf(ntuple_name,"%sTree",algo_name.c_str());
        m_tree = 0;
        ATH_CHECK(tHistSvc->getTree(ntuple_name,m_tree));
      }
      ATH_CHECK(this->book_branches());
      ATH_CHECK(m_incidentSvc.retrieve());
      m_incidentSvc->addListener(this,IncidentType::BeginEvent);
      ATH_CHECK(m_rndmSvc.retrieve());
      m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
      if (m_rndmEngine==0) {
        ATH_MSG_FATAL("Could not retrieve the random engine " << m_rndmEngineName);
        return StatusCode::FAILURE;
      }
      ATH_CHECK(detStore()->retrieve(m_detManager));
      ATH_CHECK(m_idHelperSvc.retrieve());
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
      m_strip_readStrip = new std::vector<float >();
      m_strip_channel= new std::vector< int >();
      m_strip_BCID= new std::vector< int >();
      m_strip_wedge= new std::vector< int >();
      m_strip_time= new std::vector< float >();
      
      if (m_tree) {
	      std::string ToolName = name().substr(  name().find("::")+2,std::string::npos );
        const char* n = ToolName.c_str();
        m_tree->Branch(TString::Format("%s_nStripHits",n).Data(),&m_nStripHits,TString::Format("%s_nStripHits/i",n).Data());
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
      }
      return StatusCode::SUCCESS;
    }


    void StripTdsOfflineTool::reset_ntuple_variables() {
      this->clear_ntuple_variables();
      m_nStripHits = 0;
      return;
    }

    void StripTdsOfflineTool::clear_ntuple_variables() {
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
      const EventContext& ctx = Gaudi::Hive::currentContext();

      m_strip_cache_runNumber = ctx.eventID().run_number();
      m_strip_cache_eventNumber = ctx.eventID().event_number();

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
      for(; it!=it_e; ++it) {
        const sTgcDigitCollection* coll = *it;
  
	      ATH_MSG_DEBUG( "processing collection with size " << coll->size() );
        
	      for (unsigned int item=0; item<coll->size(); item++) {
            const sTgcDigit* digit = coll->at(item);
            Identifier Id = digit->identify();
	          const MuonGM::sTgcReadoutElement* rdoEl = m_detManager->getsTgcReadoutElement(Id);
            int channel_type   = m_idHelperSvc->stgcIdHelper().channelType(Id);
 	          // process only Strip data
	          if (channel_type!=1) continue;           
            
	          Amg::Vector2D  strip_lpos;
	          Amg::Vector3D strip_gpos;
	          rdoEl->stripPosition(Id,strip_lpos);
            const auto& stripSurface=rdoEl->surface(Id); 
	          stripSurface.localToGlobal(strip_lpos, strip_gpos, strip_gpos);

	          std::string stName = m_idHelperSvc->stgcIdHelper().stationNameString(m_idHelperSvc->stgcIdHelper().stationName(Id));
            int stationEta     = m_idHelperSvc->stgcIdHelper().stationEta(Id);
            int stationPhi     = m_idHelperSvc->stgcIdHelper().stationPhi(Id);
            int wedge          = m_idHelperSvc->stgcIdHelper().multilayer(Id);
            int layer          = m_idHelperSvc->stgcIdHelper().gasGap(Id);
            int channel        = m_idHelperSvc->stgcIdHelper().channel(Id);
	          int bctag          = digit->bcTag();

	          strip_hit_number++;
            int strip_eta        = 0;
            int strip_phi        = 0;

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
            int trigger_sector = (isSmall)? stationPhi*2 : stationPhi*2-1;//
            int cache_index    = (stationEta>0)? trigger_sector + 16 : trigger_sector;
            ATH_MSG_DEBUG(     "sTGC Strip hit " << strip_hit_number << ":  Trigger Sector [" << trigger_sector << "]"
			        << "  Cache Index ["  << cache_index                         << "]" );

            // process STRIP hit time: apply the time delay, set the BC tag for the hit according to the trigger capture window               
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
            auto strip=std::make_unique<StripOfflineData>(Id,&m_idHelperSvc->stgcIdHelper(),digit);
            strip->set_locX(strip_lpos.x());
            strip->set_locY(strip_lpos.y());
            int sideid= (stationEta>0) ? 1 : 0;
            int sectortype= (isSmall==1) ? 0 : 1;
            int sectorid=stationPhi;
            int moduleid=std::abs(stationEta);
            int wedgeid=wedge;
            int layerid=layer;
            strip->setSideId(sideid);
            strip->setSectorType(sectortype);
            strip->setSectorId(sectorid);
            strip->setModuleId(moduleid);
            strip->setWedgeId(wedgeid);
            strip->setLayerId(layerid);
            strip->set_globX(strip_gpos.x());
            strip->set_globY(strip_gpos.y());
            strip->set_globZ(strip_gpos.z());
            strip->set_locZ(0);

            bool read_strip=readStrip(strip.get(),padTriggers);
            if (read_strip && (strip->bandId() ==-1 || strip->phiId()==-1 ) ){
                ATH_MSG_FATAL("StripTdsOfflineTool:NO MATCH ALL \n" <<
                    "wedge:" << strip->wedge() << "\n"
                    <<"layer:"<< strip->layer() << "\n"
                    <<"loc_x:"<< strip->locX()<< "\n");
                return cStatus::FILL_ERROR;
            }

            

            //set coordinates above ! readStrip needs that variables !
            strip->set_readStrip(read_strip);

            m_strip_cache.push_back(std::move(strip));
	      }//collections
      }//items
      this->fill_strip_validation_id();
      ATH_MSG_DEBUG( "fill_strip_cache: end of processing" );
      return OK;
  }




  bool  StripTdsOfflineTool::readStrip(StripData* strip,const std::vector<std::unique_ptr<PadTrigger>>& padTriggers){

    if (strip->bandId() !=-1){
      ATH_MSG_DEBUG("StripTdsOfflineTool:ReadStrip: BandId already set\n" <<"moduleID:"<< strip->moduleId() +1 << "\n"
		   <<"sectiorID:"<< strip->sectorId() + 1<< "\n" <<"layer:"<<strip->wedge()<< "\n");
    }
    if (strip->phiId() !=-1){
      ATH_MSG_DEBUG("StripTdsOfflineTool:ReadStrip: PhiId already set\n" <<"moduleID:"<< strip->moduleId() +1 << "\n"
		   <<"sectiorID:"<< strip->sectorId() + 1<< "\n" <<"layer:"<<strip->wedge()<< "\n");
    }
    for(const std::unique_ptr<PadTrigger>& trig :padTriggers){
        //std::shared_ptr<PadData> padIn=trig->firstPadInner();
        for(const std::shared_ptr<PadData>& pad : trig->m_pads){
          if (
                      strip->sideId()!=pad->sideId() ||
                      strip->isSmall()==pad->sectorType() || //strip returns 1 pad returns 0
                      strip->sectorId()!=pad->sectorId() ||
                      std::abs(strip->etaCenter() )> trig->etaMax() || //use abs / sideC
                      std::abs(strip->etaCenter() ) < trig->etaMin()
            )
            {
                      continue;
            }
            else{

                  strip->setBandId(trig->bandId());
                  strip->setPhiId(trig->phiId());
                  return true;
            }          
        }//pad loop
        return false;
    }//padtrigger loop
    return false;
  }
}

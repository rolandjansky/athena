/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ConcurrencyFlags.h"

#include "TrigT1NSWSimTools/StripTdsOfflineTool.h"

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
      m_detManager(nullptr),
      m_tree(nullptr)
    {
      declareInterface<NSWL1::IStripTdsTool>(this);
    }

  StatusCode StripTdsOfflineTool::initialize() {
    ATH_MSG_DEBUG( "initializing " << name() );

    ATH_MSG_DEBUG( name() << " configuration:");
    ATH_MSG_DEBUG(" " << std::setw(32) << std::setfill('.') << std::setiosflags(std::ios::left) << m_doNtuple.name() << ((m_doNtuple)? "[True]":"[False]")
                       << std::setfill(' ') << std::setiosflags(std::ios::right) );

    ATH_CHECK(m_sTgcDigitContainer.initialize());
    ATH_CHECK(m_sTgcSdoContainer.initialize(m_isMC));

    const IInterface* parent = this->parent();
    const INamedInterface* pnamed = dynamic_cast<const INamedInterface*>(parent);
    const std::string& algo_name = pnamed->name();

    if ( m_doNtuple ) {
      if (Gaudi::Concurrency::ConcurrencyFlags::numConcurrentEvents() > 1) {
        ATH_MSG_ERROR("DoNtuple is not possible in multi-threaded mode");
        return StatusCode::FAILURE;
      }

      ATH_CHECK( m_incidentSvc.retrieve() );
      m_incidentSvc->addListener(this,IncidentType::BeginEvent);

      if ( algo_name=="NSWL1Simulation" ) {
        ITHistSvc* tHistSvc;
        ATH_CHECK(service("THistSvc", tHistSvc));
        m_tree = 0;
        std::string ntuple_name = algo_name+"Tree";
        ATH_CHECK(tHistSvc->getTree(ntuple_name,m_tree));
        ATH_CHECK(this->book_branches());
      }
    }
    ATH_CHECK(detStore()->retrieve(m_detManager));
    ATH_CHECK(m_idHelperSvc.retrieve());
    return StatusCode::SUCCESS;
  }

    void StripTdsOfflineTool::handle(const Incident& inc) {
      if( inc.type()==IncidentType::BeginEvent ) {
        this->clear_ntuple_variables();
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
      m_strip_isSmall= new std::vector< bool >();
      m_strip_eta= new std::vector< float >();
      m_strip_phi= new std::vector< float >();
      m_strip_readStrip = new std::vector<bool >();
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


    void StripTdsOfflineTool::clear_ntuple_variables() {
      if(m_tree==0) return;
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
    }

  void StripTdsOfflineTool::fill_strip_validation_id(std::vector<std::unique_ptr<StripData>> &strip_cache) const {
    for (const auto &hit : strip_cache) {
      m_nStripHits++;
      ATH_MSG_DEBUG("Hits :" << m_nStripHits << " Cache strip  " << hit.get() << "  " << strip_cache.size() );

      m_stripCharge->push_back(hit->strip_charge());
      m_stripCharge_6bit->push_back(hit->strip_charge_6bit());
      m_stripCharge_10bit->push_back(hit->strip_charge_10bit());
      m_strip_readStrip->push_back(hit->readStrip());
      m_strip_channel->push_back(hit->channelId());
      m_strip_BCID->push_back(hit->trig_BCID());
      m_strip_time->push_back(hit->time());
      m_strip_wedge->push_back(hit->wedge());
    }
  }


  StatusCode StripTdsOfflineTool::gather_strip_data(std::vector<std::unique_ptr<StripData>>& strips, const std::vector<std::unique_ptr<PadTrigger>>& padTriggers) const {
      ATH_MSG_DEBUG( "gather_strip_data: start gathering all strip htis");

      std::vector<std::unique_ptr<StripData>> strip_cache;
      ATH_CHECK(fill_strip_cache(padTriggers, strip_cache));

      // delivering the required collection
      for (unsigned int i=0; i< strip_cache.size(); i++) {
        // Check if a stip should be read according to pad triggers
        strips.push_back(std::move(strip_cache.at(i)));
      }
      strip_cache.clear();
      ATH_MSG_DEBUG( "Delivered n. " << strips.size() << " STRIP hits." );
      return StatusCode::SUCCESS;
  }


    StatusCode StripTdsOfflineTool::fill_strip_cache( const std::vector<std::unique_ptr<PadTrigger>>& padTriggers, std::vector<std::unique_ptr<StripData>> &strip_cache) const {
      ATH_MSG_DEBUG( "fill_strip_cache: start filling the cache for STRIP hits" );

      if(m_isMC){
	SG::ReadHandle<MuonSimDataCollection> sdo_container(m_sTgcSdoContainer);
	if(!sdo_container.isValid()){
	  ATH_MSG_WARNING("could not retrieve the sTGC SDO container: it will not be possible to associate the MC truth");
	}
      }

      SG::ReadHandle<sTgcDigitContainer> digit_container(m_sTgcDigitContainer);
      if(!digit_container.isValid()){
        ATH_MSG_ERROR("could not retrieve the sTGC Digit container: cannot return the STRIP hits");
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

            Amg::Vector2D strip_lpos;
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

            ATH_MSG_DEBUG("sTGC Strip hit "    << strip_hit_number << ":  Station Name [" << stName << "]"
                          << "  Station Eta [" << stationEta       << "]"
                          << "  Station Phi [" << stationPhi       << "]"
                          << "  Wedge ["       << wedge            << "]"
                          << "  Layer ["       << layer            << "]"
                          << "  Type ["        << channel_type     << "]"
                          << "  ChNr ["        << channel          << "]"
                          << "  Strip Eta ["   << strip_eta        << "]"
                          << "  Strip Phi ["   << strip_phi        << "]"
                          << "  Strip bcTAg [" << bctag            << "]");

            bool isSmall = m_idHelperSvc->stgcIdHelper().isSmall(Id);
            int trigger_sector = (isSmall)? stationPhi*2 : stationPhi*2-1;//
            int cache_index    = (stationEta>0)? trigger_sector + 16 : trigger_sector;
            ATH_MSG_DEBUG("sTGC Strip hit " << strip_hit_number << ":  Trigger Sector [" << trigger_sector << "]" << "  Cache Index ["  << cache_index << "]" );

            // process STRIP hit time: apply the time delay, set the BC tag for the hit according to the trigger capture window
            ATH_MSG_DEBUG( "Filling ntuple variables" );
            if (m_doNtuple) {
              m_strip_global_X->push_back(strip_gpos.x());
              m_strip_global_Y->push_back(strip_gpos.y());
              m_strip_global_Z->push_back(strip_gpos.z());
              m_strip_local_X->push_back(strip_lpos.x());
              m_strip_local_Y->push_back(strip_lpos.y());
              m_strip_layer->push_back(layer);
              m_strip_isSmall->push_back(isSmall);
              m_strip_eta->push_back(stationEta);
              m_strip_phi->push_back(stationPhi);
            }
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
            if (read_strip && (strip->bandId() ==-1 || strip->phiId()==-1 ) ) {
                ATH_MSG_FATAL("StripTdsOfflineTool:NO MATCH ALL " << "\n"
                              << "wedge:" << strip->wedge() << "\n"
                              << "layer:"<< strip->layer() << "\n"
                              << "loc_x:"<< strip->locX()<< "\n");
            }

            //set coordinates above ! readStrip needs that variables !
            strip->set_readStrip(read_strip);
            strip_cache.push_back(std::move(strip));
        }//collections
      }//items
      if (m_doNtuple) this->fill_strip_validation_id(strip_cache);
      ATH_MSG_DEBUG( "fill_strip_cache: end of processing" );
      return StatusCode::SUCCESS;
  }


  bool StripTdsOfflineTool::readStrip(StripData* strip,const std::vector<std::unique_ptr<PadTrigger>>& padTriggers) const {

    if (strip->bandId() !=-1) {
      ATH_MSG_DEBUG("StripTdsOfflineTool:ReadStrip: BandId already set\n" << "moduleID:" << strip->moduleId() +1 << "\n"
                    << "sectiorID:" << strip->sectorId() + 1 << "\n" << "layer:" << strip->wedge() << "\n");
    }
    if (strip->phiId() !=-1) {
      ATH_MSG_DEBUG("StripTdsOfflineTool:ReadStrip: PhiId already set\n" << "moduleID:"<< strip->moduleId() +1 << "\n"
                    << "sectiorID:" << strip->sectorId() + 1 << "\n" << "layer:" << strip->wedge() << "\n");
    }
    for(const std::unique_ptr<PadTrigger>& trig :padTriggers){
      //std::shared_ptr<PadData> padIn=trig->firstPadInner();
      for(const std::shared_ptr<PadData>& pad : trig->m_pads){
        if (strip->sideId()!=pad->sideId() ||
            strip->isSmall()==pad->sectorType() || //strip returns 1 pad returns 0
            strip->sectorId()!=pad->sectorId() ||  
            std::abs(strip->etaCenter() )> trig->etaMax() || //use abs / sideC
            std::abs(strip->etaCenter() ) < trig->etaMin() ||
            strip->layer()!=pad->gasGapId()
           ) continue;
          else {
            strip->setBandId(trig->bandId());
            strip->setPhiId(trig->phiId());
            return true;
          }
      }//pad loop
    }//padtrigger loop
    return false;
  }
}

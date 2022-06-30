/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ConcurrencyFlags.h"

#include "TrigT1NSWSimTools/StripClusterTool.h"

namespace NSWL1 {

  StripClusterTool::StripClusterTool( const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type,name,parent),
    m_detManager(nullptr),
    m_tree(nullptr)
  {
    declareInterface<NSWL1::IStripClusterTool>(this);
  }

  StatusCode StripClusterTool::initialize() {
    ATH_MSG_DEBUG( "initializing " << name() );
    ATH_MSG_DEBUG( name() << " configuration:");
    ATH_MSG_DEBUG(" " << std::setw(32) << std::setfill('.') << std::setiosflags(std::ios::left) << m_doNtuple.name() << ((m_doNtuple)? "[True]":"[False]")
                      << std::setfill(' ') << std::setiosflags(std::ios::right) );

    ATH_CHECK(m_sTgcSdoContainerKey.initialize(m_isMC));

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

      if ( algo_name=="NSWL1Simulation" ){
        ITHistSvc* tHistSvc;
        ATH_CHECK(service("THistSvc", tHistSvc));
        m_tree = 0;
        std::string ntuple_name = algo_name+"Tree";
        ATH_CHECK(this->init_branches());
        ATH_CHECK(tHistSvc->getTree(ntuple_name,m_tree));
        ATH_CHECK(this->book_branches());
      }
    }

    // retrieve the MuonDetectormanager
    ATH_CHECK(detStore()->retrieve( m_detManager ));
    ATH_CHECK(m_idHelperSvc.retrieve());
    return StatusCode::SUCCESS;
  }

  void StripClusterTool::handle(const Incident& inc) {
    if( inc.type()==IncidentType::BeginEvent ) {
      this->clear_ntuple_variables();
    }
  }

  StatusCode StripClusterTool::init_branches() {
    m_cl_n = 0;
    m_cl_charge = nullptr;
    m_cl_size = nullptr;
    m_cl_x = nullptr;
    m_cl_y = nullptr;
    m_cl_z = nullptr;
    m_cl_lx = nullptr;
    m_cl_ly = nullptr;
    m_cl_lz = nullptr;
    m_cl_ltgx = nullptr;
    m_cl_ltgy = nullptr;
    m_cl_ltgz = nullptr;
    m_cl_truth_x = nullptr;
    m_cl_truth_y = nullptr;
    m_cl_truth_z = nullptr;
    m_cl_truth_lx = nullptr;
    m_cl_truth_ly = nullptr;
    m_cl_truth_lz = nullptr;
    m_cl_truth_E = nullptr;
    m_cl_truth_n = nullptr;
    m_cl_side = nullptr;
    m_cl_isSmall = nullptr;
    m_cl_wedge = nullptr;
    m_cl_sector = nullptr;
    m_cl_module = nullptr;
    m_cl_layer = nullptr;
    m_cl_bandId = nullptr;
    m_cl_phiId = nullptr;

    return StatusCode::SUCCESS;
  }

  StatusCode StripClusterTool::book_branches() {
    m_cl_n= 0;
    m_cl_charge = new std::vector< int >();
    m_cl_size = new std::vector< int >();
    m_cl_x= new std::vector< float >();
    m_cl_y= new std::vector< float >();
    m_cl_z= new std::vector< float >();
    m_cl_lx= new std::vector< float >();
    m_cl_ly= new std::vector< float >();
    m_cl_lz= new std::vector< float >();
    m_cl_ltgx= new std::vector< float >();
    m_cl_ltgy= new std::vector< float >();
    m_cl_ltgz= new std::vector< float >();
    m_cl_truth_x= new std::vector<float >();
    m_cl_truth_y= new std::vector<float >();
    m_cl_truth_z= new std::vector<float>();
    m_cl_truth_lx= new std::vector<float >();
    m_cl_truth_ly= new std::vector<float >();
    m_cl_truth_lz= new std::vector<float>();
    m_cl_truth_E= new std::vector<float >();
    m_cl_truth_n= new std::vector<int >();
    m_cl_side= new std::vector<int>();
    m_cl_isSmall= new std::vector<int>();
    m_cl_wedge= new std::vector<int>();
    m_cl_sector= new std::vector<int>();
    m_cl_module= new std::vector<int>();
    m_cl_layer= new std::vector<int>();
    m_cl_bandId= new std::vector<int>();
    m_cl_phiId= new std::vector<int>();

    if (m_tree) {
      std::string ToolName = name().substr(  name().find("::")+2,std::string::npos );
      const char* n = ToolName.c_str();
      m_tree->Branch(TString::Format("%s_cl_n",n).Data(),&m_cl_n,TString::Format("%s_cl_n/i",n).Data());
      m_tree->Branch(TString::Format("%s_cl_charge",n).Data(),&m_cl_charge);
      m_tree->Branch(TString::Format("%s_cl_x",n).Data(),&m_cl_x);
      m_tree->Branch(TString::Format("%s_cl_y",n).Data(),&m_cl_y);
      m_tree->Branch(TString::Format("%s_cl_z",n).Data(),&m_cl_z);
      m_tree->Branch(TString::Format("%s_cl_lx",n).Data(),&m_cl_lx);
      m_tree->Branch(TString::Format("%s_cl_ly",n).Data(),&m_cl_ly);
      m_tree->Branch(TString::Format("%s_cl_lz",n).Data(),&m_cl_lz);
      m_tree->Branch(TString::Format("%s_cl_ltgx",n).Data(),&m_cl_ltgx);
      m_tree->Branch(TString::Format("%s_cl_ltgy",n).Data(),&m_cl_ltgy);
      m_tree->Branch(TString::Format("%s_cl_ltgz",n).Data(),&m_cl_ltgz);
      m_tree->Branch(TString::Format("%s_cl_size",n).Data(),&m_cl_size);
      m_tree->Branch(TString::Format("%s_cl_isSmall",n).Data(),&m_cl_isSmall);
      m_tree->Branch(TString::Format("%s_cl_side",n).Data(),&m_cl_side);
      m_tree->Branch(TString::Format("%s_cl_wedge",n).Data(),&m_cl_wedge);
      m_tree->Branch(TString::Format("%s_cl_sector",n).Data(),&m_cl_sector);
      m_tree->Branch(TString::Format("%s_cl_module",n).Data(),&m_cl_module);
      m_tree->Branch(TString::Format("%s_cl_layer",n).Data(),&m_cl_layer);
      m_tree->Branch(TString::Format("%s_cl_bandId",n).Data(),&m_cl_bandId);
      m_tree->Branch(TString::Format("%s_cl_phiId",n).Data(),&m_cl_phiId);
      m_tree->Branch(TString::Format("%s_cl_truth_x",n).Data(),&m_cl_truth_x);
      m_tree->Branch(TString::Format("%s_cl_truth_y",n).Data(),&m_cl_truth_y);
      m_tree->Branch(TString::Format("%s_cl_truth_z",n).Data(),&m_cl_truth_z);
      m_tree->Branch(TString::Format("%s_cl_truth_lx",n).Data(),&m_cl_truth_lx);
      m_tree->Branch(TString::Format("%s_cl_truth_ly",n).Data(),&m_cl_truth_ly);
      m_tree->Branch(TString::Format("%s_cl_truth_lz",n).Data(),&m_cl_truth_lz);
      m_tree->Branch(TString::Format("%s_cl_truth_E",n).Data(),&m_cl_truth_E);
      m_tree->Branch(TString::Format("%s_cl_truth_n",n).Data(),&m_cl_truth_n);
    }
    return StatusCode::SUCCESS;
  }

  void StripClusterTool::clear_ntuple_variables() {
    if ( m_tree==0 ) return;

    //clear the ntuple variables
    m_cl_n = 0;
    m_cl_charge->clear();
    m_cl_x->clear();
    m_cl_y->clear();
    m_cl_z->clear();
    m_cl_lx->clear();
    m_cl_ly->clear();
    m_cl_lz->clear();
    m_cl_ltgx->clear();
    m_cl_ltgy->clear();
    m_cl_ltgz->clear();
    m_cl_size->clear();
    m_cl_truth_x->clear();
    m_cl_truth_y->clear();
    m_cl_truth_z->clear();
    m_cl_truth_E->clear();
    m_cl_truth_n->clear();
    m_cl_truth_lx->clear();
    m_cl_truth_ly->clear();
    m_cl_truth_lz->clear();
    m_cl_side->clear();
    m_cl_isSmall->clear();
    m_cl_wedge->clear();
    m_cl_sector->clear();
    m_cl_module->clear();
    m_cl_layer->clear();
    m_cl_bandId->clear();
    m_cl_phiId->clear();
  }

  StatusCode StripClusterTool::fill_strip_validation_id(std::vector<std::unique_ptr<StripClusterData>>& clusters, 
                                                        std::vector<std::shared_ptr<std::vector<std::unique_ptr<StripData> >>  > &cluster_cache) const {

    ATH_MSG_DEBUG("Cluster cache received " << cluster_cache.size());

    bool first_strip=true;
    for(const auto &this_cl : cluster_cache){
      float x_pos=0;
      float y_pos=0;
      float z_pos=0;

      float x_lpos=0;
      float y_lpos=0;
      float z_lpos=0;

      int charge=0;
      int n_strip=0;

      first_strip=true;
      float locx=-999999;
      float locy=-999999;
      if (this_cl->empty()){
        ATH_MSG_WARNING("Zero size cluster!!");
        continue;
      }

      const auto& ctx = Gaudi::Hive::currentContext();
      const MuonSimDataCollection* sdo_container = nullptr;
      if(m_isMC) {
        SG::ReadHandle<MuonSimDataCollection> readMuonSimDataCollection( m_sTgcSdoContainerKey, ctx );
        if( !readMuonSimDataCollection.isValid() ){
          ATH_MSG_WARNING("could not retrieve the sTGC SDO container: it will not be possible to associate the MC truth");
          return StatusCode::FAILURE;
        }
        sdo_container = readMuonSimDataCollection.cptr();
      }

      for(const auto &strip_cl : *this_cl){
        n_strip++;
        ATH_MSG_DEBUG("Start strip" << n_strip);
        // Save truth deposits associated with cluster should be the same on for the whole strip, so take the first one need to work on this logic
        if(m_isMC && first_strip) {
          first_strip=false;
          Identifier Id = strip_cl->Identity();
          const MuonGM::sTgcReadoutElement* rdoEl = m_detManager->getsTgcReadoutElement(Id);
          auto it = sdo_container->find(Id);
          if(it == sdo_container->end()) continue;
          const MuonSimData strip_sdo = it->second;
          std::vector<MuonSimData::Deposit> deposits;
          strip_sdo.deposits(deposits);
          //retrieve the info of the first associated hit, i.e. the fastest in time
          if (deposits.size()!=1) ATH_MSG_WARNING("Multiple cluster hits for strip!");
          if (deposits.empty()){
            ATH_MSG_WARNING("Empty hit here");
            continue;
          }

          int    truth_barcode   = deposits[0].first.barcode();
          double truth_localPosX = deposits[0].second.firstEntry();
          double truth_localPosY = deposits[0].second.secondEntry();
          Amg::Vector3D hit_gpos(0.,0.,0.);
          Amg::Vector2D lpos(truth_localPosX,truth_localPosY);
          rdoEl->surface(Id).localToGlobal(lpos, hit_gpos,hit_gpos);
          double truth_globalPosX = hit_gpos.x();
          double truth_globalPosY = hit_gpos.y();
          double truth_globalPosZ = hit_gpos.z();
          float  truth_energy     = strip_sdo.word();

          if(std::fabs(locx-lpos.x())>.001 || std::fabs(locy - lpos.y())>.001){
            ATH_MSG_DEBUG("OLD locx " << locx << " new locx " << lpos.x() << " b " << int(locx!=lpos.x()));
            ATH_MSG_DEBUG("OLD locy " << locy << " new locy " << lpos.y() << " b " << int(locy!=lpos.y()));
            ATH_MSG_DEBUG("Cluster hit, truth barcode = " << truth_barcode);
            ATH_MSG_DEBUG("Cluster hit, truth globalPosX = " << truth_globalPosX
                          << ", truth globalPosY = " << truth_globalPosY
                          << ", truth globalPosZ = " << truth_globalPosZ
                          << ", truth enegy deposit = " << truth_energy);
            ATH_MSG_DEBUG("Cluster hit, truth localPosX = " << lpos.x()
                          << ", truth localPosY = " << lpos.y()
                          << ", truth enegy deposit = " << truth_energy);

            if (m_doNtuple) {
              m_cl_truth_x->push_back( hit_gpos.x() );
              m_cl_truth_y->push_back( hit_gpos.y() );
              m_cl_truth_z->push_back( hit_gpos.z() );

              m_cl_truth_lx->push_back( lpos.x() );
              m_cl_truth_ly->push_back( lpos.y() );
              m_cl_truth_lz->push_back( 0 );
              m_cl_truth_E->push_back( truth_energy );
            }
          }
        }

        float s_charge=strip_cl->strip_charge_6bit();
        charge+=s_charge;
        x_pos+=strip_cl->globX()*s_charge;
        y_pos+=strip_cl->globY()*s_charge;
        z_pos+=strip_cl->globZ()*s_charge;

        x_lpos+=(strip_cl->locX())*s_charge;
        y_lpos+=(strip_cl->locY())*s_charge;
        z_lpos+=(strip_cl->locZ())*s_charge;


        ATH_MSG_DEBUG("Cluster ------------------------------------------" );
        ATH_MSG_DEBUG("Cluster strip charge: " << s_charge);
        ATH_MSG_DEBUG("Cluster strip loc X: " << strip_cl->locX());
        ATH_MSG_DEBUG("Cluster strip loc Y: " << strip_cl->locY());
        ATH_MSG_DEBUG("Cluster strip glob X: " << strip_cl->globX());
        ATH_MSG_DEBUG("Cluster strip glob Y: " << strip_cl->globY());
        ATH_MSG_DEBUG("Cluster strip glob Z: " << strip_cl->globZ());
        ATH_MSG_DEBUG("Cluster strip locx dist: " << locx-strip_cl->locX());
        ATH_MSG_DEBUG("Cluster strip charge o dist: " << s_charge/(locx-strip_cl->locX()));
        ATH_MSG_DEBUG("Channel " << strip_cl->channelId());

      }//end of this_cl loop

      if ( abs(x_pos/charge)<200 && abs(y_pos/charge)<200){
        ATH_MSG_WARNING("Cluster ------------------------------------------" );
        ATH_MSG_WARNING("Cluster strip charge: " << charge );
        ATH_MSG_WARNING("Cluster strip glob X: " << x_pos << x_pos/charge);
        ATH_MSG_WARNING("Cluster strip glob Y: " << y_pos << y_pos/charge);
        ATH_MSG_WARNING("Cluster strip glob Z: " << z_pos << z_pos/charge);
      }
    
      if (charge != 0){
        x_pos=x_pos/charge;
        y_pos=y_pos/charge;
        z_pos=z_pos/charge;
        x_lpos=x_lpos/charge;
        y_lpos=y_lpos/charge;
        z_lpos=z_lpos/charge;
      }
      if (m_doNtuple) {
        m_cl_x->push_back(x_pos);
        m_cl_y->push_back(y_pos);
        m_cl_z->push_back(z_pos);

        m_cl_lx->push_back(x_lpos);
        m_cl_ly->push_back(y_lpos);
        m_cl_lz->push_back(z_lpos);
        m_cl_charge->push_back(charge);
        m_cl_size->push_back(n_strip);

        m_cl_side->push_back(this_cl->at(0)->sideId() );
        m_cl_isSmall->push_back(this_cl->at(0)->isSmall() );
        m_cl_wedge->push_back(this_cl->at(0)->wedge());
        m_cl_sector->push_back(this_cl->at(0)->sectorId());
        m_cl_module->push_back(this_cl->at(0)->moduleId() );
        m_cl_layer->push_back(this_cl->at(0)->layer());
        m_cl_bandId->push_back(this_cl->at(0)->bandId());
        m_cl_phiId->push_back(this_cl->at(0)->phiId());
      }
      ATH_MSG_DEBUG("Cluster dump with X:" << x_pos << " Y: " << y_pos << " Z: " << z_pos << " cluster charge: " << charge);
      ATH_MSG_DEBUG("Cluster dump with lX:" << x_lpos << " lY: " << y_lpos << " lZ: " << z_lpos << " cluster charge: " << charge);

      auto stripClOfflData=std::make_unique<StripClusterOfflineData>(
               this_cl->at(0)->bandId(),
               this_cl->at(0)->trig_BCID(),
               this_cl->at(0)->sideId(),
               this_cl->at(0)->phiId(),
               this_cl->at(0)->isSmall(),
               this_cl->at(0)->moduleId(),
               this_cl->at(0)->sectorId(),
               this_cl->at(0)->wedge(),
               this_cl->at(0)->layer(),
               n_strip,
               charge,
               x_pos,
               y_pos,
               z_pos);
      clusters.push_back(std::move(stripClOfflData));
    }
    if (m_doNtuple) m_cl_n = clusters.size();
    ATH_MSG_DEBUG("Finished Fill");
    return StatusCode::SUCCESS;
  }


  StatusCode StripClusterTool::cluster_strip_data( std::vector<std::unique_ptr<StripData>>& strips, std::vector< std::unique_ptr<StripClusterData> >& clusters) const {

      if(strips.empty()){
        ATH_MSG_WARNING("Received 0 strip hits... Skip event");
        return StatusCode::SUCCESS;
      }
      
      std::map<uint32_t, std::vector<std::unique_ptr<StripData>> > stripMap;
      // Filter by layer:
      for (auto& st : strips) {
        // sideId [0, 1]
        auto sideId = st->sideId();
        // sectorType: small==0, large==1
        auto sectorType = st->sectorType();
        // sectorId [1,8]
        auto sectorId = st->sectorId();
        // etaId [1,3]
        auto etaId = st->moduleId();
        // wedgeId [1,2]
        auto wedgeId = st->wedge();
        // layer [1,4]
        auto layerId = st->layer();
        
        // add 1 as the first placeholder, hence cache_hash always has 7 digits
        std::string id_str = std::to_string(1)+std::to_string(sideId)+std::to_string(sectorType)+std::to_string(sectorId)+std::to_string(etaId)+std::to_string(wedgeId)+std::to_string(layerId);
        const uint32_t cache_hash = atoi(id_str.c_str()); 

        auto it = stripMap.find(cache_hash);
        if (it != stripMap.end()){
          it->second.push_back(std::move(st));
        }
        else{
          stripMap[cache_hash].push_back(std::move(st));
        }

      }
      
      std::vector< std::shared_ptr<std::vector<std::unique_ptr<StripData> >>  > cluster_cache;
      for (auto &item : stripMap) {
        std::vector<std::unique_ptr<StripData>>& stripList = item.second;
      
        //S.I sort strip w.r.t channelId in ascending order
        std::sort(stripList.begin(), stripList.end(), [](const auto& s1,const auto& s2) { return s1->channelId()<s2->channelId(); });

        auto hit=stripList.begin();
        ATH_MSG_DEBUG("Cluster Hits :" << (*hit)->channelId() << " " << m_idHelperSvc->stgcIdHelper().gasGap( (*hit)->Identity())
                      << "   " << (*hit)->moduleId() << "   " << (*hit)->sectorId() << "   " << (*hit)->wedge() << "  " << (*hit)->sideId() );
        int first_ch=(*hit)->channelId();//channel id of the first strip
        int prev_ch=-1;

        auto cr_cluster=std::make_shared< std::vector<std::unique_ptr<StripData>> >();
        
        for(auto& this_hit : stripList){
          if(!(this_hit)->readStrip() ) continue;
          if( ((this_hit)->bandId()==-1 || this_hit->phiId()==-1) ){
            ATH_MSG_WARNING("Read Strip without BandId :" << (this_hit)->channelId() << " " << m_idHelperSvc->stgcIdHelper().gasGap( (this_hit)->Identity())
                            << "   " << (this_hit)->moduleId() << "   " << (this_hit)->sectorId() << "   " << (this_hit)->wedge() << "  " << (this_hit)->sideId() );
            continue;
          }

          if (prev_ch==-1){//for the first time...
            prev_ch = first_ch;
            cr_cluster->push_back(std::move(this_hit));
            continue;
          }
                 
          int this_ch=(this_hit)->channelId();
          if ( (this_ch < prev_ch)) {
            ATH_MSG_ERROR("Hits Ordered incorrectly!!!" );
            return StatusCode::FAILURE;
          }

          if (this_ch == prev_ch || this_ch == prev_ch+1) cr_cluster->push_back(std::move(this_hit)); // form cluster with adjacent +-1 strips 
          else {
            cluster_cache.push_back(std::move(cr_cluster));                         //put the current cluster into the clusters buffer
            cr_cluster=std::make_shared<std::vector<std::unique_ptr<StripData>>>(); //create a new empty cluster and assign this hit as the first hit
            cr_cluster->push_back(std::move(this_hit));
          }

          prev_ch=this_ch;
        }

        if(!cr_cluster->empty()) cluster_cache.push_back(std::move(cr_cluster));    //don't forget the last cluster in the loop
      }

      ATH_MSG_DEBUG("Found :" << cluster_cache.size() << " clusters");
      ATH_CHECK(fill_strip_validation_id(clusters, cluster_cache));
      cluster_cache.clear();

      return StatusCode::SUCCESS;
  }
}

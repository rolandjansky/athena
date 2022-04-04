/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1NSWSimTools/StripSegmentTool.h"
#include <TVector3.h>

namespace NSWL1 {

  StripSegmentTool::StripSegmentTool( const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type,name,parent),
    m_incidentSvc("IncidentSvc",name),
    m_tree(nullptr),
    m_rIndexBits(0),
    m_dThetaBits(0),
    m_zbounds({-1,1}),
    m_etabounds({-1,1}),
    m_rbounds({-1,-1}),
    m_ridxScheme(0),
    m_dtheta_min(0),
    m_dtheta_max(0)
  {
    declareInterface<NSWL1::IStripSegmentTool>(this);
    declareProperty("DoNtuple",     m_doNtuple = false, "input the StripTds branches into the analysis ntuple");
    declareProperty("sTGC_SdoContainerName", m_sTgcSdoContainer = "sTGC_SDO", "the name of the sTGC SDO container");
    declareProperty("rIndexBits",   m_rIndexBits = 8,   "number bits in R-index calculation");
    declareProperty("dthetaBits",   m_dThetaBits = 5,   "number bits in dTheta calculation");
    declareProperty("dthetaMin",    m_dtheta_min = -15, "minimum allowed value for dtheta in mrad");
    declareProperty("dthetaMax",    m_dtheta_max = 15,  "maximum allowed value for dtheta in mrad");
    declareProperty("rIndexScheme", m_ridxScheme = 1,   "rIndex slicing scheme/ 0-->R / 1-->eta");
  }

  StatusCode StripSegmentTool::initialize() {
    ATH_MSG_DEBUG("initializing " << name() );
    ATH_MSG_DEBUG(name() << " configuration:");
    const IInterface* parent = this->parent();
    const INamedInterface* pnamed = dynamic_cast<const INamedInterface*>(parent);
    const std::string& algo_name = pnamed->name();
    if ( m_doNtuple && algo_name=="NSWL1Simulation" ) {
      ITHistSvc* tHistSvc;
      ATH_CHECK(service("THistSvc", tHistSvc));
      std::string ntuple_name = algo_name+"Tree";
      m_tree = nullptr;
      ATH_CHECK(tHistSvc->getTree(ntuple_name,m_tree));
      ATH_CHECK(this->book_branches());
    } else this->clear_ntuple_variables();
    ATH_CHECK(m_incidentSvc.retrieve());
    m_incidentSvc->addListener(this,IncidentType::BeginEvent);
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_regSelTableKey.initialize());
    return StatusCode::SUCCESS;
  }

  void StripSegmentTool::handle(const Incident& inc) {
    if( inc.type()==IncidentType::BeginEvent ) {
      this->reset_ntuple_variables();
    }
  }

  StatusCode StripSegmentTool::FetchDetectorEnvelope(){
    const MuonGM::MuonDetectorManager* p_det;
    ATH_CHECK(detStore()->retrieve(p_det));
    SG::ReadCondHandle<IRegSelLUTCondData> rh_stgcLUT(m_regSelTableKey);
    const auto regSelector = dynamic_cast<const RegSelSiLUT*>(rh_stgcLUT->payload());
    std::vector<const RegSelModule*> moduleList;
    for(const auto& i : m_idHelperSvc->stgcIdHelper().idVector()) { // all modules
      IdentifierHash moduleHashId;
      m_idHelperSvc->stgcIdHelper().get_module_hash(i, moduleHashId);
      moduleList.push_back(regSelector->Module(moduleHashId));
    }
    float etamin=-1;
    float etamax=-1;
    float rmin=-1;
    float rmax=-1;
    float zmin=-1;
    float zmax=-1;
    std::sort(moduleList.begin(),moduleList.end(),[](const auto& M1,const auto& M2){ return std::abs(M1->_etaMin()) < std::abs(M2->_etaMin());} );
    etamin=moduleList.at(0)->_etaMin();
    std::sort(moduleList.begin(),moduleList.end(),[](const auto& M1,const auto& M2){ return std::abs(M1->_etaMax()) > std::abs(M2->_etaMax());} );
    etamax=moduleList.at(0)->_etaMax();
    std::sort(moduleList.begin(),moduleList.end(),[](const auto& M1,const auto& M2){ return std::abs(M1->rMin()) < std::abs(M2->rMin());} );
    rmin=moduleList.at(0)->rMin();
    std::sort(moduleList.begin(),moduleList.end(),[](const auto& M1,const auto& M2){ return std::abs(M1->rMax()) > std::abs(M2->rMax());} );
    rmax=moduleList.at(0)->rMax();
    std::sort(moduleList.begin(),moduleList.end(),[](const auto& M1,const auto& M2){ return std::abs(M1->zMin()) < std::abs(M2->zMin());} );
    zmin=moduleList.at(0)->zMin();
    std::sort(moduleList.begin(),moduleList.end(),[](const auto& M1,const auto& M2){ return std::abs(M1->zMax()) > std::abs(M2->zMax());} );
    zmax=moduleList.at(0)->zMax();

    if(rmin<=0 || rmax<=0){
      ATH_MSG_FATAL("Unable to fetch NSW r/z boundaries");
      return StatusCode::FAILURE;
    }
    m_rbounds= std::make_pair(rmin,rmax);
    m_etabounds=std::make_pair(etamin,etamax);
    m_zbounds=std::make_pair(zmin,zmax);
    ATH_MSG_DEBUG("rmin=" << m_rbounds.first << " rmax=" << m_rbounds.second << " zmin=" << zmin << " zmax=" << zmax << " etamin=" << etamin << " etamax=" << etamax);
    return StatusCode::SUCCESS;
  }

  uint8_t StripSegmentTool::findRIdx(const float& val) const {
    unsigned int nSlices=(1<<m_rIndexBits); //256
    std::pair<float,float> range;
    switch(m_ridxScheme){
      case 0:
        range=m_rbounds;
        break;
      case 1:
        range=m_etabounds;
        break;
      default:
        break;
    }
    float step=(range.second-range.first)/nSlices;

  // the cases with val<=range.first or val>=range.second have been abandoned before
    for(uint8_t i=0;i<nSlices;i++) {
      if(range.first+i*step <= val && val < range.first+(i+1)*step) return i;
    }
    ATH_MSG_ERROR( "StripSegmentTool: findRIdx failed!");
    return 0;
  }

  uint8_t StripSegmentTool::findDtheta(const float& val) const {
    uint8_t nbins_dtheta=1<<m_dThetaBits;
    float step_dtheta=(m_dtheta_max-m_dtheta_min)/nbins_dtheta;
    for(uint8_t i=0;i<nbins_dtheta;++i) {
      if(val<m_dtheta_min+i*step_dtheta) return i;
    }
    return 0;
  }

  StatusCode StripSegmentTool::find_segments(std::vector< std::unique_ptr<StripClusterData> >& clusters,
                                             const std::unique_ptr<Muon::NSW_TrigRawDataContainer>& trgContainer){
    const auto& ctx = Gaudi::Hive::currentContext();
    int event = ctx.eventID().event_number();
    if (event == 0) ATH_CHECK(FetchDetectorEnvelope());

    if (clusters.empty()) {
      ATH_MSG_WARNING("Received event with no clusters. Skipping...");
      return StatusCode::SUCCESS;
    }

    std::map<int, std::vector<std::unique_ptr<StripClusterData>>[2] > cluster_map; // gather clusters by bandID and seperate in wedge

    int bcid=-1;
    int sectorid=-1;
    int sideid=-1;

    for(auto& cl : clusters){
      bcid=cl->BCID();
      sideid=cl->sideId();
      /* S.I : For the sector logic I guess we need sector Ids from 1 to 16 / per side
         The convention followed by different people throghout this simulation chain (starting from padTDs Tool upto here) is :
         sector Id ranges from 1 to 8 + combined with the info Small or Large.
      */
      sectorid=cl->sectorId();
      if(cl->isSmall()) sectorid*=2;
      else sectorid=sectorid*2-1;

      if(sideid == 0 ) sectorid+=16; //C

      /*****************************************************************************************************/
      auto item =cluster_map.find(cl->bandId());
      if (item != cluster_map.end()){
        item->second[cl->wedge()-1].push_back(std::move(cl));
      }
      else{
        cluster_map[cl->bandId()][cl->wedge()-1].push_back(std::move(cl));
      }
    }

    ATH_MSG_DEBUG(" Building NSW Segment RDO at sector=" << sectorid);
    auto trgRawData=std::make_unique< Muon::NSW_TrigRawData>((uint16_t)(sectorid), (uint16_t)(bcid));

    for(const auto& band : cluster_map){//main band loop
      int bandId=band.first;
      if (band.second[0].size() == 0){
        ATH_MSG_WARNING("Cluster size is zero for inner wedge trg with bandId "<<bandId<<"...skipping");
        continue;
      }
      if(band.second[1].size() == 0){
        ATH_MSG_WARNING("Cluster size is zero for outer wedge trg with bandId "<<bandId<<"...skipping");
        continue;
      }
      float glx1=0;
      float gly1=0;
      float glx2=0;
      float gly2=0;
      float glx=0;
      float gly=0;
      float charge1=0;
      float charge2=0;

      float eta=0;
      float phi=0;
      float theta=0;
      float theta_inf=0;
      float dtheta=0;
      float eta_inf=0;

      //first measuement
      float z1=0;
      for( const auto& cl : band.second[0] ){//inner
        z1+=cl->globZ()*cl->charge();
        glx1+=cl->globX()*cl->charge();
        gly1+=cl->globY()*cl->charge();
        charge1+=cl->charge();
      }

      //first measurement
      //S.I : This is SECOND measurement, not the 1st. Please be careful while copy/pasting
      float z2=0;
      for( const auto& cl : band.second[1] ){//outer
        z2+=cl->globZ()*cl->charge();
        glx2+=cl->globX()*cl->charge();
        gly2+=cl->globY()*cl->charge();
        charge2+=cl->charge();
      }
      if(charge1!=0){
        z1=z1/charge1;
        glx1=glx1/charge1;
        gly1=gly1/charge1;
      }
      if(charge2!=0){
        z2=z2/charge2;
        glx2=glx2/charge2;
        gly2=gly2/charge2;
      }

      // //centroid calc
      glx=(glx1+glx2)/2.;
      gly=(gly1+gly2)/2.;
      float avg_z=(z1+z2)/2.;

      //segment calc
      TVector3 v3_centr1(glx1,gly1,z1);
      TVector3 v3_centr2(glx2,gly2,z2);
      TVector3 v3_segment = v3_centr2 - v3_centr1;
      phi=v3_segment.Phi();
      theta=v3_segment.Theta();
      eta=v3_segment.Eta();

      //inf momentum track 
      theta_inf=v3_centr1.Theta();
      eta_inf=v3_centr1.Eta();
      dtheta=(theta_inf-theta)*1000;//In Milliradian

      ATH_MSG_DEBUG("StripSegmentTool: phi:" << phi << " theta:" << theta << " eta: " << eta << " theta_inf: " << theta_inf << " eta_inf: " << eta_inf << " dtheta: " << dtheta);
      
      //However it needs to be kept an eye on... will be something in between 7 and 15 mrad needs to be decided
      //if(std::abs(dtheta)>15) return StatusCode::SUCCESS;

      //do not get confused. this one is trigger phiId
      int phiId=band.second[0].at(0)->phiId();

      float rfar=m_zbounds.second*std::tan(theta_inf);

      if( rfar >= m_rbounds.second || rfar < m_rbounds.first || eta_inf >= m_etabounds.second || eta_inf < m_etabounds.first){
        ATH_MSG_WARNING("measured r/eta is out of detector envelope!");
        return StatusCode::SUCCESS;
      }

      uint8_t rIndex=0;
      switch(m_ridxScheme) {
        case 0:
          rIndex=findRIdx(rfar);
          break;
        case 1:
          rIndex=findRIdx(std::abs(eta_inf));
          break;
        default:
          break;
      }

      bool phiRes=true;
      bool lowRes=false;//we do not have a recipe  for a singlewedge trigger.  so lowres is always false for now
      uint8_t dtheta_int=findDtheta(dtheta);
      auto rdo_segment= std::make_unique<Muon::NSW_TrigRawDataSegment>( dtheta_int,  (uint8_t)phiId, (rIndex), lowRes,  phiRes);
      trgRawData->push_back(std::move(rdo_segment));

      if (m_doNtuple) {
        m_seg_wedge1_size->push_back(band.second[0].size());
        m_seg_wedge2_size->push_back(band.second[1].size());
        m_seg_bandId->push_back(bandId);
        m_seg_phiId->push_back(phiId);
        m_seg_rIdx->push_back(rIndex);
        m_seg_theta->push_back(theta);
        m_seg_dtheta->push_back(dtheta);
        m_seg_dtheta_int->push_back(dtheta_int);
        m_seg_eta->push_back(eta);
        m_seg_eta_inf->push_back(eta_inf);
        m_seg_phi->push_back(phi);
        m_seg_global_x->push_back(glx);
        m_seg_global_y->push_back(gly);
        m_seg_global_z->push_back(avg_z);

      }
    }//end of clmap loop
    trgContainer->push_back(std::move(trgRawData));
    return StatusCode::SUCCESS;
  }

  StatusCode StripSegmentTool::book_branches() {

    m_seg_theta = new std::vector< float >();
    m_seg_dtheta = new std::vector< float >();
    m_seg_dtheta_int = new std::vector< uint8_t >();
    m_seg_eta = new std::vector< float >();
    m_seg_eta_inf=new std::vector< float >();
    m_seg_phi = new std::vector< float >();
    m_seg_global_x = new std::vector< float >();
    m_seg_global_y = new std::vector< float >();
    m_seg_global_z = new std::vector< float >();
    m_seg_bandId = new std::vector< int >();
    m_seg_phiId = new std::vector< int >();
    m_seg_rIdx=new std::vector< int >();
    m_seg_wedge1_size = new std::vector< int >();
    m_seg_wedge2_size = new std::vector< int >();

    if (m_tree) {
      std::string ToolName = name().substr(  name().find("::")+2,std::string::npos );
      const char* n = ToolName.c_str();
      m_tree->Branch(TString::Format("%s_seg_theta",n).Data(),&m_seg_theta);
      m_tree->Branch(TString::Format("%s_seg_dtheta",n).Data(),&m_seg_dtheta);
      m_tree->Branch(TString::Format("%s_seg_dtheta_int",n).Data(),&m_seg_dtheta_int);
      m_tree->Branch(TString::Format("%s_seg_eta",n).Data(),&m_seg_eta);
      m_tree->Branch(TString::Format("%s_seg_eta_inf",n).Data(),&m_seg_eta_inf);
      m_tree->Branch(TString::Format("%s_seg_phi",n).Data(),&m_seg_phi);
      m_tree->Branch(TString::Format("%s_seg_global_x",n).Data(),&m_seg_global_x);
      m_tree->Branch(TString::Format("%s_seg_global_y",n).Data(),&m_seg_global_y);
      m_tree->Branch(TString::Format("%s_seg_global_z",n).Data(),&m_seg_global_z);
      m_tree->Branch(TString::Format("%s_seg_bandId",n).Data(),&m_seg_bandId);
      m_tree->Branch(TString::Format("%s_seg_phiId",n).Data(),&m_seg_phiId);
      m_tree->Branch(TString::Format("%s_seg_rIdx",n).Data(),&m_seg_rIdx);
      m_tree->Branch(TString::Format("%s_seg_wedge1_size",n).Data(),&m_seg_wedge1_size);
      m_tree->Branch(TString::Format("%s_seg_wedge2_size",n).Data(),&m_seg_wedge2_size);
    }
    return StatusCode::SUCCESS;
  }

  void StripSegmentTool::reset_ntuple_variables() {
    clear_ntuple_variables();
  }

  void StripSegmentTool::clear_ntuple_variables() {
    if(m_tree==nullptr) return;

    m_seg_theta->clear();
    m_seg_dtheta->clear();
    m_seg_dtheta_int->clear();
    m_seg_eta->clear();
    m_seg_eta_inf->clear();
    m_seg_phi->clear();
    m_seg_global_x->clear();
    m_seg_global_y->clear();
    m_seg_global_z->clear();
    m_seg_bandId->clear();
    m_seg_phiId->clear();
    m_seg_rIdx->clear();
    m_seg_wedge2_size->clear();
    m_seg_wedge1_size->clear();
  }
}

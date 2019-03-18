/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Athena/Gaudi includes
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

// local includes
#include "TrigT1NSWSimTools/StripSegmentTool.h"
#include "TrigT1NSWSimTools/StripOfflineData.h"
#include "TrigT1NSWSimTools/tdr_compat_enum.h"

//Event info includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"



// Muon software includes
#include "MuonAGDDDescription/sTGCDetectorHelper.h"
#include "MuonAGDDDescription/sTGCDetectorDescription.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "MuonDigitContainer/sTgcDigit.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"
// random numbers
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"

// local includes
#include "TTree.h"
#include "TVector3.h"

#include <functional>
#include <algorithm>
#include <map>
#include <utility>
#include <math.h>      

namespace NSWL1 {

    StripSegmentTool::StripSegmentTool( const std::string& type, const std::string& name, const IInterface* parent) :
      AthAlgTool(type,name,parent),
      m_incidentSvc("IncidentSvc",name),
      m_rndmSvc("AtRndmGenSvc",name),
      m_rndmEngine(0),
      m_detManager(0),
      m_sTgcIdHelper(0),
      m_tree(0),
      m_lutCreatorToolsTGC ("sTGC_RegionSelectorTable"),//name can be made configurable through JO just in case storegate config. changes in the future
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

      declareProperty("DoNtuple", m_doNtuple = true, "input the StripTds branches into the analysis ntuple"); 
      declareProperty("sTGC_SdoContainerName", m_sTgcSdoContainer = "sTGC_SDO", "the name of the sTGC SDO container");
      declareProperty("rIndexBits", m_rIndexBits = 8,   "number bits in R-index calculation");
      declareProperty("dthetaBits", m_dThetaBits = 5,   "number bits in dTheta calculation");
      declareProperty("dthetaMin", m_dtheta_min = -15,  "minimum allowed value for dtheta in mrad");
      declareProperty("dthetaMax", m_dtheta_max = 15,   "maximum allowed value for dtheta in mrad");
      declareProperty("rIndexScheme", m_ridxScheme = 1, "rIndex slicing scheme/ 0-->R / 1-->eta");
      declareProperty("NSWTrigRDOContainerName", m_trigRdoContainer = "NSWTRGRDO"," Give a name to NSW trigger rdo container");

    }

    StripSegmentTool::~StripSegmentTool() {

    }

//  
  
  StatusCode StripSegmentTool::initialize() {
      ATH_MSG_INFO("initializing " << name() );
      ATH_MSG_INFO(name() << " configuration:");       
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
          ATH_MSG_FATAL("Could not retrieve the analysis ntuple from the THistSvc");
          return sc;
        } 
        else {
          ATH_MSG_INFO("Analysis ntuple succesfully retrieved");
          sc = this->book_branches();
          if (sc.isFailure()) {
            ATH_MSG_ERROR("Cannot book the branches for the analysis ntuple");
          }
        }

      }


      // retrieve the Incident Service
      if( m_incidentSvc.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve the Incident Service");
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_INFO("Incident Service successfully rertieved");
      }
      m_incidentSvc->addListener(this,IncidentType::BeginEvent);

      // retrieve the Random Service
      if( m_rndmSvc.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve the Random Number Service");
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_INFO("Random Number Service successfully retrieved");
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
      
      ATH_CHECK( m_trigRdoContainer.initialize() );
      
      /*
      if(m_regionHandle.retrieve().isFailure()){
            ATH_MSG_FATAL("Error retrieving sTGC_RegionSelectorTable");
            return StatusCode::FAILURE;
      }      
*/    
      ATH_CHECK(m_lutCreatorToolsTGC.retrieve());
      
      
      
      ATH_CHECK( FetchDetectorEnvelope());
      return StatusCode::SUCCESS;
    }

    void StripSegmentTool::handle(const Incident& inc) {
      if( inc.type()==IncidentType::BeginEvent ) {
        this->reset_ntuple_variables();
      }
    }
    
    StatusCode StripSegmentTool::FetchDetectorEnvelope(){//S.I : Sufficient to call this only once. probably inside init()
        const auto  p_IdHelper =m_detManager->stgcIdHelper();
        const auto ModuleContext = p_IdHelper->module_context();
        
        auto regSelector = m_lutCreatorToolsTGC->getLUT();
        //auto regSelector=m_regionHandle->getLUT();
        float rmin=-1.;
        float rmax=-1.;
        float zfar=-1.;
        float znear=-1;
        float etamin=-1;
        float etamax=-1;
        int ctr=0;
          for(const auto& i : p_IdHelper->idVector()){// all modules
            IdentifierHash moduleHashId;            
            p_IdHelper->get_hash( i, moduleHashId, &ModuleContext );
            auto module=regSelector->Module(moduleHashId);
            if(module->zMax()<0) continue;
            if(ctr==0){
                rmin=module->rMin();
                rmax=module->rMax();
                etamin=module->_etaMin();
                etamax=module->_etaMax();
                /*
                 we shouldnt  care whether it's side A/C. However keep in mind that we always return positive nrs here.
                 So you may have to handle it later depending on what you want to calculate
                 */                
                zfar=module->zMax();
                znear=module->zMin();

            }
            ctr++;
            if(etamin>module->_etaMin() ) etamin=module->_etaMin();
            if(etamax<module->_etaMax() ) etamax=module->_etaMax();
            if(zfar<module->zMax()) zfar=module->zMax();
            if(znear>module->zMin()) znear=module->zMin();
            if(rmin>module->rMin()) rmin=module->rMin();
            if(rmax<module->rMax()) rmax=module->rMax();
        }
        
        if(rmin<=0 || rmax<=0){
            ATH_MSG_FATAL("Unable to fetch NSW r/z boundaries");
            return StatusCode::FAILURE;
        }
        m_rbounds= std::make_pair(rmin,rmax);
        m_etabounds=std::make_pair(etamin,etamax);

        m_zbounds=std::make_pair(znear,zfar);
        ATH_MSG_INFO("rmin="<<m_rbounds.first<<" rmax="<<m_rbounds.second<<" zfar="<<zfar<<" znear="<<znear);
        return StatusCode::SUCCESS;
    }
  
  uint8_t StripSegmentTool::findRIdx(const float& val){
     unsigned int nSlices=(1<<m_rIndexBits);
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
    if(val<=range.first) return 0;
    if(val>=range.second) return nSlices-1;
    for(uint8_t i=0;i<nSlices;i++){
            if(range.first+i*step>=val){
                return i;
            }
    }
    return 0;
  }
 
 
  uint8_t StripSegmentTool::findDtheta(const float& val){
     uint8_t nbins_dtheta=1<<m_dThetaBits;
     float step_dtheta=(m_dtheta_max-m_dtheta_min)/nbins_dtheta;
     for(uint8_t i=0;i<nbins_dtheta;i++){
            if(val<m_dtheta_min+i*step_dtheta) return i;
     }
     return 0;
 } 
 
 
    StatusCode StripSegmentTool::find_segments(std::vector< std::unique_ptr<StripClusterData> >& clusters){
      
      /** S.I If you worry(you should) about events with no clusters why dont we add this at the very beginning ? ***/
      if (clusters.size()==0){
          ATH_MSG_WARNING("Received event with no clusters. Skipping...");  
          return StatusCode::SUCCESS;
      }
      /*********************************************************/
       auto trgContainer=std::make_unique<Muon::NSW_TrigRawDataContainer>();//to be posted to the SG
      
      std::map<int, std::vector<std::unique_ptr<StripClusterData>>[2] > cluster_map; // gather clusters by bandID and seperate in wedge

       int bcid=-1;
       int sectorid=-1;
       
      for(  auto& cl : clusters){
          bcid=cl->BCID();
          /*
            S.I : For the sector logic I guess we need sector Ids from 1 to 16 / per side
          The convention followed by different people throghout this simulation chain (starting from padTDs Tool upto here) is :
          sector Id ranges from 1 to 8 + combined with the info Small or Large.
          */
          sectorid=cl->sectorId();  
	      if(cl->isSmall()) sectorid*=2;
          else sectorid=sectorid*2-1;
          /*****************************************************************************************************/
          auto item =cluster_map.find(cl->bandId());
	      if (item != cluster_map.end()){
	        item->second[cl->wedge()-1].push_back(std::move(cl));
	      }
	      else{
	        cluster_map[cl->bandId()][cl->wedge()-1].push_back(std::move(cl));
	      }
      }
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
	     float phi=0;
	     float eta_inf=0;
         float eta=0;
	     float charge1=0;
         float charge2=0;

	     //first measuement
	     float r1=0;
	     float z1=0;
	     for( const auto& cl : band.second[0] ){//inner
	       r1+=sqrt(pow(cl->globX()*cl->charge(),2)+pow(cl->globY()*cl->charge(),2));
	       z1+=cl->globZ()*cl->charge();
	       glx1+=cl->globX()*cl->charge();
	       gly1+=cl->globY()*cl->charge();
	       charge1+=cl->charge();
	     }

	     //first measurement 
	     //S.I : This is SECOND measurement, not the 1st. Please be careful while copy/pasting
	     float r2=0;
	     float z2=0;
	     for( const auto& cl : band.second[1] ){//outer
             r2+=sqrt(pow(cl->globX()*cl->charge(),2)+pow(cl->globY()*cl->charge(),2));
             z2+=cl->globZ()*cl->charge();
             glx2+=cl->globX()*cl->charge();
             gly2+=cl->globY()*cl->charge();
             charge2+=cl->charge();
         }
         if(charge1!=0){
             r1=r1/charge1;
             z1=z1/charge1;
             glx1=glx1/charge1;
             gly1=gly1/charge1;
         }
         if(charge2!=0){
            r2=r2/charge2;
            z2=z2/charge2;
            glx2=glx2/charge2;
            gly2=gly2/charge2;
         }
         glx=(glx1+glx2)/2.;
         gly=(gly1+gly2)/2.;
         float slope=(r2-r1)/(z2-z1);
         float avg_r=(r1+r2)/2.;
         float avg_z=(z1+z2)/2.;
         float inf_slope=(avg_r/avg_z);
         //float dR=slope-inf_slope;
         float theta_inf=atan(inf_slope);
         float theta=atan(slope);
         float dtheta=(theta_inf-theta)*1000;//In Milliradian
         if(avg_z>0){
            eta_inf=-log(tan(theta_inf/2));
            eta=-log(tan(theta/2));
         }
         else if(avg_z<0){
            eta_inf=log(tan(-theta_inf/2));
            eta=log(tan(theta/2));
         }
         else{
            ATH_MSG_ERROR("Segment Global Z at IP");//S.I : Error so ?
         }
         
         
         //S.I instead of doing all these stuff like below, which is quite error prone  why dont we use TVectors?
         if(glx>=0 && gly>=0){
             phi=atan(gly/glx);
         }
         
         else if(glx<0 && gly>0){
            phi=PI-atan(abs(gly/glx));
         }
         else if(glx<0 && gly<0){
            phi=-1*PI+atan(gly/glx);
         }
         else if(glx>0 && gly<0){
            phi=-atan(abs(gly/glx));
         }
         else{
            ATH_MSG_ERROR("Unexpected error, global x or global y are not a number");//S.I does this even necessary ? then what ?
         }
        
        //However it needs to be kept an eye on... will be something in between 7 and 15 mrad needs to be decided 
        //if(fabs(dtheta)>15) return StatusCode::SUCCESS; 
        
        //do not get confused. this one is trigger phiId 
        int phiId=band.second[0].at(0)->phiId();
        float delta_z=fabs(m_zbounds.second-avg_z);
        float delta_r=delta_z*tan(theta_inf);
        float rfar=avg_r+delta_r;

        if(r1>m_rbounds.second || r2>m_rbounds.second ){
            ATH_MSG_WARNING("measured r is out of detector envelope! r1="<<r1<<" r2="<<r2<<" rmax="<<m_rbounds.second);
        }
        
        
        uint8_t rIndex=0;
        switch(m_ridxScheme){
            case 0:
                rIndex=findRIdx(rfar);
                break;
            case 1:
                rIndex=findRIdx(eta_inf);
                break;
            default:
                break;   
        }
        
        
        bool phiRes=true;
        bool lowRes=false;//we do not have a recipe  for a singlewedge trigger.  so lowres is always false for now
        uint8_t dtheta_int=findDtheta(dtheta);
        auto rdo_segment= std::make_unique<Muon::NSW_TrigRawDataSegment>( dtheta_int,  (uint8_t)phiId, (rIndex), lowRes,  phiRes);      
        trgRawData->push_back(std::move(rdo_segment));
        
        //Fill ntuple info
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
        m_seg_global_r->push_back(avg_r); 
        m_seg_global_x->push_back(glx);
        m_seg_global_y->push_back(gly); 
        m_seg_global_z->push_back(avg_z); 
        m_seg_dir_r->push_back(slope); 
        m_seg_dir_y->push_back(-99); 
        m_seg_dir_z->push_back(-99);        
     
      }//end of clmap loop
       
    
      //save rdo to the SG 
      trgContainer->push_back(std::move(trgRawData));
      auto rdohandle = SG::makeHandle( m_trigRdoContainer );
      ATH_CHECK( rdohandle.record( std::move(trgContainer)));

      return StatusCode::SUCCESS;
    }

    StatusCode StripSegmentTool::book_branches() {
      // m_cl_n= 0;
      // m_cl_charge = new std::vector< int >();

      m_seg_theta = new std::vector< float >();    
      m_seg_dtheta = new std::vector< float >();
      m_seg_dtheta_int = new std::vector< uint8_t >();
      m_seg_eta = new std::vector< float >();  
      m_seg_eta_inf=new std::vector< float >();
      m_seg_phi = new std::vector< float >();
      m_seg_global_r = new std::vector< float >();
      m_seg_global_x = new std::vector< float >();
      m_seg_global_y = new std::vector< float >();
      m_seg_global_z = new std::vector< float >();
      m_seg_dir_r = new std::vector< float >();
      m_seg_dir_y = new std::vector< float >();
      m_seg_dir_z = new std::vector< float >();
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

         m_tree->Branch(TString::Format("%s_seg_global_r",n).Data(),&m_seg_global_r);
         m_tree->Branch(TString::Format("%s_seg_global_x",n).Data(),&m_seg_global_x);
         m_tree->Branch(TString::Format("%s_seg_global_y",n).Data(),&m_seg_global_y);
         m_tree->Branch(TString::Format("%s_seg_global_z",n).Data(),&m_seg_global_z);


         m_tree->Branch(TString::Format("%s_seg_dir_r",n).Data(),&m_seg_dir_r);
         m_tree->Branch(TString::Format("%s_seg_dir_y",n).Data(),&m_seg_dir_y);
         m_tree->Branch(TString::Format("%s_seg_dir_z",n).Data(),&m_seg_dir_z);
         m_tree->Branch(TString::Format("%s_seg_bandId",n).Data(),&m_seg_bandId);
         m_tree->Branch(TString::Format("%s_seg_phiId",n).Data(),&m_seg_phiId);
         m_tree->Branch(TString::Format("%s_seg_rIdx",n).Data(),&m_seg_rIdx);
         m_tree->Branch(TString::Format("%s_seg_wedge1_size",n).Data(),&m_seg_wedge1_size);
         m_tree->Branch(TString::Format("%s_seg_wedge2_size",n).Data(),&m_seg_wedge2_size);
       }
      // else { 
      //    return StatusCode::FAILURE;
      // }
      return StatusCode::SUCCESS;
    }



    void StripSegmentTool::reset_ntuple_variables() {
      // if ntuple is not booked nothing to do
      if ( m_tree==0 ) return;
      //reset the ntuple variables
      clear_ntuple_variables();
    }

    void StripSegmentTool::clear_ntuple_variables() {
      m_seg_theta->clear();    
      m_seg_dtheta->clear();
      m_seg_dtheta_int->clear();
      m_seg_eta->clear();
      m_seg_eta_inf->clear();
      m_seg_phi->clear();
      m_seg_global_r->clear();
      m_seg_global_x->clear();
      m_seg_global_y->clear();
      m_seg_global_z->clear();
      m_seg_dir_r->clear();
      m_seg_dir_y->clear();
      m_seg_dir_z->clear();
      m_seg_bandId->clear();
      m_seg_phiId->clear();
      m_seg_rIdx->clear();
      m_seg_wedge2_size->clear();
      m_seg_wedge1_size->clear();
  }


 




}

//  LocalWords:  pos lpos

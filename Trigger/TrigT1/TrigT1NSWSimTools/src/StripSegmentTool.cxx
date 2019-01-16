/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Athena/Gaudi includes
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

// local includes
#include "TrigT1NSWSimTools/StripSegmentTool.h"
#include "TrigT1NSWSimTools/StripOfflineData.h"
#include "TrigT1NSWSimTools/tdr_compat_enum.h"

#include "TTree.h"

#include <map>

namespace NSWL1 {

    StripSegmentTool::StripSegmentTool( const std::string& type, const std::string& name, const IInterface* parent) :
      AthAlgTool(type,name,parent),
      m_incidentSvc("IncidentSvc",name),
      m_tree(0)

    {
      declareInterface<NSWL1::IStripSegmentTool>(this);

      declareProperty("DoNtuple", m_doNtuple = true, "input the StripTds branches into the analysis ntuple"); 
    }

    StripSegmentTool::~StripSegmentTool() {

    }

 
  
  StatusCode StripSegmentTool::initialize() {
    ATH_MSG_INFO( "initializing " << name() ); 
    
    ATH_MSG_INFO( name() << " configuration:"); 

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
        sprintf(ntuple_name,"%sTree",algo_name.c_str());

        m_tree = 0;
        sc = tHistSvc->getTree(ntuple_name,m_tree);

        if (sc.isFailure()) {
          ATH_MSG_FATAL("Could not retrieve the analysis ntuple from the THistSvc");
          return sc;
        } else {
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

      return StatusCode::SUCCESS;
    }

    void StripSegmentTool::handle(const Incident& inc) {
      if( inc.type()==IncidentType::BeginEvent ) {
        this->reset_ntuple_variables();
      }
    }

  
    StatusCode StripSegmentTool::find_segments(std::vector< std::unique_ptr<StripClusterData> >& clusters){
      
      

      std::map<int, std::vector<std::unique_ptr<StripClusterData>>[2] > cluster_map; // gather clusters by bandID and seperate in wedge


      for(  auto& cl : clusters){
	    auto item =cluster_map.find(cl->bandId());
	    if (item != cluster_map.end()){
	      item->second[cl->wedge()-1].push_back(std::move(cl));
	    }
	    else{
	      cluster_map[cl->bandId()][cl->wedge()-1].push_back(std::move(cl));
	    }
      }
      
      for(const auto& band : cluster_map){
	    int bandId=band.first;
	    if ((band.second[0].size() == 0) || (band.second[1].size() == 0)) continue;
  
	    float glx1=0;
	    float gly1=0;
        float glx2=0;
        float gly2=0;
        float glx=0;
        float gly=0;
	    float phi=0;
	    float eta=0;
	    float charge1=0;
        float charge2=0;

	    //first measuement
	    float r1=0;
	    float z1=0;
	    for( const auto& cl : band.second[0] ){
	      r1+=sqrt(pow(cl->globX()*cl->charge(),2)+pow(cl->globY()*cl->charge(),2));
	      z1+=cl->globZ()*cl->charge();
	      glx1+=cl->globX()*cl->charge();
	      gly1+=cl->globY()*cl->charge();
	      charge1+=cl->charge();
	    }

	    //first measuement
	    float r2=0;
	    float z2=0;
	    for( const auto& cl : band.second[1] ){
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
	    float theta_inf=atan(inf_slope);
	    float theta=atan(slope);
	    float dtheta=(theta_inf-theta)*1000;//In Milliradian
	    if(avg_z>0){
	      eta=-log(tan(theta/2));
	    }
	    else if(avg_z<0){
	      eta=log(tan(-theta/2));
	   }
	   else{
	      ATH_MSG_ERROR("Segment Global Z at IP");
       }
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
	  ATH_MSG_ERROR("Unexpected error, global x or global y are not a number");
	  }

	 m_seg_wedge1_size->push_back(band.second[0].size());
	 m_seg_wedge2_size->push_back(band.second[1].size());

	 m_seg_bandId->push_back(bandId);
	 m_seg_theta->push_back(theta);
	 m_seg_dtheta->push_back(dtheta);
	 m_seg_eta->push_back(eta);
	 m_seg_phi->push_back(phi);
	 m_seg_global_r->push_back(avg_r); 
     m_seg_global_x->push_back(glx);
	 m_seg_global_y->push_back(gly); 
	 m_seg_global_z->push_back(avg_z); 
     m_seg_dir_r->push_back(slope); 
	 m_seg_dir_y->push_back(-99); 
	 m_seg_dir_z->push_back(-99); 
     }



      return StatusCode::SUCCESS;
    }

    StatusCode StripSegmentTool::book_branches() {

      m_seg_theta = new std::vector< float >();    
      m_seg_dtheta = new std::vector< float >();
      m_seg_eta = new std::vector< float >();   
      m_seg_phi = new std::vector< float >();
      m_seg_global_r = new std::vector< float >();
      m_seg_global_x = new std::vector< float >();
      m_seg_global_y = new std::vector< float >();
      m_seg_global_z = new std::vector< float >();
      m_seg_dir_r = new std::vector< float >();
      m_seg_dir_y = new std::vector< float >();
      m_seg_dir_z = new std::vector< float >();
      m_seg_bandId = new std::vector< int >();
      m_seg_wedge1_size = new std::vector< int >();
      m_seg_wedge2_size = new std::vector< int >();

       if (m_tree) {
	     std::string ToolName = name().substr(  name().find("::")+2,std::string::npos );
         const char* n = ToolName.c_str();
         m_tree->Branch(TString::Format("%s_seg_theta",n).Data(),&m_seg_theta);
         m_tree->Branch(TString::Format("%s_seg_dtheta",n).Data(),&m_seg_dtheta);

         m_tree->Branch(TString::Format("%s_seg_eta",n).Data(),&m_seg_eta);
         m_tree->Branch(TString::Format("%s_seg_phi",n).Data(),&m_seg_phi);

         m_tree->Branch(TString::Format("%s_seg_global_r",n).Data(),&m_seg_global_r);
         m_tree->Branch(TString::Format("%s_seg_global_x",n).Data(),&m_seg_global_x);
         m_tree->Branch(TString::Format("%s_seg_global_y",n).Data(),&m_seg_global_y);
         m_tree->Branch(TString::Format("%s_seg_global_z",n).Data(),&m_seg_global_z);


         m_tree->Branch(TString::Format("%s_seg_dir_r",n).Data(),&m_seg_dir_r);
         m_tree->Branch(TString::Format("%s_seg_dir_y",n).Data(),&m_seg_dir_y);
         m_tree->Branch(TString::Format("%s_seg_dir_z",n).Data(),&m_seg_dir_z);
         m_tree->Branch(TString::Format("%s_seg_bandId",n).Data(),&m_seg_bandId);
         m_tree->Branch(TString::Format("%s_seg_wedge1_size",n).Data(),&m_seg_wedge1_size);
         m_tree->Branch(TString::Format("%s_seg_wedge2_size",n).Data(),&m_seg_wedge2_size);


       }
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
      m_seg_eta->clear();   
      m_seg_phi->clear();
      m_seg_global_r->clear();
      m_seg_global_x->clear();
      m_seg_global_y->clear();
      m_seg_global_z->clear();
      m_seg_dir_r->clear();
      m_seg_dir_y->clear();
      m_seg_dir_z->clear();
      m_seg_bandId->clear();
      m_seg_wedge2_size->clear();
      m_seg_wedge1_size->clear();
  }


 




}

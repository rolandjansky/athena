/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Athena/Gaudi includes
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

// local includes
#include "TrigT1NSWSimTools/StripClusterTool.h"
#include "TrigT1NSWSimTools/StripOfflineData.h"

//Event info includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// Muon software includes
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



namespace NSWL1 {

    StripClusterTool::StripClusterTool( const std::string& type, const std::string& name, const IInterface* parent) :
      AthAlgTool(type,name,parent),
      m_incidentSvc("IncidentSvc",name),
      m_rndmSvc("AtRndmGenSvc",name),
      m_rndmEngine(0),
      m_detManager(0),
      m_sTgcIdHelper(0),
      m_tree(0),
      m_clusters()
    {
      declareInterface<NSWL1::IStripClusterTool>(this);
      declareProperty("DoNtuple", m_doNtuple = true, "input the StripTds branches into the analysis ntuple");
      declareProperty("sTGC_SdoContainerName", m_sTgcSdoContainer = "sTGC_SDO", "the name of the sTGC SDO container");
    }

    StripClusterTool::~StripClusterTool() {

      // Clear Ntuple variables
      //      if(m_cl_charge) delete m_cl_charge;
      //if(m_cl_eta) delete m_cl_eta;
      //if(m_cl_phi) delete m_cl_phi;


    }



  StatusCode StripClusterTool::initialize() {
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

        char ntuple_name[40];
        memset(ntuple_name,'\0',40*sizeof(char));
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

      return StatusCode::SUCCESS;
    }

    void StripClusterTool::handle(const Incident& inc) {
      if( inc.type()==IncidentType::BeginEvent ) {
        this->reset_ntuple_variables();
      }
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
      else {
         return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }


    void StripClusterTool::reset_ntuple_variables() {
      // if ntuple is not booked nothing to do
      if ( m_tree==0 ) return;
      //reset the ntuple variables
      clear_ntuple_variables();
    }

    void StripClusterTool::clear_ntuple_variables() {
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
      /*
      for( auto cl: m_clusters){
	    delete (cl);
      }
      */
      m_clusters.clear();
    }
void StripClusterTool::fill_strip_validation_id(std::vector<std::unique_ptr<StripClusterData>>& clusters) {

    ATH_MSG_DEBUG("M_Clusters recieved " << m_clusters.size());


    bool first_strip=true;
    const MuonSimDataCollection* sdo_container = 0;
    StatusCode sc = evtStore()->retrieve( sdo_container, m_sTgcSdoContainer.value().c_str() );
    if ( !sc.isSuccess() ) {
      ATH_MSG_WARNING("could not retrieve the sTGC SDO container: it will not be possible to associate the MC truth");
    }
    int n_clus=0;
    for(unsigned int cl_i=0; cl_i < m_clusters.size();cl_i++){
      ATH_MSG_DEBUG(" Start cl " << cl_i  << " OF " << m_clusters.size());
      float x_pos=0;
      float y_pos=0;
      float z_pos=0;

      float x_lpos=0;
      float y_lpos=0;
      float z_lpos=0;


      int charge=0;
      int n_strip=0;
      ATH_MSG_DEBUG(" Start cl " << cl_i  << " OF " << m_clusters.size());

      //std::vector< std::unique_ptr<StripData> >* this_cl=m_clusters.at(cl_i);
      auto this_cl=m_clusters.at(cl_i);
      ATH_MSG_DEBUG(" Start cl " << cl_i  << " OF " << m_clusters.size());

      first_strip=true;
      float locx=-999999;
      float locy=-999999;
      if (this_cl->size()==0){
        ATH_MSG_DEBUG("Zero size cluster!!");
  	    continue;
      }

      for(unsigned int s_i=0; s_i < this_cl->size();s_i++){
     	n_strip++;
  	    ATH_MSG_DEBUG("Start strip" << n_strip);
	    if(sdo_container && first_strip){ // Save truth deposits associated with cluster should be the same on for the whole strip, so take the first one need to work on this logic
        first_strip=false;
  	    Identifier Id = this_cl->at(s_i)->Identity();
	    const MuonGM::sTgcReadoutElement* rdoEl = m_detManager->getsTgcReadoutElement(Id);
	    const MuonSimData strip_sdo = (sdo_container->find(Id))->second;
	    std::vector<MuonSimData::Deposit> deposits;
	    strip_sdo.deposits(deposits);
	    //retrieve the info of the first associated hit, i.e. the fastest in time
      	if (deposits.size()!=1)ATH_MSG_WARNING("Multiple cluster hits for strip!");
        if (deposits.size()==0){
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

	    float  truth_energy    = strip_sdo.word();

	    //Amg::Vector2D  lpos;
        //Amg::Vector3D hit_gpos(truth_globalPosX, truth_globalPosY,truth_globalPosZ);
        //Amg::Vector3D orig(0,0,0);
	   // This orig arugment dosen't appear to do anything
	   //rdoEl->surface(Id).globalToLocal(hit_gpos, orig, lpos);

        if(fabs(locx-lpos.x())>.001 || fabs(locy - lpos.y())>.001){
      	    ATH_MSG_DEBUG("OLD locx " << locx <<" new locx "<<lpos.x() <<" b " << int(locx!=lpos.x()));
      	    ATH_MSG_DEBUG("OLD locy " << locy <<" new locy "<<lpos.y() << " b " << int(locy!=lpos.y()));
      	    ATH_MSG_DEBUG("Cluster hit, truth barcode=" << truth_barcode);
      	    ATH_MSG_DEBUG("Cluster hit, truth globalPosX="   << truth_globalPosX
      			  << ", truth globalPosY="   << truth_globalPosY
      			  << ", truth globalPosZ="   << truth_globalPosZ
      			  << ", truth enegy deposit ="   << truth_energy << std::endl);
      	    ATH_MSG_DEBUG("Cluster hit, truth localPosX="   << lpos.x()
      			  << ", truth localPosY="   <<  lpos.y()
      			  << ", truth enegy deposit ="   << truth_energy << std::endl);

	        m_cl_truth_x->push_back( hit_gpos.x() );
      	    m_cl_truth_y->push_back( hit_gpos.y() );
      	    m_cl_truth_z->push_back( hit_gpos.z() );

      	    m_cl_truth_lx->push_back( lpos.x() );
      	    m_cl_truth_ly->push_back( lpos.y() );
      	    m_cl_truth_lz->push_back( 0 );

      	    m_cl_truth_E->push_back( truth_energy );
	        //m_cl_truth_n->push_back(n_clus  );

      	    ///	    locx=lpos.x();
      	    //	    locy=lpos.y();
      	  }
	    }

        //ATH_MSG_DEBUG("Cluster " << this_cl << " strip: " << this_cl->at(s_i));
	    //ATH_MSG_DEBUG("Cluster " << this_cl << " strip: " << this_cl->at(s_i) <<" charge: " << this_cl->at(s_i)->strip_charge_6bit());
        float s_charge=this_cl->at(s_i)->strip_charge_6bit();
        charge+=s_charge;
        x_pos+=this_cl->at(s_i)->globX()*s_charge;
        y_pos+=this_cl->at(s_i)->globY()*s_charge;
        z_pos+=this_cl->at(s_i)->globZ()*s_charge;

        x_lpos+=(this_cl->at(s_i)->locX())*s_charge;
        y_lpos+=(this_cl->at(s_i)->locY())*s_charge;
        z_lpos+=(this_cl->at(s_i)->locZ()) *s_charge;


        ATH_MSG_DEBUG("Cluster ------------------------------------------" );
        ATH_MSG_DEBUG("Cluster " << cl_i << " strip charge: " << s_charge);
        ATH_MSG_DEBUG("Cluster " << cl_i << " strip loc X: " << this_cl->at(s_i)->locX());
        ATH_MSG_DEBUG("Cluster " << cl_i << " strip loc Y: " <<  this_cl->at(s_i)->locY());
        ATH_MSG_DEBUG("Cluster " << cl_i << " strip glob X: " << this_cl->at(s_i)->globX());
        ATH_MSG_DEBUG("Cluster " << cl_i << " strip glob Y: " << this_cl->at(s_i)->globY());
        ATH_MSG_DEBUG("Cluster " << cl_i << " strip glob Z: " << this_cl->at(s_i)->globZ());
        ATH_MSG_DEBUG("Cluster " << cl_i << " strip locx dist: " << locx-this_cl->at(s_i)->locX());
        ATH_MSG_DEBUG("Cluster " << cl_i << " strip charge o dist: " << s_charge/(locx-this_cl->at(s_i)->locX()));
        ATH_MSG_DEBUG("Channel " << s_i << this_cl->at(s_i)->channelId());

      }//end of this_cl loop
      if (charge != 0){
      	x_pos=x_pos/charge;
      	y_pos=y_pos/charge;
      	z_pos=z_pos/charge;
      	x_lpos=x_lpos/charge;
        y_lpos=y_lpos/charge;
        z_lpos=z_lpos/charge;
      }
      n_clus++;
      m_cl_x->push_back(x_pos);
      m_cl_y->push_back(y_pos);
      m_cl_z->push_back(z_pos);

      m_cl_lx->push_back(x_lpos);
      m_cl_ly->push_back(y_lpos);
      m_cl_lz->push_back(z_lpos);


      // Identifier Id = this_cl->at(0)->Identity();
      //Trk::LocalPosition cl_lo(x_pos,y_pos);
    // const MuonGM::sTgcReadoutElement* rdoEl = m_detManager->getsTgcReadoutElement(Id);
    //const  Trk::GlobalPosition* hit_gpos = rdoEl->surface(Id).localToGlobal(cl_lo);
    //m_cl_ltgx->push_back(hit_gpos->x());
    //m_cl_ltgy->push_back(hit_gpos->y());
    //m_cl_ltgz->push_back(hit_gpos->z());




     m_cl_charge->push_back(charge);
     m_cl_size->push_back(n_strip);

     m_cl_side->push_back(m_clusters.at(cl_i)->at(0)->sideId() );
     m_cl_isSmall->push_back(m_clusters.at(cl_i)->at(0)->isSmall() );
     m_cl_wedge->push_back(m_clusters.at(cl_i)->at(0)->wedge());
     m_cl_sector->push_back(m_clusters.at(cl_i)->at(0)->sectorId());
     m_cl_module->push_back(m_clusters.at(cl_i)->at(0)->moduleId() );
     m_cl_layer->push_back(m_clusters.at(cl_i)->at(0)->layer());
     m_cl_bandId->push_back(m_clusters.at(cl_i)->at(0)->bandId());
     m_cl_phiId->push_back(m_clusters.at(cl_i)->at(0)->phiId());
     ATH_MSG_DEBUG("Cluster dump with X:" << x_pos << " Y: " << y_pos << " Z: " << z_pos << " cluster charge: " << charge);
     ATH_MSG_DEBUG("Cluster dump with lX:" << x_lpos << " lY: " << y_lpos << " lZ: " << z_lpos << " cluster charge: " << charge);

     //S.I
     /*
     auto scod=new NSWL1::StripClusterOfflineData(m_clusters.at(cl_i)->at(0)->bandId(),
						 m_clusters.at(cl_i)->at(0)->isSmall(),
						 m_clusters.at(cl_i)->at(0)->moduleId(),
						 m_clusters.at(cl_i)->at(0)->sectorId(),
						 m_clusters.at(cl_i)->at(0)->wedge(),
						 m_clusters.at(cl_i)->at(0)->layer(),
						 n_strip,
						 charge,
						 x_pos,
						 y_pos,
						 z_pos);
     */

     auto stripClOfflData=std::make_unique<StripClusterOfflineData>(
                           m_clusters.at(cl_i)->at(0)->bandId(),
                           m_clusters.at(cl_i)->at(0)->trig_BCID(),                                         
                           m_clusters.at(cl_i)->at(0)->phiId(),
						   m_clusters.at(cl_i)->at(0)->isSmall(),
						   m_clusters.at(cl_i)->at(0)->moduleId(),
						   m_clusters.at(cl_i)->at(0)->sectorId(),
						   m_clusters.at(cl_i)->at(0)->wedge(),
						   m_clusters.at(cl_i)->at(0)->layer(),
						   n_strip,
						   charge,
						   x_pos,
						   y_pos,
						   z_pos
                        );
     clusters.push_back(std::move(stripClOfflData));
     //S.I


    }//of m_clusters loop

    ATH_MSG_DEBUG("Finished Fill");
  }


  StatusCode StripClusterTool::cluster_strip_data( std::vector<std::unique_ptr<StripData>>& strips, std::vector< std::unique_ptr<StripClusterData> >& clusters){
      
      if(strips.size()==0){
            ATH_MSG_WARNING("Received 0 strip hits... Skip event");
            return StatusCode::SUCCESS;
      }
      
      //S.I sort strip w.r.t channelId in ascending order
      std::sort(strips.begin(),strips.end(),
                [](const auto& s1,const auto& s2){return s1->channelId()<s2->channelId();}
      );
            
      /*S.I remove duplicate strip channels ...
       The rightmost way is to define an == operator for stripdata// not sure if we need to include charge+time and such though
       Im sure somehow we get duplicated strip channels. Need to check if all the other aspects like charge, time, etc are exactly the same for those ...
      */
      auto pos = std::unique(strips.begin(), strips.end(),
                             [](const auto& s1, const auto& s2){
                                 return s1->channelId()==s2->channelId() && s1->channelId()==s2->channelId();
                             }
      );
      
      strips.resize(std::distance(strips.begin(), pos)); 
      auto hit=strips.begin();
      
      auto cr_cluster=std::make_shared< std::vector<std::unique_ptr<StripData>> >();
      
      StripData* prev_hit=nullptr;
      int first_ch=(*hit)->channelId();//channel id of the first strip
      ATH_MSG_DEBUG("Cluster Hits :" << (*hit)->channelId() << " " << m_sTgcIdHelper->gasGap( (*hit)->Identity())
		    << "   " <<   (*hit)->moduleId() << "   " << (*hit)->sectorId() << "   " <<(*hit)->wedge()
		    << "  "<< (*hit)->sideId()  );
      hit++;//S.I is this ncessary ?

      for(auto & this_hit : strips){
	      if(!(this_hit)->readStrip() )continue;
          if( ((this_hit)->bandId()==-1 || this_hit->phiId()==-1) ){
	       ATH_MSG_WARNING("Read Strip without BandId :" << (this_hit)->channelId() << " " << m_sTgcIdHelper->gasGap( (this_hit)->Identity())
		      << "   " <<   (this_hit)->moduleId() << "   " << (this_hit)->sectorId() << "   " <<(this_hit)->wedge()
		      << "  "<< (this_hit)->sideId()   );
	       continue;
          }

	     bool sameMod=false;
	     if (!prev_hit){//for the first time...
           prev_hit=this_hit.get();
           cr_cluster->push_back(std::move(this_hit));
	       continue;
	     }
         else{ //executed when we are in the second strip and so on...
             static auto MatchHits=[](const auto& a1,const auto& a2){
                if( a1->sideId() != a2->sideId()  ||
                    a1->sectorId() !=a2->sectorId() ||
                    a1->isSmall() !=a2->isSmall() ||
                    a1->moduleId() !=a2->moduleId() || 
                    a1->wedge() !=a2->wedge()  || 
                    a1->layer() !=a2->layer()
                ) return false;
                
                return true;
             };     
             sameMod=MatchHits(this_hit,prev_hit);
         }
         prev_hit=this_hit.get();
         int this_chid=(this_hit)->channelId();
         if ( (this_hit->channelId()<first_ch) && sameMod ) {
             ATH_MSG_ERROR("Hits Ordered incorrectly!!!" );
             return StatusCode::FAILURE;
         }
	     if ((this_hit)->channelId()==first_ch && sameMod){
	       ATH_MSG_FATAL("Hits entered twice !!! ChannelId: " << (this_hit)->channelId() );
           return StatusCode::FAILURE;
         }
         
	     if ((this_hit)->channelId()==first_ch+1 && sameMod){
	       cr_cluster->push_back(std::move(this_hit));
	     }

	     else{
	      m_clusters.push_back(std::move(cr_cluster));//put the current cluster into the clusters buffer
	      ATH_MSG_DEBUG("Adding Cluster with " << cr_cluster->size() << "hits" << m_clusters.size() << " m_clusters so far");
          cr_cluster=std::make_shared<std::vector<std::unique_ptr<StripData>>>();//create a new empty cluster and assign this hit as the first hit
          cr_cluster->push_back(std::move(this_hit));
	     }
	     first_ch=this_chid;
      }
      if(cr_cluster->size() != 0)m_clusters.push_back(std::move(cr_cluster));//don't forget the last cluster in the loop
      // No sector implemented yet!!!
      ATH_MSG_DEBUG("Found :" << m_clusters.size() << " M_Clusters ");
      fill_strip_validation_id(clusters);
      return StatusCode::SUCCESS;
  }




}

//  LocalWords:  pos lpos

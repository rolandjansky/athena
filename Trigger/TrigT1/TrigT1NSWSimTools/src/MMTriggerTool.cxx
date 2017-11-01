/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Athena/Gaudi includes
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"


// local includes
#include "MMTriggerTool.h"

//Event info includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// Muon software includes

// random numbers
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"

// local includes
#include "TTree.h"
#include "TVector3.h"


namespace NSWL1 {

    MMTriggerTool::MMTriggerTool( const std::string& type, const std::string& name, const IInterface* parent) :
      AthAlgTool(type,name,parent),
      m_incidentSvc("IncidentSvc",name),
      m_detManager(0),
      m_MmIdHelper(0),
      m_MmDigitContainer(""),
      m_MmSdoContainer(""),
      m_doNtuple(false),
      m_tree(0)
    {
      declareInterface<NSWL1::IMMTriggerTool>(this);
      declareProperty("MM_DigitContainerName", m_MmDigitContainer = "MM_DIGITS", "the name of the MM digit container");
      declareProperty("MM_SdoContainerName"  , m_MmSdoContainer = "MM_SDO", "the name of the MM SDO container");
      declareProperty("MM_HitContainerName"  , m_MmHitContainer = "MicromegasSensitiveDetector", "the name of the MM hits container");
      declareProperty("DoNtuple", m_doNtuple = true, "input the MMStripTds branches into the analysis ntuple");
      declareProperty("Truth_ContainerName", m_Truth_ContainerName="TruthEvent","name of truth container");
      declareProperty("MuonEntryLayer_ContainerName", m_MuEntry_ContainerName="MuonEntryLayer", "name of muon entry container");

    }

    MMTriggerTool::~MMTriggerTool() {

    }

    StatusCode MMTriggerTool::initialize() {


      ATH_MSG_INFO( "initializing -- " << name() );

      ATH_MSG_INFO( name() << " configuration:");
      //ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_rndmEngineName.name() << m_rndmEngineName.value());
      ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_MmDigitContainer.name() << m_MmDigitContainer.value());
      ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_MmSdoContainer.name() << m_MmSdoContainer.value());
      ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_doNtuple.name() << ((m_doNtuple)? "[True]":"[False]")
                       << setfill(' ') << setiosflags(ios::right) );


      const IInterface* parent = this->parent();
      const INamedInterface* pnamed = dynamic_cast<const INamedInterface*>(parent);
      std::string algo_name = pnamed->name();
      if ( m_doNtuple && algo_name=="NSWL1Simulation" ) {
        ITHistSvc* tHistSvc;
        ATH_CHECK( service("THistSvc", tHistSvc) );

        char ntuple_name[40];
        memset(ntuple_name,'\0',40*sizeof(char));
        sprintf(ntuple_name,"%sTree",algo_name.c_str());

        m_tree = 0;
        ATH_CHECK( tHistSvc->getTree(ntuple_name,m_tree) );
        ATH_MSG_INFO("Analysis ntuple succesfully retrieved");
        ATH_CHECK( this->book_branches() );

      } else this->clear_ntuple_variables();

      // retrieve the Incident Service
      if( m_incidentSvc.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve the Incident Service");
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_INFO("Incident Service successfully rertieved");
      }
      m_incidentSvc->addListener(this,IncidentType::BeginEvent);

      //  retrieve the MuonDetectormanager
      if( detStore()->retrieve( m_detManager ).isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve the MuonDetectorManager");
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_INFO("MuonDetectorManager successfully retrieved");
      }

      //  retrieve the Mm offline Id helper
      if( detStore()->retrieve( m_MmIdHelper ).isFailure() ){
        ATH_MSG_FATAL("Failed to retrieve MmIdHelper");
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_INFO("MmIdHelper successfully retrieved");
      }

      //Calculate and retrieve wedge geometry, defined in MMT_struct
      par_par pars=dlm;
      m_par_large = new MMT_Parameters(dlm,'L', m_detManager); // Need to figure out where to delete this!! It's needed once per run
      m_par_small = new MMT_Parameters(dlm,'S', m_detManager); // Need to figure out where to delete this!! It's needed once per run

      return StatusCode::SUCCESS;
    }


    StatusCode MMTriggerTool::runTrigger() {

        //Retrieve the current run number and event number
        const EventInfo* pevt = 0;
        ATH_CHECK( evtStore()->retrieve(pevt) );
        int event = pevt->event_ID()->event_number();

        //////////////////////////////////////////////////////////////
        //                                                          //
        // Load Variables From Containers into our Data Structures  //
        //                                                          //
        //////////////////////////////////////////////////////////////

        std::vector<athena_entry> entries;
        map<hdst_key,hdst_entry> Hits_Data_Set_Time;
        map<int,evInf_entry> Event_Info;

        const MmDigitContainer *nsw_MmDigitContainer = nullptr;
        ATH_CHECK( evtStore()->retrieve(nsw_MmDigitContainer,"MM_DIGITS") );

        std::string wedgeType = getWedgeType(nsw_MmDigitContainer);
        if(wedgeType=="Large") m_par = m_par_large;
        if(wedgeType=="Small") m_par = m_par_small;
        if(wedgeType=="Neither") {
          ATH_MSG_INFO( "SMALL AND LARGE!! Event did (NOT) pass " );
          return StatusCode::SUCCESS;
        }

        MMLoadVariables m_load = MMLoadVariables(&(*(evtStore())), m_detManager, m_MmIdHelper, m_par);
        m_load.getMMDigitsInfo(entries, Hits_Data_Set_Time, Event_Info);
        this->fillNtuple(m_load);

        //Originally boom, this is the saved "particle_info" (originally primer)
        evInf_entry truth_info(Event_Info.find(pevt->event_ID()->event_number())->second);

        bool pass_cuts = truth_info.pass_cut;
        double trueta = truth_info.eta_ip;
        double trupt = truth_info.pt;

        // evInf_entry truth_info(Event_Info.find(pevt->event_ID()->event_number())->second);

        double tent=truth_info.theta_ent,tpos=truth_info.theta_pos,ppos=truth_info.phi_pos,pent=truth_info.phi_ent,dt=truth_info.dtheta;
        m_trigger_trueThe->push_back(tent);
        m_trigger_truePhi->push_back(ppos);
        m_trigger_trueDth->push_back(dt);

        //from MMT_Loader >>>> If entry matches find(event) adds element to vector
        std::vector<hdst_entry> hdsts(event_hdsts(event,Hits_Data_Set_Time));
        //Only consider fits if they satisfy CT and fall in wedge
        if(pass_cuts){
        //Make sure hit info is not empy
        if(!hdsts.empty()){

          //////////////////////////////////////////////////////////////
          //                                                          //
          //                Finder Applied Here                       //
          //                                                          //
          //////////////////////////////////////////////////////////////

          //Initialization of the finder: defines all the roads
          MMT_Finder m_find = MMT_Finder(m_par);

          ATH_MSG_DEBUG(  "Number of Roads Configured " <<  m_find.get_roads()  );

          //Convert hits to slopes and fill the buffer
          map<pair<int,int>,finder_entry> hitBuffer;
          for(int ihds=0; ihds<(int)hdsts.size(); ihds++){
            m_find.fillHitBuffer(hitBuffer,hdsts[ihds].entry_hit(m_par));

            hdst_info hitInfo = hdsts[ihds].entry_hit(m_par).info;

            m_trigger_VMM->push_back(hdsts[ihds].VMM_chip);
            m_trigger_plane->push_back(hdsts[ihds].plane);
            m_trigger_station->push_back(hdsts[ihds].station_eta);
            m_trigger_strip->push_back(hdsts[ihds].strip);
            m_trigger_slope->push_back(hitInfo.slope.getFloat());

          }
          if(hdsts.size()==8){
            m_trigger_trueEtaRange->push_back(trueta);
            m_trigger_truePtRange->push_back(trupt);
            if(wedgeType=="Large") {
              m_trigger_large_trueEtaRange->push_back(trueta);
              m_trigger_large_truePtRange->push_back(trupt);
            }
            if(wedgeType=="Small") {
              m_trigger_small_trueEtaRange->push_back(trueta);
              m_trigger_small_truePtRange->push_back(trupt);
            }

          }

          //////////////////////////////////////////////////////////////
          //                                                          //
          //                 Fitter Applied Here                      //
          //                                                          //
          //////////////////////////////////////////////////////////////

          MMT_Fitter m_fit = MMT_Fitter(m_par);

          //First loop over the roads and planes and apply the fitter
          int fits_occupied=0;
          const int nfit_max=1;  //MOVE THIS EVENTUALLY
          int correct_bcid=2;    //THIS TOO
          int roads = m_find.get_roads();

          vector<evFit_entry> road_fits = vector<evFit_entry>(roads,evFit_entry());

          //Variables saved for Alex T. for hardware validation
          double mxl;
          double fillmxl=-999;
          double muGlobal;
          double mvGlobal;
          vector<pair<double,double> > mxmy;

          for(int road=0; road<roads; road++){

            vector<bool> plane_is_hit;
            vector<Hit> track;
            pair<int,int>key(road,0);

            //Check if there are hits in the buffer
            m_find.checkBufferForHits(plane_is_hit, track, key, hitBuffer);
            //Look for coincidences
            int road_num=m_find.Coincidence_Gate(plane_is_hit);

            if(road_num>0){

              if(fits_occupied>=nfit_max) break;
              //Perform the fit -> calculate local, global X, UV slopes -> calculate ROI and TriggerTool signal (theta, phi, deltaTheta)
              evFit_entry candidate=m_fit.fit_event(event,track,hdsts,fits_occupied,mxmy,mxl,mvGlobal,muGlobal);
              //HERE IS THE PROBLEM
              ATH_MSG_DEBUG( "THETA " << candidate.fit_theta.getValue() << " PHI " << candidate.fit_phi.getValue() << " DTH " << candidate.fit_dtheta.getValue() );
                road_fits[road]=candidate;
                fillmxl = mxl;
                fits_occupied++;

            }

            road_fits[road].hcode=road_num;

          } //end roads

          //////////////////////////////////////////////////////////////
          //                                                          //
          //              Pass the ROI as Signal                      //
          //                                                          //
          //////////////////////////////////////////////////////////////


          bool did_clean_fit=false,did_bg_fit=false,has_6hits=false;
          if(road_fits.size()==0 and hdsts.size()==8 ) {
            ATH_MSG_DEBUG( "TruthRF0 " << tpos     << " " << ppos   << " " << dt << " " << trueta );
          }
          for(unsigned int i=0; i<road_fits.size(); i++){
            if(road_fits[i].fit_roi==0 and hdsts.size()==8) {
              ATH_MSG_DEBUG( "TruthROI0 " << tpos     << " " << ppos   << " " << dt << " " << trueta );
            }
            if(road_fits[i].fit_roi>0){
              //For the future: how do we want these to pass on as the signal?  Some new data structure?
              double fitTheta      = road_fits[i].fit_theta.getFloat();
              double fitPhi        = road_fits[i].fit_phi.getFloat();
              double fitDeltaTheta = road_fits[i].fit_dtheta.getFloat();


              ATH_MSG_DEBUG( "Truth " << tpos     << " " << ppos   << " " << dt );
              ATH_MSG_DEBUG( "FIT!! " << fitTheta << " " << fitPhi << " " << fitDeltaTheta );
              m_trigger_fitThe->push_back(fitTheta);
              m_trigger_fitPhi->push_back(fitPhi);
              m_trigger_fitDth->push_back(fitDeltaTheta);

              m_trigger_resThe->push_back(fitTheta-tpos);
              m_trigger_resPhi->push_back(fitPhi-ppos);
              m_trigger_resDth->push_back(fitDeltaTheta-dt);

              m_trigger_mx->push_back(mxmy.front().first);
              m_trigger_my->push_back(mxmy.front().second);
              m_trigger_mxl->push_back(fillmxl);

              m_trigger_mu->push_back(muGlobal);
              m_trigger_mv->push_back(mvGlobal);

              m_trigger_fitEtaRange->push_back(trueta);
              m_trigger_fitPtRange->push_back(trupt);
            if(wedgeType=="Large") {
              m_trigger_large_fitEtaRange->push_back(trueta);
              m_trigger_large_fitPtRange->push_back(trupt);
            }
            if(wedgeType=="Small") {
              m_trigger_small_fitEtaRange->push_back(trueta);
              m_trigger_small_fitPtRange->push_back(trupt);
            }

            }//fit roi > 0
          } // end road_fits
        }//end if HDSTS EMPTY
      }//end if PASS_CUTS

      //clear pointers, filled hit info

      Event_Info.erase(Event_Info.find(event));
      vector<hdst_key> kill_keys(event_hdst_keys(event,Hits_Data_Set_Time));
      return StatusCode::SUCCESS;
    }

  //Function that find the hits information and hits keys that get stored throughout the run.
  //The data structures are defined in MMT_struct

  vector<hdst_key> MMTriggerTool::event_hdst_keys(int find_event, map<hdst_key,hdst_entry>& Hits_Data_Set_Time) const{
    vector<hdst_key> ravel;
    int fnd_entries=0;
    for(map<hdst_key,hdst_entry>::const_iterator entry=Hits_Data_Set_Time.begin(); entry!=Hits_Data_Set_Time.end(); ++entry){
      if(entry->second.event==find_event){
        ravel.push_back(entry->first);
        fnd_entries++;
      }
      else if(fnd_entries>0) break;//we use the fact that maps store things according to the strict weak ordering of the key's comparison operator
    }
    return ravel;
  }

  vector<hdst_entry> MMTriggerTool::event_hdsts(int find_event, map<hdst_key,hdst_entry>& Hits_Data_Set_Time) const{
    vector<hdst_entry> bolero;
    int fnd_entries=0;
    for(map<hdst_key,hdst_entry>::const_iterator entry=Hits_Data_Set_Time.begin(); entry!=Hits_Data_Set_Time.end(); ++entry){
      if(entry->second.event==find_event){
        bolero.push_back(entry->second);
        fnd_entries++;
      }
      else if(fnd_entries>0) break;//we use the fact that maps store things according to the strict weak ordering of the key's comparison operator
    }
    return bolero;
  }
 std::string MMTriggerTool::getWedgeType(const MmDigitContainer *nsw_MmDigitContainer){
    std::vector<bool> isLargeWedge;
    //Digit loop to match to truth
    for(auto dit : *nsw_MmDigitContainer) {

      const MmDigitCollection* coll = dit;
      for (unsigned int item=0; item<coll->size(); item++) {

        const MmDigit* digit = coll->at(item);
        Identifier Id = digit->identify();

          std::string stName   = m_MmIdHelper->stationNameString(m_MmIdHelper->stationName(Id));
          string sname(stName);
          if (sname.compare("MML")==0)isLargeWedge.push_back(true);
          else isLargeWedge.push_back(false);
      }
    }
    bool allLarge = true;
    bool allSmall = true;
    for(int i=0; i<isLargeWedge.size(); i++){
      if (isLargeWedge.at(i)) allSmall = false;
      else allLarge = false;
    }
    std::string wedgeType = "Neither";
    if (allLarge) wedgeType = "Large";
    if (allSmall) wedgeType = "Small";
    return wedgeType;
  }
}//end namespace

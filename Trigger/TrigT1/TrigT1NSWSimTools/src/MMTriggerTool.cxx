/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// local includes
#include "TrigT1NSWSimTools/MMTriggerTool.h"
#include "TrigT1NSWSimTools/MMT_Finder.h"
#include "TrigT1NSWSimTools/MMT_Fitter.h"
// Athena/Gaudi includes
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "MuonIdHelpers/MmIdHelper.h"

//Muon software includes
#include "MuonDigitContainer/MmDigit.h"

//Event info includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// ROOT includes
#include "TTree.h"


using std::vector;
using std::map;
using std::pair;
using std::string;
using std::ios;
using std::setfill;
using std::setw;


namespace NSWL1 {

  MMTriggerTool::MMTriggerTool( const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type,name,parent),
    m_incidentSvc("IncidentSvc",name),
    m_detManager(0),
    m_MmIdHelper(0),
    m_MmDigitContainer(""),
    m_doNtuple(false),
    m_tree(0)
  {
    declareInterface<NSWL1::IMMTriggerTool>(this);
    declareProperty("MM_DigitContainerName", m_MmDigitContainer = "MM_DIGITS", "the name of the MM digit container");
    declareProperty("DoNtuple", m_doNtuple = true, "input the MMStripTds branches into the analysis ntuple");
  }

  MMTriggerTool::~MMTriggerTool() {

  }

  StatusCode MMTriggerTool::initialize() {

    ATH_MSG_INFO( "initializing -- " << name() );

    ATH_MSG_INFO( name() << " configuration:");
    ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_MmDigitContainer.name() << m_MmDigitContainer.value());
    ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_doNtuple.name() << ((m_doNtuple)? "[True]":"[False]")
                     << setfill(' ') << setiosflags(ios::right) );


    const IInterface* parent = this->parent();
    const INamedInterface* pnamed = dynamic_cast<const INamedInterface*>(parent);
    const std::string& algo_name = pnamed->name();
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
    const par_par standard = par_par(0.0009,4,4,0.0035,"xxuvxxuv",true);
    const par_par xxuvuvxx = par_par(0.0009,4,4,0.007,"xxuvuvxx",true,true); //.0035 for uv_tol before...
    const par_par xxuvuvxx_uvroads = par_par(0.0009,4,4,0.0035,"xxuvuvxx",true,true); //.0035 for uv_tol before...

    m_par_large = std::make_shared<MMT_Parameters>(xxuvuvxx,'L', m_detManager);
    m_par_small = std::make_shared<MMT_Parameters>(xxuvuvxx,'S', m_detManager);

    return StatusCode::SUCCESS;
  }

  StatusCode MMTriggerTool::runTrigger(const bool do_MMDiamonds) {
    //Retrieve the current run number and event number
    const EventInfo* pevt = 0;
    ATH_CHECK( evtStore()->retrieve(pevt) );
    int event = pevt->event_ID()->event_number();
    ATH_MSG_INFO("********************************************************* EVENT NUMBER = " << event);

    //////////////////////////////////////////////////////////////
    //                                                          //
    // Load Variables From Containers into our Data Structures  //
    //                                                          //
    //////////////////////////////////////////////////////////////

    const MmDigitContainer *nsw_MmDigitContainer = nullptr;
    ATH_CHECK( evtStore()->retrieve(nsw_MmDigitContainer,"MM_DIGITS") );

    std::map<std::string, std::shared_ptr<MMT_Parameters> > pars;
    pars["MML"] = m_par_large;
    pars["MMS"] = m_par_small;
    MMLoadVariables load = MMLoadVariables(&(*(evtStore())), m_detManager, m_MmIdHelper);

    std::map<std::pair<int, unsigned int>,std::vector<digitWrapper> > entries;
    std::map<std::pair<int, unsigned int>,std::map<hitData_key,hitData_entry> > Hits_Data_Set_Time;
    std::map<std::pair<int, unsigned int>,evInf_entry> Event_Info;
    ATH_CHECK( load.getMMDigitsInfo(entries, Hits_Data_Set_Time, Event_Info, pars) );
    this->fillNtuple(load);

    unsigned int particles = entries.rbegin()->first.second +1, nskip=0;

    if (entries.empty()) {
      ATH_MSG_WARNING("No digits available for processing, exiting");
      Hits_Data_Set_Time.clear();
      Event_Info.clear();
      return StatusCode::SUCCESS;
    }

    auto diamond = std::unique_ptr<MMT_Diamond>(new MMT_Diamond(m_detManager));
    if (do_MMDiamonds) {
      diamond->setTrapezoidalShape(true);
      diamond->setXthreshold(2);
      diamond->setUV(true);
      diamond->setUVthreshold(2);
      diamond->setRoadSize(8);
      diamond->setRoadSizeUpX(4);
      diamond->setRoadSizeDownX(0);
      diamond->setRoadSizeUpUV(4);
      diamond->setRoadSizeDownUV(0);
    }

    for (unsigned int i=0; i<particles; i++) {
      double trueta = -999., trupt = -999., dt = -999., tpos = -999., ppos = -999.;
      // We need to extract truth info, if available
      std::pair<int, unsigned int> pair_event (event,i);
      if (!Event_Info.empty()) {
        auto tru_it = Event_Info.find(pair_event);
        if (tru_it != Event_Info.end()) {
          evInf_entry truth_info(tru_it->second);
          trueta = truth_info.eta_ip;
          m_trigger_trueEtaRange->push_back(trueta);

          trupt = truth_info.pt;
          m_trigger_truePtRange->push_back(trupt);

          tpos=truth_info.theta_pos;
          m_trigger_trueThe->push_back(truth_info.theta_ent);

          ppos=truth_info.phi_pos;
          m_trigger_truePhi->push_back(ppos);

          dt=truth_info.dtheta;
          m_trigger_trueDth->push_back(dt);
        } else ATH_MSG_WARNING( "Extra reco particle with no truth candidate available" );
      }
      // Now let's switch to reco hits: firstly, extracting the station name we're working on...
      std::string station = "-";
      auto event_it = entries.find(pair_event);
      station = event_it->second[0].stName; // Station name is taken from the first digit! In MMLoadVariables there's a check to ensure all digits belong to the same station

      // Secondly, extracting the Phi of the station we're working on...
      int stationPhi = -999;
      digitWrapper dW = event_it->second[0];
      Identifier tmpID = dW.id();
      stationPhi = m_MmIdHelper->stationPhi(tmpID);

      // Finally, let's start with hits
      auto reco_it = Hits_Data_Set_Time.find(pair_event);
      if (reco_it != Hits_Data_Set_Time.end()) {
        if (!reco_it->second.empty()) {
          std::vector<hitData_entry> hitDatas;
          for (const auto &hit_it : reco_it->second) hitDatas.push_back(hit_it.second);
          if (do_MMDiamonds) {
            /*
             * Filling hits for each event: a new class, MMT_Hit, is called in
             * order to use both algorithms witghout interferences
             */
            diamond->createRoads_fillHits(i-nskip, hitDatas, m_detManager, pars[station], stationPhi);
            double smallest_bc = 999999.;
            for(int ihds=0; ihds<(int)hitDatas.size(); ihds++) {
              if (hitDatas[ihds].BC_time < 0.) continue;
              else if (hitDatas[ihds].BC_time < smallest_bc) smallest_bc = hitDatas[ihds].BC_time;

              // The PrintHits function below gives identical results to the following one: hitDatas[ihds].print();
              m_trigger_VMM->push_back(hitDatas[ihds].VMM_chip);
              m_trigger_plane->push_back(hitDatas[ihds].plane);
              m_trigger_station->push_back(hitDatas[ihds].station_eta);
              m_trigger_strip->push_back(hitDatas[ihds].strip);
            }
            diamond->printHits(i-nskip);
            std::vector<double> slopes = diamond->getHitSlopes();
            for (const auto &s : slopes) m_trigger_RZslopes->push_back(s);
            diamond->resetSlopes();
            slopes.clear();
            /*
             * Here we create roads with all MMT_Hit collected before (if any), then we save the results
             */
            if (diamond->getHitVector(i-nskip).size() >= (diamond->getXthreshold()+diamond->getUVthreshold())) {
              diamond->findDiamonds(i-nskip, smallest_bc, event);

              if (!diamond->getSlopeVector(i-nskip).empty()) {
                m_trigger_diamond_ntrig->push_back(diamond->getSlopeVector(i-nskip).size());
                for (const auto &slope : diamond->getSlopeVector(i-nskip)) {
                  m_trigger_diamond_stationPhi->push_back(diamond->getDiamond(i-nskip).phi);
                  m_trigger_diamond_sector->push_back(diamond->getDiamond(i-nskip).sector);
                  m_trigger_diamond_bc->push_back(slope.BC);
                  m_trigger_diamond_totalCount->push_back(slope.totalCount);
                  m_trigger_diamond_realCount->push_back(slope.realCount);
                  m_trigger_diamond_XbkgCount->push_back(slope.xbkg);
                  m_trigger_diamond_UVbkgCount->push_back(slope.uvbkg);
                  m_trigger_diamond_XmuonCount->push_back(slope.xmuon);
                  m_trigger_diamond_UVmuonCount->push_back(slope.uvmuon);
                  m_trigger_diamond_iX->push_back(slope.iRoad);
                  m_trigger_diamond_iU->push_back(slope.iRoadu);
                  m_trigger_diamond_iV->push_back(slope.iRoadv);
                  m_trigger_diamond_age->push_back(slope.age);
                  m_trigger_diamond_Xavg->push_back(slope.xavg);
                  m_trigger_diamond_Uavg->push_back(slope.uavg);
                  m_trigger_diamond_Vavg->push_back(slope.vavg);
                  m_trigger_diamond_mxl->push_back(slope.mxl);
                  m_trigger_diamond_theta->push_back(slope.theta);
                  m_trigger_diamond_eta->push_back(slope.eta);
                  m_trigger_diamond_dtheta->push_back(slope.dtheta);
                  m_trigger_diamond_phi->push_back(slope.phi);
                  m_trigger_diamond_phiShf->push_back(slope.phiShf);
                }
              } else ATH_MSG_WARNING("No output slopes to store");
            }
          } else {
            //////////////////////////////////////////////////////////////
            //                                                          //
            //                Finder Applied Here                       //
            //                                                          //
            //////////////////////////////////////////////////////////////

            //Initialization of the finder: defines all the roads
            auto find = std::unique_ptr<MMT_Finder>(new MMT_Finder(pars[station], 1));
            ATH_MSG_DEBUG(  "Number of Roads Configured " <<  find->get_roads()  );

            std::map<pair<int,int>,finder_entry> hitBuffer;
            for (const auto &hit_it : reco_it->second) {
              find->fillHitBuffer( hitBuffer, hit_it.second.entry_hit(pars[station]), pars[station] ); // Hit object, Map (road,plane) -> Finder entry
  
              hitData_info hitInfo = hit_it.second.entry_hit(pars[station]).info;
              m_trigger_VMM->push_back(hit_it.second.VMM_chip);
              m_trigger_plane->push_back(hit_it.second.plane);
              m_trigger_station->push_back(hit_it.second.station_eta);
              m_trigger_strip->push_back(hit_it.second.strip);
              m_trigger_slope->push_back(hitInfo.slope);
            }
            if (reco_it->second.size() > 7) {
              m_trigger_trueEtaRange->push_back(trueta);
              m_trigger_truePtRange->push_back(trupt);
              if (station == "MML") {
                m_trigger_large_trueEtaRange->push_back(trueta);
                m_trigger_large_truePtRange->push_back(trupt);
              }
              if (station == "MMS") {
                m_trigger_small_trueEtaRange->push_back(trueta);
                m_trigger_small_truePtRange->push_back(trupt);
              }
            }

            ////////////////////////////////////////////////////////////////
            ////                                                          //
            ////                 Fitter Applied Here                      //
            ////                                                          //
            ////////////////////////////////////////////////////////////////

            auto fit = std::unique_ptr<MMT_Fitter>(new MMT_Fitter());

            //First loop over the roads and planes and apply the fitter
            int fits_occupied = 0;
            const int nfit_max = 1;  //MOVE THIS EVENTUALLY
            int nRoads = find->get_roads();

            vector<evFit_entry> road_fits = vector<evFit_entry>(nRoads,evFit_entry());

            //Variables saved for Alex T. for hardware validation
            double mxl;
            double fillmxl = -999;
            double muGlobal;
            double mvGlobal;
            vector<pair<double,double> > mxmy;

            for (int iRoad = 0; iRoad < nRoads; iRoad++) {
              vector<bool> plane_is_hit;
              vector<Hit> track;

              //Check if there are hits in the buffer
              find->checkBufferForHits(  plane_is_hit, // Empty, To be filled by function.
                                         track,        // Empty, To be filled by function.
                                         iRoad,        // roadID
                                         hitBuffer,    // All hits. Map ( (road,plane) -> finder_entry  )
                                         pars[station] // Pointer to geometrical info class
                                      );

              //Look for coincidences
              int road_num = find->Coincidence_Gate(plane_is_hit, pars[station]);
              if (road_num > 0) {
                if (fits_occupied >= nfit_max) break;

                //Perform the fit -> calculate local, global X, UV slopes -> calculate ROI and TriggerTool signal (theta, phi, deltaTheta)
                evFit_entry candidate = fit->fit_event(event,track,hitDatas,fits_occupied,mxmy,mxl,mvGlobal,muGlobal,pars[station]);

                ATH_MSG_DEBUG( "THETA " << candidate.fit_theta << " PHI " << candidate.fit_phi << " DTH " << candidate.fit_dtheta );
                road_fits[iRoad] = candidate;
                fillmxl = mxl;
                fits_occupied++;
              }
              road_fits[iRoad].hcode = road_num;
            } //end roads

            //////////////////////////////////////////////////////////////
            //                                                          //
            //              Pass the ROI as Signal                      //
            //                                                          //
            //////////////////////////////////////////////////////////////

            if (road_fits.size() == 0 and hitDatas.size() == 8) ATH_MSG_DEBUG( "TruthRF0 " << tpos     << " " << ppos   << " " << dt << " " << trueta );

            for (unsigned int i = 0; i < road_fits.size(); i++) {
              if (road_fits[i].fit_roi == 0 and hitDatas.size() == 8) {
                ATH_MSG_DEBUG( "TruthROI0 " << tpos     << " " << ppos   << " " << dt << " " << trueta );
              }
              if (road_fits[i].fit_roi > 0) {
                //For the future: how do we want these to pass on as the signal?  Some new data structure?
                double fitTheta      = road_fits[i].fit_theta;
                double fitPhi        = road_fits[i].fit_phi;
                double fitDeltaTheta = road_fits[i].fit_dtheta;

                //need a correction for the fitted phi, taken from phi_shift in MMLoadVariables.cxx (now it's local)
	        int wedge = 0;
	        if (station == "MML") wedge = 0;
	        else if (station == "MMS") wedge = 1;
	        float n = 2*(stationPhi-1) + wedge;
	        float shift = n * M_PI/8.;
	        if(n > 8) shift = (16.-n)*M_PI/8.;
	        if(n < 8)       fitPhi = (fitPhi + shift);
	        else if(n == 8) fitPhi = (fitPhi + (fitPhi >= 0 ? -1 : 1)*shift);
	        else if(n > 8)  fitPhi = (fitPhi - shift);

	        double fitEta = -1. * TMath::Log(TMath::Tan(fitTheta/2.)); //VALE: trueta was filled!!!!

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
    
                m_trigger_fitEtaRange->push_back(fitEta);
                m_trigger_fitPtRange->push_back(trupt);
                if (station == "MML") {
                  m_trigger_large_fitEtaRange->push_back(fitEta);
                  m_trigger_large_fitPtRange->push_back(trupt);
                }
                if (station == "MMS") {
                  m_trigger_small_fitEtaRange->push_back(fitEta);
                  m_trigger_small_fitPtRange->push_back(trupt);
                }
              }
            }
          } // if-else Diamond clause
          hitDatas.clear();
        } else {
          ATH_MSG_WARNING( "Available hits are less than X+UV thresholds, skipping" );
          nskip++;
        }
      } else {
          ATH_MSG_WARNING( "Empty hit map, skipping" );
          nskip++;
      }
    } // Main particle loop
    entries.clear();
    Hits_Data_Set_Time.clear();
    Event_Info.clear();
    if(do_MMDiamonds) diamond->clearEvent();

    return StatusCode::SUCCESS;
  }
}//end namespace

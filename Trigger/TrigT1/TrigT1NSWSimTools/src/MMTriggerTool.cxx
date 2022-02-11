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
    declareProperty("DoNtuple", m_doNtuple = false, "input the MMStrip branches into the analysis ntuple");
  }

  MMTriggerTool::~MMTriggerTool() {

  }

  StatusCode MMTriggerTool::initialize() {

    ATH_MSG_DEBUG( "initializing -- " << name() );

    ATH_MSG_DEBUG( name() << " configuration:");
    ATH_MSG_DEBUG(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_MmDigitContainer.name() << m_MmDigitContainer.value());
    ATH_MSG_DEBUG(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_doNtuple.name() << ((m_doNtuple)? "[True]":"[False]")
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
      ATH_MSG_DEBUG("Analysis ntuple succesfully retrieved");
      ATH_CHECK( this->book_branches() );

    } else this->clear_ntuple_variables();

    // retrieve the Incident Service
    if( m_incidentSvc.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve the Incident Service");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("Incident Service successfully rertieved");
    }
    m_incidentSvc->addListener(this,IncidentType::BeginEvent);

    //  retrieve the MuonDetectormanager
    if( detStore()->retrieve( m_detManager ).isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve the MuonDetectorManager");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("MuonDetectorManager successfully retrieved");
    }

    //  retrieve the Mm offline Id helper
    if( detStore()->retrieve( m_MmIdHelper ).isFailure() ){
      ATH_MSG_FATAL("Failed to retrieve MmIdHelper");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("MmIdHelper successfully retrieved");
    }

    //Calculate and retrieve wedge geometry, defined in MMT_struct
    const par_par standard = par_par(0.0009,4,4,0.0035,"xxuvxxuv",true);
    const par_par xxuvuvxx = par_par(0.0009,4,4,0.007,"xxuvuvxx",true,true); //.0035 for uv_tol before...
    const par_par xxuvuvxx_uvroads = par_par(0.0009,4,4,0.0035,"xxuvuvxx",true,true); //.0035 for uv_tol before...

    m_par_large = std::make_shared<MMT_Parameters>(xxuvuvxx,'L', m_detManager);
    m_par_small = std::make_shared<MMT_Parameters>(xxuvuvxx,'S', m_detManager);

    // Variables for RDO encoding
    this->setPhiMin(-16.*M_PI/180.0);
    this->setPhiMax(16.*M_PI/180.0);
    this->setPhiBits(6);
    this->setRMin(900); // mm
    this->setRMax(5000);
    this->setRBits(8);
    this->setdThetaMin(-0.015); // rad
    this->setdThetaMax(0.015);
    this->setdThetaBits(5);

    return StatusCode::SUCCESS;
  }

  StatusCode MMTriggerTool::runTrigger(const bool do_MMDiamonds) {
    auto ctx = Gaudi::Hive::currentContext();
    int event = ctx.eventID().event_number();
    ATH_MSG_DEBUG("********************************************************* EVENT NUMBER = " << event);

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
    if (m_doNtuple) this->fillNtuple(load);

    if (entries.empty()) {
      ATH_MSG_WARNING("No digits available for processing, exiting");
      Hits_Data_Set_Time.clear();
      Event_Info.clear();
      return StatusCode::SUCCESS;
    }

    m_diamond = std::make_unique<MMT_Diamond>(m_detManager);
    if (do_MMDiamonds) {
      m_diamond->setTrapezoidalShape(true);
      m_diamond->setXthreshold(2);
      m_diamond->setUV(true);
      m_diamond->setUVthreshold(2);
      m_diamond->setRoadSize(8);
      m_diamond->setRoadSizeUpX(4);
      m_diamond->setRoadSizeDownX(0);
      m_diamond->setRoadSizeUpUV(4);
      m_diamond->setRoadSizeDownUV(0);
    }

    unsigned int particles = entries.rbegin()->first.second +1, nskip=0;
    for (unsigned int i=0; i<particles; i++) {
      double trueta = -999., trupt = -999., dt = -999., tpos = -999., ppos = -999.;
      // We need to extract truth info, if available
      std::pair<int, unsigned int> pair_event (event,i);
      if (!Event_Info.empty()) {
        auto tru_it = Event_Info.find(pair_event);
        if (tru_it != Event_Info.end()) {
          evInf_entry truth_info(tru_it->second);
          trueta = truth_info.eta_ip;
          trupt = truth_info.pt;
          tpos=truth_info.theta_pos;
          ppos=truth_info.phi_pos;
          dt=truth_info.dtheta;
          if (m_doNtuple) {
            m_trigger_trueEtaRange->push_back(trueta);
            m_trigger_truePtRange->push_back(trupt);
            m_trigger_trueThe->push_back(truth_info.theta_ent);
            m_trigger_truePhi->push_back(ppos);
            m_trigger_trueDth->push_back(dt);
          }
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
            m_diamond->createRoads_fillHits(i-nskip, hitDatas, m_detManager, pars[station], stationPhi);
            double smallest_bc = 999999.;
            for(int ihds=0; ihds<(int)hitDatas.size(); ihds++) {
              if (hitDatas[ihds].BC_time < 0.) continue;
              else if (hitDatas[ihds].BC_time < smallest_bc) smallest_bc = hitDatas[ihds].BC_time;

              // The PrintHits function below gives identical results to the following one: hitDatas[ihds].print();
              if (m_doNtuple) {
                m_trigger_VMM->push_back(hitDatas[ihds].VMM_chip);
                m_trigger_plane->push_back(hitDatas[ihds].plane);
                m_trigger_station->push_back(hitDatas[ihds].station_eta);
                m_trigger_strip->push_back(hitDatas[ihds].strip);
              }
            }
            m_diamond->printHits(i-nskip);
            std::vector<double> slopes = m_diamond->getHitSlopes();
            for (const auto &s : slopes) if (m_doNtuple) m_trigger_RZslopes->push_back(s);
            m_diamond->resetSlopes();
            slopes.clear();
            /*
             * Here we create roads with all MMT_Hit collected before (if any), then we save the results
             */
            if (m_diamond->getHitVector(i-nskip).size() >= (m_diamond->getXthreshold()+m_diamond->getUVthreshold())) {
              m_diamond->findDiamonds(i-nskip, smallest_bc, event);

              if (!m_diamond->getSlopeVector(i-nskip).empty()) {
                if (m_doNtuple) {
                  m_trigger_diamond_ntrig->push_back(m_diamond->getSlopeVector(i-nskip).size());
                  for (const auto &slope : m_diamond->getSlopeVector(i-nskip)) {
                    m_trigger_diamond_sector->push_back(m_diamond->getDiamond(i-nskip).sector);
                    m_trigger_diamond_stationPhi->push_back(m_diamond->getDiamond(i-nskip).stationPhi);
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
                    m_trigger_diamond_mx->push_back(slope.mx);
                    m_trigger_diamond_my->push_back(slope.my);
                    m_trigger_diamond_Uavg->push_back(slope.uavg);
                    m_trigger_diamond_Vavg->push_back(slope.vavg);
                    m_trigger_diamond_mxl->push_back(slope.mxl);
                    m_trigger_diamond_theta->push_back(slope.theta);
                    m_trigger_diamond_eta->push_back(slope.eta);
                    m_trigger_diamond_dtheta->push_back(slope.dtheta);
                    m_trigger_diamond_phi->push_back(slope.phi);
                    m_trigger_diamond_phiShf->push_back(slope.phiShf);
                  }
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
            auto find = std::make_unique<MMT_Finder>(pars[station], 1);
            ATH_MSG_DEBUG(  "Number of Roads Configured " <<  find->get_roads()  );

            std::map<pair<int,int>,finder_entry> hitBuffer;
            for (const auto &hit_it : reco_it->second) {
              find->fillHitBuffer( hitBuffer, hit_it.second.entry_hit(pars[station]), pars[station] ); // Hit object, Map (road,plane) -> Finder entry
  
              hitData_info hitInfo = hit_it.second.entry_hit(pars[station]).info;
              if (m_doNtuple) {
                m_trigger_VMM->push_back(hit_it.second.VMM_chip);
                m_trigger_plane->push_back(hit_it.second.plane);
                m_trigger_station->push_back(hit_it.second.station_eta);
                m_trigger_strip->push_back(hit_it.second.strip);
                m_trigger_slope->push_back(hitInfo.slope);
              }
            }
            if (reco_it->second.size() > 7 && m_doNtuple) {
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

                double fitEta = -1. * std::log(std::tan(fitTheta/2.)); //VALE: trueta was filled!!!!

                ATH_MSG_DEBUG( "Truth " << tpos     << " " << ppos   << " " << dt );
                ATH_MSG_DEBUG( "FIT!! " << fitTheta << " " << fitPhi << " " << fitDeltaTheta );
                if (m_doNtuple) {
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

    return StatusCode::SUCCESS;
  }

  StatusCode MMTriggerTool::fillRDO(Muon::NSW_TrigRawDataContainer *rdo, const bool do_MMDiamonds) {
    if(!do_MMDiamonds) return StatusCode::SUCCESS; // The old code won't be used for the moment

    for (const auto &diam : m_diamond->getDiamondVector()) {
      std::vector<int> slopeBC;
      for (const auto &slope : diam.slopes) slopeBC.push_back(slope.BC);
      std::sort(slopeBC.begin(), slopeBC.end());
      slopeBC.erase( std::unique(slopeBC.begin(), slopeBC.end()), slopeBC.end() );
      for (const auto &bc : slopeBC) {
        Muon::NSW_TrigRawData* trigRawData = new Muon::NSW_TrigRawData(diam.stationPhi, diam.side, bc);

        for (const auto &slope : diam.slopes) {
          if (bc == slope.BC) {
            Muon::NSW_TrigRawDataSegment* trigRawDataSegment = new Muon::NSW_TrigRawDataSegment();

            // Phi-id
            uint8_t phi_id = 0;
            if (slope.phiShf*M_PI/180.0 > this->getPhiMax() || slope.phiShf*M_PI/180.0 < this->getPhiMin()) trigRawDataSegment->setPhiIndex(phi_id);
            uint8_t nPhi = (1<<this->getPhiBits()) -2; // To accomodate the new phi-id encoding prescription around 0
            float phiSteps = (this->getPhiMax()-this->getPhiMin())/nPhi;
            for (uint8_t i=0; i<nPhi; i++) {
              if ((slope.phiShf*M_PI/180.0) < (this->getPhiMin()+i*phiSteps)) phi_id = i;
            }
            trigRawDataSegment->setPhiIndex(phi_id);
            if (m_doNtuple) m_trigger_diamond_TP_phi_id->push_back(phi_id);

            // R-id
            double extrapolatedR = slope.mx*7824.46; // The Z plane is a fixed value, taken from SL-TP documentation
            uint8_t R_id = 0;
            if (extrapolatedR > this->getRMax() || extrapolatedR < this->getRMin()) trigRawDataSegment->setRIndex(R_id);
            uint8_t nR = (1<<this->getRBits()) -1;
            float Rsteps = (this->getRMax()-this->getRMin())/nR;
            for (uint8_t j=0; j<nR; j++) {
              if (extrapolatedR < (this->getRMin()+j*Rsteps)) R_id = j;
            }
            trigRawDataSegment->setRIndex(R_id);
            if (m_doNtuple) m_trigger_diamond_TP_R_id->push_back(R_id);

            // DeltaTheta-id
            uint8_t dTheta_id = 0;
            if (slope.dtheta*M_PI/180.0 > this->getdThetaMax() || slope.dtheta*M_PI/180.0 < this->getdThetaMin()) trigRawDataSegment->setDeltaTheta(dTheta_id);
            uint8_t ndTheta = (1<<this->getdThetaBits()) -1;
            float dThetaSteps = (this->getdThetaMax()-this->getdThetaMin())/ndTheta;
            for (uint8_t k=0; k<ndTheta; k++) {
              if ((slope.dtheta*M_PI/180.0) < (this->getdThetaMin()+k*dThetaSteps)) dTheta_id = k;
            }
            trigRawDataSegment->setDeltaTheta(dTheta_id);
            if (m_doNtuple) m_trigger_diamond_TP_dTheta_id->push_back(dTheta_id);

            // Low R-resolution bit
            trigRawDataSegment->setLowRes(slope.lowRes);

            trigRawData->push_back(trigRawDataSegment);
          }
        }
        rdo->push_back(trigRawData);
      }
    }

    ATH_MSG_DEBUG("Filled MM RDO container now having size: " << rdo->size() << ". Clearing event information!");
    if (do_MMDiamonds) m_diamond->clearEvent();

    return StatusCode::SUCCESS;
  }
}//end namespace

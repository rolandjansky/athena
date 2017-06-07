/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Athena/Gaudi includes
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

// local includes
#include "MMTriggerTool.h"

//Event info includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

//#include "HitManagement/TimedHitPtr.h"

// Muon software includes

// random numbers
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"

// local includes
#include "TTree.h"
#include "TVector3.h"
//#include "MMStripUtil.h"

#include <functional>
#include <algorithm>
#include <map>
#include <utility>

using namespace std;

namespace NSWL1 {


    MMTriggerTool::MMTriggerTool( const std::string& type, const std::string& name, const IInterface* parent) :
      AthAlgTool(type,name,parent),
      m_incidentSvc("IncidentSvc",name),
      //m_rndmSvc("AtRndmGenSvc",name),
      //m_rndmEngine(0),
      m_detManager(0),
      m_MmIdHelper(0),
      //m_rndmEngineName(""),
      m_MmDigitContainer(""),
      m_MmSdoContainer(""),
      m_doNtuple(false),
      m_tree(0)
    {
      declareInterface<NSWL1::IMMTriggerTool>(this);

      //declareProperty("RndmEngineName", m_rndmEngineName = "MMTriggerTool", "the name of the random engine");
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


      ATH_MSG_INFO( "initializing " << name() ); 

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
          this->clear_ntuple_variables();
          ATH_MSG_FATAL("Could not retrieve the analysis ntuple from the THistSvc");
          return sc;
        } else {
          ATH_MSG_INFO("Analysis ntuple succesfully retrieved");
          sc = this->book_branches();
          if (sc.isFailure()) {
            ATH_MSG_ERROR("Cannot book the branches for the analysis ntuple");
          }
        }

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


    void MMTriggerTool::handle(const Incident& inc) {
      if( inc.type()==IncidentType::BeginEvent ) {
        std::cout << "Handling..." << std::endl;
        this->clear_ntuple_variables();
      }
    }
  
    StatusCode MMTriggerTool::book_branches() {

      m_trigger_fitThe       = new std::vector<double>();
      m_trigger_fitPhi       = new std::vector<double>();
      m_trigger_fitDth       = new std::vector<double>();
      m_trigger_trueEtaRange = new std::vector<double>();
      m_trigger_truePtRange  = new std::vector<double>();
      m_trigger_fitEtaRange  = new std::vector<double>();
      m_trigger_fitPtRange   = new std::vector<double>();
      m_trigger_resThe       = new std::vector<double>();
      m_trigger_resPhi       =  new std::vector<double>();
      m_trigger_resDth       = new std::vector<double>();

      m_trigger_large_fitThe       = new std::vector<double>();
      m_trigger_large_fitPhi       = new std::vector<double>();
      m_trigger_large_fitDth       = new std::vector<double>();
      m_trigger_large_trueEtaRange = new std::vector<double>();
      m_trigger_large_truePtRange  = new std::vector<double>();
      m_trigger_large_fitEtaRange  = new std::vector<double>();
      m_trigger_large_fitPtRange   = new std::vector<double>();
      m_trigger_large_resThe       = new std::vector<double>();
      m_trigger_large_resPhi       =  new std::vector<double>();
      m_trigger_large_resDth       = new std::vector<double>();

      m_trigger_small_fitThe       = new std::vector<double>();
      m_trigger_small_fitPhi       = new std::vector<double>();
      m_trigger_small_fitDth       = new std::vector<double>();
      m_trigger_small_trueEtaRange = new std::vector<double>();
      m_trigger_small_truePtRange  = new std::vector<double>();
      m_trigger_small_fitEtaRange  = new std::vector<double>();
      m_trigger_small_fitPtRange   = new std::vector<double>();
      m_trigger_small_resThe       = new std::vector<double>();
      m_trigger_small_resPhi       =  new std::vector<double>();
      m_trigger_small_resDth       = new std::vector<double>();

      m_trigger_VMM          = new std::vector<int>();
      m_trigger_plane        = new std::vector<int>();
      m_trigger_station      = new std::vector<int>();
      m_trigger_strip        = new std::vector<int>();
      m_trigger_slope        = new std::vector<double>();
      m_trigger_trueThe      = new std::vector<double>();
      m_trigger_truePhi      = new std::vector<double>();
      m_trigger_trueDth      = new std::vector<double>();
      m_trigger_mxl          = new std::vector<double>();
      m_trigger_mx           = new std::vector<double>();
      m_trigger_my           = new std::vector<double>();
      m_trigger_mu           = new std::vector<double>();
      m_trigger_mv           = new std::vector<double>();

      m_NSWMM_dig_stationName = new std::vector<std::string>();
      m_NSWMM_dig_stationEta  = new std::vector<int>();
      m_NSWMM_dig_stationPhi  = new std::vector<int>();
      m_NSWMM_dig_multiplet   = new std::vector<int>();
      m_NSWMM_dig_gas_gap     = new std::vector<int>();
      m_NSWMM_dig_channel     = new std::vector<int>();
 
      m_NSWMM_dig_time          = new std::vector< std::vector<float> >;
      m_NSWMM_dig_charge        = new std::vector< std::vector<float> >;
      m_NSWMM_dig_stripPosition = new std::vector< std::vector<int> >;
      m_NSWMM_dig_stripLposX    = new std::vector< std::vector<double> >;
      m_NSWMM_dig_stripLposY    = new std::vector< std::vector<double> >;
      m_NSWMM_dig_stripGposX    = new std::vector< std::vector<double> >;
      m_NSWMM_dig_stripGposY    = new std::vector< std::vector<double> >;
      m_NSWMM_dig_stripGposZ    = new std::vector< std::vector<double> >;
 
      m_NSWMM_dig_sr_time          = new std::vector< std::vector<float> >;
      m_NSWMM_dig_sr_charge        = new std::vector< std::vector<float> >;
      m_NSWMM_dig_sr_stripPosition = new std::vector< std::vector<int> >;
      m_NSWMM_dig_sr_stripLposX    = new std::vector< std::vector<double> >;
      m_NSWMM_dig_sr_stripLposY    = new std::vector< std::vector<double> >;
      m_NSWMM_dig_sr_stripGposX    = new std::vector< std::vector<double> >;
      m_NSWMM_dig_sr_stripGposY    = new std::vector< std::vector<double> >;
      m_NSWMM_dig_sr_stripGposZ    = new std::vector< std::vector<double> >;
 
      m_NSWMM_dig_truth_barcode    = new std::vector<int>;
      m_NSWMM_dig_truth_localPosX  = new std::vector<double>;
      m_NSWMM_dig_truth_localPosY  = new std::vector<double>;
      m_NSWMM_dig_truth_XZ_angle   = new std::vector<float>;
      m_NSWMM_dig_truth_globalPosX = new std::vector<double>;
      m_NSWMM_dig_truth_globalPosY = new std::vector<double>;
      m_NSWMM_dig_truth_globalPosZ = new std::vector<double>;
 
      //m_NSWMM_dig_stripForTrigger     = new std::vector<int>;      
      //m_NSWMM_dig_stripTimeForTrigger = new std::vector<float>;



      m_NSWMM_trackId  = new std::vector<int>;
      m_NSWMM_truthEl  = new std::vector<int>;
      m_NSWMM_globalTime = new std::vector<double>;
      m_NSWMM_hitGlobalPositionX = new std::vector<double>;
      m_NSWMM_hitGlobalPositionY = new std::vector<double>;
      m_NSWMM_hitGlobalPositionZ = new std::vector<double>;
      m_NSWMM_hitGlobalPositionR = new std::vector<double>;
      m_NSWMM_hitGlobalPositionP = new std::vector<double>;
      m_NSWMM_hitGlobalDirectionX = new std::vector<double>;
      m_NSWMM_hitGlobalDirectionY = new std::vector<double>;
      m_NSWMM_hitGlobalDirectionZ = new std::vector<double>;
    
      m_NSWMM_hitLocalPositionX = new std::vector<double>;
      m_NSWMM_hitLocalPositionY = new std::vector<double>;
      m_NSWMM_hitLocalPositionZ = new std::vector<double>;
    
      m_NSWMM_detector_globalPositionX = new std::vector<double>;
      m_NSWMM_detector_globalPositionY = new std::vector<double>;
      m_NSWMM_detector_globalPositionZ = new std::vector<double>;
      m_NSWMM_detector_globalPositionR = new std::vector<double>;
      m_NSWMM_detector_globalPositionP = new std::vector<double>;
    
      m_NSWMM_hitToDsurfacePositionX = new std::vector<double>;
      m_NSWMM_hitToDsurfacePositionY = new std::vector<double>;
      m_NSWMM_hitToDsurfacePositionZ = new std::vector<double>;
    
      m_NSWMM_hitToRsurfacePositionX = new std::vector<double>;
      m_NSWMM_hitToRsurfacePositionY = new std::vector<double>;
      m_NSWMM_hitToRsurfacePositionZ = new std::vector<double>;
    
      m_NSWMM_particleEncoding = new std::vector<int>;
      m_NSWMM_kineticEnergy = new std::vector<double>;
      m_NSWMM_depositEnergy = new std::vector<double>;
      m_NSWMM_StepLength = new std::vector<double>;
    
    
      m_NSWMM_sim_stationName = new std::vector<std::string>;
      m_NSWMM_sim_stationEta  = new std::vector<int>;
      m_NSWMM_sim_stationPhi  = new std::vector<int>;
      m_NSWMM_sim_multilayer  = new std::vector<int>;
      m_NSWMM_sim_layer       = new std::vector<int>;
      m_NSWMM_sim_side        = new std::vector<int>;
    
      m_NSWMM_off_stationName = new std::vector<std::string>;
      m_NSWMM_off_stationEta  = new std::vector<int>;
      m_NSWMM_off_stationPhi  = new std::vector<int>;
      m_NSWMM_off_multiplet   = new std::vector<int>;
      m_NSWMM_off_gas_gap     = new std::vector<int>;
      m_NSWMM_off_channel     = new std::vector<int>;

      if (m_tree) {
        std::string ToolName = name().substr(  name().find("::")+2,std::string::npos );
        const char* n = ToolName.c_str();
        
        m_tree->Branch(TString::Format("%s_trigger_fitThe",n).Data(),&m_trigger_fitThe);
        m_tree->Branch(TString::Format("%s_trigger_fitPhi",n).Data(),    &m_trigger_fitPhi);
        m_tree->Branch(TString::Format("%s_trigger_fitDth",n).Data(),    &m_trigger_fitDth);
        m_tree->Branch(TString::Format("%s_trigger_trueEtaRange",n).Data(),    &m_trigger_trueEtaRange);
        m_tree->Branch(TString::Format("%s_trigger_truePtRange",n).Data(),    &m_trigger_truePtRange);
        m_tree->Branch(TString::Format("%s_trigger_fitEtaRange",n).Data(),    &m_trigger_fitEtaRange);
        m_tree->Branch(TString::Format("%s_trigger_fitPtRange",n).Data(),    &m_trigger_fitPtRange);
        m_tree->Branch(TString::Format("%s_trigger_resThe",n).Data(),    &m_trigger_resThe);
        m_tree->Branch(TString::Format("%s_trigger_resPhi",n).Data(),    &m_trigger_resPhi);
        m_tree->Branch(TString::Format("%s_trigger_resDth",n).Data(),    &m_trigger_resDth);

        m_tree->Branch(TString::Format("%s_trigger_large_fitThe",n).Data(),&m_trigger_large_fitThe);
        m_tree->Branch(TString::Format("%s_trigger_large_fitPhi",n).Data(),    &m_trigger_large_fitPhi);
        m_tree->Branch(TString::Format("%s_trigger_large_fitDth",n).Data(),    &m_trigger_large_fitDth);
        m_tree->Branch(TString::Format("%s_trigger_large_trueEtaRange",n).Data(),    &m_trigger_large_trueEtaRange);
        m_tree->Branch(TString::Format("%s_trigger_large_truePtRange",n).Data(),    &m_trigger_large_truePtRange);
        m_tree->Branch(TString::Format("%s_trigger_large_fitEtaRange",n).Data(),    &m_trigger_large_fitEtaRange);
        m_tree->Branch(TString::Format("%s_trigger_large_fitPtRange",n).Data(),    &m_trigger_large_fitPtRange);
        m_tree->Branch(TString::Format("%s_trigger_large_resThe",n).Data(),    &m_trigger_large_resThe);
        m_tree->Branch(TString::Format("%s_trigger_large_resPhi",n).Data(),    &m_trigger_large_resPhi);
        m_tree->Branch(TString::Format("%s_trigger_large_resDth",n).Data(),    &m_trigger_large_resDth);

        m_tree->Branch(TString::Format("%s_trigger_small_fitThe",n).Data(),&m_trigger_small_fitThe);
        m_tree->Branch(TString::Format("%s_trigger_small_fitPhi",n).Data(),    &m_trigger_small_fitPhi);
        m_tree->Branch(TString::Format("%s_trigger_small_fitDth",n).Data(),    &m_trigger_small_fitDth);
        m_tree->Branch(TString::Format("%s_trigger_small_trueEtaRange",n).Data(),    &m_trigger_small_trueEtaRange);
        m_tree->Branch(TString::Format("%s_trigger_small_truePtRange",n).Data(),    &m_trigger_small_truePtRange);
        m_tree->Branch(TString::Format("%s_trigger_small_fitEtaRange",n).Data(),    &m_trigger_small_fitEtaRange);
        m_tree->Branch(TString::Format("%s_trigger_small_fitPtRange",n).Data(),    &m_trigger_small_fitPtRange);
        m_tree->Branch(TString::Format("%s_trigger_small_resThe",n).Data(),    &m_trigger_small_resThe);
        m_tree->Branch(TString::Format("%s_trigger_small_resPhi",n).Data(),    &m_trigger_small_resPhi);
        m_tree->Branch(TString::Format("%s_trigger_small_resDth",n).Data(),    &m_trigger_small_resDth);

        m_tree->Branch(TString::Format("%s_trigger_VMM",n).Data(),    &m_trigger_VMM);
        m_tree->Branch(TString::Format("%s_trigger_plane",n).Data(),    &m_trigger_plane);
        m_tree->Branch(TString::Format("%s_trigger_station",n).Data(),    &m_trigger_station);
        m_tree->Branch(TString::Format("%s_trigger_strip",n).Data(),    &m_trigger_strip);
        m_tree->Branch(TString::Format("%s_trigger_slope",n).Data(),    &m_trigger_slope);
        m_tree->Branch(TString::Format("%s_trigger_trueThe",n).Data(),    &m_trigger_trueThe);
        m_tree->Branch(TString::Format("%s_trigger_truePhi",n).Data(),    &m_trigger_truePhi);
        m_tree->Branch(TString::Format("%s_trigger_trueDth",n).Data(),    &m_trigger_trueDth);
        m_tree->Branch(TString::Format("%s_trigger_mxl",n).Data(),    &m_trigger_mxl);
        m_tree->Branch(TString::Format("%s_trigger_mx",n).Data(),    &m_trigger_mx);
        m_tree->Branch(TString::Format("%s_trigger_my",n).Data(),    &m_trigger_my);
        m_tree->Branch(TString::Format("%s_trigger_mu",n).Data(),    &m_trigger_mu);
        m_tree->Branch(TString::Format("%s_trigger_mv",n).Data(),    &m_trigger_mv);


        m_tree->Branch("Digits_MM_stationName", &m_NSWMM_dig_stationName);
        m_tree->Branch("Digits_MM_stationEta",  &m_NSWMM_dig_stationEta);
        m_tree->Branch("Digits_MM_stationPhi",  &m_NSWMM_dig_stationPhi);
        m_tree->Branch("Digits_MM_multiplet",   &m_NSWMM_dig_multiplet);
        m_tree->Branch("Digits_MM_gas_gap",     &m_NSWMM_dig_gas_gap);
        m_tree->Branch("Digits_MM_channel",     &m_NSWMM_dig_channel);

        m_tree->Branch("Digits_MM_time",          &m_NSWMM_dig_time);
        m_tree->Branch("Digits_MM_charge",        &m_NSWMM_dig_charge);
        m_tree->Branch("Digits_MM_stripPosition", &m_NSWMM_dig_stripPosition);
        m_tree->Branch("Digits_MM_stripLposX",    &m_NSWMM_dig_stripLposX);
        m_tree->Branch("Digits_MM_stripLposY",    &m_NSWMM_dig_stripLposY);
        m_tree->Branch("Digits_MM_stripGposX",    &m_NSWMM_dig_stripGposX);
        m_tree->Branch("Digits_MM_stripGposY",    &m_NSWMM_dig_stripGposY);
        m_tree->Branch("Digits_MM_stripGposZ",    &m_NSWMM_dig_stripGposZ);

        m_tree->Branch("Digits_MM_stripResponse_time",          &m_NSWMM_dig_sr_time);
        m_tree->Branch("Digits_MM_stripResponse_charge",        &m_NSWMM_dig_sr_charge);
        m_tree->Branch("Digits_MM_stripResponse_stripPosition", &m_NSWMM_dig_sr_stripPosition);
        m_tree->Branch("Digits_MM_stripResponse_stripLposX",    &m_NSWMM_dig_sr_stripLposX);
        m_tree->Branch("Digits_MM_stripResponse_stripLposY",    &m_NSWMM_dig_sr_stripLposY);
        m_tree->Branch("Digits_MM_stripresponse_stripGposX",    &m_NSWMM_dig_sr_stripGposX);
        m_tree->Branch("Digits_MM_stripResponse_stripGposY",    &m_NSWMM_dig_sr_stripGposY);
        m_tree->Branch("Digits_MM_stripResponse_stripGposZ",    &m_NSWMM_dig_sr_stripGposZ);

        m_tree->Branch("Digits_MM_truth_barcode",    &m_NSWMM_dig_truth_barcode);
        m_tree->Branch("Digits_MM_truth_localPosX",  &m_NSWMM_dig_truth_localPosX);
        m_tree->Branch("Digits_MM_truth_localPosY",  &m_NSWMM_dig_truth_localPosY);
        m_tree->Branch("Digits_MM_truth_XZ_angle",   &m_NSWMM_dig_truth_XZ_angle);
        m_tree->Branch("Digits_MM_truth_globalPosX", &m_NSWMM_dig_truth_globalPosX);
        m_tree->Branch("Digits_MM_truth_globalPosY", &m_NSWMM_dig_truth_globalPosY);
        m_tree->Branch("Digits_MM_truth_globalPosZ", &m_NSWMM_dig_truth_globalPosZ);

        m_tree->Branch("Hits_MM_trackId", &m_NSWMM_trackId);
        m_tree->Branch("Hits_MM_truthEl", &m_NSWMM_truthEl);
        m_tree->Branch("Hits_MM_globalTime", &m_NSWMM_globalTime);
        m_tree->Branch("Hits_MM_hitGlobalPositionX", &m_NSWMM_hitGlobalPositionX);
        m_tree->Branch("Hits_MM_hitGlobalPositionY", &m_NSWMM_hitGlobalPositionY);
        m_tree->Branch("Hits_MM_hitGlobalPositionZ", &m_NSWMM_hitGlobalPositionZ);
        m_tree->Branch("Hits_MM_hitGlobalPositionR", &m_NSWMM_hitGlobalPositionR);
        m_tree->Branch("Hits_MM_hitGlobalPositionP", &m_NSWMM_hitGlobalPositionP);
        m_tree->Branch("Hits_MM_hitGlobalDirectionX", &m_NSWMM_hitGlobalDirectionX);
        m_tree->Branch("Hits_MM_hitGlobalDirectionY", &m_NSWMM_hitGlobalDirectionY);
        m_tree->Branch("Hits_MM_hitGlobalDirectionZ", &m_NSWMM_hitGlobalDirectionZ);
    
        m_tree->Branch("Hits_MM_hitLocalPositionX", &m_NSWMM_hitLocalPositionX);
        m_tree->Branch("Hits_MM_hitLocalPositionY", &m_NSWMM_hitLocalPositionY);
        m_tree->Branch("Hits_MM_hitLocalPositionZ", &m_NSWMM_hitLocalPositionZ);
    
        m_tree->Branch("Hits_MM_detector_globalPositionX", &m_NSWMM_detector_globalPositionX);
        m_tree->Branch("Hits_MM_detector_globalPositionY", &m_NSWMM_detector_globalPositionY);
        m_tree->Branch("Hits_MM_detector_globalPositionZ", &m_NSWMM_detector_globalPositionZ);
        m_tree->Branch("Hits_MM_detector_globalPositionR", &m_NSWMM_detector_globalPositionR);
        m_tree->Branch("Hits_MM_detector_globalPositionP", &m_NSWMM_detector_globalPositionP);
    
        m_tree->Branch("Hits_MM_hitToDsurfacePositionX", &m_NSWMM_hitToDsurfacePositionX);
        m_tree->Branch("Hits_MM_hitToDsurfacePositionY", &m_NSWMM_hitToDsurfacePositionY);
        m_tree->Branch("Hits_MM_hitToDsurfacePositionZ", &m_NSWMM_hitToDsurfacePositionZ);
    
        m_tree->Branch("Hits_MM_hitToRsurfacePositionX", &m_NSWMM_hitToRsurfacePositionX);
        m_tree->Branch("Hits_MM_hitToRsurfacePositionY", &m_NSWMM_hitToRsurfacePositionY);
        m_tree->Branch("Hits_MM_hitToRsurfacePositionZ", &m_NSWMM_hitToRsurfacePositionZ);
         
    
        m_tree->Branch("Hits_MM_particleEncoding", &m_NSWMM_particleEncoding);
        m_tree->Branch("Hits_MM_kineticEnergy", &m_NSWMM_kineticEnergy);
        m_tree->Branch("Hits_MM_depositEnergy", &m_NSWMM_depositEnergy);
        m_tree->Branch("Hits_MM_StepLength", &m_NSWMM_StepLength);
    
    
        m_tree->Branch("Hits_MM_sim_stationName", &m_NSWMM_sim_stationName);
        m_tree->Branch("Hits_MM_sim_stationEta", &m_NSWMM_sim_stationEta);
        m_tree->Branch("Hits_MM_sim_stationPhi", &m_NSWMM_sim_stationPhi);
        m_tree->Branch("Hits_MM_sim_multilayer", &m_NSWMM_sim_multilayer);
        m_tree->Branch("Hits_MM_sim_layer", &m_NSWMM_sim_layer);
        m_tree->Branch("Hits_MM_sim_side", &m_NSWMM_sim_side);
    
        m_tree->Branch("Hits_MM_off_stationName", &m_NSWMM_off_stationName);
        m_tree->Branch("Hits_MM_off_stationEta", &m_NSWMM_off_stationEta);
        m_tree->Branch("Hits_MM_off_stationPhi", &m_NSWMM_off_stationPhi);
        m_tree->Branch("Hits_MM_off_multiplet", &m_NSWMM_off_multiplet);
        m_tree->Branch("Hits_MM_off_gas_gap", &m_NSWMM_off_gas_gap);
        m_tree->Branch("Hits_MM_off_channel", &m_NSWMM_off_channel);


        //m_tree->Branch("Digits_MM_stripForTrigger",     &m_NSWMM_dig_stripForTrigger);
       // m_tree->Branch("Digits_MM_stripTimeForTrigger", &m_NSWMM_dig_stripTimeForTrigger);

        

      } else { 
        return StatusCode::FAILURE;
      }

      return StatusCode::SUCCESS;
    }

    void MMTriggerTool::clear_ntuple_variables() {
      //clear the ntuple variables
      if(m_tree==0) return;

      m_trigger_fitThe->clear();
      m_trigger_fitPhi->clear();
      m_trigger_fitDth->clear();
      m_trigger_trueEtaRange->clear();
      m_trigger_truePtRange->clear();
      m_trigger_fitEtaRange->clear();
      m_trigger_fitPtRange->clear();
      m_trigger_resThe->clear();
      m_trigger_resPhi->clear();
      m_trigger_resDth->clear();

      m_trigger_large_fitThe->clear();
      m_trigger_large_fitPhi->clear();
      m_trigger_large_fitDth->clear();
      m_trigger_large_trueEtaRange->clear();
      m_trigger_large_truePtRange->clear();
      m_trigger_large_fitEtaRange->clear();
      m_trigger_large_fitPtRange->clear();
      m_trigger_large_resThe->clear();
      m_trigger_large_resPhi->clear();
      m_trigger_large_resDth->clear();

      m_trigger_small_fitThe->clear();
      m_trigger_small_fitPhi->clear();
      m_trigger_small_fitDth->clear();
      m_trigger_small_trueEtaRange->clear();
      m_trigger_small_truePtRange->clear();
      m_trigger_small_fitEtaRange->clear();
      m_trigger_small_fitPtRange->clear();
      m_trigger_small_resThe->clear();
      m_trigger_small_resPhi->clear();
      m_trigger_small_resDth->clear();

      m_trigger_VMM->clear();
      m_trigger_plane->clear();
      m_trigger_station->clear();
      m_trigger_strip->clear();
      m_trigger_slope->clear();
      m_trigger_trueThe->clear();
      m_trigger_truePhi->clear();
      m_trigger_trueDth->clear();
      m_trigger_mxl->clear();
      m_trigger_mx->clear();
      m_trigger_my->clear();
      m_trigger_mu->clear();
      m_trigger_mv->clear();
      
      // information of the module down to the channel closest to the initial G4 hit
      // size of vector is m_NSWMM_nDigits
      m_NSWMM_dig_stationName->clear();
      m_NSWMM_dig_stationEta->clear();
      m_NSWMM_dig_stationPhi->clear();
      m_NSWMM_dig_multiplet->clear();
      m_NSWMM_dig_gas_gap->clear();
      m_NSWMM_dig_channel->clear();

      // vectors of size m_NSWMM_nDigits that hold vectors in which an entry
      // corresponds to a strip that was decided to be fired by the digit
      // (information from VMM chip response emulation)
      m_NSWMM_dig_time->clear();
      m_NSWMM_dig_charge->clear();
      m_NSWMM_dig_stripPosition->clear();
      m_NSWMM_dig_stripLposX->clear();
      m_NSWMM_dig_stripLposY->clear();
      m_NSWMM_dig_stripGposX->clear();
      m_NSWMM_dig_stripGposY->clear();
      m_NSWMM_dig_stripGposZ->clear();

      // vectors of size m_NSWMM_nDigits that hold vectors in which an entry
      // corresponds to a strip that was decided to be fired by the digit
      // (information from VMM chip strip emulation)
      m_NSWMM_dig_sr_time->clear();
      m_NSWMM_dig_sr_charge->clear();
      m_NSWMM_dig_sr_stripPosition->clear();
      m_NSWMM_dig_sr_stripLposX->clear();
      m_NSWMM_dig_sr_stripLposY->clear();
      m_NSWMM_dig_sr_stripGposX->clear();
      m_NSWMM_dig_sr_stripGposY->clear();
      m_NSWMM_dig_sr_stripGposZ->clear();

      // truth information of the (1st) Geant4 hits that caused this
      // digit to be recorded (size is m_NSWMM_nDigits)
      m_NSWMM_dig_truth_barcode->clear();
      m_NSWMM_dig_truth_localPosX->clear();
      m_NSWMM_dig_truth_localPosY->clear();
      m_NSWMM_dig_truth_XZ_angle->clear();
      m_NSWMM_dig_truth_globalPosX->clear();
      m_NSWMM_dig_truth_globalPosY->clear();
      m_NSWMM_dig_truth_globalPosZ->clear();


      m_NSWMM_trackId->clear();
      m_NSWMM_truthEl->clear();
      m_NSWMM_globalTime->clear();
      m_NSWMM_hitGlobalPositionX->clear();
      m_NSWMM_hitGlobalPositionY->clear();
      m_NSWMM_hitGlobalPositionZ->clear();
      m_NSWMM_hitGlobalPositionR->clear();
      m_NSWMM_hitGlobalPositionP->clear();
      m_NSWMM_hitGlobalDirectionX->clear();
      m_NSWMM_hitGlobalDirectionY->clear();
      m_NSWMM_hitGlobalDirectionZ->clear();
    
      m_NSWMM_hitLocalPositionX->clear();
      m_NSWMM_hitLocalPositionY->clear();
      m_NSWMM_hitLocalPositionZ->clear();
    
      m_NSWMM_detector_globalPositionX->clear();
      m_NSWMM_detector_globalPositionY->clear();
      m_NSWMM_detector_globalPositionZ->clear();
      m_NSWMM_detector_globalPositionR->clear();
      m_NSWMM_detector_globalPositionP->clear();
    
      m_NSWMM_hitToDsurfacePositionX->clear();
      m_NSWMM_hitToDsurfacePositionY->clear();
      m_NSWMM_hitToDsurfacePositionZ->clear();
    
      m_NSWMM_hitToRsurfacePositionX->clear();
      m_NSWMM_hitToRsurfacePositionY->clear();
      m_NSWMM_hitToRsurfacePositionZ->clear();
    
      //m_NSWMM_FastDigitRsurfacePositionX->clear();
      //m_NSWMM_FastDigitRsurfacePositionY->clear();
    
    
      m_NSWMM_particleEncoding->clear();
      m_NSWMM_kineticEnergy->clear();
      m_NSWMM_depositEnergy->clear();
      m_NSWMM_StepLength->clear();
    
    
      m_NSWMM_sim_stationName->clear();
      m_NSWMM_sim_stationEta->clear();
      m_NSWMM_sim_stationPhi->clear();
      m_NSWMM_sim_multilayer->clear();
      m_NSWMM_sim_layer->clear();
      m_NSWMM_sim_side->clear();
    
      m_NSWMM_off_stationName->clear();
      m_NSWMM_off_stationEta->clear();
      m_NSWMM_off_stationPhi->clear();
      m_NSWMM_off_multiplet->clear();
      m_NSWMM_off_gas_gap->clear();
      m_NSWMM_off_channel->clear();

    }
    void MMTriggerTool::fillNtuple(MMLoadVariables loadedVariables){

      *m_NSWMM_dig_stationEta  = loadedVariables.histVars.NSWMM_dig_stationEta;
      *m_NSWMM_dig_stationPhi  = loadedVariables.histVars.NSWMM_dig_stationPhi;
      *m_NSWMM_dig_multiplet   = loadedVariables.histVars.NSWMM_dig_multiplet;
      *m_NSWMM_dig_gas_gap     = loadedVariables.histVars.NSWMM_dig_gas_gap;
      *m_NSWMM_dig_channel     = loadedVariables.histVars.NSWMM_dig_channel;
      *m_NSWMM_dig_time          = loadedVariables.histVars.NSWMM_dig_time;
      *m_NSWMM_dig_charge        = loadedVariables.histVars.NSWMM_dig_charge;
      *m_NSWMM_dig_stripPosition = loadedVariables.histVars.NSWMM_dig_stripPosition;
      *m_NSWMM_dig_stripLposX    = loadedVariables.histVars.NSWMM_dig_stripLposX;
      *m_NSWMM_dig_stripLposY    = loadedVariables.histVars.NSWMM_dig_stripLposY;
      *m_NSWMM_dig_stripGposX    = loadedVariables.histVars.NSWMM_dig_stripGposX;
      *m_NSWMM_dig_stripGposY    = loadedVariables.histVars.NSWMM_dig_stripGposY;
      *m_NSWMM_dig_stripGposZ    = loadedVariables.histVars.NSWMM_dig_stripGposZ;
      *m_NSWMM_dig_sr_time          = loadedVariables.histVars.NSWMM_dig_sr_time;
      *m_NSWMM_dig_sr_charge        = loadedVariables.histVars.NSWMM_dig_sr_charge;
      *m_NSWMM_dig_sr_stripPosition = loadedVariables.histVars.NSWMM_dig_sr_stripPosition;
      *m_NSWMM_dig_sr_stripLposX    = loadedVariables.histVars.NSWMM_dig_sr_stripLposX;
      *m_NSWMM_dig_sr_stripLposY    = loadedVariables.histVars.NSWMM_dig_sr_stripLposY;
      *m_NSWMM_dig_sr_stripGposX    = loadedVariables.histVars.NSWMM_dig_sr_stripGposX;
      *m_NSWMM_dig_sr_stripGposY    = loadedVariables.histVars.NSWMM_dig_sr_stripGposY;
      *m_NSWMM_dig_sr_stripGposZ    = loadedVariables.histVars.NSWMM_dig_sr_stripGposZ;
      *m_NSWMM_dig_truth_barcode    = loadedVariables.histVars.NSWMM_dig_truth_barcode;
      *m_NSWMM_dig_truth_localPosX  = loadedVariables.histVars.NSWMM_dig_truth_localPosX;
      *m_NSWMM_dig_truth_localPosY  = loadedVariables.histVars.NSWMM_dig_truth_localPosY;
      *m_NSWMM_dig_truth_XZ_angle   = loadedVariables.histVars.NSWMM_dig_truth_XZ_angle;
      *m_NSWMM_dig_truth_globalPosX = loadedVariables.histVars.NSWMM_dig_truth_globalPosX;
      *m_NSWMM_dig_truth_globalPosY = loadedVariables.histVars.NSWMM_dig_truth_globalPosY;
      *m_NSWMM_dig_truth_globalPosZ = loadedVariables.histVars.NSWMM_dig_truth_globalPosZ;
 
      //m_NSWMM_dig_stripForTrigger     = new std::vector<int>;      
      //m_NSWMM_dig_stripTimeForTrigger = new std::vector<float>;



      *m_NSWMM_trackId  = loadedVariables.histVars.NSWMM_trackId;
      *m_NSWMM_truthEl  = loadedVariables.histVars.NSWMM_truthEl;
      *m_NSWMM_globalTime = loadedVariables.histVars.NSWMM_globalTime;
      *m_NSWMM_hitGlobalPositionX = loadedVariables.histVars.NSWMM_hitGlobalPositionX;
      *m_NSWMM_hitGlobalPositionY = loadedVariables.histVars.NSWMM_hitGlobalPositionY;
      *m_NSWMM_hitGlobalPositionZ = loadedVariables.histVars.NSWMM_hitGlobalPositionZ;
      *m_NSWMM_hitGlobalPositionR = loadedVariables.histVars.NSWMM_hitGlobalPositionR;
      *m_NSWMM_hitGlobalPositionP = loadedVariables.histVars.NSWMM_hitGlobalPositionP;
      *m_NSWMM_hitGlobalDirectionX = loadedVariables.histVars.NSWMM_hitGlobalDirectionX;
      *m_NSWMM_hitGlobalDirectionY = loadedVariables.histVars.NSWMM_hitGlobalDirectionY;
      *m_NSWMM_hitGlobalDirectionZ = loadedVariables.histVars.NSWMM_hitGlobalDirectionZ;
      *m_NSWMM_hitLocalPositionX = loadedVariables.histVars.NSWMM_hitLocalPositionX;
      *m_NSWMM_hitLocalPositionY = loadedVariables.histVars.NSWMM_hitLocalPositionY;
      *m_NSWMM_hitLocalPositionZ = loadedVariables.histVars.NSWMM_hitLocalPositionZ;
      *m_NSWMM_detector_globalPositionX = loadedVariables.histVars.NSWMM_detector_globalPositionX;
      *m_NSWMM_detector_globalPositionY = loadedVariables.histVars.NSWMM_detector_globalPositionY;
      *m_NSWMM_detector_globalPositionZ = loadedVariables.histVars.NSWMM_detector_globalPositionZ;
      *m_NSWMM_detector_globalPositionR = loadedVariables.histVars.NSWMM_detector_globalPositionR;
      *m_NSWMM_detector_globalPositionP = loadedVariables.histVars.NSWMM_detector_globalPositionP;
      *m_NSWMM_hitToDsurfacePositionX = loadedVariables.histVars.NSWMM_hitToDsurfacePositionX;
      *m_NSWMM_hitToDsurfacePositionY = loadedVariables.histVars.NSWMM_hitToDsurfacePositionY;
      *m_NSWMM_hitToDsurfacePositionZ = loadedVariables.histVars.NSWMM_hitToDsurfacePositionZ;
      *m_NSWMM_hitToRsurfacePositionX = loadedVariables.histVars.NSWMM_hitToRsurfacePositionX;
      *m_NSWMM_hitToRsurfacePositionY = loadedVariables.histVars.NSWMM_hitToRsurfacePositionY;
      *m_NSWMM_hitToRsurfacePositionZ = loadedVariables.histVars.NSWMM_hitToRsurfacePositionZ;
      *m_NSWMM_particleEncoding = loadedVariables.histVars.NSWMM_particleEncoding;
      *m_NSWMM_kineticEnergy = loadedVariables.histVars.NSWMM_kineticEnergy;
      *m_NSWMM_depositEnergy = loadedVariables.histVars.NSWMM_depositEnergy;
      *m_NSWMM_sim_stationEta  = loadedVariables.histVars.NSWMM_sim_stationEta;
      *m_NSWMM_sim_stationPhi  = loadedVariables.histVars.NSWMM_sim_stationPhi;
      *m_NSWMM_sim_multilayer  = loadedVariables.histVars.NSWMM_sim_multilayer;
      *m_NSWMM_sim_layer       = loadedVariables.histVars.NSWMM_sim_layer;
      *m_NSWMM_sim_side        = loadedVariables.histVars.NSWMM_sim_side;
      *m_NSWMM_off_stationEta  = loadedVariables.histVars.NSWMM_off_stationEta;
      *m_NSWMM_off_stationPhi  = loadedVariables.histVars.NSWMM_off_stationPhi;
      *m_NSWMM_off_multiplet   = loadedVariables.histVars.NSWMM_off_multiplet;
      *m_NSWMM_off_gas_gap     = loadedVariables.histVars.NSWMM_off_gas_gap;
      *m_NSWMM_off_channel     = loadedVariables.histVars.NSWMM_off_channel;
    }

    StatusCode MMTriggerTool::runTrigger() {

        //**********Beginning of MM Harvard Alg code***************


        //Retrieve the current run number and event number
        const EventInfo* pevt = 0;
        StatusCode sc = evtStore()->retrieve(pevt);
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
        StatusCode sc_digit = evtStore()->retrieve(nsw_MmDigitContainer,"MM_DIGITS");

        std::string wedgeType = getWedgeType(nsw_MmDigitContainer);
        if(wedgeType=="Large") m_par = m_par_large;
        if(wedgeType=="Small") m_par = m_par_small;
        if(wedgeType=="Neither") {
          std::cout << "SMALL AND LARGE!! Event did (NOT) pass "<< event << std::endl;
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
              std::cout << "THETA " << candidate.fit_theta.getValue() << " PHI " << candidate.fit_phi.getValue() << " DTH " << candidate.fit_dtheta.getValue() << std::endl;
              //Removing dTheta cut for now
              //if(abs(candidate.fit_dtheta.getValue())<m_par->dtheta_cut){

                road_fits[road]=candidate;
                fillmxl = mxl;
                fits_occupied++;
              //}
            }
            road_fits[road].hcode=road_num;
          }//end roads


          //////////////////////////////////////////////////////////////
          //                                                          //
          //              Pass the ROI as Signal                      //
          //                                                          //
          //////////////////////////////////////////////////////////////

                    
          bool did_clean_fit=false,did_bg_fit=false,has_6hits=false;
          if(road_fits.size()==0 and hdsts.size()==8 ) {
            std::cout << "TruthRF0 " << tpos     << " " << ppos   << " " << dt << " " << trueta << std::endl;
          }
          for(unsigned int i=0; i<road_fits.size(); i++){
            if(road_fits[i].fit_roi==0 and hdsts.size()==8) {
              std::cout << "TruthROI0 " << tpos     << " " << ppos   << " " << dt << " " << trueta << std::endl;
            }
            if(road_fits[i].fit_roi>0){
              //For the future: how do we want these to pass on as the signal?  Some new data structure?
              double fitTheta      = road_fits[i].fit_theta.getFloat();
              double fitPhi        = road_fits[i].fit_phi.getFloat();
              double fitDeltaTheta = road_fits[i].fit_dtheta.getFloat();


              std::cout << "Truth " << tpos     << " " << ppos   << " " << dt << std::endl;
              std::cout << "FIT!! " << fitTheta << " " << fitPhi << " " << fitDeltaTheta << std::endl;
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

      
              //Check for bad fits
              // if(abs(phi)!=999){
              //   vector<hdst_key> keys(road_fits[i].fit_hit_keys);
              //   double uquant=0,vquant=0;
              //   int uct=0,vct=0;
              //   int gut=0,schlecht=0,womp=0;
              //   for(int ihit=0;ihit<(int)keys.size();ihit++){
              //     int pos=m_find.find_hdst(hdsts,keys[ihit]);
              //     int plane=hdsts[pos].plane,bcid=hdsts[pos].BC_time-10*(hdsts[pos].event+1);
              //     if(bcid==correct_bcid)gut++;
              //     else{schlecht++;womp=bcid;}
              //     if(m_par->setup.substr(plane,1)=="u"){uquant+=hdsts[ihit].entry_info(m_par).slope.getValue();uct++;}
              //     if(m_par->setup.substr(plane,1)=="v"){vquant+=hdsts[ihit].entry_info(m_par).slope.getValue();vct++;}
              //   }
              //   if(schlecht>gut)cerr<<"Particularly bad track...last bad bcid: "<<womp<<endl;                      
              // } //end phi !=999
            }//fit roi > 0
          } // end road_fits          
          
          cout<<"MMT::p_e::END EVENT #"<<event<<endl;
        
        }//end if HDSTS EMPTY     
      }//end if PASS_CUTS

      //clear pointers, filled hit info   
      
      Event_Info.erase(Event_Info.find(event));
      vector<hdst_key> kill_keys(event_hdst_keys(event,Hits_Data_Set_Time));
      return StatusCode::SUCCESS; 
    }
  //**********End of MM Harvard Alg code*******

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

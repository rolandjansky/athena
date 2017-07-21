/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Athena/Gaudi includes
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"


// local includes
#include "MMTriggerTool.h"

// local includes
#include "TTree.h"
#include "TVector3.h"

#include <functional>
#include <algorithm>
#include <map>
#include <utility>

using namespace std;

namespace NSWL1 {

    void MMTriggerTool::handle(const Incident& inc) {
      if( inc.type()==IncidentType::BeginEvent ) {
        ATH_MSG_INFO( "Handling..." );
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


}//end namespace

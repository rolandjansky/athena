/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Athena/Gaudi includes
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

// local includes
#include "TrigT1NSWSimTools/MMStripTdsOfflineTool.h"
#include "TrigT1NSWSimTools/MMStripOfflineData.h"

//Event info includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// ROOT includes
#include "TVector3.h"
#include "TMath.h"

const bool striphack=true;

namespace NSWL1 {

    struct MMStripHits {
      Identifier          t_id;
      MMStripOfflineData* t_mmstrip;
      int                 t_cache_index;

      // constructor
      MMStripHits(Identifier id, MMStripOfflineData* p, int c) { t_id = id; t_mmstrip=p; t_cache_index=c; }
    };

    typedef std::map < Identifier,std::vector<MMStripHits> > MMSTRIP_MAP;
    typedef std::map < Identifier,std::vector<MMStripHits> >::iterator MMSTRIP_MAP_IT;
    typedef std::pair< Identifier,std::vector<MMStripHits> > MMSTRIP_MAP_ITEM;


    MMStripTdsOfflineTool::MMStripTdsOfflineTool( const std::string& type, const std::string& name, const IInterface* parent) :
      AthAlgTool(type,name,parent),
      m_incidentSvc("IncidentSvc",name),
      m_mmstrip_cache_runNumber(-1),
      m_mmstrip_cache_eventNumber(-1),
      m_mmstrip_cache_status(CLEARED),
      m_doNtuple(false),
      m_tree(0)


    {
      declareInterface<NSWL1::IMMStripTdsTool>(this);

      declareProperty("DoNtuple", m_doNtuple = true, "input the MMStripTds branches into the analysis ntuple");

      // reserve enough slots for the trigger sectors and fills empty vectors
      m_mmstrip_cache.reserve(32);
      std::vector< std::vector<MMStripData*> >::iterator it = m_mmstrip_cache.begin();
      m_mmstrip_cache.insert(it,32,std::vector<MMStripData*>());
    }

    MMStripTdsOfflineTool::~MMStripTdsOfflineTool() {
      // clear the internal cache
      this->clear_cache();
    }

    void MMStripTdsOfflineTool::clear_cache() {
      for (unsigned int i=0; i<m_mmstrip_cache.size(); i++) {
        std::vector<MMStripData*>& sector_mmstrips = m_mmstrip_cache.at(i);
        for (unsigned int p=0; p< sector_mmstrips.size(); p++) {
          delete sector_mmstrips.at(p);
        }
        sector_mmstrips.clear();
      }
    }

    StatusCode MMStripTdsOfflineTool::initialize() {


      ATH_MSG_INFO( "initializing " << name() );

      ATH_MSG_INFO( name() << " configuration:");
      ATH_MSG_INFO(" " << std::setw(32) << std::setfill('.') << std::setiosflags(std::ios::left) << m_doNtuple.name() << ((m_doNtuple)? "[True]":"[False]")
		   << std::setfill(' ') << std::setiosflags(std::ios::right) );



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

      return StatusCode::SUCCESS;
    }

    void MMStripTdsOfflineTool::handle(const Incident& inc) {
      if( inc.type()==IncidentType::BeginEvent ) {
        this->clear_cache();
        this->reset_ntuple_variables();
        m_mmstrip_cache_status = CLEARED;
      }
    }


    StatusCode MMStripTdsOfflineTool::book_branches() {

      m_fitThe = new std::vector<double>();
      m_fitPhi = new std::vector<double>();
      m_fitDth = new std::vector<double>();
      m_tru_etarange = new std::vector<double>();
      m_tru_ptrange = new std::vector<double>();
      m_fit_etarange = new std::vector<double>();
      m_fit_ptrange = new std::vector<double>();
      m_res_the = new std::vector<double>();
      m_res_phi =  new std::vector<double>();
      m_res_dth = new std::vector<double>();

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
        m_tree->Branch(TString::Format("%s_fitThe",n).Data(),&m_fitThe);
        m_tree->Branch(TString::Format("%s_fitPhi",n).Data(),&m_fitPhi);
        m_tree->Branch(TString::Format("%s_fitDth",n).Data(),&m_fitDth);
        m_tree->Branch(TString::Format("%s_tru_etarange",n).Data(),&m_tru_etarange);
        m_tree->Branch(TString::Format("%s_tru_ptrange",n).Data(),&m_tru_ptrange);
        m_tree->Branch(TString::Format("%s_fit_etarange",n).Data(),&m_fit_etarange);
        m_tree->Branch(TString::Format("%s_fit_ptrange",n).Data(),&m_fit_ptrange);
        m_tree->Branch(TString::Format("%s_res_the",n).Data(),&m_res_the);
        m_tree->Branch(TString::Format("%s_res_phi",n).Data(),&m_res_phi);
        m_tree->Branch(TString::Format("%s_res_dth",n).Data(),&m_res_dth);

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


      } else {
        return StatusCode::FAILURE;
      }

      return StatusCode::SUCCESS;
    }


    void MMStripTdsOfflineTool::reset_ntuple_variables() {
      // if ntuple is not booked nothing to do
      if ( m_tree==0 ) return;

      //reset the ntuple variables

      clear_ntuple_variables();
    }

    void MMStripTdsOfflineTool::clear_ntuple_variables() {
      //clear the ntuple variables


      m_fitThe->clear();
      m_fitPhi->clear();
      m_fitDth->clear();
      m_tru_etarange->clear();
      m_tru_ptrange->clear();
      m_fit_etarange->clear();
      m_fit_ptrange->clear();
      m_res_the->clear();
      m_res_phi->clear();
      m_res_dth->clear();

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


    void MMStripTdsOfflineTool::fill_mmstrip_validation_id() { }


    StatusCode MMStripTdsOfflineTool::gather_mmstrip_data(std::vector<MMStripData*>& mmstrips, int side, int sector) {
      ATH_MSG_DEBUG( "gather_mmstrip_data: start gathering the String hits for side " << side << ", sector " << sector );

      // check side and sector parameters

      if ( side <-1 || side >1 ) {
        ATH_MSG_ERROR( "requested side " << side << " is out of range: [-1==All, 0==side C, 1==side A]");
        return StatusCode::FAILURE;
      }

      if ( sector <-1 || sector >15 ) {
        ATH_MSG_ERROR( "requested sector " << sector << " is out of range: [-1==All, 0 - 15]");
        return StatusCode::FAILURE;
      }

      // retrieve the current run number and event number
      const EventInfo* pevt = 0;
      StatusCode sc = evtStore()->retrieve(pevt);

      if ( !sc.isSuccess() ) {
        ATH_MSG_WARNING( "Could not retrieve the EventInfo, so cannot associate run and event number to the current Strip cache" );
        m_mmstrip_cache_runNumber   = -1;
        m_mmstrip_cache_eventNumber = -1;

      } else {
        m_mmstrip_cache_runNumber   = pevt->event_ID()->run_number();
        m_mmstrip_cache_eventNumber = pevt->event_ID()->event_number();


      }

      if (m_mmstrip_cache_status==CLEARED) {


      } //end if cleared

//**********End of MM Harvard Alg code*******

//*********MMT Loader implementation below **********


      // check the Strip cache status
      if ( m_mmstrip_cache_status!=OK ) {
        ATH_MSG_ERROR ( "MM Strip cache is in a bad status!" );

        return StatusCode::FAILURE;
      }


      // delivering the required collection
      if (side==-1 && sector==-1) {
        //return the full set
        ATH_MSG_DEBUG( "copying the full MM Strip hit set" );
        for (unsigned int i=0; i<32; i++) mmstrips.insert( mmstrips.end(), m_mmstrip_cache.at(i).begin(), m_mmstrip_cache.at(i).end() );

      }
      else if (side!=-1 && sector==-1) {
        //return all the trigger sectors for the given side
        ATH_MSG_DEBUG( "copying the MM Strip hit set of all trigger sector in side " << ((side)? "C" : "A") );
        unsigned int start = (side)? 16 : 0;
        unsigned int stop  = (side)? 32 :16;
        for (unsigned int i=start; i<stop; i++) mmstrips.insert( mmstrips.end(), m_mmstrip_cache.at(i).begin(), m_mmstrip_cache.at(i).end() );

      }
      else if (side==-1 && sector!=-1) {
        // return the required trigger sectors
        ATH_MSG_DEBUG( "copying the MM Strip hit set of all trigger sector " << sector << " in both side" );
        mmstrips.insert( mmstrips.end(), m_mmstrip_cache.at(sector).begin(), m_mmstrip_cache.at(sector).end() );
        mmstrips.insert( mmstrips.end(), m_mmstrip_cache.at(sector+16).begin(), m_mmstrip_cache.at(sector+16).end() );

      }
      else {
        // return the required trigger sector
        ATH_MSG_DEBUG( "copying the MM Strip hit set of all trigger sector " << sector << " in side " << ((side)? "C" : "A") );
        int i = side*16+sector;
        mmstrips.insert( mmstrips.end(), m_mmstrip_cache.at(i).begin(), m_mmstrip_cache.at(i).end() );

      }

      ATH_MSG_DEBUG( "delivered n. " << mmstrips.size() << " MM Strip hits." );
       //comment out end
      return StatusCode::SUCCESS;
    }


    MMStripTdsOfflineTool::cStatus MMStripTdsOfflineTool::fill_mmstrip_cache(){


      ATH_MSG_DEBUG( "fill_mmstrip_cache: end of processing" );

      return OK;
      }

  double MMStripTdsOfflineTool::phi_shift(double athena_phi) const{
    return athena_phi+(athena_phi>=0?-1:1)*TMath::Pi();
    return (-1.*athena_phi+(athena_phi>=0?1.5*TMath::Pi():-0.5*TMath::Pi()));
  }
  void MMStripTdsOfflineTool::xxuv_to_uvxx(TVector3& hit,int plane)const{
    if(plane<4)return;
    else if(plane==4)hit_rot_stereo_bck(hit);//x to u
    else if(plane==5)hit_rot_stereo_fwd(hit);//x to v
    else if(plane==6)hit_rot_stereo_fwd(hit);//u to x
    else if(plane==7)hit_rot_stereo_bck(hit);//v to x
  }

  void MMStripTdsOfflineTool::hit_rot_stereo_fwd(TVector3& hit)const{
    double degree=TMath::DegToRad()*(m_par->stereo_degree.getFixed());
    if(striphack) hit.SetY(hit.Y()*cos(degree));
    else{
      double xnew=hit.X()*cos(degree)+hit.Y()*sin(degree),ynew=-hit.X()*sin(degree)+hit.Y()*cos(degree);
      hit.SetX(xnew);hit.SetY(ynew);
    }
  }

  void MMStripTdsOfflineTool::hit_rot_stereo_bck(TVector3& hit)const{
    double degree=-TMath::DegToRad()*(m_par->stereo_degree.getFixed());
    if(striphack) hit.SetY(hit.Y()*cos(degree));
    else{
      double xnew=hit.X()*cos(degree)+hit.Y()*sin(degree),ynew=-hit.X()*sin(degree)+hit.Y()*cos(degree);
      hit.SetX(xnew);hit.SetY(ynew);
    }
  }

  std::vector<hitData_key> MMStripTdsOfflineTool::event_hitData_keys(int find_event) const{
    std::vector<hitData_key> ravel;
    int fnd_entries=0;
    for(std::map<hitData_key,hitData_entry>::const_iterator entry=m_Hits_Data_Set_Time.begin(); entry!=m_Hits_Data_Set_Time.end(); ++entry){
      if(entry->second.event==find_event){
        ravel.push_back(entry->first);
        fnd_entries++;
      }
      else if(fnd_entries>0) break;//we use the fact that maps store things according to the strict weak ordering of the key's comparison operator
    }
    return ravel;
  }


  int MMStripTdsOfflineTool::Get_Strip_ID(double X,double Y,int plane) const{  //athena_strip_id,module_y_center,plane)
    if(Y==-9999) return -1;
    std::string setup(m_par->setup);
    double strip_width=m_par->strip_width.getFixed(), degree=TMath::DegToRad()*(m_par->stereo_degree.getFixed());//,vertical_strip_width_UV = strip_width/cos(degree);
    double y_hit=Y;
    int setl=setup.length();
    if(plane>=setl||plane<0){
      ATH_MSG_ERROR("Pick a plane in [0,"<<setup.length()<<"] not "<<plane); exit(1);
    }
    std::string xuv=setup.substr(plane,1);
    if(xuv=="u"){//||xuv=="v"){
      if(striphack)return ceil(Y*cos(degree)/strip_width);
      y_hit = X*sin(degree)+Y*cos(degree);
    }
    else if(xuv=="v"){
      if(striphack)return ceil(Y*cos(degree)/strip_width);
      y_hit = -X*sin(degree)+Y*cos(degree);
    }
    else if(xuv!="x"){
      ATH_MSG_ERROR("Invalid plane option " << xuv); exit(2);
    }
    double strip_hit = ceil(y_hit*1./strip_width);
    return strip_hit;
  }

  int MMStripTdsOfflineTool::Get_VMM_chip(int strip) const{  //Not Finished... Rough
    int strips_per_VMM = 64;
    return ceil(1.*strip/strips_per_VMM);
  }

  int MMStripTdsOfflineTool::strip_number(int station,int plane,int spos)const{
    if (station<=0||station>m_par->n_stations_eta) {
      int base_strip = 0;

      return base_strip;
    }
    if (plane<0||plane>(int)m_par->setup.size()) {
      int base_strip = 0;

      return base_strip;
    }
    bool do_auto=false;
    //if true do strip # (ceil(Y/strip_width); what's currently fed into the algorithm)  calculation based on evenly spaced eta assumption of stations
    double H=m_par->H.getFixed()/*,h=m_par->h1,z=m_par->z_nominal[plane],z0=m_par->z_nominal.front()*/,ybase=m_par->ybases[plane][station-1].getFixed();
    if(do_auto){
      //-log(tan(0.5(atan(y/z))))=eta
      //this is the even y spacing
      if(m_par->dlm_new) ybase=H+1100.*(station-1);
      else ybase=H+950.*(station-1);
      /*//this is the even eta spacing version
      double etalo=-log(tan(0.5*atan((h+H)/z))),etahi=-log(tan(0.5*atan(H/z))),inc=(etahi-etalo)/m_par->n_stations_eta;
      double this_eta=etalo+inc*(station-1);
      ybase=z*tan(2*atan(exp(-1.*this_eta)));
      */
    }
    double width=m_par->strip_width.getFixed(); 
    std::string plane_char=m_par->setup.substr(plane,1);
    int base_strip=ceil(ybase/width)+spos;
    return base_strip;
  }
  bool MMStripTdsOfflineTool::Mimic_VMM_Chip_Deadtime(hitData_entry& candy){//** ASK BLC IF THERE'S A WAY TO DO THIS SINGLE ENTRY

    if(candy.strip<=0){

      candy.VMM_chip=0;//clear out non-hits  (i.e. empty events -- probably from cuts in generation)

      return false;
    }
    else{
      int VMM_chip=candy.VMM_chip-1,plane=candy.plane,time=candy.time;  //true time in sub ns from simulation
      if(!m_VMM_chip_status[VMM_chip][plane]){ //is the chip active?
        if(m_VMM__chip_last_hit_time[VMM_chip][plane] + m_VMM_deadtime <= time){  //if not, should the chip be active?
	  m_VMM__chip_last_hit_time[VMM_chip][plane]=0;
	  m_VMM_chip_status[VMM_chip][plane]=true;
        }
        else{
	  candy.VMM_chip=0;
	  return false;
        }
      }
      if(m_VMM_chip_status[VMM_chip][plane]){ //is the chip active?

        m_VMM_chip_status[VMM_chip][plane]=false;
        m_VMM__chip_last_hit_time[VMM_chip][plane]=time;
      }
    }
    return true;
  }

  std::vector<hitData_entry> MMStripTdsOfflineTool::event_hitDatas(int find_event) const{
    std::vector<hitData_entry> bolero;
    int fnd_entries=0;
    for(std::map<hitData_key,hitData_entry>::const_iterator entry=m_Hits_Data_Set_Time.begin(); entry!=m_Hits_Data_Set_Time.end(); ++entry){
      if(entry->second.event==find_event){
        bolero.push_back(entry->second);
        fnd_entries++;
      }
      else if(fnd_entries>0) break;//we use the fact that maps store things according to the strict weak ordering of the key's comparison operator
    }
    return bolero;
  }

  int MMStripTdsOfflineTool::eta_bin(double theta) const{
    int ebin=-999; double eta=-log(tan(0.5*theta));
    for(int i=0;i<=m_n_etabins;i++){
      if(eta<m_etabins[i]){
        ebin=i-1;
        break;
      }
    }
    //we want the histograms binned on truth eta
    //event selection eta cuts are done on theta_ip, not theta_pos
    if(ebin==-1) return 0;
    else if(ebin==-999) return m_n_etabins-1;
    return ebin;
  }

}//end namespace

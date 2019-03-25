/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Athena/Gaudi includes
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

// local includes
#include "TrigT1NSWSimTools/MMFPGAOfflineTool.h"
#include "TrigT1NSWSimTools/MMStripOfflineData.h"
#include "TrigT1NSWSimTools/MMCandidateOfflineData.h"

//Event info includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// local includes
#include "TTree.h"
#include "TVector3.h"

//#include <functional>
//#include <algorithm>

using namespace std;


namespace NSWL1 {

    MMFPGAOfflineTool::MMFPGAOfflineTool( const string& type, const string& name, const IInterface* parent) :
      AthAlgTool(type,name,parent),
      m_incidentSvc("IncidentSvc",name),
      m_mmcandidate_cache_runNumber(-1),
      m_mmcandidate_cache_eventNumber(-1),
      m_doNtuple(false),
      m_tree(0)

    {
      memset (m_mmcandidate_cache_status,CLEARED,sizeof(cStatus));

      declareInterface<NSWL1::IMMFPGATool>(this);

      declareProperty("DoNtuple", m_doNtuple = false, "input the MMStripTds branches into the analysis ntuple"); 

      // reserve enough slots for the trigger sectors and fills empty vectors
      m_trigger_candidates.reserve(32);
      vector< vector<MMCandidateData*> >::iterator it = m_trigger_candidates.begin();
      m_trigger_candidates.insert(it,32,vector<MMCandidateData*>());
    }

    MMFPGAOfflineTool::~MMFPGAOfflineTool() {
    }


    StatusCode MMFPGAOfflineTool::initialize() {
      ATH_MSG_INFO( "initializing " << name() ); 

      ATH_MSG_INFO( name() << " configuration:");
      ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_doNtuple.name() << ((m_doNtuple)? "[True]":"[False]")
                       << setfill(' ') << setiosflags(ios::right) );

 

      const IInterface* parent = this->parent();
      const INamedInterface* pnamed = dynamic_cast<const INamedInterface*>(parent);
      string algo_name = pnamed->name();

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

    void MMFPGAOfflineTool::handle(const Incident& inc) {
      if( inc.type()==IncidentType::BeginEvent ) {
        this->clear_cache();  
        this->reset_ntuple_variables();
        memset (m_mmcandidate_cache_status,CLEARED,sizeof(cStatus));
      }
    }

    void MMFPGAOfflineTool::clear_cache() {
      for (unsigned int i=0; i<m_trigger_candidates.size(); i++) {
        vector<MMCandidateData*>& sector_candidates = m_trigger_candidates.at(i);
        for (unsigned int c=0; c< sector_candidates.size(); c++) {
          delete sector_candidates.at(c);
        }
        sector_candidates.clear();
      }
    }


    StatusCode MMFPGAOfflineTool::book_branches() {
      m_nMMCandidates = 0;

      if (m_tree) {
        string ToolName = name().substr(  name().find("::")+2,string::npos );
        const char* n = ToolName.c_str();
        m_tree->Branch(TString::Format("%s_nMMCandidates",n).Data(),&m_nMMCandidates,TString::Format("%s_nMMCandidates/i",n).Data());
      } else { 
        return StatusCode::FAILURE;
      }

      return StatusCode::SUCCESS;
    }


    void MMFPGAOfflineTool::reset_ntuple_variables() {
      // if ntuple is not booked nothing to do
      if ( m_tree==0 ) return;

      //reset the ntuple variables
      m_nMMCandidates = 0;
    }

    void MMFPGAOfflineTool::clear_ntuple_variables() {
      //clear the ntuple variables
      m_nMMCandidates = 0;
    }

  
    StatusCode MMFPGAOfflineTool::gather_mmcandidate_data(vector<MMStripData*>& mmstrips,
                                                          vector<MMCandidateData*>& mmcandidates,
                                                          int side, int sector) {
      ATH_MSG_DEBUG( "gather_mmcandidate_data: start gathering the String hits for side " << side << ", sector " << sector );

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
        m_mmcandidate_cache_runNumber   = -1;
        m_mmcandidate_cache_eventNumber = -1;
      } else {
        m_mmcandidate_cache_runNumber   = pevt->event_ID()->run_number();
        m_mmcandidate_cache_eventNumber = pevt->event_ID()->event_number();
      }

      if (this->apply_trigger_logic(mmstrips) != OK) {
          ATH_MSG_ERROR("FPGA trigger solgic simulation not succesfull!");
          return StatusCode::FAILURE;
      }


      // delivering the required collection
      if (side==-1 && sector==-1) {
        //return the full set
        ATH_MSG_DEBUG( "copying the full MM trigger candidate set" );
        for (unsigned int i=0; i<32; i++) mmcandidates.insert( mmcandidates.end(), m_trigger_candidates.at(i).begin(), m_trigger_candidates.at(i).end() );
      }
      else if (side!=-1 && sector==-1) {
        //return all the trigger sectors for the given side
        ATH_MSG_DEBUG( "copying the MM trigger candidate set of all trigger sector in side " << ((side)? "C" : "A") );
        unsigned int start = (side)? 16 : 0;
        unsigned int stop  = (side)? 32 :16;
        for (unsigned int i=start; i<stop; i++) mmcandidates.insert( mmcandidates.end(), m_trigger_candidates.at(i).begin(), m_trigger_candidates.at(i).end() );
      }
      else if (side==-1 && sector!=-1) {
        // return the required trigger sectors
        ATH_MSG_DEBUG( "copying the MM trigger candidate set of all trigger sector " << sector << " in both side" );
        mmcandidates.insert( mmcandidates.end(), m_trigger_candidates.at(sector).begin(), m_trigger_candidates.at(sector).end() );
        mmcandidates.insert( mmcandidates.end(), m_trigger_candidates.at(sector+16).begin(), m_trigger_candidates.at(sector+16).end() );
      }
      else {
        // return the required trigger sector
        ATH_MSG_DEBUG( "copying the MM trigger candidate set of all trigger sector " << sector << " in side " << ((side)? "C" : "A") );
        int i = side*16+sector;
        mmcandidates.insert( mmcandidates.end(), m_trigger_candidates.at(i).begin(), m_trigger_candidates.at(i).end() );
      }

      ATH_MSG_DEBUG( "delivered n. " << mmcandidates.size() << " MM trigger candidates." );

      return StatusCode::SUCCESS;
    }

    MMFPGAOfflineTool::cStatus MMFPGAOfflineTool::apply_trigger_logic(vector<MMStripData*>& mmstrips) {

      for (int side=0; side<2; side++) {
        for (int sector=0; sector<16; sector++) {

          /** Compute the trigger sector */
          int tsector = side*16 + sector;

          /** Filter the data that belong to this trigger sector */
          vector<MMStripData*> strips_in_tsector;
          for (unsigned int i=0;i<mmstrips.size();i++) {
            if ( (mmstrips[i]->sideId()==side)&&(mmstrips[i]->sectorId()==sector) ) {
              strips_in_tsector.push_back(mmstrips[i]);
            }
          }

          if (strips_in_tsector.size()!=0) {

            /** if cach is already filled trigger the error */
            if (m_mmcandidate_cache_status[tsector]!=CLEARED) {
                m_mmcandidate_cache_status[tsector] = FILL_ERROR;
                return m_mmcandidate_cache_status[tsector];
            }

            /** process the trigger logic and fille the relative candidate cache */

          }
        }  
      }

      return OK;
  
    } 

} // end of NSWL1  namespace

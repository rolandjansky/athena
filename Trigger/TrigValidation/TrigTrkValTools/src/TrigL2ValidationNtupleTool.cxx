/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrigL2ValidationNtupleTool.cxx
//   Source file for class TrigL2ValidationNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Pavel.Ruzicka@cern.ch
///////////////////////////////////////////////////////////////////

//Root inlcudes
#include "TTree.h"

//Gaudi includes
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

// InDetTrack includes
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"

// Event info includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrigTrkValTools/TrigL2ValidationNtupleTool.h"

const float  TrigL2ValidationNtupleTool::s_errorEntry = -99999.0; 

// constructor
TrigL2ValidationNtupleTool::TrigL2ValidationNtupleTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
        AthAlgTool(t,n,p),         // retrieve as private tools
        m_nt(0),
        m_lastEventNumber(0)
        
{
   
    declareInterface<ITrigL2ValidationNtupleTool>(this);

    // Declare the properties
    declareProperty("BookNewNtuple",                m_bookNewNtuple = false,                "Create the ntuple tree or use existing one?");
    declareProperty("NtupleFileName",               m_ntupleFileName = "/NTUPLES/FILE1",    "Ntuple file handle");
    declareProperty("NtupleDirectoryName",          m_ntupleDirName = "FitterValidation",   "dircetory name for ntuple tree");
    declareProperty("NtupleTreeName",               m_ntupleTreeName = "Validation",        "Name of the ntuple tree");
}

// destructor
TrigL2ValidationNtupleTool::~TrigL2ValidationNtupleTool() {}




// initialize
StatusCode TrigL2ValidationNtupleTool::initialize() {

    // create ntuple tree
  if (m_bookNewNtuple) {
    // ---------------------------
    // retrive pointer to THistSvc
    ITHistSvc *tHistSvc;
    StatusCode sc =  service("THistSvc", tHistSvc);
    if (sc.isFailure()) {
        msg(MSG::ERROR) << "Unable to retrieve pointer to THistSvc" << endreq;
        return sc;
    }

    // ---------------------------
    // create tree and register it to THistSvc

    m_nt = new TTree(TString(m_ntupleTreeName), "InDetTrack Validation");
    // NB: we must not delete the tree, this is done by THistSvc

    std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleDirName+"/"+m_ntupleTreeName;
    sc = tHistSvc->regTree(fullNtupleName, m_nt);
    if (sc.isFailure()) {
        msg(MSG::ERROR) << "Unable to register TTree : " << fullNtupleName << endreq;
        return sc;
    }
    // add the ntuple branches (this function has to be called by the client of this tool, if m_bookNewNtuple is set to false...)
    addNtupleItems(m_nt);
  }
   
    m_TrackIDcounter = 0;
    return StatusCode::SUCCESS;
}

///////////////////////////////////////
/// finalize
///////////////////////////////////////
StatusCode TrigL2ValidationNtupleTool::finalize() {

  msg(MSG::INFO) << "start finalize() in " << name() << endreq;

   if (m_nt) {
    delete m_nt;
    m_nt = 0;
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigL2ValidationNtupleTool::addNtupleItems( TTree* tree ) const {
    if (!tree) return StatusCode::FAILURE;
    //-----------------
    // add items  *** Note: Documentation is in the header file, doxygen and wikis! ***
    // 
    // event info:
    tree->Branch("RunNumber",           &m_runNumber,   "run_number/I");
    tree->Branch("EventNumber",         &m_eventNumber, "event_number/I");
    tree->Branch("TrackID",             &m_TrackID,     "ID_of_track_in_event/I");
    tree->Branch("TrackAuthor",         &m_trackAuthor, "track_author/i");
    tree->Branch("chi2",                &m_chi2,        "chi2/F");

    // reconstructed track parameters
    tree->Branch("RecD0",               &m_Rec_d0,      "Reconstructed_d0/F");
    tree->Branch("RecZ0",               &m_Rec_z0,      "Reconstructed_z0/F");
    tree->Branch("RecPhi0",             &m_Rec_phi0,    "Reconstructed_phi0/F");
    tree->Branch("RecEta",              &m_Rec_eta,     "Reconstructed_eta/F");
    tree->Branch("RecPt",               &m_Rec_pT,      "Reconstructed_pT/F");
    
    // errors of reconstructed track parameters
    tree->Branch("RecErrD0",            &m_errord0,     "err_d0/F");
    tree->Branch("RecErrZ0",            &m_errorz0,     "err_z0/F");
    tree->Branch("RecErrPhi0",          &m_errorphi0,   "err_phi0/F");
    tree->Branch("RecErrEta",           &m_erroreta,    "err_eta/F");
    tree->Branch("RecErrPt",            &m_errorpT,     "err_pT/F");

    //track particle summary parameters
    tree->Branch("nPixelHits",   &m_nPixelHits,   "numberOfPixelHits/I");
    tree->Branch("nSCTHits",     &m_nSCTHits,       "numberOfSCTHits/I");
    tree->Branch("nTRTHits",     &m_nTRTHits,       "numberOfTRTHits/I");
    tree->Branch("nTRTHighTrshHits", &m_nTRTHighTrshHits, "numberOfTRTHighTresholdHits/I");

     ATH_MSG_INFO ("added own branches to ntuple");

    return StatusCode::SUCCESS;
}


StatusCode TrigL2ValidationNtupleTool::writeTrackData (
    const TrigInDetTrack& track)  const {

    if (!m_nt) {
        msg(MSG::ERROR) << "writeTrackData(...) can only be used, if property BookNewNtuple is set to true"  << endreq;
        return StatusCode::FAILURE;
    }
    StatusCode sc;

    ATH_MSG_INFO ("in writeTrackData(trk, indx)");

    sc = fillTrackData(track);
    if (sc.isFailure()) return sc;
    
    return writeRecord(m_nt);
}

//////////////////////////////////////
/// fill track data into variables without actually writing the record
//////////////////////////////////////
StatusCode TrigL2ValidationNtupleTool::fillTrackData (const TrigInDetTrack& track) const {

  // ---------------------------------------
    // detect new event, reset TrackParticle counter if new event
    const EventInfo* eventInfo;
    if ((evtStore()->retrieve(eventInfo)).isFailure()) {
      msg(MSG::WARNING) << "Could not retrieve event info" << endreq;
      m_runNumber   = (int)s_errorEntry;
      m_eventNumber = (int)s_errorEntry;
      return StatusCode::FAILURE;
      }
  
   EventID* myEventID=eventInfo->event_ID();
   if (m_lastEventNumber!=myEventID->event_number()) {
        // we have a new event, reset TrackParticleID:
        m_TrackIDcounter = 0;
        m_lastEventNumber = myEventID->event_number();
      }
    m_TrackIDcounter++;
    m_TrackID = m_TrackIDcounter;
    m_eventNumber = myEventID->event_number();
    m_runNumber   = myEventID->run_number(); 
    
   
    ATH_MSG_VERBOSE ("Event: " << m_eventNumber << ", Run: "<< m_runNumber  << " TrackID: " << m_TrackID);
            
    //----------------------------------------------
    // fill track parameters in ntuple

    m_nPixelHits = track.NPixelSpacePoints();
    m_nSCTHits = track.NSCT_SpacePoints();
    m_nTRTHits = track.NStrawHits();
    m_nTRTHighTrshHits = track.NTRHits();

    m_chi2 = (float)track.chi2();
    m_trackAuthor = track.algorithmId();
    
    ATH_MSG_VERBOSE ("# Pixel hits: " << m_nPixelHits << ", # SCT hits: "<< m_nSCTHits<<", # TRT hits: "<<m_nTRTHits );
    
    const TrigInDetTrackFitPar* param = track.param();
    if (param != NULL && fillTrackParameters(param).isFailure())  msg(MSG::WARNING) << "InDetTrack parameters could not be written to the ntuple" << endreq;
    
    return StatusCode::SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/// fill the parameters in ntuple
/////////////////////////////////////////////////////////////////////////////
StatusCode TrigL2ValidationNtupleTool::fillTrackParameters(const TrigInDetTrackFitPar* param) const {

  ATH_MSG_VERBOSE ("in fillTrackParam");

    if (!param) {
        msg(MSG::WARNING) << "Something is wrong - track has no param at all!" << endreq;
        m_Rec_d0     = 0;
        m_Rec_z0     = 0;
        m_Rec_phi0   = 0;
        m_Rec_eta    = 0;
        m_Rec_pT     = 0;
        return StatusCode::FAILURE;
    }

    ATH_MSG_VERBOSE("Fill Param Parameters now");
    // track parameters
    m_Rec_d0     = param->a0();
    m_Rec_z0     = param->z0();
    m_Rec_phi0   = param->phi0();
    m_Rec_eta    = param->eta();
    m_Rec_pT     = param->pT();
    
    m_errord0    = param->ea0();;
    m_errorz0    = param->ez0();
    m_errorphi0  = param->ephi0();
    m_erroreta   = param->eeta();
    m_errorpT    = param->epT();
       
   
    ATH_MSG_DEBUG ("Trackparameters: d0=" << m_Rec_d0 << ", z0=" << m_Rec_z0 << ", phi0=" << m_Rec_phi0 << ", eta=" << m_Rec_eta);

    return StatusCode::SUCCESS;
}


StatusCode TrigL2ValidationNtupleTool::writeRecord(TTree* tree) const {
    if (!tree) return StatusCode::FAILURE;
    ATH_MSG_INFO ("Writting InDet Trigger tracks into the Tree");
    tree->Fill();
    resetVariables();
    return StatusCode::SUCCESS;
}

void TrigL2ValidationNtupleTool::resetVariables() const {
    m_nPixelHits = 0;
    m_nSCTHits = 0;
 }



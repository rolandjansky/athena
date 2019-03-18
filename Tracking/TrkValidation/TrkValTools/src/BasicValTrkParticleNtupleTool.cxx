/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// BasicValidationNtupleTool.cxx
//   Source file for class BasicValidationNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////


#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkValTools/BasicValTrkParticleNtupleTool.h"
#include <bitset>

const float  Trk::BasicValTrkParticleNtupleTool::s_errorEntry = -99999.0; 

// constructor
Trk::BasicValTrkParticleNtupleTool::BasicValTrkParticleNtupleTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
        AthAlgTool(t,n,p),         // retrieve as private tools
        m_bookNewNtuple{},
        m_nt(nullptr),
        m_TrackIDcounter{},
        m_lastEventNumber(0),
        m_runNumber{},
        m_eventNumber{},
        m_TrackID{},
        m_Rec_d0{},     
         m_Rec_z0{},     
         m_Rec_phi0{},   
         m_Rec_eta{},    
         m_Rec_qOverP{}, 

         m_errord0{},    
         m_errorz0{},    
         m_errorphi0{},  
         m_errortheta0{},
         m_errorqoverp{},
         m_chi2{},
        m_numberOfPixelHits{},           
         m_numberOfContribPixelLayers{},  
         m_numberOfPixelHoles{},          
         m_numberOfPixelDeadSensors{},    
         m_numberOfSCTHits{},             
         m_numberOfSCTHoles{},            
         m_numberOfSCTDeadSensors{},      
         m_numberOfTRTHits{},             
         m_numberOfTRTHoles{},            
         m_numberOfTRTDeadStraws{},       
         m_numberOfTRTHighThresholdHits{},
         m_idHitPattern{}    
{
   
    declareInterface<IValidationNtupleTool>(this);
    // Declare the properties
    declareProperty("BookNewNtuple",                m_bookNewNtuple = false,                "Create the ntuple tree or use existing one?");
    declareProperty("NtupleFileName",               m_ntupleFileName = "/NTUPLES/FILE1",    "Ntuple file handle");
    declareProperty("NtupleDirectoryName",          m_ntupleDirName = "FitterValidation",   "dircetory name for ntuple tree");
    declareProperty("NtupleTreeName",               m_ntupleTreeName = "Validation",        "Name of the ntuple tree");
}

// destructor
Trk::BasicValTrkParticleNtupleTool::~BasicValTrkParticleNtupleTool() {}




// initialize
StatusCode Trk::BasicValTrkParticleNtupleTool::initialize() {

  ATH_CHECK( m_evt.initialize() );

    // create ntuple tree
  if (m_bookNewNtuple) {
    // ---------------------------
    // retrive pointer to THistSvc
    ITHistSvc *tHistSvc;
    StatusCode sc =  service("THistSvc", tHistSvc);
    if (sc.isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve pointer to THistSvc");
        return sc;
    }

    // ---------------------------
    // create tree and register it to THistSvc

    m_nt = new TTree(TString(m_ntupleTreeName), "Track Particle Validation");
    // NB: we must not delete the tree, this is done by THistSvc

    std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleDirName+"/"+m_ntupleTreeName;
    sc = tHistSvc->regTree(fullNtupleName, m_nt);
    if (sc.isFailure()) {
        ATH_MSG_ERROR("Unable to register TTree : " << fullNtupleName);
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
StatusCode Trk::BasicValTrkParticleNtupleTool::finalize() {

  ATH_MSG_DEBUG("start finalize() in " << name());
  if (m_nt) {
    delete m_nt;
    m_nt = 0;
  }

  return StatusCode::SUCCESS;
}

StatusCode Trk::BasicValTrkParticleNtupleTool::addNtupleItems( TTree* tree ) const {
    if (!tree) return StatusCode::FAILURE;
    //-----------------
    // add items  *** Note: Documentation is in the header file, doxygen and wikis! ***
    // 
    // event info:
    tree->Branch("RunNumber",           &m_runNumber,   "run_number/I");
    tree->Branch("EventNumber",         &m_eventNumber, "event_number/I");
    tree->Branch("TrackID",             &m_TrackID,     "ID_of_track_in_event/b");

    // reconstructed track parameters
    tree->Branch("RecD0",               &m_Rec_d0,      "Reconstructed_d0/F");
    tree->Branch("RecZ0",               &m_Rec_z0,      "Reconstructed_z0/F");
    tree->Branch("RecPhi0",             &m_Rec_phi0,    "Reconstructed_phi0/F");
    tree->Branch("RecEta",              &m_Rec_eta,     "Reconstructed_eta/F");
    tree->Branch("RecQoverP",           &m_Rec_qOverP,  "Reconstructed_Q_over_p/F");
    
    // errors of reconstructed track parameters
    tree->Branch("RecErrD0",            &m_errord0,      "err_d0/F");
    tree->Branch("RecErrZ0",            &m_errorz0,      "err_z0/F");
    tree->Branch("RecErrPhi0",          &m_errorphi0,    "err_phi0/F");
    tree->Branch("RecErrTheta",         &m_errortheta0,  "err_theta/F");
    tree->Branch("RecErrQoverP",        &m_errorqoverp,  "err_Q_over_p/F");
   
    // fit quality parameters 
    tree->Branch("chi2",                &m_chi2,                "chi2/F");

    //track particle summary parameters
    tree->Branch("nPixelHits",            &m_numberOfPixelHits,            "numberOfPixelHits/b");
    tree->Branch("nContribPixelLayers",   &m_numberOfContribPixelLayers,   "numberOfContribPixelLayers/b");
    tree->Branch("nPixelHoles",           &m_numberOfPixelHoles,           "numberOfPixelHoles/b");
    tree->Branch("nPixelDeadSensors",     &m_numberOfPixelDeadSensors,     "numberOfPixelDeadSensors/b");
    tree->Branch("nSCTHits",              &m_numberOfSCTHits,              "numberOfSCTHits/b");
    tree->Branch("nSCTHoles",             &m_numberOfSCTHoles,             "numberOfSCTHoles/b");
    tree->Branch("nSCTDeadSensors"    ,   &m_numberOfSCTDeadSensors,       "numberOfSCTDeadSensors/b");
    tree->Branch("nTRTHits"  ,            &m_numberOfTRTHits,              "numberOfTRTHits/b");
    tree->Branch("nTRTHoles",             &m_numberOfTRTHoles,             "numberOfTRTHoles/b");
    tree->Branch("nTRTDeadStraws",        &m_numberOfTRTDeadStraws,        "numberOfTRTDeadStraws/b");
    tree->Branch("nTRTHighThresholdHits", &m_numberOfTRTHighThresholdHits, "numberOfTRTHighThresholdHits/b");
    
    tree->Branch("idHitPattern",          &m_idHitPattern,                 "idHitPattern/l");

     ATH_MSG_DEBUG ("added own branches to ntuple");

    return StatusCode::SUCCESS;
}


StatusCode Trk::BasicValTrkParticleNtupleTool::writeTrackParticleData (
     const Trk::TrackParticleBase& track)  const {
    if (!m_nt) {
        ATH_MSG_ERROR("writeTrackParticleData(...) can only be used, if property BookNewNtuple is set to true" );
        return StatusCode::FAILURE;
    }
    StatusCode sc;

    ATH_MSG_DEBUG ("in writeTrackParticleData(trk, indx)");

    sc = fillTrackParticleData(track);
    if (sc.isFailure()) return sc;
    
    return writeRecord(m_nt);
}

//////////////////////////////////////
/// fill track data into variables without actually writing the record
//////////////////////////////////////
StatusCode Trk::BasicValTrkParticleNtupleTool::fillTrackParticleData (
    const Trk::TrackParticleBase& track) const {

  // ---------------------------------------
    // detect new event, reset TrackParticle counter if new event
  SG::ReadHandle<xAOD::EventInfo> evt(m_evt);
    if (!evt.isValid()) {
      ATH_MSG_WARNING("Could not retrieve event info");
      m_runNumber   = (int)s_errorEntry;
      m_eventNumber = (int)s_errorEntry;
      return StatusCode::FAILURE;
      }
  
   if (m_lastEventNumber!=evt->eventNumber()) {
        // we have a new event, reset TrackParticleID:
        m_TrackIDcounter = 0;
        m_lastEventNumber = evt->eventNumber();
      }
    m_TrackIDcounter++;
    m_TrackID = (unsigned char)m_TrackIDcounter;
    m_eventNumber = evt->eventNumber();
    m_runNumber   = evt->runNumber(); 
   
    ATH_MSG_VERBOSE ("Event: " << m_eventNumber << ", Run: "<< m_runNumber  << " TrackID: " << m_TrackID);
            
    //----------------------------------------------
    // fill track parameters in ntuple
    const Trk::Perigee* perpars = track.perigee();
    if (perpars != NULL && fillTrkParticlePerigee(perpars).isFailure())  ATH_MSG_WARNING("Perigee parameters could not be written to ntuple");
    
    const Trk::TrackSummary* summary = track.trackSummary();
    if((!summary) || fillTrkParticleSummary(summary).isFailure()) ATH_MSG_WARNING("Summary parameters could not be written to ntuple");

    const Trk::FitQuality* fitQuality = track.fitQuality();
    if((!fitQuality) || fillFitQualityData(fitQuality).isFailure() ) ATH_MSG_WARNING("Fit Quality data could not be written to ntuple");

    return StatusCode::SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/// fill the perigee in ntuple
/////////////////////////////////////////////////////////////////////////////
StatusCode Trk::BasicValTrkParticleNtupleTool::fillTrkParticlePerigee(const Trk::Perigee* perigee) const {

  ATH_MSG_VERBOSE ("in fillTrackPerigee");

    if (!perigee) {
        ATH_MSG_WARNING("Something is wrong - track has no perigee at all!");
        m_Rec_d0    = 0;
        m_Rec_z0    = 0;
        m_Rec_phi0  = 0;
        m_Rec_eta = 0;
        m_Rec_qOverP = 0;
        return StatusCode::FAILURE;
    }

    ATH_MSG_VERBOSE("Fill Perigee Parameters now");
    // track parameters
    m_Rec_d0    = perigee->parameters()[Trk::d0];
    m_Rec_z0    = perigee->parameters()[Trk::z0];
    m_Rec_phi0  = perigee->parameters()[Trk::phi0];
    m_Rec_eta   = perigee->eta();
    m_Rec_qOverP = perigee->parameters()[Trk::qOverP];
    
    if (perigee->covariance()) {

        // error of track parameters
        m_errord0       = Amg::error(*perigee->covariance(),Trk::d0);
        m_errorz0       = Amg::error(*perigee->covariance(),Trk::z0);
        m_errorphi0     = Amg::error(*perigee->covariance(),Trk::phi0);
        m_errortheta0   = Amg::error(*perigee->covariance(),Trk::theta);
        m_errorqoverp   = Amg::error(*perigee->covariance(),Trk::qOverP);


    } // end if measPerigee
    ATH_MSG_DEBUG ("Trackparameters: d0=" << m_Rec_d0 << ", z0=" << m_Rec_z0 << ", phi0=" << m_Rec_phi0 << ", eta=" << m_Rec_eta);

    return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
/// fill the perigee in ntuple
/////////////////////////////////////////////////////////////////////////////
StatusCode Trk::BasicValTrkParticleNtupleTool::fillTrkParticleSummary(const Trk::TrackSummary* summary) const {

  ATH_MSG_VERBOSE ("in fillTrackSummary");

    if (!summary) {
        ATH_MSG_WARNING("Something is wrong - track has no summary at all!");
        m_numberOfPixelHits  = 0;
        m_numberOfSCTHits    = 0;

        return StatusCode::FAILURE;
    }

    ATH_MSG_VERBOSE("Fill Summary Parameters now");
    // summary of the track
    m_numberOfPixelHits            = (unsigned char)summary->get(Trk::numberOfPixelHits);
    m_numberOfContribPixelLayers   = (unsigned char)summary->get(Trk::numberOfContribPixelLayers);
    m_numberOfPixelHoles           = (unsigned char)summary->get(Trk::numberOfPixelHoles);
    m_numberOfPixelDeadSensors     = (unsigned char)summary->get(Trk::numberOfPixelDeadSensors);
    m_numberOfSCTHits              = (unsigned char)summary->get(Trk::numberOfSCTHits); 
    m_numberOfSCTHoles             = (unsigned char)summary->get(Trk::numberOfSCTHoles);
    m_numberOfSCTDeadSensors       = (unsigned char)summary->get(Trk::numberOfSCTDeadSensors);
    m_numberOfTRTHits              = (unsigned char)summary->get(Trk::numberOfTRTHits);
    m_numberOfTRTHoles             = (unsigned char)summary->get(Trk::numberOfTRTHoles);
    m_numberOfTRTDeadStraws        = (unsigned char)summary->get(Trk::numberOfTRTDeadStraws);
    m_numberOfTRTHighThresholdHits = (unsigned char)summary->get(Trk::numberOfTRTHighThresholdHits);
    
    std::bitset<Trk::numberOfDetectorTypes> hitPattern; // all position are set to 0
    for (int i=0; i<Trk::numberOfDetectorTypes; ++i) {
      if (summary->isHit(static_cast<Trk::DetectorType>(i))) hitPattern.set(i,1); // set position i to 1
						      }
    m_idHitPattern = hitPattern.to_ulong();
  
    ATH_MSG_DEBUG ("Track summary: number of Pixel hit=" << m_numberOfPixelHits << ", number of SCT hits=" << m_numberOfSCTHits );

    return StatusCode::SUCCESS;
}

StatusCode Trk::BasicValTrkParticleNtupleTool::fillFitQualityData(const Trk::FitQuality* fitQuality) const {
  if (!fitQuality) {
        ATH_MSG_WARNING("Something is wrong - track has no fit quality data !!");
        m_chi2  = 0;
                   
        return StatusCode::FAILURE;
      }

  if(fitQuality->numberDoF() == 0) {
                                    ATH_MSG_WARNING("Number of DOF is zero !! Could not normalize chi2 ");
				    return StatusCode::FAILURE;
                                    }
  
  else m_chi2  = (float)fitQuality->chiSquared()/(float)fitQuality->numberDoF();
        
   return StatusCode::SUCCESS;
}

StatusCode Trk::BasicValTrkParticleNtupleTool::writeRecord(TTree* tree) const {
    if (!tree) return StatusCode::FAILURE;
    ATH_MSG_VERBOSE ("Writting Track Particles into the Tree");
    tree->Fill();
    resetVariables();
    return StatusCode::SUCCESS;
}

void Trk::BasicValTrkParticleNtupleTool::resetVariables() const {
   m_numberOfPixelHits=0;  
   m_numberOfContribPixelLayers=0;
   m_numberOfPixelHoles=0;
   m_numberOfPixelDeadSensors=0;
   m_numberOfSCTHits=0; 
   m_numberOfSCTHoles=0; 
   m_numberOfSCTDeadSensors=0;
   m_numberOfTRTHits=0;
   m_numberOfTRTHoles=0;
   m_numberOfTRTDeadStraws=0;
   m_numberOfTRTHighThresholdHits=0;
   m_idHitPattern=0;
}

StatusCode Trk::BasicValTrkParticleNtupleTool::writeTrackData (
        const Trk::Track&,
        const int,
        const unsigned int ) const{return StatusCode::SUCCESS;}
        //const Trk::FitterStatusCode  ) const {return StatusCode::SUCCESS;}

  
    StatusCode Trk::BasicValTrkParticleNtupleTool::fillTrackData (
        const Trk::Track&,
        const int,
        const unsigned int ) const{return StatusCode::SUCCESS;}
        //const Trk::FitterStatusCode ) const {return StatusCode::SUCCESS;}
  
       
    StatusCode Trk::BasicValTrkParticleNtupleTool::fillTrackParameter (
        const Trk::TrackParameters*,
        const int ) const {return StatusCode::SUCCESS;}

      
    StatusCode Trk::BasicValTrkParticleNtupleTool::fillMeasurementData (
        const Trk::MeasurementBase*,
        const Trk::TrackParameters* ) const {return StatusCode::SUCCESS;}

           
    StatusCode Trk::BasicValTrkParticleNtupleTool::fillOutlierData (
        const Trk::MeasurementBase*,
        const Trk::TrackParameters*,
        const Trk::FitQualityOnSurface* ) const {return StatusCode::SUCCESS;}

    
    StatusCode Trk::BasicValTrkParticleNtupleTool::fillHoleData (
        const Trk::TrackStateOnSurface&) const {return StatusCode::SUCCESS;}

    
    StatusCode Trk::BasicValTrkParticleNtupleTool::fillTrackTruthData ( 
					    const TrackParameters*&,
                                            const TrackTruth&, 
                                            const int) const {return StatusCode::SUCCESS;}



    StatusCode Trk::BasicValTrkParticleNtupleTool::writeProtoTrajectoryData (
        const Trk::ProtoTrajectory&,
        const int,
        const Trk::Perigee*,
        const unsigned int ) const{return StatusCode::SUCCESS;}
	//const Trk::FitterStatusCode) const {return StatusCode::SUCCESS;}

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// BasicValidationNtupleTool.cxx
//   Source file for class BasicValidationNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann -at- cern.ch, Wolfgang.Liebig -at- cern.ch
///////////////////////////////////////////////////////////////////
#include "TTree.h"
// Trk
#include "TrackInformationNtupleTool.h"
#include "TrkValInterfaces/IValidationNtupleHelperTool.h"
#include "TrkTrack/Track.h"
#include "TrkParticleBase/TrackParticleBase.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkFitterUtils/ProtoTrackStateOnSurface.h"

// constructor
Trk::TrackInformationNtupleTool::TrackInformationNtupleTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
        AthAlgTool(t,n,p),
        m_TrackIDcounter(0),
        m_lastEventNumber(0),
         m_runNumber{},  
         m_eventNumber{}, 
         m_TrackID{},     
         m_iterIndex{},   
         m_fitStatusCode{},   
         m_trackFitAuthor{},  
         m_trackSeedAuthor{}, 
         m_particleHypothesis{},
         m_Rec_chi2overNdof{}, 
         m_ndof{},        
         m_nHits{}     
{
  declareInterface<ITrackValidationNtupleTool>(this);
}

// destructor
Trk::TrackInformationNtupleTool::~TrackInformationNtupleTool() {}


///////////////////////////////////////
// initialize
///////////////////////////////////////
StatusCode Trk::TrackInformationNtupleTool::initialize() {

  ATH_CHECK( m_evt.initialize() );
  return StatusCode::SUCCESS;
}

///////////////////////////////////////
// finalize
///////////////////////////////////////
StatusCode Trk::TrackInformationNtupleTool::finalize() {

  ATH_MSG_DEBUG ("nothing specific finalized in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Trk::TrackInformationNtupleTool::addNtupleItems( TTree* tree ) const {
    if (!tree) return StatusCode::FAILURE;
    //-----------------
    // add items  *** Note: Documentation is in the header file, doxygen and wikis! ***
    // 
    // event info:
    tree->Branch("RunNumber",           &m_runNumber         );
    tree->Branch("EventNumber",         &m_eventNumber       );
    tree->Branch("TrackID",             &m_TrackID           );
    tree->Branch("IterationIndex",      &m_iterIndex         );
    tree->Branch("nHits",               &m_nHits             );
    tree->Branch("Chi2overNdof",        &m_Rec_chi2overNdof  );
    tree->Branch("Ndof",                &m_ndof              );
    tree->Branch("FitterStatusCode",    &m_fitStatusCode     );
    tree->Branch("TrackFitAuthor",      &m_trackFitAuthor    );
    tree->Branch("TrackSeedAuthor",     &m_trackSeedAuthor   );
    tree->Branch("TrackParticleHypo",   &m_particleHypothesis);
    
    ATH_MSG_VERBOSE ("added branches to ntuple");
    return StatusCode::SUCCESS;
}

//////////////////////////////////////
/// fill track data into variables without actually writing the record
//////////////////////////////////////
StatusCode Trk::TrackInformationNtupleTool::fillTrackData (
    const Trk::Track& track,
    const int iterationIndex,
    const unsigned int fitStatCode ) const {

  ATH_MSG_VERBOSE ("in fillTrackData(trk, indx)");
  // ---------------------------------------
  // detect new event, reset Track counter if new event
  SG::ReadHandle<xAOD::EventInfo> evt(m_evt);
  if(!evt.isValid()) {
    msg(MSG::WARNING) << "Could not retrieve event info" << endmsg;
    m_runNumber   = (int)s_errorEntry;
    m_eventNumber = (int)s_errorEntry;
  } else {
    if (m_lastEventNumber!=evt->eventNumber()) {
      // we have a new event, reset TrackID:
      m_TrackIDcounter = 0;
      m_lastEventNumber = evt->eventNumber();
    }
    // ---------------------------------------------
    // track id (increase if a new iteration was started = iterationIndex==0)
    if (iterationIndex == 0) m_TrackIDcounter++;
    m_TrackID = m_TrackIDcounter;
    m_iterIndex = iterationIndex;
    m_eventNumber = evt->eventNumber();
    m_runNumber   = evt->runNumber();
  }
  ATH_MSG_VERBOSE ("Event: " << m_eventNumber << ", Run: "
                   << m_runNumber  << " TrackID: " << m_TrackID 
                   << " iteration index: " << m_iterIndex);
  
  m_fitStatusCode = int(fitStatCode);
  m_trackFitAuthor = track.info().trackFitter() ;
  m_trackSeedAuthor = getSeed(track.info());
  m_particleHypothesis = int(track.info().particleHypothesis());
  if (track.fitQuality() != NULL) {
    m_Rec_chi2overNdof = (track.fitQuality()->numberDoF()>0) ? track.fitQuality()->chiSquared()/track.fitQuality()->numberDoF() : 0.0; // 0.0 expected for underconstrained fit anyway
    m_ndof=track.fitQuality()->numberDoF();
  } else {
    m_Rec_chi2overNdof = s_errorEntry;
    m_ndof             = s_errorEntry;
  }

  ATH_MSG_VERBOSE ("counts # of track states with measurement or outlier");
  // Get pointer to track state on surfaces
  const DataVector<const Trk::TrackStateOnSurface>* trackStates=track.trackStateOnSurfaces();
  m_nHits = 0;
  if (trackStates != NULL) {

    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator it=trackStates->begin();
         it!=trackStates->end();
         it++) {

      if (!(*it)) {
        msg(MSG::WARNING) << "TrackStateOnSurface == Null" << endmsg;
        continue;
      }
      if ((*it)->type(Trk::TrackStateOnSurface::Measurement) ||
          (*it)->type(Trk::TrackStateOnSurface::Outlier)     ) {

        const Trk::MeasurementBase *measurement = (*it)->measurementOnTrack();
        if (!measurement) {
          msg(MSG::WARNING) << "measurementOnTrack == Null for a TrackStateOnSurface "
                            << "of type Measurement or Outlier" << endmsg;
          return StatusCode::FAILURE;
        } else ++m_nHits;

      }
    }
  }
  return StatusCode::SUCCESS;
}

//////////////////////////////////////
/// fill trackparticle data into variables without actually writing the record
//////////////////////////////////////
StatusCode Trk::TrackInformationNtupleTool::fillTrackParticleData 
( const Trk::TrackParticleBase& particle) const
{
  ATH_MSG_VERBOSE ("in fillTrackData(trk, indx)");
  // ---------------------------------------
  // detect new event, reset Track counter if new event
  SG::ReadHandle<xAOD::EventInfo> evt(m_evt);
  if(!evt.isValid()) {
    msg(MSG::WARNING) << "Could not retrieve event info" << endmsg;
    m_runNumber   = (int)s_errorEntry;
    m_eventNumber = (int)s_errorEntry;
  } else {
    if (m_lastEventNumber!=evt->eventNumber()) {
      // we have a new event, reset TrackID:
      m_TrackIDcounter = 0;
      m_lastEventNumber = evt->eventNumber();
    }
    // ---------------------------------------------
    m_TrackID = m_TrackIDcounter++;
    m_eventNumber = evt->eventNumber();
    m_runNumber   = evt->runNumber();
  }
  ATH_MSG_VERBOSE ("Event: " << m_eventNumber << ", Run: "
                   << m_runNumber  << " TrackID: " << m_TrackID);
  
  m_trackFitAuthor = particle.info().trackFitter() ;
  m_trackSeedAuthor = getSeed(particle.info());
  m_particleHypothesis = int(particle.info().particleHypothesis());
  if (particle.fitQuality() != NULL) {
    m_Rec_chi2overNdof = (particle.fitQuality()->numberDoF()>0) ? particle.fitQuality()->chiSquared()/particle.fitQuality()->numberDoF() : 0.0; // 0.0 expected for underconstrained fit anyway
    m_ndof=particle.fitQuality()->numberDoF();
  } else {
    m_Rec_chi2overNdof = s_errorEntry;
    m_ndof             = s_errorEntry;
  }

  // fill variable nHits (more for compatibility with track-based filling)
  const Trk::TrackSummary* summary = particle.trackSummary();
  if (summary==NULL) m_nHits = -1;
  else {
    m_nHits = (unsigned char)summary->get(Trk::numberOfPixelHits);
    m_nHits += (unsigned char)summary->get(Trk::numberOfSCTHits);
    m_nHits += (unsigned char)summary->get(Trk::numberOfTRTHits);
    m_nHits += (unsigned char)summary->get(Trk::numberOfMdtHits);
    m_nHits += (unsigned char)summary->get(Trk::numberOfTgcEtaHits);
    m_nHits += (unsigned char)summary->get(Trk::numberOfTgcPhiHits);
    m_nHits += (unsigned char)summary->get(Trk::numberOfRpcEtaHits);
    m_nHits += (unsigned char)summary->get(Trk::numberOfRpcPhiHits);
    m_nHits += (unsigned char)summary->get(Trk::numberOfCscEtaHits);
    m_nHits += (unsigned char)summary->get(Trk::numberOfCscPhiHits);
  }
  return StatusCode::SUCCESS;
}

//////////////////////////////////////
// fill ntuple data of a given proto-trajectory (function used for fitter validation)
//////////////////////////////////////
StatusCode Trk::TrackInformationNtupleTool::fillProtoTrajectoryData 
(  const Trk::ProtoTrajectory& trajectory,
   const int iterationIndex,
   const Trk::Perigee*,
   const unsigned int fitStatCode) const
   //const Trk::FitterStatusCode fitStatCode) const
{
  ATH_MSG_VERBOSE ("in fillProtoTrajectoryData(protoTraj, indx)");
  SG::ReadHandle<xAOD::EventInfo> evt(m_evt);
  if(!evt.isValid()) {
    msg(MSG::ERROR) << "Could not retrieve event info" << endmsg;
    return StatusCode::FAILURE;
  }
  
  if (m_lastEventNumber!=evt->eventNumber()) {
    // we have a new event!
    // reset TrackID:
    m_TrackIDcounter = 0;
    m_lastEventNumber = evt->eventNumber();
  }
  // ---------------------------------------------
  // track id (increase if a new iteration was started = iterationIndex==0)
  if (iterationIndex == 0) {
    m_TrackIDcounter++;
  }
  m_TrackID = m_TrackIDcounter;
  m_iterIndex = iterationIndex;
  m_eventNumber = evt->eventNumber();
  m_runNumber   = evt->runNumber();

  ATH_MSG_VERBOSE ("Event: " << m_eventNumber << MSG::VERBOSE << " TrackID: " << m_TrackID << " iteration index: " << m_iterIndex);
  m_fitStatusCode = fitStatCode;

  // Loop over all proto track states on surfaces
  Trk::ProtoTrajectory::const_iterator it = trajectory.begin();
  for (; it!=trajectory.end(); it++) {
    // get the measurement            
    const Trk::MeasurementBase *measurement = it->measurement();
    if (!measurement) {
      msg(MSG::WARNING) << "ProtoTrackStateOnSurface with no measurement on proto trajectory, ignore it" << endmsg;
      continue;
    } else ++m_nHits;
  }
  return StatusCode::SUCCESS;
}

//////////////////////////////////////
// reset variables
//////////////////////////////////////
void Trk::TrackInformationNtupleTool::resetVariables() const {
  // reset the counters
  m_nHits = 0;
}


int Trk::TrackInformationNtupleTool::getSeed (const Trk::TrackInfo& info) const
{
  int seed = -1;
  if (info.patternRecoInfo(Trk::TrackInfo::Muonboy))
    seed = (int)(Trk::TrackInfo::Muonboy);
  else if (info.patternRecoInfo(Trk::TrackInfo::Moore))
    seed = (int)(Trk::TrackInfo::Moore);
  else if (info.patternRecoInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_Cosmic))
    seed = (int)(Trk::TrackInfo::SiSpacePointsSeedMaker_Cosmic);
  else if (info.patternRecoInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_BeamGas))
    seed = (int)(Trk::TrackInfo::SiSpacePointsSeedMaker_BeamGas);
  else if (info.patternRecoInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_LowMomentum))
    seed = (int)(Trk::TrackInfo::SiSpacePointsSeedMaker_LowMomentum);
  else if (info.patternRecoInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_HeavyIon))
    seed = (int)(Trk::TrackInfo::SiSpacePointsSeedMaker_HeavyIon);
  else if (info.patternRecoInfo(Trk::TrackInfo::SiSPSeededFinder))
    seed = (int)(Trk::TrackInfo::SiSPSeededFinder);
  else if (info.patternRecoInfo(Trk::TrackInfo::SiCTBTracking))
    seed = (int)(Trk::TrackInfo::SiCTBTracking);
  else if (info.patternRecoInfo(Trk::TrackInfo::InDetAmbiguitySolver))
    seed = (int)(Trk::TrackInfo::InDetAmbiguitySolver);
  else if (info.patternRecoInfo(Trk::TrackInfo::TRTSeededTrackFinder))
    seed = (int)(Trk::TrackInfo::TRTSeededTrackFinder);
  else if (info.patternRecoInfo(Trk::TrackInfo::TRTSeededSingleSpTrackFinder))
    seed = (int)(Trk::TrackInfo::TRTSeededSingleSpTrackFinder);
  else if (info.patternRecoInfo(Trk::TrackInfo::TRTStandalone))
    seed = (int)(Trk::TrackInfo::TRTStandalone);
  else if (info.patternRecoInfo(Trk::TrackInfo::iPatRec))
    seed = (int)(Trk::TrackInfo::iPatRec);
  else if (info.patternRecoInfo(Trk::TrackInfo::xKalmanLegacyCnv))
    seed = (int)(Trk::TrackInfo::xKalmanLegacyCnv);
  else if (info.patternRecoInfo(Trk::TrackInfo::SimpleAmbiguityProcessorTool))
    seed = (int)(Trk::TrackInfo::SimpleAmbiguityProcessorTool);
  else if (info.patternRecoInfo(Trk::TrackInfo::InDetAmbiTrackSelectionTool))
    seed = (int)(Trk::TrackInfo::InDetAmbiTrackSelectionTool);
  else if (info.patternRecoInfo(Trk::TrackInfo::TrigIDSCAN))
    seed = (int)(Trk::TrackInfo::TrigIDSCAN);
  else if (info.patternRecoInfo(Trk::TrackInfo::TrigSiTrack))
    seed = (int)(Trk::TrackInfo::TrigSiTrack);
  else if (info.patternRecoInfo(Trk::TrackInfo::TrigTRTxK))
    seed = (int)(Trk::TrackInfo::TrigTRTxK);
  else if (info.patternRecoInfo(Trk::TrackInfo::TrigTRTLUT))
    seed = (int)(Trk::TrackInfo::TrigTRTLUT);
  else if (info.patternRecoInfo(Trk::TrackInfo::MuidStandAlone))
    seed = (int)(Trk::TrackInfo::MuidStandAlone);
  else if (info.patternRecoInfo(Trk::TrackInfo::MuidCombined))
    seed = (int)(Trk::TrackInfo::MuidCombined);
  else if (info.patternRecoInfo(Trk::TrackInfo::MuidVertexAssociator))
    seed = (int)(Trk::TrackInfo::MuidVertexAssociator);
  else if (info.patternRecoInfo(Trk::TrackInfo::MuGirl))
    seed = (int)(Trk::TrackInfo::MuGirl);
  else if (info.patternRecoInfo(Trk::TrackInfo::MuGirlLowBeta))
    seed = (int)(Trk::TrackInfo::MuGirlLowBeta);
  else if (info.patternRecoInfo(Trk::TrackInfo::MuGirlUnrefitted))
    seed = (int)(Trk::TrackInfo::MuGirlUnrefitted);
  else if (info.patternRecoInfo(Trk::TrackInfo::STACO))
    seed = (int)(Trk::TrackInfo::STACO);
  else if (info.patternRecoInfo(Trk::TrackInfo::MuTag))
    seed = (int)(Trk::TrackInfo::MuTag);
  else if (info.patternRecoInfo(Trk::TrackInfo::Fatras))
    seed = (int)(Trk::TrackInfo::Fatras);
  else if (info.patternRecoInfo(Trk::TrackInfo::FatrasSimulation))
    seed = (int)(Trk::TrackInfo::FatrasSimulation);
  ATH_MSG_VERBOSE ("got seed "<<seed<<" for info " << info);
  return seed;
}

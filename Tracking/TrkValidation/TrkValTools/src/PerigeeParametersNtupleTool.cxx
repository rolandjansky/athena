/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// PerigeeParametersNtupleTool.cxx
//   Source file for class PerigeeParametersNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann -at- cern.ch, Wolfgang.Liebig -at- cern.ch
///////////////////////////////////////////////////////////////////
#include "TTree.h"
// Trk
#include "PerigeeParametersNtupleTool.h"
//#include "TrkValInterfaces/IMeasurementOnTrackNtupleTool.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrkTrack/Track.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkFitterUtils/ProtoTrackStateOnSurface.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkParameters/TrackParameters.h"
//Truth
#include "TrkTruthData/TrackTruth.h"
#include "AtlasHepMC/GenParticle.h"

// constructor
Trk::PerigeeParametersNtupleTool::PerigeeParametersNtupleTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
        AthAlgTool(t,n,p),
        m_extrapolator(""), // ("Trk::Extrapolator/AtlasExtrapolator"),
        m_doTruth(true),     
         m_Rec_d0{},      
         m_Rec_z0{},      
         m_Rec_phi0{},    
         m_Rec_theta{},   
         m_Rec_eta{},     
         m_Rec_qOverP{},  

         m_errord0{},     
         m_errorz0{},     
         m_errorphi0{},   
         m_errortheta0{}, 
         m_errorqoverp{}, 


         m_mc_d0{},      
         m_mc_z0{},      
         m_mc_phi0{},    
         m_mc_theta{},   
         m_mc_qOverP{},  
         m_mc_qOverPt{}, 
         m_mc_eta{},     

         m_mc_diff_d0{}, 
         m_mc_diff_z0{}, 
         m_mc_diff_phi0{},
         m_mc_diff_theta{},
         m_mc_diff_qOverP{},

         m_mc_pull_d0{},  
         m_mc_pull_z0{},  
         m_mc_pull_phi0{},
         m_mc_pull_theta{},
         m_mc_pull_qOverP{},

         m_mc_particleID{},
         m_mc_barcode{},  
         m_mc_truthTreeIndex{},
         m_mc_energy{},   

         m_mc_prob{}    

{
  declareInterface<ITrackValidationNtupleTool>(this);
  declareProperty("ExtrapolatorTool", m_extrapolator, "Extrapolator, eg for tracks without Perigee");
  declareProperty("DoTruth",          m_doTruth,      "Toggle if to Write truth data");

}

// destructor
Trk::PerigeeParametersNtupleTool::~PerigeeParametersNtupleTool() {}


///////////////////////////////////////
// initialize
///////////////////////////////////////
StatusCode Trk::PerigeeParametersNtupleTool::initialize() {

  ATH_MSG_DEBUG ("nothing specific initialized in " << name()); 
  return StatusCode::SUCCESS;
}

///////////////////////////////////////
// finalize
///////////////////////////////////////
StatusCode Trk::PerigeeParametersNtupleTool::finalize() {

  ATH_MSG_DEBUG ("nothing specific finalized in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Trk::PerigeeParametersNtupleTool::addNtupleItems( TTree* tree ) const {
    if (!tree) return StatusCode::FAILURE;
    //-----------------
    // add items  *** Note: Documentation is in the header file, doxygen and wikis! ***
    // 
    // reconstructed track parameters
    tree->Branch("RecD0",               &m_Rec_d0            );
    tree->Branch("RecZ0",               &m_Rec_z0            );
    tree->Branch("RecPhi0",             &m_Rec_phi0          );
    tree->Branch("RecTheta",            &m_Rec_theta         );
    tree->Branch("RecEta",              &m_Rec_eta           );
    tree->Branch("RecQoverP",           &m_Rec_qOverP        );
    
    // errors of reconstructed track parameters
    tree->Branch("RecErrD0",            &m_errord0           );
    tree->Branch("RecErrZ0",            &m_errorz0           );
    tree->Branch("RecErrPhi0",          &m_errorphi0         );
    tree->Branch("RecErrTheta",         &m_errortheta0       );
    tree->Branch("RecErrQoverP",        &m_errorqoverp       );

    // Truth entries
    if ( m_doTruth ){
        tree->Branch( "trk_Mc_d0",          &m_mc_d0               );
        tree->Branch( "trk_Mc_z0",          &m_mc_z0               );
        tree->Branch( "trk_Mc_phi0",        &m_mc_phi0             );
        tree->Branch( "trk_Mc_theta",       &m_mc_theta            );
        tree->Branch( "trk_Mc_qOverP",      &m_mc_qOverP           );
        tree->Branch( "trk_Mc_qOverPt",     &m_mc_qOverPt          );
        tree->Branch( "trk_Mc_eta",         &m_mc_eta              );
        
        tree->Branch( "trk_Mc_diff_d0",     &m_mc_diff_d0          );
        tree->Branch( "trk_Mc_diff_z0",     &m_mc_diff_z0          );
        tree->Branch( "trk_Mc_diff_phi0",   &m_mc_diff_phi0        );
        tree->Branch( "trk_Mc_diff_theta",  &m_mc_diff_theta       );
        tree->Branch( "trk_Mc_diff_qOverP", &m_mc_diff_qOverP      );
            
        tree->Branch( "trk_Mc_pull_d0",     &m_mc_pull_d0          );
        tree->Branch( "trk_Mc_pull_z0",     &m_mc_pull_z0          );
        tree->Branch( "trk_Mc_pull_phi0",   &m_mc_pull_phi0        );
        tree->Branch( "trk_Mc_pull_theta",  &m_mc_pull_theta       );
        tree->Branch( "trk_Mc_pull_qOverP", &m_mc_pull_qOverP      );
            
        tree->Branch( "trk_Mc_particleID",  &m_mc_particleID       );
        tree->Branch( "trk_Mc_barcode",     &m_mc_barcode          );
        tree->Branch( "trk_Mc_energy",      &m_mc_energy           );
        tree->Branch( "trk_Mc_prob",        &m_mc_prob             );
        tree->Branch( "trk_Mc_truthTreeIndex",&m_mc_truthTreeIndex );
    }

    // general track properties other than perigee are given through
    // Trk::TrackInformationNtupleTool
    
    ATH_MSG_VERBOSE ("added branches to ntuple");
    return StatusCode::SUCCESS;
}

//////////////////////////////////////
/// fill track data into variables without actually writing the record
//////////////////////////////////////
StatusCode Trk::PerigeeParametersNtupleTool::fillTrackData (
    const Trk::Track& track,
    const int  /*iterationIndex*/,
    const unsigned int /*fitStatCode*/ ) const {
    //const Trk::FitterStatusCode /*fitStatCode*/ ) const {

  ATH_MSG_VERBOSE ("in fillTrackData(trk, indx)");

  //----------------------------------------------
  // fill track parameters in ntuple
  const Trk::Perigee* perpars = track.perigeeParameters();
  if (perpars != NULL && fillTrackPerigee(perpars).isFailure()) {
    msg(MSG::WARNING) << "Perigee parameters could not be written to ntuple" << endmsg;
  }
  if (perpars == NULL) {
    if ( // track.info().author() == Trk::Track::SiSPSeededTrackFinder && 
        !m_extrapolator.empty() ) {
      ATH_MSG_VERBOSE ("try extrapolate SiSPSeeded track to perigee");
      const Trk::PerigeeSurface perSurf;
      perpars = dynamic_cast<const Trk::Perigee *>
        (m_extrapolator->extrapolate(track, perSurf, Trk::anyDirection, false, Trk::nonInteracting));
      if (perpars != NULL && fillTrackPerigee(perpars).isFailure()) {
        msg(MSG::WARNING) << "Newly made perigee parameters could not be "
                          << "written to ntuple" << endmsg;
      }
      if (perpars != NULL) delete perpars;
    } else 
      msg(MSG::WARNING) << "No perigee parameters, but they are the main validation object!"
                        << endmsg;
  }
  return StatusCode::SUCCESS;
}

//////////////////////////////////////
/// fill trackparticle data into variables without actually writing the record
//////////////////////////////////////
StatusCode Trk::PerigeeParametersNtupleTool::fillTrackParticleData 
( const Trk::TrackParticleBase& particle) const
{

  //----------------------------------------------
  // fill track parameters in ntuple
  const Trk::Perigee* perpars = particle.perigee();
  if (perpars != NULL && fillTrackPerigee(perpars).isFailure())
    ATH_MSG_WARNING ("Perigee parameters could not be written to ntuple");
  return StatusCode::SUCCESS;
}

//////////////////////////////////////
// fill ntuple data of a given proto-trajectory (function used for fitter validation)
//////////////////////////////////////
StatusCode Trk::PerigeeParametersNtupleTool::fillProtoTrajectoryData 
(  const Trk::ProtoTrajectory& trajectory,
   const int,
   const Trk::Perigee* myPerigee,
   const unsigned int ) const 
  // const Trk::FitterStatusCode) const
{
  ATH_MSG_VERBOSE ("in fillProtoTrajectoryData(protoTraj, indx)");
  
  if (myPerigee != NULL) {
    if (fillTrackPerigee(myPerigee).isFailure()) {
      ATH_MSG_WARNING ("Newly made perigee parameters could not be written to ntuple");
    }
  } else {
    ProtoTrajectory::const_iterator it = trajectory.begin();
    const Trk::TrackParameters* nearestParam   = 0;
    double smallest_perp = 999999.;
    for ( ; it!= trajectory.end(); ++it) {
      if (!it->isOutlier() && (it->smoothedTrackParameters())) {
        if (it->smoothedTrackParameters()->position().perp() < smallest_perp) {
          nearestParam = it->smoothedTrackParameters();
          smallest_perp = nearestParam->position().perp();
        }
      }
    }
    if (nearestParam!=NULL) {
      const Trk::PerigeeSurface   perSurf;
      const Trk::Perigee* perpars = dynamic_cast<const Trk::Perigee *>
        (m_extrapolator->extrapolate(*nearestParam, perSurf, Trk::anyDirection, false, Trk::pion));
      if (perpars != NULL && fillTrackPerigee(perpars).isFailure()) {
        ATH_MSG_WARNING ("Newly made perigee parameters could not be written to ntuple");
      }
      delete perpars;
    }
  }
  return StatusCode::SUCCESS;
}

//////////////////////////////////////
// reset variables
//////////////////////////////////////
void Trk::PerigeeParametersNtupleTool::resetVariables() const {
  // reset the counters
}

StatusCode Trk::PerigeeParametersNtupleTool::fillTrackTruthData ( const TrackParameters*& truePerigee, const TrackTruth& trackTruth, const int indexInTruthTree ) const
{

  if ( !m_doTruth ) return StatusCode::SUCCESS;

  ATH_MSG_VERBOSE ("Starting fillTrackTruthData()");

  const HepMcParticleLink& particleLink = trackTruth.particleLink();

  m_mc_barcode = particleLink.barcode();
  m_mc_prob=trackTruth.probability();
  m_mc_truthTreeIndex = indexInTruthTree;
  
  const HepMC::GenParticle* genParticle = particleLink.cptr();

  if ( genParticle ){
    m_mc_particleID = genParticle->pdg_id();
    m_mc_energy     = genParticle->momentum().e();
  }

  if (truePerigee == NULL) return StatusCode::SUCCESS; // fake fakes don't have true perigee

  m_mc_d0      = truePerigee->parameters()[Trk::d0];
  m_mc_z0      = truePerigee->parameters()[Trk::z0];
  m_mc_phi0    = truePerigee->parameters()[Trk::phi0];
  m_mc_theta   = truePerigee->parameters()[Trk::theta];
  m_mc_qOverP  = truePerigee->parameters()[Trk::qOverP];
  
  if ( sin( m_mc_theta ) != 0. )
    m_mc_qOverPt = m_mc_qOverP / sin( m_mc_theta );

  m_mc_eta     = truePerigee->eta();
  
  m_mc_diff_d0     = m_Rec_d0     - m_mc_d0;
  m_mc_diff_z0     = m_Rec_z0     - m_mc_z0;
  m_mc_diff_phi0   = m_Rec_phi0   - m_mc_phi0;
  m_mc_diff_theta  = m_Rec_theta  - m_mc_theta; 
  m_mc_diff_qOverP = m_Rec_qOverP - m_mc_qOverP;
  
  m_mc_pull_d0     = m_errord0     ? m_mc_diff_d0     / m_errord0     : 0.;
  m_mc_pull_z0     = m_errorz0     ? m_mc_diff_z0     / m_errorz0     : 0.;
  m_mc_pull_phi0   = m_errorphi0   ? m_mc_diff_phi0   / m_errorphi0   : 0.;
  m_mc_pull_theta  = m_errortheta0 ? m_mc_diff_theta  / m_errortheta0 : 0.;
  m_mc_pull_qOverP = m_errorqoverp ? m_mc_diff_qOverP / m_errorqoverp : 0.;

  return StatusCode::SUCCESS;

}

/////////////////////////////////////////////////////////////////////////////
/// fill the perigee in ntuple
/////////////////////////////////////////////////////////////////////////////
StatusCode Trk::PerigeeParametersNtupleTool::fillTrackPerigee(const Trk::Perigee* perigee) const {

  ATH_MSG_VERBOSE ("in fillTrackPerigee");

  //    // get track parameters
  //    const Trk::Perigee *perigee = track->perigeeParameters();
  if (!perigee) {
    msg(MSG::WARNING) << "Something is wrong - track has no perigee at all!" << endmsg;
    m_Rec_d0    = 0;
    m_Rec_z0    = 0;
    m_Rec_phi0  = 0;
    m_Rec_theta = 0;
    m_Rec_qOverP = 0;
    return StatusCode::FAILURE;
  }

  ATH_MSG_VERBOSE("Fill Perigee Parameters now");
  // track parameters
  m_Rec_d0    = perigee->parameters()[Trk::d0];
  m_Rec_z0    = perigee->parameters()[Trk::z0];
  m_Rec_phi0  = perigee->parameters()[Trk::phi0];
  m_Rec_theta = perigee->parameters()[Trk::theta];
  m_Rec_eta   = perigee->eta();
  m_Rec_qOverP = perigee->parameters()[Trk::qOverP];
  ATH_MSG_DEBUG ("Trackparameters: q/p=" << m_Rec_qOverP);

  // get measured track parameters for errors
  if (perigee->covariance()) {

    // error of track parameters
    m_errord0       = Amg::error(*perigee->covariance(),Trk::d0);
    m_errorz0       = Amg::error(*perigee->covariance(),Trk::z0);
    m_errorphi0     = Amg::error(*perigee->covariance(),Trk::phi0);
    m_errortheta0   = Amg::error(*perigee->covariance(),Trk::theta);
    m_errorqoverp   = Amg::error(*perigee->covariance(),Trk::qOverP);


  } // end if measPerigee

  // TO-DO treat condition of no measured pars

  ATH_MSG_DEBUG ("Trackparameters: d0=" << m_Rec_d0 << ", z0=" << m_Rec_z0 << ", phi0=" << m_Rec_phi0 << ", theta=" << m_Rec_theta);

  return StatusCode::SUCCESS;
}

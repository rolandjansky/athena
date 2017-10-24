///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// Author: Alexander Melzer (alexander.melzer@cern.ch)
//
 
#include "DerivationFrameworkExotics/BJetRegressionVariables.h"
#include "TLorentzVector.h"

 
namespace DerivationFramework {
 
  BJetRegressionVariables::BJetRegressionVariables(const std::string& t,
						   const std::string& n,
						   const IInterface* p) :
    AthAlgTool(t,n,p),
    m_containerName(""),
    m_assocTracksName(""),
    m_minTrackPt()
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("ContainerName", m_containerName);
    declareProperty("AssociatedTracks", m_assocTracksName);
    declareProperty("MinTrackPtCuts", m_minTrackPt);
  }

  BJetRegressionVariables::~BJetRegressionVariables(){
 }
 
  StatusCode BJetRegressionVariables::initialize()
  {
    ATH_MSG_INFO("initializing BJetRegressionVariables...");
    if (m_containerName=="") {
      ATH_MSG_ERROR("No Jet collection provided for BJetRegressionVariables!");
      return StatusCode::FAILURE;
    }
    if (m_assocTracksName=="") {
      ATH_MSG_ERROR("No TrackParticle type provided for BJetRegressionVariables!");
      return StatusCode::FAILURE;
    }
    if(m_minTrackPt.size() == 0){
      ATH_MSG_ERROR("No track pt cut provided for BJetRegressionVariables!");
      return StatusCode::FAILURE;
    }else{
      for(auto const& pTCutItr: m_minTrackPt) {
	if(pTCutItr < 500){
	  ATH_MSG_ERROR("Provided track pt cut <500 MeV for BJetRegressionVariables!");
	  return StatusCode::FAILURE;
	}
      }
    }
    return StatusCode::SUCCESS;
  }
 
  StatusCode BJetRegressionVariables::finalize()
  {
    return StatusCode::SUCCESS;
  }
 
  StatusCode BJetRegressionVariables::addBranches() const
  {
    const xAOD::JetContainer* jets = evtStore()->retrieve< const xAOD::JetContainer >( m_containerName );
    if( ! jets ) {
      ATH_MSG_ERROR ("Couldn't retrieve Jets with key: " << m_containerName );
      return StatusCode::FAILURE;
    }
    for(xAOD::JetContainer::const_iterator jetItr = jets->begin(); jetItr!=jets->end(); ++jetItr){
      std::vector<const xAOD::TrackParticle*> tracks;
      bool havetracks = (*jetItr)->getAssociatedObjects(m_assocTracksName, tracks);
      if ( ! havetracks ) ATH_MSG_WARNING("Associated tracks not found");
      ATH_MSG_DEBUG("Successfully retrieved track particles.");
      for (size_t iCut = 0; iCut < m_minTrackPt.size(); ++iCut) {
	const float minPt = m_minTrackPt[iCut];
	BJetRegressionVariables::TrackMomentStruct moments = getSumTrackPt(minPt,tracks);
	const std::string baseName = getMomentBaseName(minPt);
	SG::AuxElement::Decorator< float > scalSumPtTrk("ScalSumPtTrk"+baseName);
	SG::AuxElement::Decorator< float > vecSumPtTrk("VecSumPtTrk"+baseName);
	scalSumPtTrk( **jetItr )   = moments.vecSumPtTrk;
	vecSumPtTrk( **jetItr )    = moments.scalSumPtTrk;
      }
      
    }
    return StatusCode::SUCCESS;
  }//addBranches
  
  BJetRegressionVariables::TrackMomentStruct BJetRegressionVariables::getSumTrackPt(const float minTrackPt, const std::vector<const xAOD::TrackParticle*>& tracks) const
  {
    BJetRegressionVariables::TrackMomentStruct moments;
    TLorentzVector trackP4Sum;
    double trackPtSum = 0;

    for (size_t iTrack = 0; iTrack < tracks.size(); ++iTrack)
      {
	const xAOD::TrackParticle* track = tracks[iTrack];
	const double trackPt = track->pt();
	const TLorentzVector trackP4 = track->p4();
	
	if (trackPt < minTrackPt) {continue;}
	
	trackPtSum         += trackPt;
	trackP4Sum         += trackP4;
      }
    moments.scalSumPtTrk = trackPtSum;
    moments.vecSumPtTrk  = trackP4Sum.Pt();
    
    return moments;
    
  }//getSumTrackPt

  const std::string BJetRegressionVariables::getMomentBaseName(const float minTrackPt) const
  {
    int value = round(minTrackPt);
    if (fabs(value - minTrackPt) > 0.1)
      ATH_MSG_WARNING("Cut float and int disagree: " << minTrackPt << " float vs " << value << " int");
    
    std::ostringstream sout;
    sout << "Pt" << value;
    return sout.str();
  }
 
}//namespace

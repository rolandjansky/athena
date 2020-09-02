/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RecMomentumQualityValidation.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkValAlgs/RecMomentumQualityValidation.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkParameters/TrackParameters.h"
//#include "TrkParameters/Perigee.h"
#include "AtlasHepMC/GenParticle.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkEventUtils/IdentifierExtractor.h"


//================ Constructor =================================================

Trk::RecMomentumQualityValidation::RecMomentumQualityValidation(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_inputTrackCollection("Tracks"),
  m_trackTruthCollection("TrackTruthCollection"),
    m_truthToTrack("Trk::TruthToTrack/TruthToTrack"),
  m_trackSelector(""), //InDet::InDetTrackSelectorTool/InDetTrackSelectorTool),
  m_idHelper(0),
  m_nHundred(4),m_nFifty(4),m_nTwenty(4),m_nTen(4),m_nFakeOrLost(4),
  m_tHundred(4),m_tFifty(4),m_tTwenty(4),m_tTen(4),m_tFakeOrLost(4)
{

  // Get Parameter values from JobOptions file
  declareProperty("InputTrackCollection",       m_inputTrackCollection);
  declareProperty("TrackTruthCollection",       m_trackTruthCollection);
  declareProperty("TruthToTrackTool",           m_truthToTrack);
  declareProperty("TrackSelectorTool",          m_trackSelector);

}

//================ Destructor =================================================

Trk::RecMomentumQualityValidation::~RecMomentumQualityValidation()
{}


//================ Initialisation =================================================

StatusCode Trk::RecMomentumQualityValidation::initialize()
{

  StatusCode sc = StatusCode::SUCCESS;
  if (sc.isFailure()) { /* mute sc*/ }

  // Get the Track Selector Tool
  if ( !m_trackSelector.empty() ) {
    sc = m_trackSelector.retrieve();
    if (sc.isFailure()) {
      msg(MSG::FATAL) << "Could not retrieve "<< m_trackSelector <<" (to select the tracks which are to be counted) "<< endmsg;
      msg(MSG::INFO) << "Set the ToolHandle to None if track selection is supposed to be disabled" << endmsg;
      return sc;
    }
  }
  sc = m_truthToTrack.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not retrieve "<< m_truthToTrack << endmsg;
    return sc;
  }

  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    ATH_MSG_ERROR ("Could not get AtlasDetectorID helper" );
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO( "initialize() successful in " << name() );
  return StatusCode::SUCCESS;

}

//================ Finalisation =================================================

StatusCode Trk::RecMomentumQualityValidation::finalize()
{
  // Code entered here will be executed once at the end of the program run.

  if (msgLvl(MSG::INFO)) printTable();

  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode Trk::RecMomentumQualityValidation::execute()
{

  // Retrieving the Trackcollection specified via m_inputTrackCollection
  StatusCode sc = StatusCode::SUCCESS;
  const TrackCollection* trackCollection = 0;

  if (m_inputTrackCollection!="") {
      sc = evtStore()->retrieve(trackCollection,m_inputTrackCollection);
      if (sc.isFailure())
         ATH_MSG_ERROR( "TrackCollection "<<m_inputTrackCollection<<" not found!" );
      else
         ATH_MSG_VERBOSE("TrackCollection " << m_inputTrackCollection<<" found." );

    } else {
         ATH_MSG_ERROR("No Track collection given!" );
         return StatusCode::FAILURE;
  }

  const TrackTruthCollection* trackTruthCollection = 0;
  if (m_trackTruthCollection!="") {
    sc = evtStore()->retrieve(trackTruthCollection, m_trackTruthCollection);
    if (sc.isFailure())
      ATH_MSG_ERROR( "TruthCollection "<<m_trackTruthCollection<<" not found!" );
    else
      ATH_MSG_VERBOSE("TruthCollection " << m_trackTruthCollection<<" found." );
  } else {
    ATH_MSG_ERROR("No Truth collection given!" );
    return StatusCode::FAILURE;
  }

  // Looping over the tracks of retrieved trackcollection
  TrackCollection::const_iterator trackIterator = trackCollection->begin();
  for (;trackIterator!=trackCollection->end();++trackIterator) {

    /*    const Trk::TrackInfo& info = (*trackIterator)->info();
          if ( ! info.patternRecoInfo(Trk::TrackInfo::TRTStandalone) &&
          ! info.patternRecoInfo(Trk::TrackInfo::TRTSeededSingleSpTrackFinder)) { */
    if (!m_trackSelector.empty() && (*trackIterator)!=NULL && 
        m_trackSelector->decision(**trackIterator) ) {
      const Trk::TrackParameters* generatedTrackPerigee(0);

      // find matching truth particle
      const TrackTruth* trackTruth = 0;
      const HepMC::GenParticle* genParticle = 0;
      TrackTruthCollection::const_iterator truthIterator 
        = trackTruthCollection->find( trackIterator - (*trackCollection).begin() );
      if ( truthIterator == trackTruthCollection->end() ){
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No matching truth particle found for track" << endmsg;
      } else {
        trackTruth = &((*truthIterator).second);
        if ( !(trackTruth->particleLink().isValid()) ) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Link to generated particle information is not there - assuming a lost G4 particle ('fake fake')." << endmsg;
          //        genParticle = m_visibleParticleWithoutTruth; // with pdg_id 0
        } else {
          genParticle = trackTruth->particleLink().cptr();
          if ( genParticle!=NULL && genParticle->pdg_id() == 0 ) {
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Associated Particle ID " << genParticle->pdg_id()
                                                    << " does not conform to PDG requirements... ignore it!" << endmsg;
            genParticle = 0;
          } 
        }
      }
      if (genParticle) {
        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<
          "Associated Particle ID: " << genParticle->pdg_id() << endmsg;
        // Perform extrapolation to generate perigee parameters
        if ( genParticle->production_vertex() )
          generatedTrackPerigee = m_truthToTrack->makePerigeeParameters( genParticle );
      }

      // Getting the information if a track has TRT hits or not
      bool track_has_trthits(false);
      const DataVector<const Trk::TrackStateOnSurface > *currentTSOSList
        = (*trackIterator)->trackStateOnSurfaces();
      DataVector< const Trk::TrackStateOnSurface >::const_iterator itTSOS
      = currentTSOSList->begin();
      for (;itTSOS!=currentTSOSList->end();++itTSOS) {
        const Trk::MeasurementBase* mb = (*itTSOS)->measurementOnTrack();
        Identifier id = Trk::IdentifierExtractor::extract(mb);
        if (id.is_valid() && m_idHelper->is_trt(id)) {
          track_has_trthits=true;
          break;
        }
      }
      const TrackParameters* reconstructedPerigee = (*trackIterator)->perigeeParameters();

      if (generatedTrackPerigee != NULL) {
        
        if (!reconstructedPerigee) return StatusCode::FAILURE;

        //      double this_eta = generatedTrackPerigee->eta();
        double this_eta = reconstructedPerigee->eta();
        
        double truth_over_recP = generatedTrackPerigee->parameters()[Trk::qOverP]
          / reconstructedPerigee->parameters()[Trk::qOverP];
        if ( (truth_over_recP > 0.9) && (truth_over_recP < 1.1)) {
          monitorTrackFits(m_nTen,this_eta);
          monitorTrackFits(m_nTwenty,this_eta);
          monitorTrackFits(m_nFifty,this_eta);
          monitorTrackFits(m_nHundred,this_eta);
          if (track_has_trthits) {
            monitorTrackFits(m_tTen, this_eta);
            monitorTrackFits(m_tTwenty, this_eta);
            monitorTrackFits(m_tFifty, this_eta);
            monitorTrackFits(m_tHundred, this_eta);
          }
          
        } else if ( (truth_over_recP > 0.8) && (truth_over_recP < 1.2) ) {
          monitorTrackFits(m_nTwenty,this_eta);
          monitorTrackFits(m_nFifty,this_eta);
          monitorTrackFits(m_nHundred,this_eta);
          if (track_has_trthits) {
            monitorTrackFits(m_tTwenty, this_eta);
            monitorTrackFits(m_tFifty, this_eta);
            monitorTrackFits(m_tHundred, this_eta);
          }
        } else if ( (truth_over_recP > 0.5) && (truth_over_recP < 1.5) ) {
          monitorTrackFits(m_nFifty,this_eta);
          monitorTrackFits(m_nHundred,this_eta);
          if (track_has_trthits) {
            monitorTrackFits(m_tFifty, this_eta);
            monitorTrackFits(m_tHundred, this_eta);
          }
        } else {
          monitorTrackFits(m_nHundred,this_eta);
          if (track_has_trthits) monitorTrackFits(m_tHundred, this_eta);
        }

        delete generatedTrackPerigee;
      } else {
        double this_eta = reconstructedPerigee->eta();
        monitorTrackFits(m_nFakeOrLost, this_eta);
        if (track_has_trthits) monitorTrackFits(m_tFakeOrLost, this_eta);
      }

    } // if it is from a certain track selector/pattern reco info

  }   //loop trackCollection end


  return StatusCode::SUCCESS;
}

//============================================================================================

void Trk::RecMomentumQualityValidation::monitorTrackFits(std::vector<unsigned int>& Ntracks,
                                         const double& eta) const {
  Ntracks[Trk::RecMomentumQualityValidation::iAll] += 1;
  if (fabs(eta) < 0.80 ) ++Ntracks[Trk::RecMomentumQualityValidation::iBarrel];
  else if (fabs(eta) < 1.60) ++Ntracks[Trk::RecMomentumQualityValidation::iTransi];
  else if (fabs(eta) < 2.40) ++Ntracks[Trk::RecMomentumQualityValidation::iEndcap];
}


void Trk::RecMomentumQualityValidation::printTable() const {

  unsigned int iw = 2;
  std::string d = "    ";
  for (unsigned int iref=100; iref<m_nHundred[iAll]; iref*=10, ++iw, d+="  ") {}
  std::cout << "---------------------------------------------------------------------------------" << std::endl;
  std::cout << "  "<< name() << " results " 
            << (m_trackSelector.empty() ? " " : "(with track selection)") << std::endl;
  std::cout << "---------------------------------------------------------------------------------" << std::endl;
  std::cout << " q/p truth vicinity -- Any "<<d<<" 50% "<<d<<" 20% "<<d<<" 10%"<<d<<"noTruth  " << std::endl;
  std::cout << "---------------------------------------------------------------------------------" << std::endl;
  std::cout << "  total (Si+TRT)  :" << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nHundred[iAll] << " ("<< std::setw(iw)<<m_tHundred[iAll]<<") "
            << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nFifty[iAll] << " ("<< std::setw(iw)<<m_tFifty[iAll]<<") "
            << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nTwenty[iAll] << " ("<< std::setw(iw)<<m_tTwenty[iAll]<<") "
            << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nTen[iAll] << " ("<< std::setw(iw)<<m_tTen[iAll]<<") "
            << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nFakeOrLost[iAll] << " ("<< std::setw(iw)<<m_tFakeOrLost[iAll]<<") "
            << std::endl;
  std::cout << "  barrel (Si+TRT) :" << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nHundred[iBarrel] << " ("<< std::setw(iw)<<m_tHundred[iBarrel]<<") "
            << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nFifty[iBarrel] << " ("<< std::setw(iw)<<m_tFifty[iBarrel]<<") "
            << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nTwenty[iBarrel] <<" ("<< std::setw(iw)<<m_tTwenty[iBarrel]<<") "
            << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nTen[iBarrel] << " ("<< std::setw(iw)<<m_tTen[iBarrel]<<") "
            << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nFakeOrLost[iBarrel] << " ("<< std::setw(iw)<<m_tFakeOrLost[iBarrel]<<") "
            << std::endl;
  std::cout << "  transition      :" << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nHundred[iTransi] << " ("<< std::setw(iw)<<m_tHundred[iTransi]<<") "
            << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nFifty[iTransi] << " ("<< std::setw(iw)<<m_tFifty[iTransi]<<") "
            << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nTwenty[iTransi] <<" ("<< std::setw(iw)<<m_tTwenty[iTransi]<<") "
            << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nTen[iTransi] << " ("<< std::setw(iw)<<m_tTen[iTransi]<<") "
            << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nFakeOrLost[iTransi] << " ("<< std::setw(iw)<<m_tFakeOrLost[iTransi]<<") "
            << std::endl;
  std::cout << "  endcap (Si+TRT) :" << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nHundred[iEndcap] << " ("<< std::setw(iw)<<m_tHundred[iEndcap]<<") "
            << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nFifty[iEndcap] << " ("<< std::setw(iw)<<m_tFifty[iEndcap]<<") "
            << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nTwenty[iEndcap] <<" ("<< std::setw(iw)<<m_tTwenty[iEndcap]<<") "
            << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nTen[iEndcap] << " ("<< std::setw(iw)<<m_tTen[iEndcap]<<") "
            << std::setiosflags(std::ios::dec) << std::setw(iw+1)
            << m_nFakeOrLost[iEndcap] << " ("<< std::setw(iw)<<m_tFakeOrLost[iEndcap]<<") "
            << std::endl;
  std::cout << "---------------------------------------------------------------------------------" << std::endl << std::endl;
  return;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkInDet/EGammaTracksThinning.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "FourMomUtils/P4Helpers.h"

DerivationFramework::EGammaTracksThinning::EGammaTracksThinning(const std::string& type, 
                                                    const std::string& name, 
                                                    const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_thinningSvc( "ThinningSvc", name ),
  m_tracksCollectionName("Tracks"),
  m_electronsContainerName("Electrons"),
  m_photonsContainerName("Photons"),
  m_dr(0.5),
  m_minEtEg(0)
{
  declareInterface<DerivationFramework::IThinningTool>(this);
  declareProperty("ThinningService", m_thinningSvc);
  declareProperty("tracksCollectionName" , m_tracksCollectionName = "Tracks");
  declareProperty("electronContainerName", m_electronsContainerName = "Electrons");
  declareProperty("photonContainerName"  , m_photonsContainerName   = "Photons");
  declareProperty("deltaR"               , m_dr=0.5);
  declareProperty("minEtEg"              , m_minEtEg = 0 );
}

StatusCode DerivationFramework::EGammaTracksThinning::initialize(){

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::EGammaTracksThinning::finalize(){

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::EGammaTracksThinning::doThinning() const {

  // retrieve the tracks collection

  const TrackCollection* trackCollection = evtStore()->retrieve< const TrackCollection >( m_tracksCollectionName );
  if ( !trackCollection ) 
    {
      ATH_MSG_WARNING( "Container '" << m_tracksCollectionName  
                       << "' could not be retrieved from StoreGate !" 
                       << " Will NOT use this container for thinning in all subsequent processing!!!"
                       << " Please fix your job options..." );
      return StatusCode::FAILURE;
    }
  else
    {
      ATH_MSG_DEBUG( "Container '" << m_tracksCollectionName << "' retrieved from StoreGate" );      
    }

  // retrieve the electron collection

  const xAOD::ElectronContainer* electronContainer = evtStore()->retrieve < const xAOD::ElectronContainer > (m_electronsContainerName);
  if ( !electronContainer )
    {
      ATH_MSG_WARNING( "Container '" << m_electronsContainerName 
                       << "' could not be retrieved from StoreGate!" ); 
      return StatusCode::FAILURE;
    } 
  else
    {
      ATH_MSG_DEBUG( "Container '" << m_electronsContainerName
                     << "' retrieved from StoreGate" );
    }

   // retrieve the photon collection
  
  const xAOD::PhotonContainer* photonContainer = evtStore()->retrieve < const xAOD::PhotonContainer > (m_photonsContainerName);
  if ( !photonContainer )
    {
      ATH_MSG_WARNING( "Container '" << m_photonsContainerName 
                       << "' could not be retrieved from StoreGate!" ); 
      return StatusCode::FAILURE;
    } 
  else
    {
      ATH_MSG_DEBUG( "Container '" << m_photonsContainerName
                     << "' retrieved from StoreGate" );
    }

  // loop over electrons and for each electron mark the interesting tracks

  std::set<int> goodTrackIDs;

  xAOD::ElectronContainer::const_iterator eleItr    = electronContainer->begin();
  xAOD::ElectronContainer::const_iterator eleItrEnd = electronContainer->end();
  for (; eleItr != eleItrEnd; ++eleItr)
    {
      if ( (*eleItr)->pt() > m_minEtEg ) 
        {
          
          ATH_MSG_DEBUG(  "Electron at eta = " << (*eleItr)->eta() << " phi = " << (*eleItr)->phi() );          
          
          // get the list of tracks associated to this object

          std::set<int> eg_trackList =  findGoodTracks(trackCollection, (*eleItr)->p4(), m_dr );

          // update the global list of tracks to keep

          goodTrackIDs.insert(eg_trackList.begin() , eg_trackList.end() ) ;
          
        }
      
    }
  
  // loop over photons and for each photon mark the interesting tracks

  xAOD::PhotonContainer::const_iterator phoItr    = photonContainer->begin();
  xAOD::PhotonContainer::const_iterator phoItrEnd = photonContainer->end();
  for (; phoItr != phoItrEnd; ++phoItr)
    {
      if ( (*phoItr)->pt() > m_minEtEg ) 
        {
          
          ATH_MSG_DEBUG(  "Photon at eta = " << (*phoItr)->eta() << " phi = " << (*phoItr)->phi() );          
          
          // get the list of tracks associated to this object

          std::set<int> eg_trackList =  findGoodTracks(trackCollection, (*phoItr)->p4(), m_dr );

          // update the global list of tracks to keep

          goodTrackIDs.insert(eg_trackList.begin() , eg_trackList.end() ) ;
          
        }
      
    }
    
  // now do the real thinning and keep all marked tracks

  ATH_CHECK( thinTracks( trackCollection, goodTrackIDs ) );
  
  ATH_MSG_DEBUG( "Track thinning : tracks = " << trackCollection->size() 
                << " accepted = " << goodTrackIDs.size() ); 
  
  return StatusCode::SUCCESS;
}

// ========================================================
// For agiven electron/photon candidate this method
// marks the interesing tracks
// =======================================================

std::set<int> DerivationFramework::EGammaTracksThinning::findGoodTracks(const TrackCollection* trackCont,
                                                               const TLorentzVector& candHepLorentz, 
                                                               double maxDeltaR ) const 
{

  std::set<int> goodTracks;

  TrackCollection::const_iterator trackItr    = trackCont->begin();
  TrackCollection::const_iterator trackItrEnd = trackCont->end();
  long int i=0; // This is the counter...
  for (; trackItr != trackItrEnd; ++trackItr)
    {
      const Trk::Track* track = (*trackItr);
      
      if (!track){ continue; }	

      // use track eta/phi at perigee

      double trketa = 0.,trkphi = 0;
      const Trk::Perigee* startPerigee = track->perigeeParameters();
      trketa = startPerigee->eta();
      trkphi = startPerigee->parameters()[Trk::phi0];
  
      double deltaEta = trketa - candHepLorentz.Eta() ;
      double deltaPhi = P4Helpers::deltaPhi(trkphi, candHepLorentz.Phi() );
      double deltaR = sqrt(deltaEta*deltaEta+deltaPhi*deltaPhi);
    
      ATH_MSG_DEBUG( "Thin Tracks: eta = " << trketa << "  phi = " << trkphi << " deltaR = " << deltaR );       	 
      if ( deltaR < maxDeltaR )
        {
          goodTracks.insert( i );
          ATH_MSG_DEBUG( "  Track in the cone, adding: " << " eta=" << trketa << " phi="    << trkphi );
        } 
     
      ++i; // Increment the counter
            
    } 
  
  return goodTracks;

}

// ========================================================
// Once the list of tracks is built this method does the real
// thinning
// =======================================================

StatusCode DerivationFramework::EGammaTracksThinning::thinTracks( const TrackCollection * trackCollection , 
                                                                  const std::set<int>& goodTrackIDs ) const
{
  
  ATH_MSG_DEBUG( "==> thinTracks " << name() << "..." );
  
  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  // First, create the mask to be used for thinning
  
  std::vector<bool> mask(trackCollection->size());
  
  // Create also some bookkeeping counters
  unsigned long nTotal  = 0;
  unsigned long nKeep   = 0;
  unsigned long nReject = 0;
  
  // loop over tracks
  
  for ( std::size_t i=0; i<trackCollection->size(); ++i )
    {
      ++nTotal;
      if ( goodTrackIDs.find( i ) != goodTrackIDs.end() )
        {
          mask[i] = true;
          ++nKeep;
        }
      else
        {
          mask[i] = false;
          ++nReject;
        }
    } // End loop over Tracks

  // Write out a statistics message
  ATH_MSG_DEBUG( " EGammaTracksThinning statistics: tracks processed " << nTotal 
                << " kept = " << nKeep
                << " rejected " << nReject );
  
  // Perform the actual thinning
  ATH_CHECK( m_thinningSvc->filter(*trackCollection, mask, IThinningSvc::Operator::Or) );
    
  ///Return
  return sc;    
    
}

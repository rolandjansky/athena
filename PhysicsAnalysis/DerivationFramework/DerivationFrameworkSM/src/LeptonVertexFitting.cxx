#include "DerivationFrameworkSM/LeptonVertexFitting.h"

#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODEventInfo/EventInfo.h"
#include <vector>
#include <string>

namespace DerivationFramework {

  LeptonVertexFitting::LeptonVertexFitting( const std::string& t,
					    const std::string& n,
					    const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_sgName(""),
    m_vtxTool(0)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty( "ElectronContainerName" , m_electronContainerName = "", "Name of the Electron container  " );
    declareProperty( "UseGSFTracks"          , m_useGSFTracks        = true, "Use pion or GSF tracks for ele  " );
    declareProperty( "MuonContainerName"     , m_muonContainerName     = "", "Name of the Muon container      " );
    declareProperty( "TrackContainerName"    , m_trackContainerName    = "", "Name of the Track container     " );
    declareProperty( "DecorationPrefix"      , m_sgName );
    declareProperty( "VtxFittingTool"        , m_vtxTool                   , "The configured TrkToLeptonPVTool" );
  }

  StatusCode LeptonVertexFitting::addBranches() const
  {
        

    // Get the EventInfo object
    const xAOD::EventInfo* evtInfo = evtStore()->retrieve< const xAOD::EventInfo >("EventInfo");
    if( !evtInfo ) {
      ATH_MSG_ERROR ("Couldn't retrieve EventInfo " );
      return StatusCode::FAILURE;
    }

    // The code does a vertex fit using electron and muon tracks, and for each track in a track container fits another vertex using electron, muon tracks and that track
    // - we decorate vertex z, chi2 and n.d.f of the pure lepton vertex to the event info
    SG::AuxElement::Decorator<float> decor_leptonVtx_z     ( m_sgName + "LeptonVtx_z"     ); 
    SG::AuxElement::Decorator<float> decor_leptonVtx_chi2  ( m_sgName + "LeptonVtx_chi2"  ); 
    SG::AuxElement::Decorator<float> decor_leptonVtx_ndf   ( m_sgName + "LeptonVtx_ndf"   ); 

    // - we decorate the same information to each track 
    SG::AuxElement::Decorator<float> decor_leptrkVtx_z     ( m_sgName + "LepTrkVtx_z"     ); 
    SG::AuxElement::Decorator<float> decor_leptrkVtx_chi2  ( m_sgName + "LepTrkVtx_chi2"  ); 
    SG::AuxElement::Decorator<float> decor_leptrkVtx_ndf   ( m_sgName + "LepTrkVtx_ndf"   ); 


    // Get the electron and muon containers
    const xAOD::IParticleContainer* electrons = evtStore()->retrieve< const xAOD::IParticleContainer >( m_electronContainerName );
    if( !electrons ) {
      ATH_MSG_ERROR ("Couldn't retrieve IParticles with key: " << m_electronContainerName );
      return StatusCode::FAILURE;
    }
    const xAOD::IParticleContainer* muons = evtStore()->retrieve< const xAOD::IParticleContainer >( m_muonContainerName );
    if( !muons ) {
      ATH_MSG_ERROR ("Couldn't retrieve IParticles with key: " << m_muonContainerName );
      return StatusCode::FAILURE;
    }


    // Loop the electron and muon containers and put the track particles in a vector
    std::vector<const xAOD::TrackParticle*> leptons;
    
    // Write mask for each element and record to SG for subsequent selection
    for ( xAOD::IParticleContainer::const_iterator eleItr = electrons->begin(); eleItr!=electrons->end(); ++eleItr ) {

      xAOD::Type::ObjectType type = (*eleItr)->type();
      if ( type!=xAOD::Type::Electron ) {
	ATH_MSG_ERROR ("Wrong particle type (not an electron) being passed to LeptonVertexFitting");
	return StatusCode::FAILURE;
      }

      const xAOD::Electron* e = static_cast<const xAOD::Electron*>(*eleItr);
      if ( !passElectronSelection(e) ) {
	continue;
      }
      const xAOD::TrackParticle* trk = m_useGSFTracks ? e->trackParticle() : xAOD::EgammaHelpers::getOriginalTrackParticle(e);
      leptons.push_back(trk);

    }
    for ( xAOD::IParticleContainer::const_iterator muItr = muons->begin(); muItr!=muons->end(); ++muItr ) {

      xAOD::Type::ObjectType type = (*muItr)->type();
      if ( type!=xAOD::Type::Muon ) {
	ATH_MSG_ERROR ("Wrong particle type (not a muon) being passed to LeptonVertexFitting");
	return StatusCode::FAILURE;
      }

      const xAOD::Muon* m = static_cast<const xAOD::Muon*>(*muItr);
      if ( !passMuonSelection(m) ) {
	continue;
      }
      // Get the track particle in the following order
      //   1. CombinedTrackParticle
      //   2. InnerDetectorTrackParticle
      //   3. ExtrapolatedMuonSpectrometerTrackParticle
      //   4. MSOnlyExtrapolatedMuonSpectrometerTrackParticle
      //   5. MuonSpectrometerTrackParticle    
      const xAOD::TrackParticle* trk = *m->primaryTrackParticleLink();
      leptons.push_back(trk);

    }

    if ( leptons.size() < 2 ) { // we need at least two leptons to fit a vertex, and cuts are chosen to be looser than at analysis level
      ATH_MSG_DEBUG(" Less than two leptons passed to vertex fit ");
      decor_leptonVtx_z   (*evtInfo) = -999.;
      decor_leptonVtx_chi2(*evtInfo) =  999.;
      decor_leptonVtx_ndf (*evtInfo) =    0.;
      return StatusCode::SUCCESS;
    }

    // fit the lepton vertex with Vadim's tool
    std::unique_ptr<const xAOD::Vertex> leptonVtx = std::move(m_vtxTool->npartVertex(leptons));
    
    if ( leptonVtx ) {
      decor_leptonVtx_z   (*evtInfo) = leptonVtx->z();
      decor_leptonVtx_chi2(*evtInfo) = leptonVtx->chiSquared();
      decor_leptonVtx_ndf (*evtInfo) = leptonVtx->numberDoF();
    } else {
      ATH_MSG_WARNING("Problems in vertex fit for leptons ");
      decor_leptonVtx_z   (*evtInfo) = -999.;
      decor_leptonVtx_chi2(*evtInfo) =  999.;
      decor_leptonVtx_ndf (*evtInfo) =    0.;
      return StatusCode::SUCCESS;
    }


    // Loop the tracks
    const xAOD::TrackParticleContainer* tracks = evtStore()->retrieve< const xAOD::TrackParticleContainer>(m_trackContainerName);
    
    if(!tracks) {
      ATH_MSG_DEBUG("Couldn't retrieve TrackParticleContainer with key " << m_trackContainerName);
      return StatusCode::FAILURE;
    }

    int nTracks = 0;
    for ( xAOD::TrackParticleContainer::const_iterator trackItr=tracks->begin(); trackItr!=tracks->end(); ++trackItr ) {
      
      const xAOD::TrackParticle* trk = (*trackItr);
      std::vector<const xAOD::TrackParticle*> tracks = leptons;
      tracks.push_back(trk);

      std::unique_ptr<const xAOD::Vertex> leptrkVtx = std::move(m_vtxTool->npartVertex(tracks));
      
      if ( leptrkVtx ) {
	decor_leptrkVtx_z   (**trackItr) = leptrkVtx->z();
	decor_leptrkVtx_chi2(**trackItr) = leptrkVtx->chiSquared();
	decor_leptrkVtx_ndf (**trackItr) = leptrkVtx->numberDoF();
      } else {
	ATH_MSG_INFO ("Problems in vertex fit. Our z positions are " << trk->z0() << " for the track and " << leptonVtx->z() << " for the lepton vtx ");
	decor_leptrkVtx_z   (**trackItr) = -999.;
	decor_leptrkVtx_chi2(**trackItr) =   1e8;
	decor_leptrkVtx_ndf (**trackItr) =    0.;
      }
      
      nTracks++;
      
    }
    
    ATH_MSG_DEBUG( "Fitted vertices made of " << leptons.size() << " leptons (chi2/ndf = " << leptonVtx->chiSquared() << "/" << leptonVtx->numberDoF() << ") and " << nTracks << " tracks " );
    return StatusCode::SUCCESS;

  }

  bool LeptonVertexFitting::passElectronSelection(const xAOD::Electron* electron) const {

    if( !electron ) return false;

    // OQ
    const bool passOQ = electron->isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON);
    if ( !passOQ ) return false;

    // pT>15 GeV
    const double pT = electron->pt();
    if ( pT<15000. ) return false;

    // eta<2.47 excluding crack
    const xAOD::CaloCluster* cluster = electron->caloCluster();
    if ( !cluster ) {
      ATH_MSG_ERROR("Couldn't get the CaloCluster for this electron");
      return false;
    }
    const double absEta = std::abs(cluster->eta());
    const bool passEta = ( absEta < 1.37 || ( 1.52 < absEta && absEta < 2.47 ) );
    if ( !passEta ) return false;
  
    // medium LH ID
    bool passID = false;
    // in derivation production we decorate the DFCommon variable to the electron
    // this tool should run after the e/y tools
    if ( !electron->passSelection( passID, "DFCommonElectronsLHMedium") ) { 

      // if the tool runs before the e/y tools we take the ID decision run in the reco
      if ( !electron->passSelection( passID, "LHMedium") ) {
	ATH_MSG_WARNING("Problems obtaining electron ID decision");
	return false;
      }
    }

    // return the decision
    return passID;

  }

  bool LeptonVertexFitting::passMuonSelection(const xAOD::Muon* muon) const {

    if( !muon ) return false;

    // pT>15 GeV
    const double pT = muon->pt();
    if ( pT<15000. ) return false;

    // eta<2.4
    const double absEta = std::abs(muon->eta());
    if ( absEta>2.4 ) return false;
 
    // for muons we only check for the derivation framework preselection and goodmuon flags
    bool passID = true;
    if ( muon->isAvailable<char>("DFCommonMuonsPreselection") ) {
      passID = passID && bool(muon->auxdata<char>("DFCommonMuonsPreselection"));
    } else {
      return false;
    }
    if ( muon->isAvailable<char>("DFCommonGoodMuon") ) {
      passID = passID && bool(muon->auxdata<char>("DFCommonGoodMuon"));
    } else {
      return false;
    }

    // return the decision
    return passID;

  }

}

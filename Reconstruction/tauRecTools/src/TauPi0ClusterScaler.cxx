/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauPi0ClusterScaler.cxx
// package:     Reconstruction/tauRec
// authors:     Stephanie Yuen, Benedict Winter, Will Davey
// date:        2014-08-04
//
//-----------------------------------------------------------------------------

#include <vector>

#include "TauPi0ClusterScaler.h"
#include "xAODPFlow/PFO.h"

//#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "xAODTau/TauJet.h"
#include "tauRecTools/ITauToolBase.h"

using std::vector;
using std::string;

//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

TauPi0ClusterScaler::TauPi0ClusterScaler( const string& name ) :
    TauRecToolBase(name)
    , m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool")
    , m_chargedPFOContainer(0)
    , m_chargedPFOContainerName("TauChargedParticleFlowObjects")
    , m_chargedPFOAuxStore(0)
{
    declareProperty("ParticleCaloExtensionTool", m_caloExtensionTool);
    declareProperty("ChargedPFOContainerName", m_chargedPFOContainerName); 
    declareProperty("runOnAOD", m_AODmode=false);
    declareProperty("storeCaloSamplings", m_storeCaloSamplings=true);
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------

TauPi0ClusterScaler::~TauPi0ClusterScaler()
{
}


StatusCode TauPi0ClusterScaler::initialize()
{
    // retrieve tools
    ATH_MSG_DEBUG( "Retrieving tools" );
    CHECK( m_caloExtensionTool.retrieve() );

    return StatusCode::SUCCESS;
}

StatusCode TauPi0ClusterScaler::eventInitialize() {

    //---------------------------------------------------------------------
    // Create charged PFO container
    //---------------------------------------------------------------------
  if(!m_AODmode){
    m_chargedPFOContainer = new xAOD::PFOContainer();
    m_chargedPFOAuxStore = new xAOD::PFOAuxContainer();
    m_chargedPFOContainer->setStore(m_chargedPFOAuxStore);
    CHECK( evtStore()->record(m_chargedPFOContainer, m_chargedPFOContainerName ) );
    CHECK( evtStore()->record( m_chargedPFOAuxStore, m_chargedPFOContainerName + "Aux." ) );
  }
  else{
    CHECK( evtStore()->retrieve(m_chargedPFOContainer, m_chargedPFOContainerName) );
    CHECK( evtStore()->retrieve( m_chargedPFOAuxStore, m_chargedPFOContainerName + "Aux." ) );
  }

  //Check if TauTracks have sampling decorations
  const xAOD::TauTrackContainer* tauTracks = 0;
  ATH_CHECK( evtStore()->retrieve(tauTracks, "TauTracks") );
  for( const xAOD::TauTrack* trk : *tauTracks ){
    if( trk->isAvailable<float>("CaloSamplingEtaEM") ) {
      m_caloSamplingsStored = true;
      break;
    }
    m_caloSamplingsStored = false;
  }

  return StatusCode::SUCCESS;

}

StatusCode TauPi0ClusterScaler::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}


StatusCode TauPi0ClusterScaler::execute(xAOD::TauJet& pTau)
{
    // Clear vector of cell-based charged PFO Links. Required when rerunning on xAOD level.
    pTau.clearProtoChargedPFOLinks();

    //incase tau is rejected, still fill vector of samplings
    //so that TauTracks are consistently decorated
    //---------------------------------------------------------------------
    // get tau tracks
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // prepare extrapolation of tracks to calo layers 
    //---------------------------------------------------------------------
    // clear vectors related to track extrapolation
    // have to do that for each tau so it cannot be done in eventInitialize,
    // which is called once per event (and not once per tau)
    m_tracksEtaAtSampling.clear();
    m_tracksPhiAtSampling.clear();
    vector<const xAOD::TauTrack*> tracks;
    for(xAOD::TauTrack* track : pTau.allTracks()){
      
      float extrap_eta_EM, extrap_phi_EM, extrap_eta_Had, extrap_phi_Had;

      if(m_caloSamplingsStored==false) {
	//no decorations, so do calo extrapolations
	int sampling_EM, sampling_Had;
	if(fabs(track->eta())<1.45) sampling_EM = CaloSampling::EMB2;//2
	else sampling_EM = CaloSampling::EME2;//6
	if(fabs(track->eta())<1.0) sampling_Had = CaloSampling::TileBar1;
	else if(fabs(track->eta())<1.5) sampling_Had = CaloSampling::TileExt1;
	else sampling_Had = CaloSampling::HEC1;

	getExtrapolatedPositions(track, sampling_EM, extrap_eta_EM, extrap_phi_EM);
	getExtrapolatedPositions(track, sampling_Had, extrap_eta_Had, extrap_phi_Had);	
	if(m_storeCaloSamplings) {
	  //store the values in tracks
	  track->setDetail(xAOD::TauJetParameters::CaloSamplingEtaEM, extrap_eta_EM);
	  track->setDetail(xAOD::TauJetParameters::CaloSamplingPhiEM, extrap_phi_EM);
	  track->setDetail(xAOD::TauJetParameters::CaloSamplingEtaHad, extrap_eta_Had);
	  track->setDetail(xAOD::TauJetParameters::CaloSamplingPhiHad, extrap_phi_Had);
	}
      }
      else {
	//no need to perform calo extrapolation
	track->detail(xAOD::TauJetParameters::CaloSamplingEtaEM, extrap_eta_EM );
	track->detail(xAOD::TauJetParameters::CaloSamplingPhiEM, extrap_phi_EM );
	track->detail(xAOD::TauJetParameters::CaloSamplingEtaHad, extrap_eta_Had);
	track->detail(xAOD::TauJetParameters::CaloSamplingPhiHad, extrap_phi_Had);
      }
      if(track->flag(xAOD::TauJetParameters::classifiedCharged)) {
	//now fill the extrapolated values in the v<v<float> >
	tracks.push_back(track);
	m_tracksEtaAtSampling.push_back({extrap_eta_EM, extrap_eta_Had});
	m_tracksPhiAtSampling.push_back({extrap_phi_EM, extrap_phi_Had});
      }
    }
    

    //---------------------------------------------------------------------
    // only run on 1-5 prong taus 
    //---------------------------------------------------------------------
    if (pTau.nTracks() == 0 || pTau.nTracks() >5 ) {
        return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("ClusterScaler: new tau. \tpt = " << pTau.pt() << "\teta = " << pTau.eta() << "\tphi = " << pTau.phi() << "\tnprongs = " << pTau.nTracks());
    
    //---------------------------------------------------------------------
    // get energy in HCal associated to the different tracks
    //---------------------------------------------------------------------
    vector<vector<ElementLink<xAOD::IParticleContainer> > > hadPFOLinks;
    vector<double> EestInEcal = this->getEstEcalEnergy(tracks,pTau, hadPFOLinks);

    //---------------------------------------------------------------------
    // Create charged PFOs
    //---------------------------------------------------------------------
    for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
        const xAOD::TrackParticle* track = tracks.at(iTrack)->track();
        xAOD::PFO* chargedPFO = new xAOD::PFO();
        m_chargedPFOContainer->push_back(chargedPFO);
        ElementLink<xAOD::TrackParticleContainer> myTrackLink = pTau.trackNonConst(iTrack)->trackLinks()[0];
        if(!chargedPFO->setTrackLink(myTrackLink)) ATH_MSG_WARNING("Could not add Track to PFO");
        chargedPFO->setCharge(track->charge());
        chargedPFO->setP4(track->pt(),track->eta(),track->phi(),track->m());
        // Create element link from tau to charged PFO
        ElementLink<xAOD::PFOContainer> PFOElementLink;
        PFOElementLink.toContainedElement( *m_chargedPFOContainer, chargedPFO );
        // Create element links to hadronic PFOs associated to tracks
        if(!chargedPFO->setAssociatedParticleLinks(xAOD::PFODetails::TauShot,hadPFOLinks.at(iTrack)))
            ATH_MSG_WARNING("Couldn't add hadronic PFO links to charged PFO!");
        pTau.addProtoChargedPFOLink( PFOElementLink );
    }

    //---------------------------------------------------------------------
    // get closest EM cluster to track for subtraction
    //---------------------------------------------------------------------
    vector<float> smallestDRSquared;
    vector<int> closestCluster;
    for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
        // The following vectors will store information on the clusters closest to the tracks
        smallestDRSquared.push_back( 0.0016 ); // TODO: retune, for now match clusters that are closer than DR=0.04 to the track
        closestCluster.push_back(-1);
    }

    int thisCluster = 0;
    unsigned nNeutPFO = pTau.nProtoNeutralPFOs();
    for(unsigned int iNeutPFO=0; iNeutPFO<nNeutPFO; iNeutPFO++, thisCluster++) {
        const xAOD::PFO* curNeutPFO_const = pTau.protoNeutralPFO( iNeutPFO );

        for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
            if(EestInEcal.at(iTrack)<0.001) continue; // No need to subtract

            TLorentzVector extTrack;
            extTrack.SetPtEtaPhiE(tracks.at(iTrack)->pt(), m_tracksEtaAtSampling.at(iTrack).at(0), m_tracksPhiAtSampling.at(iTrack).at(0), tracks.at(iTrack)->e());
            // get eta/phi distance of cell to track
            double deltaEta = extTrack.Eta()-curNeutPFO_const->eta();
            double deltaPhi = TVector2::Phi_mpi_pi( extTrack.Phi() - curNeutPFO_const->phi());;

            double deltaRToTrack_squared = deltaEta*deltaEta+deltaPhi*deltaPhi;
            ATH_MSG_DEBUG("Track number " << iTrack << ", extTrack.Eta() = " << extTrack.Eta() << ", extTrack.Phi() = " << extTrack.Phi());

            if(deltaRToTrack_squared>=smallestDRSquared.at(iTrack)) continue;
            smallestDRSquared.at(iTrack) = deltaRToTrack_squared;
            closestCluster.at(iTrack) = thisCluster;
        }
    }

    //---------------------------------------------------------------------
    // Set cluster energies to what they originally were before subtraction, in case subtraction is rerun on xAOD level
    //---------------------------------------------------------------------
    for(unsigned int iNeutPFO=0; iNeutPFO<nNeutPFO; iNeutPFO++) {
        const xAOD::PFO* curNeutPFO_const = pTau.protoNeutralPFO( iNeutPFO );
        xAOD::PFO* curNeutPFO = const_cast<xAOD::PFO*>(curNeutPFO_const);
        //float originalE = 0;
        //curNeutPFO->attribute(xAOD::PFODetails::PFOAttributes::protoasymmetryInEM1WRTTrk,originalE);
        //curNeutPFO->setP4(originalE/TMath::CosH(curNeutPFO->eta()), curNeutPFO->eta(), curNeutPFO->phi(), curNeutPFO->m());

        // Retrieve original 4momentum from the cluster linked to the PFO
        double originalPt  = curNeutPFO->cluster(0)->pt();
        double originalEta = curNeutPFO->cluster(0)->eta(); 
        double originalPhi = curNeutPFO->cluster(0)->phi();
        double originalM   = curNeutPFO->cluster(0)->m();
        //ATH_MSG_INFO("PFO 4mom: pt << "<<curNeutPFO->pt()<<"\teta"<<curNeutPFO->eta()<<"\tphi"<<curNeutPFO->phi()<<"\tm"<<curNeutPFO->m()<<"\te"<<curNeutPFO->e()             );
        //ATH_MSG_INFO("Cl  4mom: pt << "<<originalPt      <<"\teta"<<originalEta      <<"\tphi"<<originalPhi      <<"\tm"<<originalM <<"\te"<<curNeutPFO->cluster(0)->e()<<"\n");
        curNeutPFO->setP4(originalPt, originalEta, originalPhi, originalM);
    }

    //---------------------------------------------------------------------
    // scale charged pion energy out of closest EMNeutPFO to track
    //---------------------------------------------------------------------
    thisCluster = 0;
    for(unsigned int iNeutPFO=0; iNeutPFO<nNeutPFO; iNeutPFO++, thisCluster++) {
        const xAOD::PFO* curNeutPFO_const = pTau.protoNeutralPFO( iNeutPFO );
        xAOD::PFO* curNeutPFO = const_cast<xAOD::PFO*>(curNeutPFO_const);

        float originalE = curNeutPFO->e();
        //ATH_MSG_INFO("iNeutPFO " << iNeutPFO << "\t before sub " << originalE/1000);
        //Scale out charged pion energy for each track
        for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
            if(thisCluster!=closestCluster.at(iTrack)) continue; // No EMNeutPFO found
            if(EestInEcal.at(iTrack)<0.001) continue;   // No need to subtract
            if(originalE - EestInEcal.at(iTrack) >0. ){
                float newE = originalE - EestInEcal.at(iTrack);
                curNeutPFO->setP4(newE/TMath::CosH(curNeutPFO->eta()) , curNeutPFO->eta(), curNeutPFO->phi(), curNeutPFO->m());
                //ATH_MSG_INFO("should subtract " << EestInEcal.at(iTrack)/1000 << " GeV from EMNeutPFO " << iNeutPFO);
                //float orgE = curNeutPFO->cluster(0)->e();
                //ATH_MSG_INFO("orgE = " << orgE << "\tPFO->e() = " << curNeutPFO->e());
            }
            // Set it to something that will make the EMNeutPFO fail the pt cut
            else {
              curNeutPFO->setP4(100 , curNeutPFO->eta(), curNeutPFO->phi(), curNeutPFO->m());
              //ATH_MSG_INFO("should remove EMNeutPFO " << iNeutPFO);
            }
        }
        //ATH_MSG_INFO("iNeutPFO " << iNeutPFO << "\t after sub " << curNeutPFO->e()/1000);
        //ATH_MSG_INFO("");
    }

    ATH_MSG_DEBUG("End of TauPi0ClusterScaler::execute");

    return StatusCode::SUCCESS;
}

void TauPi0ClusterScaler::getExtrapolatedPositions(
    const xAOD::TauTrack* track,
    int sampling, float& extrap_eta, float& extrap_phi)
{
  extrap_eta=-10;
  extrap_phi=-10;
  ATH_MSG_DEBUG( "Try extrapolation of track with pt = " << track->pt() << ", eta " << track->eta() << ", phi" << track->phi()
		 << " to layer " << sampling);
  const Trk::CaloExtension* caloExtension = 0;
  if (!m_caloExtensionTool->caloExtension(*track->track(),caloExtension)
      || caloExtension->caloLayerIntersections().size() < (unsigned int)(sampling+1)) return;
  // store if track extrapolation successful, only use entry layer
  const Trk::TrackParameters* param_at_calo = caloExtension->caloLayerIntersections().at(sampling);
  if (param_at_calo) {
    ATH_MSG_DEBUG( "Extrapolated track with eta=" << track->eta()
		   << " phi="<<track->phi()
		   << " to eta=" << param_at_calo->position().eta()
		   << " phi="<<param_at_calo->position().phi()
		   );
    extrap_eta=param_at_calo->position().eta();
    extrap_phi=param_at_calo->position().phi();
  }
  else ATH_MSG_DEBUG("Could not extrapolate track with pt = " << track->pt() << ", eta " << track->eta() << ", phi" << track->phi()
		       << " to layer " << sampling);
}

vector<double> TauPi0ClusterScaler::getEstEcalEnergy(
    vector<const xAOD::TauTrack*> tracks,
    const xAOD::TauJet& pTau,
    vector<vector<ElementLink<xAOD::IParticleContainer> > >& hadPFOLinks)
{
    // Vector that stores hadronic energy and PFO links associated to tracks
    vector<ElementLink<xAOD::IParticleContainer> > emptyLinkVector;
    vector<double> EHcal;
        for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
        EHcal.push_back(0.);
        hadPFOLinks.push_back(emptyLinkVector);
    }

    ATH_MSG_DEBUG("new tau. eta = " << pTau.eta() << "\t phi = " << pTau.phi() );

    //int PFONumber = -1;
    unsigned nHadPFO = pTau.nHadronicPFOs();
    for(unsigned int iHadPFO=0; iHadPFO<nHadPFO; iHadPFO++){
        const xAOD::PFO* curHadPFO = pTau.hadronicPFO( iHadPFO );
        
        /*
        // For debugging...
        PFONumber++;
        double deltaEtaToTau = pTau.eta()-curHadPFO->eta();
        double deltaPhiToTau = TVector2::Phi_mpi_pi( pTau.phi() - curHadPFO->phi());

        double deltaRToTau_squared = deltaEtaToTau*deltaEtaToTau+deltaPhiToTau*deltaPhiToTau;
        ATH_MSG_INFO("PFO number << " << PFONumber << "\t energy = " << curHadPFO->e() << "\t eta = " << curHadPFO->eta() << "\t phi = " << curHadPFO->phi() <<
                     "\t deltaEtaToTau = " << deltaEtaToTau << "\t deltaPhiToTau = " << deltaPhiToTau << "\t deltaRToTau_squared = " << deltaRToTau_squared );
        */


        // Assign PFO to track
        int closestTrack = -1;
        double dRToClosestTrack_squared = 0.16; // XXX can be tuned later
        for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
	    const xAOD::TrackParticle* track = tracks.at(iTrack)->track();

            // set extrapolated track direction
            TLorentzVector extTrack;
            extTrack.SetPtEtaPhiE(track->pt(), m_tracksEtaAtSampling.at(iTrack).at(1), m_tracksPhiAtSampling.at(iTrack).at(1), track->e());

            // get eta/phi distance of cell to track
            double deltaEta = extTrack.Eta()-curHadPFO->eta();
            double deltaPhi = TVector2::Phi_mpi_pi( extTrack.Phi() - curHadPFO->phi());;
            double deltaRToTrack_squared = deltaEta*deltaEta+deltaPhi*deltaPhi;

            if(deltaRToTrack_squared>=dRToClosestTrack_squared) continue;
            closestTrack = iTrack;
            dRToClosestTrack_squared = deltaRToTrack_squared;
        }
        if(closestTrack == -1){
            //ATH_MSG_INFO("dRToClosestTrack_squared = " << dRToClosestTrack_squared << ". Skip PFO for Hcal estimate. \tPFO_energy = " << curHadPFO->e()/1000.);
            continue; // Didn't find a track
        }
        EHcal.at(closestTrack) += curHadPFO->e();

        ElementLink<xAOD::PFOContainer> hadPFOElementLink = pTau.hadronicPFOLinks().at(iHadPFO);
        ElementLink<xAOD::IParticleContainer> hadElementLink;
        hadPFOElementLink.toPersistent();
        hadElementLink.resetWithKeyAndIndex( hadPFOElementLink.persKey(), hadPFOElementLink.persIndex() );
        if (!hadElementLink.isValid()) ATH_MSG_WARNING("Created an invalid element link to xAOD::PFO");
        hadPFOLinks.at(closestTrack).push_back(hadElementLink);
        //ATH_MSG_INFO("PFO associated to track " << closestTrack << "\tPFO_energy = " << curHadPFO->e()/1000. << "\tEHcal.at(closestTrack) = " << EHcal.at(closestTrack) );
    }
    // Get energy estimate in ECAL
    vector<double> E_ests;
    for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
        double E_est = tracks.at(iTrack)->e()-EHcal.at(iTrack);
        // energy cant be less than 0
        if(E_est < 0) E_est = 0.;
        // energy cant be more than track momentum
        if(E_est > tracks.at(iTrack)->e()) E_est = tracks.at(iTrack)->e();
        E_ests.push_back(E_est);
    }
    return E_ests;
}


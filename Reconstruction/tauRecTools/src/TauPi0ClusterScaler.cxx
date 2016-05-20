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
    // Create vector with default values
    for (int layer = 0 ; layer != CaloCell_ID::FCAL0; ++layer) {
        m_defaultValues.push_back(-10.);
    }

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

    //---------------------------------------------------------------------
    // only run on 1-5 prong taus 
    //---------------------------------------------------------------------
    if (pTau.nTracks() == 0 || pTau.nTracks() >5 ) {
        return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("ClusterScaler: new tau. \tpt = " << pTau.pt() << "\teta = " << pTau.eta() << "\tphi = " << pTau.phi() << "\tnprongs = " << pTau.nTracks());

    //---------------------------------------------------------------------
    // get tau tracks
    //---------------------------------------------------------------------
    vector<const xAOD::TrackParticle*> tracks;
    for(unsigned iTrack = 0; iTrack<pTau.nTracks();++iTrack){
        const xAOD::TrackParticle* track = pTau.track(iTrack);
        tracks.push_back(track);
    }

    //---------------------------------------------------------------------
    // prepare extrapolation of tracks to calo layers 
    //---------------------------------------------------------------------
    // clear vectors related to track extrapolation
    // have to do that for each tau so it cannot be done in eventInitialize,
    // which is called once per event (and not once per tau)
    m_tracksEtaAtSampling.clear();
    m_tracksPhiAtSampling.clear();
    m_extrapolatedSamplings.clear();
    // Fill with default values
    for(int layer = 0 ; layer != CaloCell_ID::FCAL0; ++layer) {
         m_extrapolatedSamplings.push_back(false);
    }
    for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
        m_tracksEtaAtSampling.push_back( m_defaultValues );
        m_tracksPhiAtSampling.push_back( m_defaultValues );
    }

    //---------------------------------------------------------------------
    // get energy in HCal associated to the different tracks
    //---------------------------------------------------------------------
    vector<vector<ElementLink<xAOD::IParticleContainer> > > hadPFOLinks;
    vector<double> EestInEcal = this->getEstEcalEnergy(tracks,pTau, hadPFOLinks);

    //---------------------------------------------------------------------
    // Create charged PFOs
    //---------------------------------------------------------------------
    for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
        const xAOD::TrackParticle* track = tracks.at(iTrack);
        xAOD::PFO* chargedPFO = new xAOD::PFO();
        m_chargedPFOContainer->push_back(chargedPFO);
        ElementLink<xAOD::TrackParticleContainer> myTrackLink = pTau.trackLinks().at(iTrack);
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
        int maxESample = 2;
        if (fabs(curNeutPFO_const->eta()) > 1.45) maxESample = 6;
        // check if tracks have been extrapolated to this sampling. Do so if this is not the case
        if(m_extrapolatedSamplings.at(maxESample)==false){
           this->getExtrapolatedPositions(tracks,maxESample);
           m_extrapolatedSamplings.at(maxESample)=true;
        }

        for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
            if(EestInEcal.at(iTrack)<0.001) continue; // No need to subtract

            TLorentzVector extTrack;
            extTrack.SetPtEtaPhiE(tracks.at(iTrack)->pt(), m_tracksEtaAtSampling.at(iTrack).at(maxESample), m_tracksPhiAtSampling.at(iTrack).at(maxESample), tracks.at(iTrack)->e());
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
    vector<const xAOD::TrackParticle*> tracks,
    int sampling)
{
    for (unsigned iTrack = 0 ; iTrack < tracks.size(); ++iTrack ) {
        // get the extrapolation into the calo
        ATH_MSG_DEBUG( "Try extrapolation of track with pt = " << tracks.at(iTrack)->pt() << ", eta " << tracks.at(iTrack)->eta() << ", phi" << tracks.at(iTrack)->phi()
                      << " to layer " << sampling);
        const Trk::CaloExtension* caloExtension = 0;
        if (!m_caloExtensionTool->caloExtension(*tracks.at(iTrack),caloExtension)
            || caloExtension->caloLayerIntersections().size() < (unsigned int)(sampling+1)) return;

        // store if track extrapolation successful, only use entry layer
        const Trk::TrackParameters* param_at_calo = caloExtension->caloLayerIntersections().at(sampling);
        if (param_at_calo) {
            ATH_MSG_DEBUG( "Extrapolated track with eta=" << tracks.at(iTrack)->eta()
                            << " phi="<<tracks.at(iTrack)->phi()
                            << " to eta=" << param_at_calo->position().eta()
                            << " phi="<<param_at_calo->position().phi()
                            );
            m_tracksEtaAtSampling.at(iTrack).at(sampling)=param_at_calo->position().eta();
            m_tracksPhiAtSampling.at(iTrack).at(sampling)=param_at_calo->position().phi();
        }
        else ATH_MSG_DEBUG("Could not extrapolate track with pt = " << tracks.at(iTrack)->pt() << ", eta " << tracks.at(iTrack)->eta() << ", phi" << tracks.at(iTrack)->phi()
                          << " to layer " << sampling);
    }
}

vector<double> TauPi0ClusterScaler::getEstEcalEnergy(
    vector<const xAOD::TrackParticle*> tracks,
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

        // Decide which sampling to extrapolate to. Choose Hcal samplings that usually contain most energy (|eta| dependent)
        int                                   sample = 13; //         |eta| <= 1.0
        if     ( fabs(curHadPFO->eta())>1.5 ) sample =  9; //  1.5 <  |eta| 
        else if( fabs(curHadPFO->eta())>1.0 ) sample = 19; //  1.0 <  |eta| <= 1.5

        // check if tracks have been extrapolated to this sampling. Do so if this is not the case
        if(m_extrapolatedSamplings.at(sample)==false){
           this->getExtrapolatedPositions(tracks,sample);
           ATH_MSG_DEBUG("Extrapolate to layer " << sample << "\teta = "
                         << m_tracksEtaAtSampling.at(0).at(sample) << "\t phi = " << m_tracksPhiAtSampling.at(0).at(sample) );
           m_extrapolatedSamplings.at(sample)=true;
        }

        // Assign PFO to track
        int closestTrack = -1;
        double dRToClosestTrack_squared = 0.16; // XXX can be tuned later
        for(unsigned iTrack = 0; iTrack<tracks.size();++iTrack){
            const xAOD::TrackParticle* track = tracks.at(iTrack);

            // set extrapolated track direction
            TLorentzVector extTrack;
            extTrack.SetPtEtaPhiE(track->pt(), m_tracksEtaAtSampling.at(iTrack).at(sample), m_tracksPhiAtSampling.at(iTrack).at(sample), track->e());

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


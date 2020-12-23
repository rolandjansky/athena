/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
//  TrigMuonEFInfoToMuonCnvTool.cxx
//
//  Created by Mark Owen on 12/03/2014.
//

#include "TrigMuonEFInfoToMuonCnvTool.h"

#include "AthLinks/ElementLink.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

/**
 * Constructor - should declare any property settings in here.
 */
TrigMuonEFInfoToMuonCnvTool::TrigMuonEFInfoToMuonCnvTool(const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type,name,parent)
{
  declareInterface<ITrigMuonEFInfoToMuonCnvTool>(this);

  declareProperty( "MakeIDTrackLink", m_makeIDTrackLink = true);
  declareProperty( "xAODEFInDetTrackParticleContainerName", m_inDetTrackParticles = "EFInDetTrackParticles" );   
}

/**
 * Initialize the tool
 */
StatusCode TrigMuonEFInfoToMuonCnvTool::initialize() {
    
    if(m_makeIDTrackLink) ATH_MSG_INFO("Will make links to EF ID xAOD track particles");
    else ATH_MSG_INFO("Will not make links to EF ID xAOD track particles");
    ATH_MSG_INFO("EF ID track particle container name: " << m_inDetTrackParticles);
    
    return StatusCode::SUCCESS;
}

/**
 * Convert input TrigMuonEFInfo object into multuple xAOD::Muon objects.
 * The xAOD::Muon objects are stored into the MuonContainer passed to the function.
 * Since the xAOD::Muons hold links to xAOD::TrackParticles, then these TrackParticles are created
 * and stored in the containers passed to the function (if not 0).
 */
StatusCode TrigMuonEFInfoToMuonCnvTool::convertTrigMuonEFInfo(const TrigMuonEFInfo& efinfo,
                                             xAOD::MuonContainer& muoncontainer,
                                             xAOD::TrackParticleContainer* combParticleContainer,
                                             xAOD::TrackParticleContainer* extrapParticleContainer) const {

    ATH_MSG_DEBUG("Start conversion of TrigMuonEFInfo, n(muons) to convert = " << efinfo.TrackContainer()->size());
    const int nmuin = muoncontainer.size();
    // loop on TrigMuonEFInfoTracks attached to this TrigMuonEFInfo object
    for( auto infotrk : *(efinfo.TrackContainer()) ) {
        
        if( !infotrk->hasExtrapolatedTrack() && !infotrk->hasCombinedTrack() ){
            ATH_MSG_WARNING("TrigMuonEFInfoTrack has no extrapolated or combined track, will not be converted to xAOD");
            continue;
        }
        
        // TrigMuonEFInfoTracks are essentially muons, so create an xAOD::Muon at this point and put it in the container
        xAOD::Muon* muon = new xAOD::Muon();
        muoncontainer.push_back(muon);
        
        // make extrapolated TrackParticle and fill it
        if(infotrk->hasExtrapolatedTrack()) {
            const TrigMuonEFTrack* extraptrk = infotrk->ExtrapolatedTrack();
            xAOD::TrackParticle* extrapTrackPart = new xAOD::TrackParticle();
            if(extrapParticleContainer) extrapParticleContainer->push_back( extrapTrackPart );
            else extrapTrackPart->makePrivateStore();
            
            StatusCode sc = convertTrigMuonEFTrack( *extraptrk, *extrapTrackPart );
            if(sc.isFailure()) return sc;
            
	    // create and set element link to the xAOD::TrackParticle
	    if(extrapParticleContainer) {
	      ElementLink<xAOD::TrackParticleContainer> link( *extrapParticleContainer, extrapParticleContainer->size() - 1 );
	      muon->setTrackParticleLink( xAOD::Muon::MuonSpectrometerTrackParticle, link );
	    }

            // set main muon 4-vector to the extrapolated track
            muon->setP4(extraptrk->pt(),extraptrk->eta(),extraptrk->phi());
            // set some author flags - note this relies on the 'knowledge' that we only ran muid at EF before 2015
            muon->addAllAuthor(xAOD::Muon::MuidSA);
            muon->setAuthor(xAOD::Muon::MuidSA);
            muon->setMuonType(xAOD::Muon::MuonStandAlone);
	    
	    // clean up the track particle in case we did not put it in a container
	    if(!extrapParticleContainer) delete extraptrk;
        }
        
        // make combined TrackParticle object and fill it
        if(infotrk->hasCombinedTrack()) {
            const TrigMuonEFCbTrack* combtrk = infotrk->CombinedTrack();
            xAOD::TrackParticle* combinedTrackPart = new xAOD::TrackParticle();
            if(combParticleContainer) combParticleContainer->push_back( combinedTrackPart );
            else combinedTrackPart->makePrivateStore();
            
            StatusCode sc = convertTrigMuonEFTrack( *combtrk, *combinedTrackPart );
            if(sc.isFailure()) return sc;
            
	    // create and set element link to the xAOD::TrackParticle
	    if(combParticleContainer) {
	      ElementLink<xAOD::TrackParticleContainer> link( *combParticleContainer, combParticleContainer->size() - 1 );
	      muon->setTrackParticleLink( xAOD::Muon::CombinedTrackParticle, link );
	    }

            // set main muon 4-vector to the combined track
            muon->setP4(combtrk->pt(),combtrk->eta(),combtrk->phi());
            // set some author flags - note this relies on the 'knowledge' that we ran muid and mugirl at EF before 2015
            if( infotrk->MuonType() == TrigMuonEFInfoTrack::MuonTypeEFMuon ) {
                muon->addAllAuthor(xAOD::Muon::MuidCo);
                muon->setAuthor(xAOD::Muon::MuidCo);
                muon->setMuonType(xAOD::Muon::Combined);
            } else {
                muon->addAllAuthor(xAOD::Muon::MuGirl);
                muon->setAuthor(xAOD::Muon::MuGirl);
		// we set all MuGirl to be combined because we set the combined track, could change this once ID track link works
		muon->setMuonType(xAOD::Muon::Combined);
                //if( infotrk->MuonType() == TrigMuonEFInfoTrack::MuGirlMuonTag) muon->setMuonType(xAOD::Muon::SegmentTagged);
                //else muon->setMuonType(xAOD::Muon::Combined);
            }
            
            if(m_makeIDTrackLink) {
                // create element link to the ID track
                const ElementLink<Rec::TrackParticleContainer>& oldLink = combtrk->getIDTrackParticleLink();
                if(oldLink.isValid()) {
                    // This code assumes that the tracks in the xAOD EF ID track container are in the same order as in the Rec::TrackParticle container
                    // Same assumption was made in the offline code, 
                    ElementLink<xAOD::TrackParticleContainer> newLink;
                    std::string name = m_inDetTrackParticles;
                    if (name.empty())
                      name = oldLink.dataID();
                    newLink.resetWithKeyAndIndex(  name, oldLink.index() );
                    if(newLink.isValid()) ATH_MSG_VERBOSE("Successfully created element link to ID xAOD track particle");
                    else ATH_MSG_WARNING("Created an invalid element link to ID xAOD track particle");
                        
                } else ATH_MSG_WARNING("TrigMuonEFCbTrack with invalid element link to ID track - cannot create link for xAOD muon");
            }//create element link to ID track
            
	    // clean up the track particle in case we did not put it in a container
	    if(!combParticleContainer) delete combinedTrackPart;

        } // valid combinedTrack
        
    }// loop on TrigMuonEFInfoTracks in the TrigMuonEFInfo objects
    
    ATH_MSG_DEBUG("Conversion finished, n(muons) added to xAOD container = " << muoncontainer.size()-nmuin);
    
    return StatusCode::SUCCESS;
    
}// end convertTrigMuonEFInfo function

/// Convert full TrigMuonEFInfoContainer into xAOD::MuonContainer
StatusCode TrigMuonEFInfoToMuonCnvTool::convertTrigMuonEFInfoContainer(const TrigMuonEFInfoContainer& efinfocont,
                                                  xAOD::MuonContainer& muoncontainer,
                                                  xAOD::TrackParticleContainer* combParticleContainer,
                                                  xAOD::TrackParticleContainer* extrapParticleContainer) const {
    //loop on TrigMunEFInfo objects and convert them
    for(auto efinfo : efinfocont) {
        StatusCode sc = convertTrigMuonEFInfo( *efinfo, muoncontainer, combParticleContainer, extrapParticleContainer );
        if(sc.isFailure()) return sc;
    }
    
    return StatusCode::SUCCESS;
}// end convertTrigMuonEFInfoContainer

/**
 * Fill xAOD::TrackParticle with values from TrigMuonEFTrack
 */
StatusCode TrigMuonEFInfoToMuonCnvTool::convertTrigMuonEFTrack(const TrigMuonEFTrack& eftrack, xAOD::TrackParticle& trackpart) const {
    double p = 0.0001;
    const double charge = eftrack.charge();
    if( std::abs(eftrack.p()) < p ) {
        ATH_MSG_WARNING("TrigMuonEFTrack has p close to zero = " << eftrack.p() << "set to " << p);
    } else {
        p = std::abs(eftrack.p());
    }
    const double qoverp = charge / p;
    double theta = atan(eftrack.tanTh());
    if(theta < 0.0)  theta += M_PI ;  // atan returns theta in -pi/2 -> pi/2, so convert to 0 -> pi

    trackpart.setDefiningParameters( eftrack.d0(), eftrack.z0(), eftrack.phi(), theta, qoverp);
    trackpart.setParametersOrigin( eftrack.posX(), eftrack.posY(), eftrack.posZ() );

    /*if(abs(eftrack.pt() - trackpart.pt())> 10.0) {
      if(eftrack.pt() < 1000000.0) {
	ATH_MSG_ERROR("pT mismatch, parameters are:");
	ATH_MSG_ERROR("input d0, z0, phi, theta, qoverp = " << eftrack.d0() << ", " << eftrack.z0() << ", " << eftrack.phi() << ", " << theta << ", " << qoverp);
	ATH_MSG_ERROR("old e, pt, eta, phi, m = " << std::fixed << std::setprecision(8) << eftrack.e() << ", " << eftrack.pt() << ", " << eftrack.eta() << ", " << eftrack.phi() << ", " << eftrack.m());
	ATH_MSG_ERROR("new e, pt, eta, phi, m = " << std::fixed << std::setprecision(8) << trackpart.e() << ", " << trackpart.pt() << ", " << trackpart.eta() << ", " << trackpart.phi() << ", " << trackpart.m());
	return StatusCode::FAILURE;
      }
      }*/

    return StatusCode::SUCCESS;
} // end convertTrigMuonEFTrack function

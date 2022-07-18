/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "MuonPRDTest/MMSimHitVariables.h"

#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonSimEvent/MicromegasHitIdHelper.h"

namespace MuonPRDTest {
    MMSimHitVariables::MMSimHitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "MM_Sim", false, msglvl), m_key{container_name} {}

    bool MMSimHitVariables::declare_keys() { return declare_dependency(m_key); }

    bool MMSimHitVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fill MmSimHitVariables()");
        SG::ReadHandle<MMSimHitCollection> mmContainer{m_key, ctx};
        if (!mmContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve digit container " << m_key.fullKey());
            return false;
        }
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }
        unsigned int n_hits{0};
        // Get the MM Id hit helper
        const MicromegasHitIdHelper* mmhhelper = MicromegasHitIdHelper::GetHelper();
        if (!mmContainer->size()) ATH_MSG_DEBUG("MM Sim container is empty");
        for (const MMSimHit& hit : *mmContainer) {
            int simId = hit.MMId();

            if(hit.depositEnergy()==0.) continue; // SimHits without energy loss are not recorded. 

            // connect the hit with the MC truth
            int barcode = hit.particleLink().barcode();
            m_NSWMM_trackId.push_back(barcode);
            m_NSWMM_globalTime.push_back(hit.globalTime());

            const Amg::Vector3D& globalPosition = hit.globalPosition();
            m_NSWMM_hitGlobalPosition.push_back(globalPosition);

            const Amg::Vector3D& globalDirection = hit.globalDirection();
            m_NSWMM_hitGlobalDirection.push_back(globalDirection);

            m_NSWMM_particleEncoding.push_back(hit.particleEncoding());
            m_NSWMM_kineticEnergy.push_back(hit.kineticEnergy());
            m_NSWMM_depositEnergy.push_back(hit.depositEnergy());

            std::string stname  = mmhhelper->GetStationName(simId);
            int steta           = mmhhelper->GetZSector(simId);
            int stphi           = mmhhelper->GetPhiSector(simId);
            int multilayer      = mmhhelper->GetMultiLayer(simId);
            int layer           = mmhhelper->GetLayer(simId);
            int side            = mmhhelper->GetSide(simId);

            if( stphi == 0 ){
                ATH_MSG_ERROR("MicroMegas validation: unexpected phi range " << stphi);
                return false;
            }

            Identifier offId = idHelperSvc()->mmIdHelper().channelID( stname[2] == 'L' ?  "MML" : "MMS",
				               side == 1 ? steta+1 : -steta-1,
				               (stphi-1)/2+1,multilayer,layer,1 );
            m_NSWMM_Id.push_back(offId);

            // sanity checks
            if( !idHelperSvc()->mmIdHelper().is_mm(offId) ){
                ATH_MSG_WARNING("MM id is not a mm id! " << idHelperSvc()->mmIdHelper().print_to_string(offId));
            }
            if( !idHelperSvc()->mmIdHelper().is_muon(offId) ){
                ATH_MSG_WARNING("MM id is not a muon id! " << idHelperSvc()->mmIdHelper().print_to_string(offId));
            }
            if( idHelperSvc()->mmIdHelper().is_mdt(offId)||idHelperSvc()->mmIdHelper().is_rpc(offId)||idHelperSvc()->mmIdHelper().is_tgc(offId)||idHelperSvc()->mmIdHelper().is_csc(offId)||idHelperSvc()->mmIdHelper().is_stgc(offId) ){
                ATH_MSG_WARNING("MM id has wrong technology type! " << idHelperSvc()->mmIdHelper().is_mdt(offId) << " " << idHelperSvc()->mmIdHelper().is_rpc(offId)
                                << " " << idHelperSvc()->mmIdHelper().is_tgc(offId) << " " << idHelperSvc()->mmIdHelper().is_csc(offId) << " " << idHelperSvc()->mmIdHelper().is_stgc(offId) );
            }
            if( idHelperSvc()->mmIdHelper().gasGap(offId) != layer ) {
                ATH_MSG_WARNING("MM id has bad layer field! " << idHelperSvc()->mmIdHelper().print_to_string(offId)  );
            }

            int isSmall = stname[2] == 'S';
            ATH_MSG_DEBUG("MicroMegas geometry, retrieving detector element for: isSmall " << isSmall << " eta " << idHelperSvc()->mmIdHelper().stationEta(offId)
                          << " phi " << idHelperSvc()->mmIdHelper().stationPhi(offId) << " ml " << idHelperSvc()->mmIdHelper().multilayer(offId) );

            const MuonGM::MMReadoutElement* detEl = MuonDetMgr->getMMReadoutElement(offId);
            if (!detEl) {
                ATH_MSG_ERROR("MMSimHitVariables::fillVariables() - Failed to retrieve MMReadoutElement for "<<idHelperSvc()->mmIdHelper().print_to_string(offId).c_str());
                return false;
            }

            // surface
            const Trk::PlaneSurface& surf = detEl->surface(offId);
            // compute hit position within the detector element/surfaces
            Amg::Transform3D     gToL = detEl->absTransform().inverse();
            Amg::Vector3D hpos(hit.globalPosition().x(),hit.globalPosition().y(),hit.globalPosition().z());
            Amg::Vector3D dSurface_pos = gToL*hpos;

            // compute the hit position on the readout plane (same as in MuonFastDigitization)
            Amg::Vector3D rSurface_pos = surf.transform().inverse()*hpos;
     
            Amg::Vector2D  posOnSurfUnProjected(rSurface_pos.x(),rSurface_pos.y());

            // check where the readout plane is located and compute the local direction accordingly 
            Amg::Vector3D ldir(0., 0., 0.);
            ldir = surf.transform().inverse().linear()*Amg::Vector3D(hit.globalDirection().x(), hit.globalDirection().y(), hit.globalDirection().z());
  
            double scale, scaletop;
            double gasgap = 5.;
   
            scale = -rSurface_pos.z()/ldir.z();
            scaletop = (gasgap+rSurface_pos.z())/ldir.z();
      	
            Amg::Vector3D hitOnSurface = rSurface_pos + scale*ldir;
            Amg::Vector3D hitOnTopSurface = rSurface_pos + scaletop*ldir;
            Amg::Vector2D posOnSurf (hitOnSurface.x(), hitOnSurface.y());
            Amg::Vector2D posOnTopSurf (hitOnTopSurface.x(),hitOnTopSurface.y());
        
  
            int stripNumber = detEl->stripNumber(posOnSurf,offId);
     
            // perform bound check (making the call from the detector element to consider edge passivation)
            m_NSWMM_isInsideBounds.push_back( detEl->insideActiveBounds(offId, posOnSurf) );

            if( stripNumber == -1 ){
                ATH_MSG_WARNING("MicroMegas validation: failed to obtain strip number " << idHelperSvc()->mmIdHelper().print_to_string(offId) );
                ATH_MSG_WARNING(" pos " << posOnSurf << " z " << rSurface_pos.z() );
                stripNumber = 1;
            }
    
            Identifier oldId = offId;
            offId = idHelperSvc()->mmIdHelper().channelID(offId, idHelperSvc()->mmIdHelper().multilayer(offId), idHelperSvc()->mmIdHelper().gasGap(offId),stripNumber);
            if( idHelperSvc()->mmIdHelper().gasGap(offId) != layer ) {
                ATH_MSG_WARNING("MicroMegas validation: MM id has bad layer field(2)! " << std::endl << " " << idHelperSvc()->mmIdHelper().print_to_string(offId) << std::endl
                                << " " << idHelperSvc()->mmIdHelper().print_to_string(oldId) << " stripN " << stripNumber );
            }

            Amg::Vector2D fastDigitPos(0.,0.);
            if( !detEl->stripPosition(offId,fastDigitPos ) ){
                ATH_MSG_WARNING("MicroMegas validation: failed to obtain local position for identifier " << idHelperSvc()->mmIdHelper().print_to_string(offId) );
            }

            Amg::Vector3D detpos = detEl->globalPosition();
            ATH_MSG_DEBUG("Global hit   : r " << hit.globalPosition().perp() << ", phi " << hit.globalPosition().phi() << ", z " << hit.globalPosition().z()
                          << "; detEl: r " << detpos.perp() << ", phi " << detpos.phi() << ", z " << detpos.z()
                          << "; surf z "   << surf.center().z() << ", ml "  << multilayer << ", l " << layer );
            ATH_MSG_DEBUG(" detEl: x " << dSurface_pos.x() << " y " << dSurface_pos.y() << " z " << dSurface_pos.z());
            ATH_MSG_DEBUG("MM Fast digit: x " << fastDigitPos.x() << " y " << fastDigitPos.y()
                          << ", gToL: x " << rSurface_pos.x() << " y " << rSurface_pos.y() << " z " << rSurface_pos.z() );

            // Fill ntuple with the hit/surface/digit positions
            m_NSWMM_detector_globalPosition.push_back(detpos);
            m_NSWMM_hitToDsurfacePosition.push_back(dSurface_pos);
            m_NSWMM_hitToRsurfacePosition.push_back(rSurface_pos);

            m_NSWMM_FastDigitRsurfacePositionX.push_back( posOnSurf.x() );
            m_NSWMM_FastDigitRsurfacePositionY.push_back( posOnSurf.y() );

            ++n_hits;
        }
        m_NSWMM_nSimHits = n_hits;

        ATH_MSG_DEBUG("processed " << m_NSWMM_nSimHits << " MM hits");
        return true;
    }
}

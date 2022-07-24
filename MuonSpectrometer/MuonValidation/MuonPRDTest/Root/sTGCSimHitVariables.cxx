/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "MuonPRDTest/sTGCSimHitVariables.h"

#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonSimEvent/sTgcHitIdHelper.h"
namespace MuonPRDTest {
    sTGCSimHitVariables::sTGCSimHitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "sTGC_Sim", false, msglvl), m_key{container_name} {}

    bool sTGCSimHitVariables::declare_keys() { return declare_dependency(m_key); }

    bool sTGCSimHitVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fill sTGCSimHitVariables()");
        SG::ReadHandle<sTGCSimHitCollection> stgcContainer{m_key, ctx};
        if (!stgcContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve SimHit container " << m_key.fullKey());
            return false;
        }
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }
        unsigned int n_hits{0};
        // Get the sTGC Id hit helper
        const sTgcHitIdHelper* stgchhelper = sTgcHitIdHelper::GetHelper();
        const Amg::Vector3D dummyVector(-9999.9, 0.0, 0.0);
        if (!stgcContainer->size()) ATH_MSG_DEBUG("sTGC Sim container is empty");
        for (const sTGCSimHit& hit : *stgcContainer) {
            if(hit.depositEnergy()==0.) continue; // SimHits without energy loss are not recorded. 

            for( int type=0;type<=2;++type ){ 
                int simId = hit.sTGCId();
                std::string stname = stgchhelper->GetStationName(simId);
                int steta          = stgchhelper->GetZSector(simId);
                int stphi          = stgchhelper->GetPhiSector(simId);
                int multilayer     = stgchhelper->GetMultiLayer(simId);
                int layer          = stgchhelper->GetLayer(simId);
                int side           = stgchhelper->GetSide(simId);

                if ( stphi==0 ) {
                    ATH_MSG_ERROR("unexpected phi range: " << stphi);
                    return false;
                }

                // Old [7/12/12] implementation of the Station Name is: T[0-3][L/S][P/C] 
                // Current implementation of the Station Name is: Q[L/S][1-3][P/C] 
                int detNumber = -999, wedgeId = -999, wedgeType = -999;
                if(stname.length()!=4) {
                    ATH_MSG_WARNING("sTGC validation: station Name exceeds 4 charactes, filling dummy information for detNumber, wedgeId and wedgeType");
                } 
                else {
                    detNumber = atoi(stname.substr(2,1).c_str());
                    wedgeId   = (stname.substr(1,1).compare("L")) ? 0 : 1;
                    wedgeType = (stname.substr(3,1).compare("P")) ? 0 : 1;
                }

                Identifier offId = idHelperSvc()->stgcIdHelper().channelID(stname[1] == 'L' ?  "STL" : "STS",
				                   side == 1 ? steta+1 : -steta-1,
				                   (stphi-1)/2+1,multilayer,layer,1,1 );
                m_NSWsTGC_Id.push_back(offId);
                std::string stName   = idHelperSvc()->stgcIdHelper().stationNameString(idHelperSvc()->stgcIdHelper().stationName(offId));
                int off_channel      = idHelperSvc()->stgcIdHelper().channel(offId);

                int isSmall = stName[2] == 'S';

                if( type == 2 && off_channel == 63) {
                    ATH_MSG_DEBUG("Found sTGC Wire Sim Hit with channel number 63 (dead region), skipping this hit");
                    continue;
                }

                const MuonGM::sTgcReadoutElement* detEl = MuonDetMgr->getsTgcReadoutElement(offId);
                if (!detEl) {
                    ATH_MSG_ERROR("sTGCSimHitVariables::fillVariables() - Failed to retrieve sTgcReadoutElement for "<<idHelperSvc()->stgcIdHelper().print_to_string(offId).c_str());
                    return false;
                }

                if( !idHelperSvc()->stgcIdHelper().is_stgc(offId) ){
                    ATH_MSG_WARNING("sTgc id is not a stgc id! " << idHelperSvc()->stgcIdHelper().print_to_string(offId));
                }
                if( !idHelperSvc()->stgcIdHelper().is_muon(offId) ){
                    ATH_MSG_WARNING("sTgc id is not a muon id! " << idHelperSvc()->stgcIdHelper().print_to_string(offId));
                }
                if( idHelperSvc()->stgcIdHelper().is_mdt(offId)||idHelperSvc()->stgcIdHelper().is_rpc(offId)||idHelperSvc()->stgcIdHelper().is_tgc(offId)||idHelperSvc()->stgcIdHelper().is_csc(offId)||idHelperSvc()->stgcIdHelper().is_mm(offId) ){
                    ATH_MSG_WARNING("sTgc id has wrong technology type! " << idHelperSvc()->stgcIdHelper().is_mdt(offId) << " " << idHelperSvc()->stgcIdHelper().is_rpc(offId)
                                    << " " << idHelperSvc()->stgcIdHelper().is_tgc(offId) << " " << idHelperSvc()->stgcIdHelper().is_csc(offId) << " " << idHelperSvc()->stgcIdHelper().is_mm(offId) );
                }
                if( idHelperSvc()->stgcIdHelper().gasGap(offId) != layer ) {
                    ATH_MSG_WARNING("sTgc id has bad layer field! " << idHelperSvc()->stgcIdHelper().print_to_string(offId)  );
                }

                // connect the hit with the MC truth
                int barcode = hit.particleLink().barcode();
                m_NSWsTGC_trackId.push_back(barcode);
                m_NSWsTGC_wedgeId.push_back(wedgeId);
                m_NSWsTGC_wedgeType.push_back(wedgeType);
                m_NSWsTGC_detectorNumber.push_back(detNumber);

                m_NSWsTGC_globalTime.push_back(hit.globalTime());
                const  Amg::Vector3D& globalPosition = hit.globalPosition();
                m_NSWsTGC_hitGlobalPosition.push_back(globalPosition);

                const  Amg::Vector3D& globalDirection = hit.globalDirection();
                m_NSWsTGC_hitGlobalDirection.push_back(globalDirection);

                m_NSWsTGC_particleEncoding.push_back(hit.particleEncoding());
                m_NSWsTGC_depositEnergy.push_back(hit.depositEnergy());
                m_NSWsTGC_kineticEnergy.push_back(hit.kineticEnergy());

                const  Amg::Vector3D& globalPrePosition = hit.globalPrePosition();
                m_NSWsTGC_hitGlobalPrePosition.push_back(globalPrePosition);
                if (hit.kineticEnergy() < 0.0) {
                    m_NSWsTGC_hitGlobalPrePosition.push_back(dummyVector);
                }

                ATH_MSG_DEBUG("sTGC geometry, retrieving detector element for: isSmall " << isSmall << " eta " << idHelperSvc()->stgcIdHelper().stationEta(offId)
                              << " phi " << idHelperSvc()->stgcIdHelper().stationPhi(offId) << " ml " << idHelperSvc()->stgcIdHelper().multilayer(offId) );

                Identifier newId = idHelperSvc()->stgcIdHelper().channelID(idHelperSvc()->stgcIdHelper().parentID(offId), idHelperSvc()->stgcIdHelper().multilayer(offId), idHelperSvc()->stgcIdHelper().gasGap(offId),type,1);
      
                // compute hit position within the detector element/surfaces
                const Trk::PlaneSurface& surf = detEl->surface(newId);
                Amg::Transform3D     gToL = detEl->absTransform().inverse();
                Amg::Vector3D hpos(hit.globalPosition().x(),hit.globalPosition().y(),hit.globalPosition().z());
                Amg::Vector3D dSurface_pos = gToL*hpos;

                // compute the hit position on the readout plane (same as in MuonFastDigitization)
                Amg::Vector3D rSurface_pos = surf.transform().inverse()*hpos;
                Amg::Vector3D ldir = surf.transform().inverse().linear()*Amg::Vector3D(hit.globalDirection().x(),hit.globalDirection().y(),hit.globalDirection().z());

                ATH_MSG_DEBUG("sTGC channel type:" << type);

                double scale = -rSurface_pos.z()/ldir.z();
                Amg::Vector3D hitOnSurface = rSurface_pos + scale*ldir;

                // hitOnSurface.x() will be susequent smeared to simulate the detector resolution, here we do not apply any smearing
                Amg::Vector2D  posOnSurf(hitOnSurface.x(), rSurface_pos.y());

                // remember whether the given hit is inside the active volume (and produces a valid digit)
                m_NSWsTGC_isInsideBounds.push_back( surf.insideBounds(posOnSurf) );

                int stripNumber = detEl->stripNumber(posOnSurf,newId);
                if( stripNumber == -1 ){
                    ATH_MSG_WARNING("sTGC validation: failed to obtain strip number " << idHelperSvc()->stgcIdHelper().print_to_string(offId) );
                    ATH_MSG_WARNING(" pos " << posOnSurf << " z " << rSurface_pos.z() );
                    //stripNumber = 1;
                }
                Identifier oldId = offId;
                offId = idHelperSvc()->stgcIdHelper().channelID(offId, idHelperSvc()->stgcIdHelper().multilayer(offId), idHelperSvc()->stgcIdHelper().gasGap(offId),1,stripNumber);
                if( idHelperSvc()->stgcIdHelper().gasGap(offId) != layer ) {
                    ATH_MSG_WARNING("sTGC validation: sTgc id has bad layer field(2)! " << std::endl << " " << idHelperSvc()->stgcIdHelper().print_to_string(offId) << std::endl
                                    << " " << idHelperSvc()->stgcIdHelper().print_to_string(oldId) << " stripN " << stripNumber );
                }

                Amg::Vector2D fastDigitPos(0,0);
                if( !detEl->stripPosition(offId,fastDigitPos) ){
                    ATH_MSG_WARNING("sTGC validation: failed to obtain local position for identifier " << idHelperSvc()->stgcIdHelper().print_to_string(offId) );
                }

                Amg::Vector3D detpos = detEl->globalPosition();
                ATH_MSG_DEBUG("sTGC Global hit: r " << hit.globalPosition().perp() << ", phi " << hit.globalPosition().phi() << ", z " << hit.globalPosition().z()
                              << "; detEl: r " << detpos.perp()     << ", phi " << detpos.phi()   << ", z " << detpos.z()
                              << "; surf z "   << surf.center().z() << ", ml "  << multilayer << ", l " << layer );

                ATH_MSG_DEBUG(" detEl: x " << dSurface_pos.x() << " y " << dSurface_pos.y() << " z " << dSurface_pos.z());
                ATH_MSG_DEBUG("sTGC Fast digit: x " << fastDigitPos.x() << " y " << fastDigitPos.y()
                              << ", gToL: x " << rSurface_pos.x() << " y " << rSurface_pos.y() << " z " << rSurface_pos.z() );

                // Fill ntuple with the hit/surface/digit positions
                m_NSWsTGC_detector_globalPosition.push_back(detpos);

                m_NSWsTGC_hitToDsurfacePosition.push_back(dSurface_pos);

                m_NSWsTGC_hitToRsurfacePosition.push_back(rSurface_pos);

                m_NSWsTGC_FastDigitRsurfacePositionX.push_back(posOnSurf.x());
                m_NSWsTGC_FastDigitRsurfacePositionY.push_back(posOnSurf.y());
                m_NSWsTGC_stripNumber.push_back(stripNumber);
            }
            ++n_hits;
        }
        m_NSWsTGC_nSimHits = n_hits;

        ATH_MSG_DEBUG("processed " << m_NSWsTGC_nSimHits << " sTgc hits");
        return true;
    }
}

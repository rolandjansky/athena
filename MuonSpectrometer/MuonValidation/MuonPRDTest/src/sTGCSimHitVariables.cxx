/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "sTGCSimHitVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonSimEvent/sTGCSimHitCollection.h"
#include "MuonSimEvent/sTgcSimIdToOfflineId.h"

#include "MuonReadoutGeometry/sTgcReadoutElement.h"

#include "TTree.h"

StatusCode sTGCSimHitVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr) 
{
  ATH_MSG_DEBUG("do fillNSWsTGCHitVariables()");

  CHECK( this->clearVariables() );

  const sTGCSimHitCollection *nswContainer = nullptr;
  CHECK( m_evtStore->retrieve(nswContainer, m_ContainerName.c_str()) );

  ATH_MSG_DEBUG("ReadMuonSimHits: Retrieved " << nswContainer->size() << " sTGC hits!" ); 

  // Get sTGC Helper
  sTgcHitIdHelper* hitHelper = sTgcHitIdHelper::GetHelper();
  sTgcSimIdToOfflineId simToOffline(m_sTgcIdHelper);
  
  if(!nswContainer->size()) ATH_MSG_DEBUG(m_ContainerName<<" container empty");
  for(const sTGCSimHit& hit : *nswContainer) {
    if(hit.depositEnergy()==0.) continue; // SimHits without energy loss are not recorded. 

    // SimHits do not have channel type.
    // Compute channel type in Val Alg to be able to validate
    for( int type=0;type<=2;++type ){ 

      // Read information about the sTGC hits; make sanity checks and printout
      int simId = hit.sTGCId(); 
      std::string sim_stationName = hitHelper->GetStationName(simId);
      int sim_stationEta          = hitHelper->GetZSector(simId);
      int sim_stationPhi          = hitHelper->GetPhiSector(simId);
      int sim_multilayer          = hitHelper->GetMultiLayer(simId);
      int sim_layer               = hitHelper->GetLayer(simId);
      int sim_side                = hitHelper->GetSide(simId);

      ATH_MSG_DEBUG(     "sTGC SimHit  id:  Station Name [" << sim_stationName << " ]"
                      << " Station Eta ["  << sim_stationEta               << "]"
                      << " Station Phi ["  << sim_stationPhi               << "]"
                      << " MultiLayer ["   << sim_multilayer               << "]"
                      << " Layer  ["       << sim_layer                    << "]"
                      << " Side ["         << sim_side                     << "]" );

      if ( sim_stationPhi==0 ) {
          ATH_MSG_ERROR("unexpected phi range: " << sim_stationPhi);
          return StatusCode::FAILURE;
      }
      // Old [7/12/12] implementation of the Station Name is: T[0-3][L/S][P/C] 
      // Current implementation of the Station Name is: Q[L/S][1-3][P/C] 
      int detNumber = -999, wedgeId = -999, wedgeType = -999;
      if(sim_stationName.length()!=4) {
        ATH_MSG_WARNING("sTGC validation: station Name exceeds 4 charactes, filling dummy information for detNumber, wedgeId and wedgeType");
      } 
      else {
        detNumber = atoi(sim_stationName.substr(2,1).c_str());
        wedgeId   = (sim_stationName.substr(1,1).compare("L")) ? 0 : 1;
        wedgeType = (sim_stationName.substr(3,1).compare("P")) ? 0 : 1;
      }


      //  convert simHit id to offline id; make sanity checks; retrieve the associated detector element.
      // For offline identifier, station Name is: STS or STL
      Identifier offId = simToOffline.convert(hit.sTGCId());
      

      std::string stName   = m_sTgcIdHelper->stationNameString(m_sTgcIdHelper->stationName(offId));
      int off_stationEta   = m_sTgcIdHelper->stationEta(offId); 
      int off_stationPhi   = m_sTgcIdHelper->stationPhi(offId);
      int off_multiplet    = m_sTgcIdHelper->multilayer(offId);
      int off_gas_gap      = m_sTgcIdHelper->gasGap(offId);
      int off_channel_type = m_sTgcIdHelper->channelType(offId);
      int off_channel      = m_sTgcIdHelper->channel(offId);

      int isSmall = stName[2] == 'S';

      if( type == 2 && off_channel == 63) {
        ATH_MSG_DEBUG("Found sTGC Wire Sim Hit with channel number 63 (dead region), skipping this hit");
        continue;
      }

      const MuonGM::sTgcReadoutElement* detEl = MuonDetMgr->getsTgcReadoutElement(offId);
      if (!detEl) {
        ATH_MSG_ERROR("sTGCSimHitVariables::fillVariables() - Failed to retrieve sTgcReadoutElement for "<<m_sTgcIdHelper->print_to_string(offId).c_str());
        return StatusCode::FAILURE;
      }

      if( !m_sTgcIdHelper->is_stgc(offId) ){
          ATH_MSG_WARNING("sTgc id is not a stgc id! " << m_sTgcIdHelper->print_to_string(offId));
      }
      if( !m_sTgcIdHelper->is_muon(offId) ){
          ATH_MSG_WARNING("sTgc id is not a muon id! " << m_sTgcIdHelper->print_to_string(offId));
      }
      if( m_sTgcIdHelper->is_mdt(offId)||m_sTgcIdHelper->is_rpc(offId)||m_sTgcIdHelper->is_tgc(offId)||m_sTgcIdHelper->is_csc(offId)||m_sTgcIdHelper->is_mm(offId) ){
          ATH_MSG_WARNING("sTgc id has wrong technology type! " << m_sTgcIdHelper->is_mdt(offId) << " " << m_sTgcIdHelper->is_rpc(offId)
                          << " " << m_sTgcIdHelper->is_tgc(offId) << " " << m_sTgcIdHelper->is_csc(offId) << " " << m_sTgcIdHelper->is_mm(offId) );
      }
      if( m_sTgcIdHelper->gasGap(offId) != sim_layer ) {
          ATH_MSG_WARNING("sTgc id has bad layer field! " << m_sTgcIdHelper->print_to_string(offId)  );
      }
      // connect the hit with the MC truth
      int barcode = hit.particleLink().barcode();
      m_NSWsTGC_trackId.push_back(barcode);

      m_NSWsTGC_globalTime.push_back(hit.globalTime());
      const  Amg::Vector3D& globalPosition = hit.globalPosition();
      m_NSWsTGC_hitGlobalPositionX.push_back(globalPosition.x());
      m_NSWsTGC_hitGlobalPositionY.push_back(globalPosition.y());
      m_NSWsTGC_hitGlobalPositionZ.push_back(globalPosition.z());
      m_NSWsTGC_hitGlobalPositionR.push_back(globalPosition.perp());
      m_NSWsTGC_hitGlobalPositionP.push_back(globalPosition.phi());
      const  Amg::Vector3D& globalDirection = hit.globalDirection();
      m_NSWsTGC_hitGlobalDirectionX.push_back(globalDirection.x());
      m_NSWsTGC_hitGlobalDirectionY.push_back(globalDirection.y());
      m_NSWsTGC_hitGlobalDirectionZ.push_back(globalDirection.z());

      m_NSWsTGC_particleEncoding.push_back(hit.particleEncoding());
      m_NSWsTGC_depositEnergy.push_back(hit.depositEnergy());
      m_NSWsTGC_kineticEnergy.push_back(hit.kineticEnergy());

      const  Amg::Vector3D& globalPrePosition = hit.globalPrePosition();
      m_NSWsTGC_hitGlobalPrePositionX.push_back(globalPrePosition.x());
      m_NSWsTGC_hitGlobalPrePositionY.push_back(globalPrePosition.y());
      m_NSWsTGC_hitGlobalPrePositionZ.push_back(globalPrePosition.z());
      m_NSWsTGC_hitGlobalPrePositionR.push_back(globalPrePosition.perp());
      m_NSWsTGC_hitGlobalPrePositionP.push_back(globalPrePosition.phi());
      if (hit.kineticEnergy() < 0.0) {
        m_NSWsTGC_hitGlobalPrePositionX.push_back(-9999.9);
        m_NSWsTGC_hitGlobalPrePositionY.push_back(-9999.9);
        m_NSWsTGC_hitGlobalPrePositionZ.push_back(-9999.9);
        m_NSWsTGC_hitGlobalPrePositionR.push_back(-9999.9);
        m_NSWsTGC_hitGlobalPrePositionP.push_back(-9999.9);
      }


      // Fill Ntuple with SimId data
      m_NSWsTGC_sim_stationName   .push_back(sim_stationName);
      m_NSWsTGC_sim_stationEta    .push_back(sim_stationEta);
      m_NSWsTGC_sim_stationPhi    .push_back(sim_stationPhi);
      m_NSWsTGC_wedgeId           .push_back(wedgeId);
      m_NSWsTGC_wedgeType         .push_back(wedgeType);
      m_NSWsTGC_detectorNumber    .push_back(detNumber);
      m_NSWsTGC_sim_multilayer    .push_back(sim_multilayer);
      m_NSWsTGC_sim_layer         .push_back(sim_layer);
      m_NSWsTGC_sim_side          .push_back(sim_side);
      
      ATH_MSG_DEBUG(     "sTGC Offline id:  Station Name [" << stName << "]"
                      << " Station Eta ["  << off_stationEta      << "]"
                      << " Station Phi ["  << off_stationPhi      << "]"
                      << " Multiplet  ["   << off_multiplet       << "]"
                      << " GasGap ["       << off_gas_gap         << "]"
                      << " Type ["         << off_channel_type    << "]"
                      << " ChNr ["         << off_channel         << "]" );

      ATH_MSG_DEBUG("sTGC geometry, retrieving detector element for: isSmall " << isSmall << " eta " << m_sTgcIdHelper->stationEta(offId)
                    << " phi " << m_sTgcIdHelper->stationPhi(offId) << " ml " << m_sTgcIdHelper->multilayer(offId) );

      
      Identifier newId = m_sTgcIdHelper->channelID(m_sTgcIdHelper->parentID(offId), m_sTgcIdHelper->multilayer(offId), m_sTgcIdHelper->gasGap(offId),type,1);
      
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
        ATH_MSG_WARNING("sTGC validation: failed to obtain strip number " << m_sTgcIdHelper->print_to_string(offId) );
        ATH_MSG_WARNING(" pos " << posOnSurf << " z " << rSurface_pos.z() );
        //stripNumber = 1;
      }
      Identifier oldId = offId;
      offId = m_sTgcIdHelper->channelID(offId, m_sTgcIdHelper->multilayer(offId), m_sTgcIdHelper->gasGap(offId),1,stripNumber);
      if( m_sTgcIdHelper->gasGap(offId) != sim_layer ) {
        ATH_MSG_WARNING("sTGC validation: sTgc id has bad layer field(2)! " << std::endl << " " << m_sTgcIdHelper->print_to_string(offId) << std::endl
                        << " " << m_sTgcIdHelper->print_to_string(oldId) << " stripN " << stripNumber );
      }

      Amg::Vector2D fastDigitPos(0,0);
      if( !detEl->stripPosition(offId,fastDigitPos) ){
        ATH_MSG_WARNING("sTGC validation: failed to obtain local position for identifier " << m_sTgcIdHelper->print_to_string(offId) );
      }

      Amg::Vector3D detpos = detEl->globalPosition();
      ATH_MSG_DEBUG("sTGC Global hit: r " << hit.globalPosition().perp() << ", phi " << hit.globalPosition().phi() << ", z " << hit.globalPosition().z()
                      << "; detEl: r " << detpos.perp()     << ", phi " << detpos.phi()   << ", z " << detpos.z()
                      << "; surf z "   << surf.center().z() << ", ml "  << sim_multilayer << ", l " << sim_layer );

      ATH_MSG_DEBUG(" detEl: x " << dSurface_pos.x() << " y " << dSurface_pos.y() << " z " << dSurface_pos.z());
      ATH_MSG_DEBUG("sTGC Fast digit: x " << fastDigitPos.x() << " y " << fastDigitPos.y()
                      << ", gToL: x " << rSurface_pos.x() << " y " << rSurface_pos.y() << " z " << rSurface_pos.z() );



      // Fill Ntuple with offline ID data
      m_NSWsTGC_off_stationName   .push_back(stName);
      m_NSWsTGC_off_stationEta    .push_back(off_stationEta);                                         
      m_NSWsTGC_off_stationPhi    .push_back(off_stationPhi);
      m_NSWsTGC_off_multiplet     .push_back(off_multiplet);
      m_NSWsTGC_off_gas_gap       .push_back(off_gas_gap);
      m_NSWsTGC_off_channel_type  .push_back(type);
      // The offline IdHelper class will be updated to assign wiregroup ID to SimHit. 
      // As a temporary solution stripnumber is used directly (also in MM)
      off_channel = stripNumber;
      m_NSWsTGC_off_channel       .push_back(off_channel);


      // Fill ntuple with the hit/surface/digit positions
      m_NSWsTGC_detector_globalPositionX.push_back( detpos.x() );
      m_NSWsTGC_detector_globalPositionY.push_back( detpos.y() );
      m_NSWsTGC_detector_globalPositionZ.push_back( detpos.z() );
      m_NSWsTGC_detector_globalPositionR.push_back( detpos.perp() );
      m_NSWsTGC_detector_globalPositionP.push_back( detpos.phi() );

      m_NSWsTGC_hitToDsurfacePositionX.push_back( dSurface_pos.x() );
      m_NSWsTGC_hitToDsurfacePositionY.push_back( dSurface_pos.y() );
      m_NSWsTGC_hitToDsurfacePositionZ.push_back( dSurface_pos.z() );

      m_NSWsTGC_hitToRsurfacePositionX.push_back( rSurface_pos.x() );
      m_NSWsTGC_hitToRsurfacePositionY.push_back( rSurface_pos.y() );
      m_NSWsTGC_hitToRsurfacePositionZ.push_back( rSurface_pos.z() );

      m_NSWsTGC_FastDigitRsurfacePositionX.push_back( posOnSurf.x() );
      m_NSWsTGC_FastDigitRsurfacePositionY.push_back( posOnSurf.y() );

      

      m_NSWsTGC_stripNumber.push_back(stripNumber);
      m_NSWsTGC_wireNumber.push_back(-999);

      ATH_MSG_DEBUG("---------- Hit processing ends!");
      /*
      ATH_MSG_DEBUG(     " NSW Hits E = "      << hit.depositEnergy()
          << ", Global X sTGC = "  << globalPosition.x()
          << ", Global Y sTGC = "  << globalPosition.y()
          << ", Global Z sTGC = "  << globalPosition.z()
          << ", time = "           << hit.globalTime()
      */    
      m_NSWsTGC_nSimHits++;
    }
 }

  ATH_MSG_DEBUG("processed " << m_NSWsTGC_nSimHits << " sTGC sim hits");
  return StatusCode::SUCCESS;
}


StatusCode sTGCSimHitVariables::clearVariables() 
{
  m_NSWsTGC_nSimHits = 0;
  m_NSWsTGC_trackId.clear();
  m_NSWsTGC_isInsideBounds.clear();
  m_NSWsTGC_globalTime.clear();
  m_NSWsTGC_hitGlobalPositionX.clear();
  m_NSWsTGC_hitGlobalPositionY.clear();
  m_NSWsTGC_hitGlobalPositionZ.clear();
  m_NSWsTGC_hitGlobalPositionR.clear();
  m_NSWsTGC_hitGlobalPositionP.clear();
  m_NSWsTGC_hitGlobalDirectionX.clear();
  m_NSWsTGC_hitGlobalDirectionY.clear();
  m_NSWsTGC_hitGlobalDirectionZ.clear();
  m_NSWsTGC_hitGlobalPrePositionX.clear();
  m_NSWsTGC_hitGlobalPrePositionY.clear();
  m_NSWsTGC_hitGlobalPrePositionZ.clear();
  m_NSWsTGC_hitGlobalPrePositionR.clear();
  m_NSWsTGC_hitGlobalPrePositionP.clear();
  m_NSWsTGC_detector_globalPositionX.clear();
  m_NSWsTGC_detector_globalPositionY.clear();
  m_NSWsTGC_detector_globalPositionZ.clear();
  m_NSWsTGC_detector_globalPositionR.clear();
  m_NSWsTGC_detector_globalPositionP.clear();
  m_NSWsTGC_hitToDsurfacePositionX.clear();
  m_NSWsTGC_hitToDsurfacePositionY.clear();
  m_NSWsTGC_hitToDsurfacePositionZ.clear();
  m_NSWsTGC_hitToRsurfacePositionX.clear();
  m_NSWsTGC_hitToRsurfacePositionY.clear();
  m_NSWsTGC_hitToRsurfacePositionZ.clear();
  m_NSWsTGC_FastDigitRsurfacePositionX.clear();
  m_NSWsTGC_FastDigitRsurfacePositionY.clear();
  m_NSWsTGC_particleEncoding.clear();
  m_NSWsTGC_depositEnergy.clear();
  m_NSWsTGC_kineticEnergy.clear();
  m_NSWsTGC_sim_stationName.clear();
  m_NSWsTGC_wedgeId.clear();
  m_NSWsTGC_wedgeType.clear();
  m_NSWsTGC_detectorNumber.clear();
  m_NSWsTGC_sim_stationEta.clear();
  m_NSWsTGC_sim_stationPhi.clear();
  m_NSWsTGC_sim_multilayer.clear();
  m_NSWsTGC_sim_layer.clear();
  m_NSWsTGC_sim_side.clear();
  m_NSWsTGC_stripNumber.clear();
  m_NSWsTGC_wireNumber.clear();
  m_NSWsTGC_off_stationName.clear();
  m_NSWsTGC_off_stationEta.clear();
  m_NSWsTGC_off_stationPhi.clear();
  m_NSWsTGC_off_multiplet.clear();
  m_NSWsTGC_off_gas_gap.clear();
  m_NSWsTGC_off_channel_type.clear();
  m_NSWsTGC_off_channel.clear();
  return StatusCode::SUCCESS;
}

void sTGCSimHitVariables::deleteVariables() 
{ 
  return;
}

StatusCode sTGCSimHitVariables::initializeVariables() 
{
  if(m_tree) {
    m_tree->Branch("Hits_sTGC_n", &m_NSWsTGC_nSimHits, "Hits_sTGC_nSimHits/i");
    m_tree->Branch("Hits_sTGC_trackId", &m_NSWsTGC_trackId);
    m_tree->Branch("Hits_sTGC_isInsideBounds", &m_NSWsTGC_isInsideBounds);
    m_tree->Branch("Hits_sTGC_globalTime", &m_NSWsTGC_globalTime);
    m_tree->Branch("Hits_sTGC_hitGlobalPositionX", &m_NSWsTGC_hitGlobalPositionX);
    m_tree->Branch("Hits_sTGC_hitGlobalPositionY", &m_NSWsTGC_hitGlobalPositionY);
    m_tree->Branch("Hits_sTGC_hitGlobalPositionZ", &m_NSWsTGC_hitGlobalPositionZ);
    m_tree->Branch("Hits_sTGC_hitGlobalPositionR", &m_NSWsTGC_hitGlobalPositionR);
    m_tree->Branch("Hits_sTGC_hitGlobalPositionP", &m_NSWsTGC_hitGlobalPositionP);
    m_tree->Branch("Hits_sTGC_hitGlobalDirectionX", &m_NSWsTGC_hitGlobalDirectionX);
    m_tree->Branch("Hits_sTGC_hitGlobalDirectionY", &m_NSWsTGC_hitGlobalDirectionY);
    m_tree->Branch("Hits_sTGC_hitGlobalDirectionZ", &m_NSWsTGC_hitGlobalDirectionZ);
    m_tree->Branch("Hits_sTGC_hitGlobalPrePositionX", &m_NSWsTGC_hitGlobalPrePositionX);
    m_tree->Branch("Hits_sTGC_hitGlobalPrePositionY", &m_NSWsTGC_hitGlobalPrePositionY);
    m_tree->Branch("Hits_sTGC_hitGlobalPrePositionZ", &m_NSWsTGC_hitGlobalPrePositionZ);
    m_tree->Branch("Hits_sTGC_hitGlobalPrePositionR", &m_NSWsTGC_hitGlobalPrePositionR);
    m_tree->Branch("Hits_sTGC_hitGlobalPrePositionP", &m_NSWsTGC_hitGlobalPrePositionP);
    m_tree->Branch("Hits_sTGC_detector_globalPositionX", &m_NSWsTGC_detector_globalPositionX);
    m_tree->Branch("Hits_sTGC_detector_globalPositionY", &m_NSWsTGC_detector_globalPositionY);
    m_tree->Branch("Hits_sTGC_detector_globalPositionZ", &m_NSWsTGC_detector_globalPositionZ);
    m_tree->Branch("Hits_sTGC_detector_globalPositionR", &m_NSWsTGC_detector_globalPositionR);
    m_tree->Branch("Hits_sTGC_detector_globalPositionP", &m_NSWsTGC_detector_globalPositionP);
    m_tree->Branch("Hits_sTGC_hitToDsurfacePositionX", &m_NSWsTGC_hitToDsurfacePositionX);
    m_tree->Branch("Hits_sTGC_hitToDsurfacePositionY", &m_NSWsTGC_hitToDsurfacePositionY);
    m_tree->Branch("Hits_sTGC_hitToDsurfacePositionZ", &m_NSWsTGC_hitToDsurfacePositionZ);
    m_tree->Branch("Hits_sTGC_hitToRsurfacePositionX", &m_NSWsTGC_hitToRsurfacePositionX);
    m_tree->Branch("Hits_sTGC_hitToRsurfacePositionY", &m_NSWsTGC_hitToRsurfacePositionY);
    m_tree->Branch("Hits_sTGC_hitToRsurfacePositionZ", &m_NSWsTGC_hitToRsurfacePositionZ);
    m_tree->Branch("Hits_sTGC_FastDigitRsurfacePositionX", &m_NSWsTGC_FastDigitRsurfacePositionX);
    m_tree->Branch("Hits_sTGC_FastDigitRsurfacePositionY", &m_NSWsTGC_FastDigitRsurfacePositionY);
    m_tree->Branch("Hits_sTGC_particleEncoding", &m_NSWsTGC_particleEncoding);
    m_tree->Branch("Hits_sTGC_depositEnergy", &m_NSWsTGC_depositEnergy);
    m_tree->Branch("Hits_sTGC_kineticEnergy", &m_NSWsTGC_kineticEnergy);
    m_tree->Branch("Hits_sTGC_sim_stationName", &m_NSWsTGC_sim_stationName);
    m_tree->Branch("Hits_sTGC_wedgeId", &m_NSWsTGC_wedgeId);
    m_tree->Branch("Hits_sTGC_wedgeType", &m_NSWsTGC_wedgeType);
    m_tree->Branch("Hits_sTGC_detectorNumber", &m_NSWsTGC_detectorNumber);
    m_tree->Branch("Hits_sTGC_sim_stationEta", &m_NSWsTGC_sim_stationEta);
    m_tree->Branch("Hits_sTGC_sim_stationPhi", &m_NSWsTGC_sim_stationPhi);
    m_tree->Branch("Hits_sTGC_sim_multilayer", &m_NSWsTGC_sim_multilayer);
    m_tree->Branch("Hits_sTGC_sim_layer", &m_NSWsTGC_sim_layer);
    m_tree->Branch("Hits_sTGC_sim_side", &m_NSWsTGC_sim_side);
    m_tree->Branch("Hits_sTGC_stripNumber", &m_NSWsTGC_stripNumber);
    m_tree->Branch("Hits_sTGC_wireNumber", &m_NSWsTGC_wireNumber);
    m_tree->Branch("Hits_sTGC_off_stationName", &m_NSWsTGC_off_stationName);
    m_tree->Branch("Hits_sTGC_off_stationEta", &m_NSWsTGC_off_stationEta);                  
    m_tree->Branch("Hits_sTGC_off_stationPhi", &m_NSWsTGC_off_stationPhi);
    m_tree->Branch("Hits_sTGC_off_multiplet", &m_NSWsTGC_off_multiplet);
    m_tree->Branch("Hits_sTGC_off_gas_gap", &m_NSWsTGC_off_gas_gap);
    m_tree->Branch("Hits_sTGC_off_channel_type", &m_NSWsTGC_off_channel_type);
    m_tree->Branch("Hits_sTGC_off_channel", &m_NSWsTGC_off_channel);  
  }
  return StatusCode::SUCCESS;
}

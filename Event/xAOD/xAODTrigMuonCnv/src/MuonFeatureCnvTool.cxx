/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
// EDM include(s):
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/L2StandAloneMuon.h"
#include "xAODTrigMuon/TrigMuonDefs.h"

#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"

// Local include(s):
#include "MuonFeatureCnvTool.h"

MuonFeatureCnvTool::MuonFeatureCnvTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : AthAlgTool( type, name, parent ) {
  
  // Declare the interface(s) provided by the tool:
  declareInterface< IMuonFeatureCnvTool >( this );
}

StatusCode MuonFeatureCnvTool::initialize() {
  
  // Greet the user:
  ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
  
  // Return gracefully:
  return StatusCode::SUCCESS;
}

StatusCode MuonFeatureCnvTool::convertMuonFeature( const MuonFeature* aodMf,
                                                   xAOD::L2StandAloneMuon* xaod ) {
  
  if (aodMf) {
    xaod->setPt          (aodMf->pt());
    xaod->setEta         (aodMf->eta());
    xaod->setPhi         (aodMf->phi());
    xaod->setRoIWord     (aodMf->roiId());
    xaod->setSAddress    (aodMf->saddress());
    xaod->setEtaMS       (aodMf->eta());
    xaod->setPhiMS       (aodMf->phi());
    xaod->setDirPhiMS    (aodMf->dir_phi());
    xaod->setRMS         (aodMf->radius());
    xaod->setZMS         (aodMf->zeta());
    xaod->setDirZMS      (aodMf->dir_zeta());
    xaod->setBeta        (aodMf->beta());
    xaod->setBarrelRadius  (aodMf->br_radius());
    xaod->setBarrelSagitta (aodMf->br_sagitta());
    xaod->setEndcapAlpha   (aodMf->ec_alpha());
    xaod->setEndcapBeta    (aodMf->ec_beta());

    if ( aodMf->saddress() == -1 ) { 
      xaod->setSuperPoint (xAOD::L2MuonParameters::Chamber::EndcapInner,
                           aodMf->sp1_r(), aodMf->sp1_z(), aodMf->sp1_slope());
      xaod->setSuperPoint (xAOD::L2MuonParameters::Chamber::EndcapMiddle,
                           aodMf->sp2_r(), aodMf->sp2_z(), aodMf->sp2_slope());
      xaod->setSuperPoint (xAOD::L2MuonParameters::Chamber::EndcapOuter,
                           aodMf->sp3_r(), aodMf->sp3_z(), aodMf->sp3_slope());
    } else {
      xaod->setSuperPoint (xAOD::L2MuonParameters::Chamber::BarrelInner,
                           aodMf->sp1_r(), aodMf->sp1_z(), aodMf->sp1_slope());
      xaod->setSuperPoint (xAOD::L2MuonParameters::Chamber::BarrelMiddle,
                           aodMf->sp2_r(), aodMf->sp2_z(), aodMf->sp2_slope());
      xaod->setSuperPoint (xAOD::L2MuonParameters::Chamber::BarrelOuter,
                           aodMf->sp3_r(), aodMf->sp3_z(), aodMf->sp3_slope());
    }

    if (aodMf->dq_var1()==1) xaod->setIsTgcFailure(1);
    else xaod->setIsTgcFailure(0);

    if (aodMf->dq_var2()==1) xaod->setIsRpcFailure(1);
    else xaod->setIsRpcFailure(0);

    ATH_MSG_VERBOSE("Orig vs xAOD: Pt:       " << aodMf->pt() << " " << xaod->pt());
    ATH_MSG_VERBOSE("Orig vs xAOD: Eta:      " << aodMf->eta() << " " << xaod->eta());
    ATH_MSG_VERBOSE("Orig vs xAOD: phi:      " << aodMf->phi() << " " << xaod->phi());
  }
  
  // Return gracefully:
  return StatusCode::SUCCESS;
}

StatusCode MuonFeatureCnvTool::convertMuonFeature( const MuonFeatureDetails* aodMfd,
                                                   xAOD::L2StandAloneMuon* xaod ) {
  
  if (aodMfd) {
    xaod->setCapacities (aodMfd->max_rpc_hits_capacity(), aodMfd->max_tgc_hits_capacity(), aodMfd->max_mdt_hits_capacity(), 100);

    xaod->setAlgoId            ((int)aodMfd->id());
    xaod->setTeId              (aodMfd->te_id());
    xaod->setLvl1Id            (aodMfd->lvl1_id());
    xaod->setLumiBlock         (aodMfd->lumi_block());
    xaod->setMuonDetMask       (aodMfd->muondetmask());
    xaod->setRoiId             (aodMfd->roi_id());
    xaod->setRoiSystem         (aodMfd->roi_system());
    xaod->setRoiSubsystem      (aodMfd->roi_subsystem());
    xaod->setRoiSector         (aodMfd->roi_sector());
    xaod->setRoiNumber         (aodMfd->roi_number());
    xaod->setRoiThreshold      (aodMfd->roi_threshold());

    xaod->setRpcFitInn (aodMfd->rpc1_x(), aodMfd->rpc1_y(), aodMfd->rpc1_z());
    xaod->setRpcFitMid (aodMfd->rpc2_x(), aodMfd->rpc2_y(), aodMfd->rpc2_z());
    xaod->setRpcFitOut (aodMfd->rpc3_x(), aodMfd->rpc3_y(), aodMfd->rpc3_z());
    
    xaod->setTgcInn    (aodMfd->tgc_Inn_eta(), aodMfd->tgc_Inn_phi(),
                              aodMfd->tgc_Inn_r(), aodMfd->tgc_Inn_z());
    xaod->setTgcInnF   (aodMfd->tgc_Inn_rho_std(), aodMfd->tgc_Inn_rho_N(),
                              aodMfd->tgc_Inn_phi_std(), aodMfd->tgc_Inn_phi_N());
    xaod->setTgcMid1   (aodMfd->tgc_Mid1_eta(), aodMfd->tgc_Mid1_phi(),
                              aodMfd->tgc_Mid1_r(), aodMfd->tgc_Mid1_z());
    xaod->setTgcMid2   (aodMfd->tgc_Mid2_eta(), aodMfd->tgc_Mid2_phi(),
                              aodMfd->tgc_Mid2_r(), aodMfd->tgc_Mid2_z());
    xaod->setTgcMidF   (aodMfd->tgc_Mid_rho_chi2(), aodMfd->tgc_Mid_rho_N(),
                        aodMfd->tgc_Mid_phi_chi2(), aodMfd->tgc_Mid_phi_N());
    xaod->setTgcPt     (aodMfd->tgc_PT());

    const std::vector< std::vector<int> >& chamber_type_1 = aodMfd->chamber_type_1();
    const std::vector< std::vector<float> >& aw           = aodMfd->aw();
    const std::vector< std::vector<float> >& bw           = aodMfd->bw();
    const std::vector< std::vector<float> >& zetaMin      = aodMfd->zetaMin();
    const std::vector< std::vector<float> >& zetaMax      = aodMfd->zetaMax();
    const std::vector< std::vector<float> >& radMin       = aodMfd->radMin();
    const std::vector< std::vector<float> >& radMax       = aodMfd->radMax();
    const std::vector< std::vector<float> >& etaMin       = aodMfd->etaMin();
    const std::vector< std::vector<float> >& etaMax       = aodMfd->etaMax();
    for (int sector=0; sector<2; sector++) {
      for (int station=0; station<4; station++) {
        xaod->setChamberType1(station, sector, chamber_type_1[sector][station]);
        xaod->setChamberType2(station, sector, chamber_type_1[sector][station]);
        xaod->setRoad(station, sector, aw[sector][station], bw[sector][station]);
        xaod->setRegionZ(station, sector, zetaMin[sector][station], zetaMax[sector][station]);
        xaod->setRegionR(station, sector, radMin[sector][station], radMax[sector][station]);
        xaod->setRegionEta(station, sector, etaMin[sector][station], etaMax[sector][station]);
      }      
    }

    const std::vector<uint32_t>& pad_hit_onlineId = aodMfd->pad_hit_onlineId();
    const std::vector<uint32_t>& pad_hit_code     = aodMfd->pad_hit_code();
    const std::vector<float>&    pad_hit_x        = aodMfd->pad_hit_x();
    const std::vector<float>&    pad_hit_y        = aodMfd->pad_hit_y();
    const std::vector<float>&    pad_hit_z        = aodMfd->pad_hit_z();
    const std::vector<float>&    pad_hit_r        = aodMfd->pad_hit_r();
    const std::vector<float>&    pad_hit_p        = aodMfd->pad_hit_p();
    for (unsigned int i=0; i< pad_hit_onlineId.size(); i++) {
      if ( i >= pad_hit_code.size() || i >= pad_hit_x.size() || i >= pad_hit_y.size() || 
           i >= pad_hit_z.size() || i >= pad_hit_r.size() || i >= pad_hit_p.size() ) continue;
      xaod->setRpcHit( (uint32_t)pad_hit_onlineId[i], pad_hit_code[i],
                       pad_hit_x[i], pad_hit_y[i], pad_hit_z[i],
                       0.,pad_hit_r[i], pad_hit_p[i],"");
    }


    const std::vector<float>&    tgc_Inn_rho_hit_phi    = aodMfd->tgc_Inn_rho_hit_phi();
    const std::vector<float>&    tgc_Inn_rho_hit_r      = aodMfd->tgc_Inn_rho_hit_r();
    const std::vector<float>&    tgc_Inn_rho_hit_z      = aodMfd->tgc_Inn_rho_hit_z();
    const std::vector<float>&    tgc_Inn_rho_hit_width  = aodMfd->tgc_Inn_rho_hit_width();
    const std::vector<uint32_t>& tgc_Inn_rho_hit_in_seg = aodMfd->tgc_Inn_rho_hit_in_seg();
    const std::vector<float>&    tgc_Inn_phi_hit_phi    = aodMfd->tgc_Inn_phi_hit_phi();
    const std::vector<float>&    tgc_Inn_phi_hit_r      = aodMfd->tgc_Inn_phi_hit_r();
    const std::vector<float>&    tgc_Inn_phi_hit_z      = aodMfd->tgc_Inn_phi_hit_z();
    const std::vector<float>&    tgc_Inn_phi_hit_width  = aodMfd->tgc_Inn_phi_hit_width();
    const std::vector<uint32_t>& tgc_Inn_phi_hit_in_seg = aodMfd->tgc_Inn_phi_hit_in_seg();
    const std::vector<float>&    tgc_Mid_rho_hit_phi    = aodMfd->tgc_Mid_rho_hit_phi();
    const std::vector<float>&    tgc_Mid_rho_hit_r      = aodMfd->tgc_Mid_rho_hit_r();
    const std::vector<float>&    tgc_Mid_rho_hit_z      = aodMfd->tgc_Mid_rho_hit_z();
    const std::vector<float>&    tgc_Mid_rho_hit_width  = aodMfd->tgc_Mid_rho_hit_width();
    const std::vector<uint32_t>& tgc_Mid_rho_hit_in_seg = aodMfd->tgc_Mid_rho_hit_in_seg();
    const std::vector<float>&    tgc_Mid_phi_hit_phi    = aodMfd->tgc_Mid_phi_hit_phi();
    const std::vector<float>&    tgc_Mid_phi_hit_r      = aodMfd->tgc_Mid_phi_hit_r();
    const std::vector<float>&    tgc_Mid_phi_hit_z      = aodMfd->tgc_Mid_phi_hit_z();
    const std::vector<float>&    tgc_Mid_phi_hit_width  = aodMfd->tgc_Mid_phi_hit_width();
    const std::vector<uint32_t>& tgc_Mid_phi_hit_in_seg = aodMfd->tgc_Mid_phi_hit_in_seg();
    for (unsigned int i=0; i< tgc_Inn_rho_hit_phi.size(); i++) {
      if (i >= tgc_Inn_rho_hit_r.size() || i >= tgc_Inn_rho_hit_z.size() ||
          i >= tgc_Inn_rho_hit_width.size() || i >= tgc_Inn_rho_hit_in_seg.size() || 
          i >= tgc_Inn_phi_hit_phi.size() || i >= tgc_Inn_phi_hit_r.size() || 
          i >= tgc_Inn_phi_hit_z.size() || i >= tgc_Inn_phi_hit_width.size() || 
          i >= tgc_Inn_phi_hit_in_seg.size() || i >= tgc_Mid_rho_hit_phi.size() || 
          i >= tgc_Mid_rho_hit_r.size() || i >= tgc_Mid_rho_hit_z.size() || 
          i >= tgc_Mid_rho_hit_width.size() || i >= tgc_Mid_rho_hit_in_seg.size() || 
          i >= tgc_Mid_phi_hit_phi.size() || i >= tgc_Mid_phi_hit_r.size() || 
          i >= tgc_Mid_phi_hit_z.size() || i >= tgc_Mid_phi_hit_width.size() || 
          i >= tgc_Mid_phi_hit_in_seg.size() ) continue;
      xaod->setTgcHit(0.,tgc_Inn_rho_hit_phi[i], tgc_Inn_rho_hit_r[i], tgc_Inn_rho_hit_z[i],
		      tgc_Inn_rho_hit_width[i], (int)tgc_Inn_rho_hit_in_seg[i], 0, 0, 0);
    }

    const std::vector<uint32_t>& mdt_onlineId      = aodMfd->mdt_onlineId();
    const std::vector<uint32_t>& mdt_offlineId     = aodMfd->mdt_offlineId();
    const std::vector<float>&    mdt_tube_r        = aodMfd->mdt_tube_r();
    const std::vector<float>&    mdt_tube_z        = aodMfd->mdt_tube_z();
    const std::vector<float>&    mdt_tube_residual = aodMfd->mdt_tube_residual();
    const std::vector<float>&    mdt_tube_time     = aodMfd->mdt_tube_time();
    const std::vector<float>&    mdt_tube_space    = aodMfd->mdt_tube_space();
    const std::vector<float>&    mdt_tube_sigma    = aodMfd->mdt_tube_sigma();
    int chamber = 999;
    for (unsigned int i=0; i< mdt_onlineId.size(); i++) {
      if ( i >= mdt_offlineId.size() || i >= mdt_tube_r.size() || 
	   i >= mdt_tube_z.size() || i >= mdt_tube_residual.size() || 
	   i >= mdt_tube_time.size() || i >= mdt_tube_space.size() || 
	   i >= mdt_tube_sigma.size() ) continue;
      xaod->setMdtHit(mdt_onlineId[i], (int)mdt_offlineId[i], (int)chamber,
		      mdt_tube_r[i], mdt_tube_z[i], 0., mdt_tube_residual[i],
		      mdt_tube_time[i], mdt_tube_space[i], mdt_tube_sigma[i]);
    }

    ATH_MSG_VERBOSE("Orig vs xAOD: TGC PT:      " << aodMfd->tgc_PT() << " " << xaod->tgcPt());
  }
  
  // Return gracefully:
  return StatusCode::SUCCESS;
}

/// Convert full MuonFeatureContainer and MuonFeatureDetailsContainer into xAOD::L2StandAloneMuonContainer
StatusCode MuonFeatureCnvTool::convertMuonFeatureContainer(const MuonFeatureContainer* aodMf,
                                                           const MuonFeatureDetailsContainer* aodMfd,
                                                           xAOD::L2StandAloneMuonContainer* xaod) {
  if (!aodMf) {
    ATH_MSG_ERROR( "Null pointer for input MuonFeatureContainter" );
    return StatusCode::FAILURE;
  }

  if (!aodMfd) {
    ATH_MSG_ERROR( "Null pointer for input MuonFeatureDetailsContainter" );
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_VERBOSE( "Found " << aodMf->size() << " input elements" );
  
  // Fill the xAOD container:
  MuonFeatureContainer::const_iterator citMf;
  MuonFeatureContainer::const_iterator citMfBegin = aodMf->begin();
  MuonFeatureContainer::const_iterator citMfEnd   = aodMf->end();

  std::vector<std::pair <bool, const MuonFeatureDetails*>> vMfd;
  MuonFeatureDetailsContainer::const_iterator citMfd;
  MuonFeatureDetailsContainer::const_iterator citMfdBegin = aodMfd->begin();
  MuonFeatureDetailsContainer::const_iterator citMfdEnd   = aodMfd->end();
  for (citMfd = citMfdBegin; citMfd != citMfdEnd; ++citMfd) {
    vMfd.emplace_back(false, *citMfd);
  }

  citMfd = citMfdBegin;

  for (citMf = citMfBegin; citMf != citMfEnd; ++citMf) {

    const MuonFeature* mfObj = *citMf;
    xAOD::L2StandAloneMuon* xaodObj = new xAOD::L2StandAloneMuon();
    xaod->push_back( xaodObj );
    
    StatusCode sc = convertMuonFeature( mfObj, xaodObj );
    if(sc.isFailure()) return sc;
    
    for (unsigned int i=0; i<vMfd.size(); i++) {

      if (vMfd[i].first) continue;
      
      if ( (int)(*citMf)->roiId() == (int)vMfd[i].second->roi_id() &&
           (int)(*citMf)->algoId() == (int)vMfd[i].second->id() ) {
        
        vMfd[i].first = true;
        
        const MuonFeatureDetails* mfdObj = vMfd[i].second;
                
        StatusCode sc = convertMuonFeature( mfdObj, xaodObj );
        if(sc.isFailure()) return sc;

        break;
      }

    } // loop for MuonFeatureDetailsContainer
  } // loop for MuonFeatureContainer

  ATH_MSG_VERBOSE( "Output Container has now " << xaod->size() << " elements" );
  
  return StatusCode::SUCCESS;
}// end convertTrigMuonEFInfoContainer


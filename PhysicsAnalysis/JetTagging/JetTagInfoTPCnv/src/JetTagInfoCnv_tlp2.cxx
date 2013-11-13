/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Implement the top level container's converter.
///

#include "JetTagInfoTPCnv/JetTagInfoCnv_tlp2.h"
#include <iostream>

namespace Analysis {

  ///
  /// Set it all up - all the converters!
  ///
  JetTagInfoCnv_tlp2::JetTagInfoCnv_tlp2 (void)
  {
    /// Once this order has been used, never
    /// change it. No idea why -- have just seen
    /// this in other file comments.
    
    addTPConverter (&m_baseTagInfoCnv);
    addTPConverter (&m_ipInfoBaseCnv);
    addTPConverter (&m_svInfoBaseCnv);
    addTPConverter (&m_svInfoPlusCnv);
    addTPConverter (&m_svxSummaryCnv);
    addTPConverter (&m_ipInfoPlusCnv);
    addTPConverter (&m_ipTrackInfoCnv);
    addTPConverter (&m_truthInfoCnv);
    addTPConverter (&m_softLeptonTruthInfoCnv);
    addTPConverter (&m_slTruthInfoCnv);
    addTPConverter (&m_atlInfoCnv);
    addTPConverter (&m_secVtxInfoCnv);
    addTPConverter (&m_jetProbInfoBaseCnv);
    addTPConverter (&m_jetFitterTagInfoCnv);
    addTPConverter (&m_softElectronInfoCnv);
    addTPConverter (&m_seTrackInfoCnv);
    addTPConverter (&m_softMuonInfoCnv);
    addTPConverter (&m_smTrackInfoCnv);
    addTPConverter (&m_lifetimeInfoCnv);

    addTPConverter (&m_trackAssociationCnv);
    addTPConverter (&m_electronAssociationCnv);
    addTPConverter (&m_photonAssociationCnv);
    addTPConverter (&m_muonAssociationCnv);
    addTPConverter (&m_svxAssociationCnv);
    addTPConverter (&m_trkNavCnv);
    addTPConverter (&m_eleNavCnv);
    addTPConverter (&m_phoNavCnv);
    addTPConverter (&m_muoNavCnv);

    addTPConverter (&m_recVertexCnv);
    addTPConverter (&m_errorMatrixCnv);
    addTPConverter (&m_vertexCnv);
    addTPConverter (&m_fitQualityCnv);

    addTPConverter (&m_trackCountingInfoCnv);

    addTPConverter (&m_msvInfoPlusCnv);
    addTPConverter (&m_msvVtxCnv);


  }

  ///
  /// Set the actual storage
  ///
  void JetTagInfoCnv_tlp2::setPStorage (JetTagInfo_tlp2 *storage)
  {
    m_baseTagInfoCnv.setPStorage(&storage->m_baseTagInfo);
    m_ipInfoBaseCnv.setPStorage(&storage->m_ipInfoBase);
    m_svInfoBaseCnv.setPStorage(&storage->m_svInfoBase);
    m_svInfoPlusCnv.setPStorage(&storage->m_svInfoPlus);
    m_svxSummaryCnv.setPStorage(&storage->m_svxSummary);
    m_ipInfoPlusCnv.setPStorage(&storage->m_ipInfoPlus);
    m_ipTrackInfoCnv.setPStorage(&storage->m_ipTrackInfo);
    m_truthInfoCnv.setPStorage(&storage->m_truthInfo);
    m_softLeptonTruthInfoCnv.setPStorage(&storage->m_softLeptonTruthInfo);
    m_slTruthInfoCnv.setPStorage(&storage->m_slTruthInfo);
    m_atlInfoCnv.setPStorage(&storage->m_atlInfo);
    m_secVtxInfoCnv.setPStorage(&storage->m_secVtxInfo);
    m_jetProbInfoBaseCnv.setPStorage(&storage->m_jetProbInfoBase);
    m_jetFitterTagInfoCnv.setPStorage(&storage->m_jetFitterTagInfo);
    m_softElectronInfoCnv.setPStorage(&storage->m_softElectronInfo);
    m_seTrackInfoCnv.setPStorage(&storage->m_seTrackInfo);
    m_softMuonInfoCnv.setPStorage(&storage->m_softMuonInfo);
    m_smTrackInfoCnv.setPStorage(&storage->m_smTrackInfo);
    m_lifetimeInfoCnv.setPStorage(&storage->m_lifetimeInfo);
    m_trackCountingInfoCnv.setPStorage(&storage->m_trackCountingInfo);
    
    m_trackAssociationCnv.setPStorage(&storage->m_navAssociationCommon);
    m_electronAssociationCnv.setPStorage(&storage->m_navAssociationCommon);
    m_photonAssociationCnv.setPStorage(&storage->m_navAssociationCommon);
    m_muonAssociationCnv.setPStorage(&storage->m_navAssociationCommon);
    m_svxAssociationCnv.setPStorage(&storage->m_navSvxAssociation);
    m_trkNavCnv.setPStorage(&storage->m_navigable);
    m_eleNavCnv.setPStorage(&storage->m_navigable);
    m_muoNavCnv.setPStorage(&storage->m_navigable);
    m_phoNavCnv.setPStorage(&storage->m_navigable);

    m_recVertexCnv.setPStorage(&storage->m_recVertex);
    m_errorMatrixCnv.setPStorage(&storage->m_errorMatrix);
    m_vertexCnv.setPStorage(&storage->m_vertex);
    m_fitQualityCnv.setPStorage(&storage->m_fitQuality);

    m_msvInfoPlusCnv.setPStorage(&storage->m_msvInfoPlus);
    m_msvVtxCnv.setPStorage(&storage->m_msvVtx);


  }
}

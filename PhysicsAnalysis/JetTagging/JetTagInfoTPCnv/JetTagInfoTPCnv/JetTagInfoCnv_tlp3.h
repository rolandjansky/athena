/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Top level tag info converter. Access the items themselves!
///
#ifndef JETTAGINFOTPCNV_JETTAGINFOCNV_P3
#define JETTAGINFOTPCNV_JETTAGINFOCNV_P3

/// SVInfoPlus must be included first due to the
/// @*#&@& protected and private #defines - these
/// are required to get the ElementLinkVectorCnv to
/// work properly. Same with NavigableCnv. What a mess.
#include "DataModelAthenaPool/NavigableCnv_p1.h"
#include "JetTagInfoTPCnv/SVInfoPlusCnv_p1.h"

/// Normal includes below.
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelExtTPConverter.h"

#include "JetTagInfoTPCnv/JetTagInfo_tlp3.h"

#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"
#include "JetTagInfoTPCnv/IPInfoBaseCnv_p1.h"
#include "JetTagInfoTPCnv/SVInfoBaseCnv_p1.h"
#include "JetTagInfoTPCnv/MultiSVInfoPlusCnv_p1.h"
#include "JetTagInfoTPCnv/MSVVtxInfoCnv_p2.h"
#include "JetTagInfoTPCnv/GbbNNTagInfoCnv_p1.h"
#include "JetTagInfoTPCnv/SvxSummaryCnv_p1.h"
#include "JetTagInfoTPCnv/IPInfoPlusCnv_p1.h"
#include "JetTagInfoTPCnv/IPTrackInfoCnv_p1.h"
#include "JetTagInfoTPCnv/TruthInfoCnv_p1.h"
#include "JetTagInfoTPCnv/SoftLeptonTruthInfoCnv_p1.h"
#include "JetTagInfoTPCnv/SLTrueInfoCnv_p1.h"
#include "JetTagInfoTPCnv/AtlfInfoCnv_p1.h"
#include "JetTagInfoTPCnv/SecVtxInfoCnv_p1.h"
#include "JetTagInfoTPCnv/JetProbInfoBaseCnv_p1.h"
#include "JetTagInfoTPCnv/JetFitterTagInfoCnv_p1.h"
#include "JetTagInfoTPCnv/SoftElectronInfoCnv_p1.h"
#include "JetTagInfoTPCnv/SETrackInfoCnv_p1.h"
#include "JetTagInfoTPCnv/SoftMuonInfoCnv_p1.h"
#include "JetTagInfoTPCnv/SMTrackInfoCnv_p1.h"
#include "JetTagInfoTPCnv/LifetimeInfoCnv_p1.h"
#include "JetTagInfoTPCnv/TrackCountingInfoCnv_p1.h"

#include "JetTagInfoTPCnv/TrackAssociationCnv_p1.h"
#include "JetTagInfoTPCnv/ElectronAssociationCnv_p1.h"
#include "JetTagInfoTPCnv/PhotonAssociationCnv_p1.h"
#include "JetTagInfoTPCnv/MuonAssociationCnv_p1.h"
#include "JetTagInfoTPCnv/ISvxAssociationCnv_p1.h"

#include "TrkEventTPCnv/VxVertex/RecVertexCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"
#include "TrkEventTPCnv/VxVertex/VertexCnv_p1.h"

namespace Analysis {

  // Top level continer converter for teh JetTagInfo subclasses.
  class JetTagInfoCnv_tlp3
    : public AthenaPoolTopLevelExtTPConverter<JetTagInfo_tlp3> {

    public:

    // Register the converters
    JetTagInfoCnv_tlp3 (void);

    // When versions change, change this...
    unsigned short getConverterID(void) {return 1;}

    // Save it!
    void setPStorage (JetTagInfo_tlp3 *storage);

    BaseTagInfoCnv_p1 m_baseTagInfoCnv;
    IPInfoBaseCnv_p1 m_ipInfoBaseCnv;
    SVInfoBaseCnv_p1 m_svInfoBaseCnv;
    SVInfoPlusCnv_p1 m_svInfoPlusCnv;
    MultiSVInfoPlusCnv_p1 m_msvInfoPlusCnv;
    MSVVtxInfoCnv_p2 m_msvVtxCnv;
    GbbNNTagInfoCnv_p1 m_gbbnnTagCnv;
    SvxSummaryCnv_p1 m_svxSummaryCnv;
    IPInfoPlusCnv_p1 m_ipInfoPlusCnv;
    IPTrackInfoCnv_p1 m_ipTrackInfoCnv;
    TruthInfoCnv_p1 m_truthInfoCnv;
    SoftLeptonTruthInfoCnv_p1 m_softLeptonTruthInfoCnv;
    SLTrueInfoCnv_p1 m_slTruthInfoCnv;
    AtlfInfoCnv_p1 m_atlInfoCnv;
    SecVtxInfoCnv_p1 m_secVtxInfoCnv;
    JetProbInfoBaseCnv_p1 m_jetProbInfoBaseCnv;
    JetFitterTagInfoCnv_p1 m_jetFitterTagInfoCnv;
    SoftElectronInfoCnv_p1 m_softElectronInfoCnv;
    SETrackInfoCnv_p1 m_seTrackInfoCnv;
    SoftMuonInfoCnv_p1 m_softMuonInfoCnv;
    SMTrackInfoCnv_p1 m_smTrackInfoCnv;
    LifetimeInfoCnv_p1 m_lifetimeInfoCnv;
    TrackCountingInfoCnv_p1 m_trackCountingInfoCnv;

    TrackAssociationCnv_p1 m_trackAssociationCnv;
    ElectronAssociationCnv_p1 m_electronAssociationCnv;
    PhotonAssociationCnv_p1 m_photonAssociationCnv;
    MuonAssociationCnv_p1 m_muonAssociationCnv;
    ISvxAssociationCnv_p1 m_svxAssociationCnv;
    NavigableCnv_p1<Navigable<Rec::TrackParticleContainer,double>,float> m_trkNavCnv;
    NavigableCnv_p1<Navigable<ElectronContainer,double>,float> m_eleNavCnv;
    NavigableCnv_p1<Navigable<MuonContainer,double>,float> m_muoNavCnv;
    NavigableCnv_p1<Navigable<PhotonContainer,double>,float> m_phoNavCnv;

    RecVertexCnv_p1 m_recVertexCnv;
    ErrorMatrixCnv_p1 m_errorMatrixCnv;
    VertexCnv_p1 m_vertexCnv;
    FitQualityCnv_p1 m_fitQualityCnv;
  };
  
}

#endif


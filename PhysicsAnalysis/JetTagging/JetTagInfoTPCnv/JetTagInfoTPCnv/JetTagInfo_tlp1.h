/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// The top level container that will store
/// the various tag info objects.
///
#ifndef JETTAGINFOTPCNV_JETTAGINFOTLP1_H
#define JETTAGINFOTPCNV_JETTAGINFOTLP1_H

#include "JetTagInfoTPCnv/BaseTagInfo_p1.h"
#include "JetTagInfoTPCnv/IPInfoBase_p1.h"
#include "JetTagInfoTPCnv/IPInfoPlus_p1.h"
#include "JetTagInfoTPCnv/IPTrackInfo_p1.h"
#include "JetTagInfoTPCnv/TruthInfo_p1.h"
#include "JetTagInfoTPCnv/SoftLeptonTruthInfo_p1.h"
#include "JetTagInfoTPCnv/SLTrueInfo_p1.h"
#include "JetTagInfoTPCnv/AtlfInfo_p1.h"
#include "JetTagInfoTPCnv/SecVtxInfo_p1.h"
#include "JetTagInfoTPCnv/JetFitterTagInfo_p1.h"
#include "JetTagInfoTPCnv/JetProbInfoBase_p1.h"
#include "JetTagInfoTPCnv/LifetimeInfo_p1.h"
#include "JetTagInfoTPCnv/SoftElectronInfo_p1.h"
#include "JetTagInfoTPCnv/SETrackInfo_p1.h"
#include "JetTagInfoTPCnv/SoftMuonInfo_p1.h"
#include "JetTagInfoTPCnv/SMTrackInfo_p1.h"
#include "JetTagInfoTPCnv/SVInfoPlus_p1.h"
#include "JetTagInfoTPCnv/SVInfoBase_p1.h"
#include "JetTagInfoTPCnv/SvxSummary_p1.h"
#include "JetTagInfoTPCnv/TrackCountingInfo_p1.h"

#include "JetEventTPCnv/JetAssociationBase_p1.h"
#include "JetTagInfoTPCnv/NavAssociationCommon_p1.h"
#include "JetTagInfoTPCnv/ISvxAssociation_p1.h"

#include "TrkEventTPCnv/VxVertex/RecVertex_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/FitQuality_p1.h"
#include "TrkEventTPCnv/VxVertex/Vertex_p1.h"

#include "DataModelAthenaPool/Navigable_p1.h"

#include <vector>

namespace Analysis {

  /// Contains a vector of evrything that we will store...
  class JetTagInfo_tlp1 {
  public:
    std::vector<BaseTagInfo_p1> m_baseTagInfo;
    std::vector<IPInfoBase_p1> m_ipInfoBase;
    std::vector<IPInfoPlus_p1> m_ipInfoPlus;
    std::vector<IPTrackInfo_p1> m_ipTrackInfo;
    std::vector<TruthInfo_p1> m_truthInfo;
    std::vector<SoftLeptonTruthInfo_p1> m_softLeptonTruthInfo;
    std::vector<SLTrueInfo_p1> m_slTruthInfo;
    std::vector<AtlfInfo_p1> m_atlInfo;
    std::vector<SecVtxInfo_p1> m_secVtxInfo;
    std::vector<JetProbInfoBase_p1> m_jetProbInfoBase;
    std::vector<JetFitterTagInfo_p1> m_jetFitterTagInfo;
    std::vector<LifetimeInfo_p1> m_lifetimeInfo;
    std::vector<SoftElectronInfo_p1> m_softElectronInfo;
    std::vector<SETrackInfo_p1> m_seTrackInfo;
    std::vector<SoftMuonInfo_p1> m_softMuonInfo;
    std::vector<SMTrackInfo_p1> m_smTrackInfo;
    std::vector<SVInfoPlus_p1> m_svInfoPlus;
    std::vector<SVInfoBase_p1> m_svInfoBase;
    std::vector<SvxSummary_p1> m_svxSummary;
    std::vector<TrackCountingInfo_p1> m_trackCountingInfo;

    std::vector<JetAssociationBase_p1> m_jetAssociationBase;
    std::vector<Navigable_p1<unsigned int, float> > m_navigable;
    std::vector<NavAssociationCommon_p1> m_navAssociationCommon;
    std::vector<ISvxAssociation_p1> m_navSvxAssociation;

    std::vector<Trk::RecVertex_p1> m_recVertex;
    std::vector<Trk::Vertex_p1> m_vertex;
    std::vector<Trk::HepSymMatrix_p1> m_errorMatrix;
    std::vector<Trk::FitQuality_p1> m_fitQuality;
  };
}
#endif

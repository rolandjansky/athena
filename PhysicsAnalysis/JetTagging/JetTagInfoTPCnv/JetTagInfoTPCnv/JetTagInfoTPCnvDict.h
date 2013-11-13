/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_JETTAGINFOTPCNVDICT_H
#define JETTAGINFOTPCNV_JETTAGINFOTPCNVDICT_H

#include "JetTagInfoTPCnv/BaseTagInfo_p1.h"
#include "JetTagInfoTPCnv/IPInfoBase_p1.h"
#include "JetTagInfoTPCnv/IPTrackInfo_p1.h"
#include "JetTagInfoTPCnv/IPInfoPlus_p1.h"
#include "JetTagInfoTPCnv/TruthInfo_p1.h"
#include "JetTagInfoTPCnv/JetTagInfo_tlp1.h"
#include "JetTagInfoTPCnv/JetTagInfo_tlp2.h"
#include "JetTagInfoTPCnv/SoftLeptonTruthInfo_p1.h"
#include "JetTagInfoTPCnv/SLTrueInfo_p1.h"
#include "JetTagInfoTPCnv/SecVtxInfo_p1.h"
#include "JetTagInfoTPCnv/JetProbInfoBase_p1.h"
#include "JetTagInfoTPCnv/JetFitterTagInfo_p1.h"
#include "JetTagInfoTPCnv/LifetimeInfo_p1.h"
#include "JetTagInfoTPCnv/TrackCountingInfo_p1.h"
#include "JetTagInfoTPCnv/SoftElectronInfo_p1.h"
#include "JetTagInfoTPCnv/SETrackInfo_p1.h"
#include "JetTagInfoTPCnv/SoftMuonInfo_p1.h"
#include "JetTagInfoTPCnv/SMTrackInfo_p1.h"
#include "JetTagInfoTPCnv/SVInfoPlus_p1.h"
#include "JetTagInfoTPCnv/MultiSVInfoPlus_p1.h"
#include "JetTagInfoTPCnv/MSVVtxInfo_p1.h"
#include "JetTagInfoTPCnv/MSVVtxInfo_p2.h"
#include "JetTagInfoTPCnv/GbbNNTagInfo_p1.h"
#include "JetTagInfoTPCnv/SvxSummary_p1.h"
#include "JetTagInfoTPCnv/JetTagInfoCnv_tlp1.h"
#include "JetTagInfoTPCnv/JetTagInfoCnv_tlp2.h"
#include "JetTagInfoTPCnv/JetTagInfoCnv_tlp3.h"
#include "JetTagInfoTPCnv/NavAssociationCommon_p1.h"
#include "JetTagInfoTPCnv/ISvxAssociation_p1.h"

#include <vector>

namespace {
 struct temp {
   std::vector<Analysis::BaseTagInfo_p1>  m_BaseTagInfo;
   std::vector<Analysis::IPInfoBase_p1>   m_IPInfoBase;
   std::vector<Analysis::IPTrackInfo_p1>  m_IPTrackInfo;
   std::vector<Analysis::IPInfoPlus_p1>   m_IPInfoPlus;
   std::vector<Analysis::TruthInfo_p1>    m_TruthInfo;
   std::vector<Analysis::SoftLeptonTruthInfo_p1> m_SoftLeptonTruthInfo;
   std::vector<Analysis::SLTrueInfo_p1> m_SLTrueInfo;
   std::vector<Analysis::SecVtxInfo_p1> m_secVtxInfo;
   std::vector<Analysis::JetProbInfoBase_p1> m_jetProbInfoBase;
   std::vector<Analysis::JetFitterTagInfo_p1> m_jetFitterTagInfo;
   std::vector<Analysis::LifetimeInfo_p1> m_lifetimeInfo;
   std::vector<Analysis::TrackCountingInfo_p1> m_trackCountingInfo;
   std::vector<Analysis::SoftElectronInfo_p1> m_softElectronInfo;
   std::vector<Analysis::SETrackInfo_p1> m_seTrackInfo;
   std::vector<Analysis::SoftMuonInfo_p1> m_softMuonInfo;
   std::vector<Analysis::SMTrackInfo_p1> m_smTrackInfo;
   std::vector<Analysis::SVInfoPlus_p1> m_svInfoPlus;
   std::vector<Analysis::MultiSVInfoPlus_p1> m_multiSVInfoPlus;
   std::vector<Analysis::MSVVtxInfo_p1> m_msvVtxInfo;
   std::vector<Analysis::MSVVtxInfo_p2> m_msvVtxInfo2;
   std::vector<Analysis::GbbNNTagInfo_p1> m_gbbnnTag;
   std::vector<Analysis::SVInfoBase_p1> m_svInfoBase;
   std::vector<Analysis::SvxSummary_p1> m_svxSummary;
   std::vector<Analysis::NavAssociationCommon_p1> m_navAssCommon;
   std::vector<Analysis::ISvxAssociation_p1> m_svxAss;
 };
}

#endif

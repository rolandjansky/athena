/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_JETTAGINFODICT_H
#define JETTAGINFO_JETTAGINFODICT_H

// #include "JetTagInfo/ITagInfo.h"
#include "JetTagInfo/BaseTagInfo.h"
#include "JetTagInfo/TruthInfo.h"
#include "JetTagInfo/SoftLeptonTruthInfo.h"
#include "JetTagInfo/SLTrueInfo.h"
#include "JetTagInfo/SecVtxInfo.h"
#include "JetTagInfo/LifetimeInfo.h"
#include "JetTagInfo/SETrackInfo.h"
#include "JetTagInfo/SoftElectronInfo.h"
#include "JetTagInfo/SMTrackInfo.h"
#include "JetTagInfo/SoftMuonInfo.h"
#include "JetTagInfo/IPTrackInfo.h"
#include "JetTagInfo/IPInfoBase.h"
#include "JetTagInfo/IPInfoPlus.h"
#include "JetTagInfo/SVInfoBase.h"
#include "JetTagInfo/SVInfoPlus.h"
#include "JetTagInfo/SVTrackInfo.h"
#include "JetTagInfo/JetProbInfoBase.h"
#include "JetTagInfo/TrackGrade.h"
#include "JetTagInfo/TrackGradesDefinition.h"
#include "JetTagInfo/AtlfInfo.h"
#include "JetTagInfo/JetFitterTagInfo.h"
#include "JetTagInfo/TrackCountingInfo.h"
#include "JetTagInfo/MSVVtxInfo.h"
#include "JetTagInfo/MultiSVInfoPlus.h"
#include "JetTagInfo/GbbNNTagInfo.h"
#include <string>
#include <vector>

namespace JetTagInfoDict{

  struct temp {

    std::vector<JetTagInfoBase*> m_tagInfos;

 };
} // end JetTagInfoDict namespace


#endif

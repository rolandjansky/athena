/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASSOCIATIONUTILS_LINKDEF_H
#define ASSOCIATIONUTILS_LINKDEF_H

#ifdef __CLING__

#include "AssociationUtils/OverlapRemovalTool.h"
#include "AssociationUtils/BaseOverlapTool.h"
#include "AssociationUtils/DeltaROverlapTool.h"
#include "AssociationUtils/EleEleOverlapTool.h"
#include "AssociationUtils/EleMuSharedTrkOverlapTool.h"
#include "AssociationUtils/EleJetOverlapTool.h"
#include "AssociationUtils/MuJetOverlapTool.h"
#include "AssociationUtils/AltMuJetOverlapTool.h"
#include "AssociationUtils/TauLooseEleOverlapTool.h"
#include "AssociationUtils/TauLooseMuOverlapTool.h"
#include "AssociationUtils/TauJetOverlapTool.h"
#include "AssociationUtils/TauAntiTauJetOverlapTool.h"
#include "AssociationUtils/ObjLinkOverlapTool.h"
#include "AssociationUtils/ToolBox.h"
#include "AssociationUtils/BJetHelper.h"
#include "AssociationUtils/DeltaRMatcher.h"
#include "AssociationUtils/MuJetGhostDRMatcher.h"
#include "AssociationUtils/OverlapDecorationHelper.h"
#include "AssociationUtils/OverlapLinkHelper.h"

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ namespace ORUtils;
#pragma link C++ class ORUtils::IOverlapRemovalTool+;
#pragma link C++ class ORUtils::IOverlapTool+;
#pragma link C++ class ORUtils::BaseOverlapTool+;
#pragma link C++ class ORUtils::OverlapRemovalTool+;

#pragma link C++ class ORUtils::DeltaROverlapTool+;
#pragma link C++ class ORUtils::EleEleOverlapTool+;
#pragma link C++ class ORUtils::EleJetOverlapTool+;
#pragma link C++ class ORUtils::EleMuSharedTrkOverlapTool+;
#pragma link C++ class ORUtils::MuJetOverlapTool+;
#pragma link C++ class ORUtils::AltMuJetOverlapTool+;
#pragma link C++ class ORUtils::TauLooseEleOverlapTool+;
#pragma link C++ class ORUtils::TauLooseMuOverlapTool+;
#pragma link C++ class ORUtils::TauJetOverlapTool+;
#pragma link C++ class ORUtils::TauAntiTauJetOverlapTool+;
#pragma link C++ class ORUtils::ObjLinkOverlapTool+;

// Helpers
#pragma link C++ class ORUtils::ToolBox+;
#pragma link C++ class ORUtils::BJetHelper+;
#pragma link C++ class ORUtils::DeltaRMatcher+;
#pragma link C++ class ORUtils::SlidingDeltaRMatcher+;
#pragma link C++ class ORUtils::MuJetGhostDRMatcher+;
#pragma link C++ class ORUtils::OverlapDecorationHelper+;
#pragma link C++ class ORUtils::OverlapLinkHelper+;

#endif // __CLING__

#endif // ASSOCIATIONUTILS_LINKDEF_H

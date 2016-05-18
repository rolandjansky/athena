/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __PANTAUALGS__
#define __PANTAUALGS__

#include "PanTauAlgs/PanTauSeed.h"
#include "PanTauAlgs/TauClassificationTypes.h"
/* #include "PanTauAlgs/TauConstituent.h" */
#include "PanTauAlgs/TauFeature.h"

#include "PanTauAlgs/ITool_PanTauTools.h"
#include "PanTauAlgs/ITool_InformationStore.h"
#include "PanTauAlgs/ITool_InputConverter.h"
#include "PanTauAlgs/ITool_ModeDiscriminator.h"
#include "PanTauAlgs/ITool_TauConstituentGetter.h"
#include "PanTauAlgs/ITool_TauConstituentSelector.h"

#include "PanTauAlgs/PanTauProcessor.h"
#include "PanTauAlgs/Tool_DecayModeDeterminator.h"
#include "PanTauAlgs/Tool_DetailsArranger.h"
#include "PanTauAlgs/Tool_FeatureExtractor.h"
#include "PanTauAlgs/Tool_InformationStore.h"
#include "PanTauAlgs/Tool_InputConverter.h"
#include "PanTauAlgs/Tool_ModeDiscriminator.h"
#include "PanTauAlgs/Tool_TauConstituentGetter.h"
#include "PanTauAlgs/Tool_TauConstituentSelector.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace PanTau;

#pragma link C++ class PanTau::PanTauSeed2;
/* #pragma link C++ class PanTau::TauConstituent; */
#pragma link C++ class PanTau::TauFeature2;

#pragma link C++ class PanTau::ITool_PanTauTools;
#pragma link C++ class PanTau::ITool_InformationStore;
#pragma link C++ class PanTau::ITool_InputConverter;
#pragma link C++ class PanTau::ITool_ModeDiscriminator;
#pragma link C++ class PanTau::ITool_TauConstituentGetter;
#pragma link C++ class PanTau::ITool_TauConstituentSelector;

#pragma link C++ class PanTau::Tool_DecayModeDeterminator;
#pragma link C++ class PanTau::Tool_DetailsArranger;
#pragma link C++ class PanTau::Tool_FeatureExtractor;
#pragma link C++ class PanTau::Tool_InformationStore;
#pragma link C++ class PanTau::Tool_InputConverter;
#pragma link C++ class PanTau::Tool_ModeDiscriminator;
#pragma link C++ class PanTau::Tool_TauConstituentGetter;
#pragma link C++ class PanTau::Tool_TauConstituentSelector;

#pragma link C++ enum PanTau::TauClassificationTypes::TruthTauType;
#pragma link C++ enum PanTau::TauClassificationTypes::SeedTauType;
#pragma link C++ enum PanTau::TauClassificationTypes::PanTauRecoMode;

//#pragma link C++ enum TauID::Types::MethodType;

#endif
#endif

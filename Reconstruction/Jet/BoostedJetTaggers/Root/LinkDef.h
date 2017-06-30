// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetAnalysisInterfaces/IJetSelectorTool.h"

#include "BoostedJetTaggers/SmoothedWZTagger.h"
#include "BoostedJetTaggers/BoostedXbbTagger.h"
#include "BoostedJetTaggers/SmoothedTopTagger.h"
#include "BoostedJetTaggers/JSSWTopTaggerBDT.h"
#include "BoostedJetTaggers/JSSWTopTaggerDNN.h"
#include "BoostedJetTaggers/TopoclusterTopTagger.h"
#include "BoostedJetTaggers/JetQGTagger.h"

#ifdef __ROOTCLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ class IJetSelectorTool;

#pragma link C++ class SmoothedWZTagger;
#pragma link C++ class SmoothedTopTagger;
#pragma link C++ class BoostedXbbTagger;
#pragma link C++ class JSSWTopTaggerBDT;
#pragma link C++ class JSSWTopTaggerDNN;
#pragma link C++ class TopoclusterTopTagger;
#pragma link C++ class JetQGTagger;

#pragma link C++ class SubstructureTopTagger;
#pragma link C++ namespace STTHelpers;

#pragma link C++ class STTHelpers::TopTagSmoothMassCut;
#pragma link C++ class STTHelpers::TopTagSmoothTau32Cut;

#pragma link C++ function STTHelpers::configSubstTagger;


#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <JetRec/FastJetInterfaceTool.h>
#include <JetRec/JetByVertexFinder.h>
#include <JetRec/JetConstitRemover.h>
#include <JetRec/JetConstituentsRetriever.h>
#include <JetRec/JetDumper.h>
#include <JetRec/JetFilterTool.h>
#include <JetRec/JetFinder.h>
#include <JetRec/JetFromPseudojet.h>
#include <JetRec/JetModifierBase.h>
#include <JetRec/JetPruner.h>
#include <JetRec/JetPseudojetCopier.h>
#include <JetRec/JetPseudojetRetriever.h>
#include <JetRec/JetReclusterer.h>
#include <JetRec/JetRecTool.h>
#include <JetRec/JetSorter.h>
#include <JetRec/JetSplitter.h>
#include <JetRec/JetToolRunner.h>
#include <JetRec/JetTrimmer.h>
#include <JetRec/JetSoftDrop.h>
#include <JetRec/JetModifiedMassDrop.h>
#include <JetRec/MuonSegmentPseudoJetGetter.h>
#include <JetRec/PseudoJetGetter.h>
#include <JetRec/PseudoJetGetterRegistry.h>
#include <JetRec/JetSJCalibrationGroomer.h>

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ namespace FastJetInterface;
#pragma link C++ class FastJetInterfaceTool+;

#pragma link C++ class JetByVertexFinder+;
#pragma link C++ class JetConstitRemover+;
#pragma link C++ class JetConstituentsRetriever+;
#pragma link C++ class JetDumper+;
#pragma link C++ class JetFilterTool;
#pragma link C++ class JetFinder+;
#pragma link C++ class JetFromPseudojet+;
#pragma link C++ class JetModifierBase+;
#pragma link C++ class JetPruner+;
#pragma link C++ class JetPseudojetCopier+;
#pragma link C++ class JetPseudojetRetriever+;
#pragma link C++ class JetRecTool+;
#pragma link C++ class JetReclusterer+;
#pragma link C++ class JetSorter+;
#pragma link C++ class JetSplitter+;
#pragma link C++ class JetToolRunner+;
#pragma link C++ class JetTrimmer+;
#pragma link C++ class JetSoftDrop+;
#pragma link C++ class JetModifiedMassDrop+;
#pragma link C++ class MuonSegmentPseudoJetGetter+;
#pragma link C++ class PseudoJetGetter+;
#pragma link C++ class PseudoJetGetterRegistry+;
#pragma link C++ class JetSJCalibrationGroomer+;

#endif

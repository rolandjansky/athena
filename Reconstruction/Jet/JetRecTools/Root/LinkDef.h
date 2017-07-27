/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <JetRecTools/JetTrackSelectionTool.h>
#include <JetRecTools/TrackVertexAssociationTool.h>
#include <JetRecTools/TrackPseudoJetGetter.h>

#include <JetRecTools/JetConstituentModSequence.h>
#include <JetRecTools/JetConstituentModifierBase.h>
#include <JetRecTools/CaloClusterConstituentsOrigin.h>
#include <JetRecTools/ConstituentSubtractorTool.h>
#include <JetRecTools/JetConstituentModifierBase.h>
#include <JetRecTools/JetConstituentModSequence.h>
#include <JetRecTools/SoftKillerWeightTool.h>
#include <JetRecTools/VoronoiWeightTool.h>
#include <JetRecTools/PFlowPseudoJetGetter.h>
#include <JetRecTools/CorrectPFOTool.h>
#include <JetRecTools/ChargedHadronSubtractionTool.h>
#include <JetRecTools/PuppiWeightTool.h>

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ class JetTrackSelectionTool+;
#pragma link C++ class TrackVertexAssociationTool+;
#pragma link C++ class TrackPseudoJetGetter+;

#pragma link C++ class JetConstituentModSequence+;
#pragma link C++ class JetConstituentModifierBase+;
#pragma link C++ class CaloClusterConstituentsOrigin+;
#pragma link C++ class ConstituentSubtractorTool+;
#pragma link C++ class JetConstituentModifierBase+;
#pragma link C++ class JetConstituentModSequence+;
#pragma link C++ class SoftKillerWeightTool+;
#pragma link C++ class VoronoiWeightTool+;
#pragma link C++ class CorrectPFOTool+;
#pragma link C++ class ChargedHadronSubtractionTool+;
#pragma link C++ class PuppiWeightTool+;

#pragma link C++ class PFlowPseudoJetGetter+;


#endif

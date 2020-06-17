/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PFlowUtils/IWeightPFOTool.h"
#include "JetMomentTools/JetClusterMomentsTool.h"
#include "JetMomentTools/JetConstitFourMomTool.h"
#include "JetMomentTools/JetECPSFractionTool.h"
#include "JetMomentTools/JetForwardJvtTool.h"
#include "JetMomentTools/JetLArHVTool.h"
#include "JetMomentTools/JetMuonSegmentMomentsTool.h"
#include "JetMomentTools/JetOriginCorrectionTool.h"
#include "JetMomentTools/JetPtAssociationTool.h"
#include "JetMomentTools/JetTrackMomentsTool.h"
#include "JetMomentTools/JetTrackSumMomentsTool.h"
#include "JetMomentTools/JetVertexFractionTool.h"
#include "JetMomentTools/JetVertexTaggerTool.h"
#include "JetMomentTools/JetWidthTool.h"
#include "JetMomentTools/JetCaloEnergies.h"
#include "JetMomentTools/JetQGTaggerVariableTool.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ class JetClusterMomentsTool+;
#pragma link C++ class JetConstitFourMomTool+;
#pragma link C++ class JetECPSFractionTool+;
#pragma link C++ class JetForwardJvtTool+;
#pragma link C++ class JetLArHVTool+;
#pragma link C++ class JetMuonSegmentMomentsTool+;
#pragma link C++ class JetOriginCorrectionTool+;
#pragma link C++ class JetPtAssociationTool+;
#pragma link C++ class JetTrackMomentsTool+;
#pragma link C++ class JetTrackSumMomentsTool+;
#pragma link C++ class JetVertexFractionTool+;
#pragma link C++ class JetVertexTaggerTool+;
#pragma link C++ class JetWidthTool+;
#pragma link C++ class JetCaloEnergies+;
#pragma link C++ class JetQGTaggerVariableTool+;

#endif

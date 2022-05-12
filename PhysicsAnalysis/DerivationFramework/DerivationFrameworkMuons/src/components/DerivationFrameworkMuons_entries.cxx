/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkMuons/AnalysisMuonThinningTool.h"
#include "DerivationFrameworkMuons/IDTrackCaloDepositsDecoratorTool.h"
#include "DerivationFrameworkMuons/MuonJetDrTool.h"
#include "DerivationFrameworkMuons/MuonTPExtrapolationTool.h"
#include "DerivationFrameworkMuons/MuonTPJpsiVertexFittingAlg.h"
#include "DerivationFrameworkMuons/MuonTruthClassifierFallback.h"
#include "DerivationFrameworkMuons/MuonTruthIsolationTool.h"
#include "DerivationFrameworkMuons/VertexDecoratorAlg.h"
#include "DerivationFrameworkMuons/dimuonTaggingTool.h"
#include "DerivationFrameworkMuons/isolationDecorator.h"

DECLARE_COMPONENT(DerivationFramework::MuonTruthClassifierFallback)
DECLARE_COMPONENT(DerivationFramework::MuonTruthIsolationTool)
DECLARE_COMPONENT(DerivationFramework::MuonJetDrTool)
DECLARE_COMPONENT(DerivationFramework::dimuonTaggingTool)
DECLARE_COMPONENT(DerivationFramework::isolationDecorator)
DECLARE_COMPONENT(DerivationFramework::AnalysisMuonThinningTool)
DECLARE_COMPONENT(MuonTPExtrapolationTool)
DECLARE_COMPONENT(IDTrackCaloDepositsDecoratorTool)
DECLARE_COMPONENT(MuonTPJpsiVertexFittingAlg)

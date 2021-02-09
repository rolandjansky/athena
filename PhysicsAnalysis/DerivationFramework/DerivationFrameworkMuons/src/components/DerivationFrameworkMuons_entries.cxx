/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkMuons/dimuonTaggingTool.h"
#include "DerivationFrameworkMuons/isolationDecorator.h"
#include "DerivationFrameworkMuons/mcpDecorator.h"
#include "DerivationFrameworkMuons/MuonTPExtrapolationTool.h"
#include "DerivationFrameworkMuons/IDTrackCaloDepositsDecoratorTool.h"
#include "DerivationFrameworkMuons/MuonIDCovMatrixDecorator.h"
#include "DerivationFrameworkMuons/MuonJetDrTool.h"
#include "DerivationFrameworkMuons/MuonTruthClassifierFallback.h"
#include "DerivationFrameworkMuons/MuonTruthIsolationTool.h"
#include "DerivationFrameworkMuons/MuonTPJpsiVertexFittingAlg.h"
#include "DerivationFrameworkMuons/VertexDecoratorAlg.h"

DECLARE_COMPONENT (DerivationFramework::MuonTruthClassifierFallback)
DECLARE_COMPONENT (DerivationFramework::MuonTruthIsolationTool)
DECLARE_COMPONENT (DerivationFramework::MuonJetDrTool)

DECLARE_COMPONENT( DerivationFramework::dimuonTaggingTool )
DECLARE_COMPONENT( DerivationFramework::isolationDecorator )
DECLARE_COMPONENT( DerivationFramework::mcpDecorator )
DECLARE_COMPONENT( MuonTPExtrapolationTool )
DECLARE_COMPONENT( IDTrackCaloDepositsDecoratorTool )
DECLARE_COMPONENT( DerivationFramework::MuonIDCovMatrixDecorator )
DECLARE_COMPONENT( DerivationFramework::MuonJetDrTool )

DECLARE_COMPONENT(MuonTPJpsiVertexFittingAlg)

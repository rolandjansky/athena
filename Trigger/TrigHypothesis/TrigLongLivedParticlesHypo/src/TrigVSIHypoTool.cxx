/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigVSIHypoTool.h"

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include <array>

using namespace TrigCompositeUtils;

TrigVSIHypoTool::TrigVSIHypoTool( const std::string& type,const std::string& name,const IInterface* parent )
: AthAlgTool( type, name, parent ),
m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode TrigVSIHypoTool::initialize()  {
   return StatusCode::SUCCESS;
}

StatusCode TrigVSIHypoTool::decide(  eventVtxInfo& vtxinfo )  const {
   if ( vtxinfo.previousDecisionIDs.count( m_decisionId.numeric() ) == 0 ) {
      ATH_MSG_DEBUG("Already rejected");
      return StatusCode::SUCCESS;
   }

   bool passNTrkCut = false;
   bool passRCut = false;
   bool passPtCut = false;
   bool passChi2Cut = false;
   bool passMassCut = false;

   bool passSelection = false;

   for (auto vertex : vtxinfo.vertices) {
      passNTrkCut = false;
      passRCut = false;
      passPtCut = false;
      passChi2Cut = false;
      passMassCut = false;

      if ( vertex == nullptr ) continue;
      const size_t ntrk  = vertex->nTrackParticles();
      const float  pT    = (vertex->isAvailable<float>("vsi_pT"))?    vertex->auxdata<float>("vsi_pT")    : 0.;
      const float  chi2  = vertex->chiSquared();
      const float  mass  = (vertex->isAvailable<float>("vsi_mass"))?  vertex->auxdata<float>("vsi_mass")  : 0.;

      const float  x_ = vertex->position().x();
      const float  y_ = vertex->position().y();
      const float  r  = std::sqrt(x_*x_+y_*y_);

      if (  ntrk >= m_minNTrks
         && (ntrk <= m_maxNTrks || m_maxNTrks <= m_minNTrks)
      )  passNTrkCut = true;

      if (  r >= m_minR
         && r <= m_maxR
      )  passRCut = true;

      if (  pT > m_minPt
         && pT < m_maxPt
      )  passPtCut = true;

      if (  chi2 < m_maxChi2
      )  passChi2Cut = true;

      if (  mass > m_minMass
         && mass < m_maxMass
      ) passMassCut = true;

      std::array<bool,5> decisions{ passNTrkCut, passRCut, passPtCut, passChi2Cut, passMassCut };

      if ( m_logicAnd ) {
         passSelection = std::all_of( decisions.begin(), decisions.end(), [](bool k ){return k;} );
      } else {
         passSelection = std::any_of( decisions.begin(), decisions.end(), [](bool k ){return k;} );
      }

      if (passSelection) break;
   }

   if ( !passSelection ) {
      return StatusCode::SUCCESS;
   } else {
      addDecisionID( m_decisionId.numeric(), vtxinfo.decision );
      ATH_MSG_DEBUG ("REGTEST event accepted");
   }

   return StatusCode::SUCCESS;
}

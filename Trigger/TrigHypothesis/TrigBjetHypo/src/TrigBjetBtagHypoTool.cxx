/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
   */

// ************************************************
//
// NAME:  TrigBjetBtagHypoTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypoTool
//
// AUTHOR:   Carlo Varni
// EMAIL: carlo.varni@cern.ch
//
// ************************************************

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "TrigBjetBtagHypoTool.h"
#include "xAODBTagging/BTagging.h"

TrigBjetBtagHypoTool::TrigBjetBtagHypoTool( const std::string& type,
    const std::string& name,
    const IInterface* parent ) :
  AthAlgTool( type, name, parent ),
  m_decisionId(  HLT::Identifier::fromToolName( name ) ) {}

  // -----------------------------------------------------------------------------------------------------------------

  StatusCode TrigBjetBtagHypoTool::initialize()  {

    ATH_MSG_DEBUG(  "declareProperty review:"   );
    ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId  );

    if ( not m_monTool.name().empty() ) {
      ATH_CHECK( m_monTool.retrieve() );
      ATH_MSG_DEBUG("MonTool name: " << m_monTool);
    }
    ATH_CHECK(m_checks.retrieve());

    for (const auto& monpair: m_monMap) {
      m_monPairs.emplace_back(monpair.first, monpair.second);
    }

    return StatusCode::SUCCESS;
  }

// -----------------------------------------------------------------------------------------------------------------

StatusCode TrigBjetBtagHypoTool::decide( std::vector< TrigBjetBtagHypoToolInfo >& bTagInfos ) const {

  ATH_MSG_DEBUG( "Executing "<< name() );

  for ( TrigBjetBtagHypoToolInfo& bTagInfo : bTagInfos ) {

    // -------------------------------------
    // Check the HypoTool's chain is active
    if ( not TrigCompositeUtils::passed( getId().numeric(),bTagInfo.previousDecisionIDs ) )
      continue;

    // -------------------------------------
    // Check Primary Vertex
    const xAOD::Vertex *vertex = *(bTagInfo.vertexEL);

    ATH_MSG_DEBUG( "Event Vertex [x,y,z]=["
        << vertex->x()
        << ","<<  vertex->y()
        << ","<<  vertex->z() <<  "]" );

    ATH_MSG_DEBUG( "   ** Vertex Type = " << vertex->vertexType() );

    // -------------------------------------
    // Compute trigger decision
    bool pass = true;
    std::string stage = "pass";

    int bits = bTagInfo.beamSpot->beamStatus();
    // beamspot is converged if first and second bit are set
    bool converged = ((bits | 0b11) == bits);
    // beamspot is online if thrid bit is set
    bool online = ((bits | 0b001) == bits);

    ATH_MSG_DEBUG(
      "Beamspot status: 0x" << std::hex << bits << ", "
      "converged: " << std::boolalpha << converged << ", "
      "online: " << online);

    if (m_vetoBadBeamspot && !converged && online) {
      ATH_MSG_DEBUG( "Beamspot has problems" );
      ATH_MSG_DEBUG( "Trigger decision is FALSE" );
      pass = false;
      stage = "no beamspot";
    } else if ( vertex->vertexType() != xAOD::VxType::VertexType::PriVtx ) {
      ATH_MSG_DEBUG( "Vertex is not a valid primary vertex!" );
      ATH_MSG_DEBUG( "Trigger decision is FALSE" );
      pass = false;
      stage = "no primary vertex";
    } else {
      const xAOD::BTagging *btagging = *(bTagInfo.btaggingEL);

      // monitor floats
      std::vector<Monitored::Scalar<float>> mons;
      for (const auto& monpair: m_monPairs) {
        mons.emplace_back(monpair.second, monpair.first(*btagging));
      }

      if ( not btagging->auxdata<char>("SV1_isDefaults") ){
	mons.emplace_back("SV1_masssvx",btagging->auxdata<float>("SV1_masssvx"));
	mons.emplace_back("SV1_efracsvx",btagging->auxdata<float>("SV1_efracsvx"));
      }
      if ( not btagging->auxdata<char>("JetFitter_isDefaults") ){
	mons.emplace_back("JetFitter_mass",btagging->auxdata<float>("JetFitter_mass"));
	mons.emplace_back("JetFitter_energyFraction",btagging->auxdata<float>("JetFitter_energyFraction"));
      }



      std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> mons_wrappers(
        mons.begin(), mons.end());
      Monitored::Group(m_monTool, mons_wrappers);

      for (const auto& check: m_checks) {
        if (!check->passThreshold(*btagging)) {
          pass = false;
          stage = "fail";
          break;
        }
      }
    }

    Monitored::Group(m_monTool, Monitored::Scalar("stage", stage));


    // -------------------------------------
    if ( pass ) {
      ATH_MSG_DEBUG( "Selection cut satisfied, accepting the event" );
      TrigCompositeUtils::addDecisionID( getId().numeric(),bTagInfo.decision );
    } else {
      ATH_MSG_DEBUG( "Selection cut not satisfied, rejecting the event" );
    }

    ATH_MSG_DEBUG( "PRINTING DECISION" );
    ATH_MSG_DEBUG( *bTagInfo.decision );
  }

  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------------------------------------------------

TrigCompositeUtils::DecisionID TrigBjetBtagHypoTool::decisionId() const { return m_decisionId.numeric(); }
const HLT::Identifier TrigBjetBtagHypoTool::getId() const { return m_decisionId; }

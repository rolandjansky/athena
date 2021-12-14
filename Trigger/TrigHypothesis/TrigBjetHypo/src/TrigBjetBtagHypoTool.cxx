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

TrigBjetBtagHypoTool::TrigBjetBtagHypoTool( const std::string& type,
    const std::string& name,
    const IInterface* parent ) :
  AthAlgTool( type, name, parent ),
  m_decisionId(  HLT::Identifier::fromToolName( name ) ) {}

  // -----------------------------------------------------------------------------------------------------------------

  StatusCode TrigBjetBtagHypoTool::initialize()  {

    ATH_MSG_DEBUG(  "declareProperty review:"   );
    ATH_MSG_DEBUG(  "   " << m_acceptAll  );
    ATH_MSG_DEBUG(  "   " << m_bTaggingCut   );
    ATH_MSG_DEBUG(  "   " << m_cFrac   );
    ATH_MSG_DEBUG(  "   " << m_tagger     );

    ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId  );

    if ( not m_monTool.name().empty() ) {
      ATH_CHECK( m_monTool.retrieve() );
      ATH_MSG_DEBUG("MonTool name: " << m_monTool);
    }

    if (m_cFrac < 0 || 1 < m_cFrac) {
      ATH_MSG_ERROR("c-jet fraction for b-tagger likelihood ratio calculation should be between 0 and 1!");
      ATH_MSG_ERROR("c-jet fraction requested: " << m_cFrac);
      return StatusCode::FAILURE;
    }

    if (m_tagger == "") {
      ATH_MSG_ERROR("no b-jet tagger requested; please provid a b-jet tagger string!");
      return StatusCode::FAILURE;
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

    if ( vertex->vertexType() != xAOD::VxType::VertexType::PriVtx ) {
      ATH_MSG_DEBUG( "Vertex is not a valid primary vertex!" );
      ATH_MSG_DEBUG( "Trigger decision is FALSE" );
      pass = false;
    } else {

      const xAOD::BTagging *btagging = *(bTagInfo.btaggingEL);
      double btaggingWeight = -1000;

      double pu = -1;
      double pb = -1;
      double pc = -1;

      btagging->pu(m_tagger, pu);
      btagging->pb(m_tagger, pb);
      btagging->pc(m_tagger, pc);

      btaggingWeight = log( pb / (pu * (1 - m_cFrac) + m_cFrac * pc) );


      auto monitor_btag_pu = Monitored::Scalar( "btag_pu", pu);
      auto monitor_btag_pb = Monitored::Scalar( "btag_pb", pb);
      auto monitor_btag_pc = Monitored::Scalar( "btag_pc", pc);
      auto monitor_btag_weight = Monitored::Scalar( "btag_llr", btaggingWeight);

      auto monitor_group_for_btag_weights =
        Monitored::Group( m_monTool, monitor_btag_pu, monitor_btag_pb, monitor_btag_pc, monitor_btag_weight);

      ATH_MSG_DEBUG( "Cutting b-tagging weight at " << m_bTaggingCut.value() );

      if ( btaggingWeight < m_bTaggingCut )
        pass = false;
    }

    // -------------------------------------
    if ( pass == true ) {
      ATH_MSG_DEBUG( "Selection cut satisfied, accepting the event" );
      TrigCompositeUtils::addDecisionID( getId().numeric(),bTagInfo.decision );
    } else {
      ATH_MSG_DEBUG( "Selection cut not satisfied, rejecting the event" );
    }

    ATH_MSG_DEBUG( "b-Tagging decision is " << (pass?"TRUE":"FALSE") );
    ATH_MSG_DEBUG( "PRINTING DECISION" );
    ATH_MSG_DEBUG( *bTagInfo.decision );
  }

  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------------------------------------------------

TrigCompositeUtils::DecisionID TrigBjetBtagHypoTool::decisionId() const { return m_decisionId.numeric(); }
const HLT::Identifier TrigBjetBtagHypoTool::getId() const { return m_decisionId; }

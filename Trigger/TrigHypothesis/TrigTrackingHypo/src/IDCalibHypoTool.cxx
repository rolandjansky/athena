/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

  * Trigger Hypo Tool for IDCalib stream trigger
  * author Kunihiro Nagano <kunihiro.nagano@cern.ch>
*/

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "IDCalibHypoTool.h"
#include "GaudiKernel/PhysicalConstants.h"

using namespace TrigCompositeUtils;

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

IDCalibHypoTool::IDCalibHypoTool( const std::string& type, const std::string& name, const IInterface* parent ) 
   : AthAlgTool( type, name, parent ),
     m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode IDCalibHypoTool::initialize()
{
   ATH_MSG_INFO( "Initialization completed successfully:" );
   ATH_MSG_INFO( "  cutTrackPtGeV = " << m_cutTrackPtGeV ); 
   ATH_MSG_INFO( "Tool configured for chain/id: " << m_decisionId );

   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode IDCalibHypoTool::decide( std::vector<IDCalibHypoInfo>& toolInputs ) const 
{
   size_t numTrigger = m_cutTrackPtGeV.size(); 
   size_t numTrks    = toolInputs.size();
   
   ATH_MSG_DEBUG( "Number of Tracks = " << numTrks );

   if ( numTrigger == 1 ) {
      ATH_MSG_DEBUG( "Applying selection of single for " << m_decisionId );
      return inclusiveSelection(toolInputs);
   }
   else {
      ATH_MSG_DEBUG( "Applying selection of multiplicity for " << m_decisionId );
      return multiplicitySelection(toolInputs);
   }

   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode IDCalibHypoTool::inclusiveSelection(std::vector<IDCalibHypoInfo>& toolInputs) const
{
   bool isPassed = false;
   unsigned int iTrk=0;
   for ( auto& input: toolInputs ) {
      ATH_MSG_DEBUG( "--- iTrk=" << iTrk << " ---" );
      if ( TrigCompositeUtils::passed( m_decisionId.numeric(), input.previousDecisionsIDs ) ) {
         if ( decideOnSingleObject( input, 0 )==true ) {
            ATH_MSG_DEBUG( "   Passed selection --> adding DecisionID" );
	    isPassed = true;
            TrigCompositeUtils::addDecisionID(m_decisionId, input.decision);
         }
      } else {
	 ATH_MSG_DEBUG( "   Not match DecisionID: " << m_decisionId );
      } 
      ++iTrk;
   }  

   ATH_MSG_DEBUG( "Inclusive selection isPassed = " << isPassed );
   return StatusCode::SUCCESS; 
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode IDCalibHypoTool::multiplicitySelection(std::vector<IDCalibHypoInfo>& toolInputs) const
{
   HLT::Index2DVec passingSelection( m_cutTrackPtGeV.size() );

   for ( size_t cutIndex=0; cutIndex < m_cutTrackPtGeV.size(); ++cutIndex ) {
      size_t elementIndex{ 0 };
      for ( auto& input: toolInputs ) {
         if ( TrigCompositeUtils::passed( m_decisionId.numeric(), input.previousDecisionsIDs ) ) {
            if ( decideOnSingleObject( input, cutIndex ) == true ) {
               ATH_MSG_DEBUG( "Pass through selection " << m_decisionId << " : Event[" << elementIndex << "]" );
               passingSelection[cutIndex].push_back( elementIndex );
            }
         }
	 else {
            ATH_MSG_DEBUG( "Not match DecisionID " << m_decisionId );
         }
         elementIndex++;
      }
      // If no object passes the selection, multipul selection should stop.
      if ( passingSelection[cutIndex].empty() ) {
         ATH_MSG_DEBUG( "No object passed selection " << cutIndex << " rejecting" );
         return StatusCode::SUCCESS;
      }
   }

   std::set<size_t> passingIndices;
   HLT::elementsInUniqueCombinations( passingSelection, passingIndices );

   if ( passingIndices.empty() ) {
      ATH_MSG_DEBUG( "No track passed through selection " << m_decisionId );
      return StatusCode::SUCCESS;
   }

   for ( auto idx: passingIndices ) {
      ATH_MSG_DEBUG( "track[" << idx << "] passes through Chain/ID " << m_decisionId << " with pT" );
      TrigCompositeUtils::addDecisionID( m_decisionId.numeric(), toolInputs[idx].decision );
   }

   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

bool IDCalibHypoTool::decideOnSingleObject( IDCalibHypoInfo& input, size_t cutIndex ) const
{
   bool is_passed = false;

   // values
   float pt = input.track->pt();
   pt /= Gaudi::Units::GeV;

   // pt selection
   if( pt >= m_cutTrackPtGeV[cutIndex] ) is_passed = true;

   //
   ATH_MSG_DEBUG( "   isPassed = " << is_passed << ", cut index / pT = " << cutIndex << " / " << pt );
   return is_passed;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

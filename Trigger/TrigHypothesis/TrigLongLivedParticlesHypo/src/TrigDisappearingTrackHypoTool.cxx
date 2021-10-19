/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

  * Trigger Hypo Tool, that is aimed at triggering disappearing track candidates
  * author Kunihiro Nagano <kunihiro.nagano@cern.ch>
*/

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "TrigDisappearingTrackHypoTool.h"
#include "GaudiKernel/PhysicalConstants.h"

using namespace TrigCompositeUtils;

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

TrigDisappearingTrackHypoTool::TrigDisappearingTrackHypoTool( const std::string& type, 
							      const std::string& name, 
							      const IInterface* parent ) 
   : AthAlgTool( type, name, parent ),
     m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

TrigDisappearingTrackHypoTool::~TrigDisappearingTrackHypoTool() {}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigDisappearingTrackHypoTool::initialize()
{
   ATH_MSG_INFO( "Initialization completed successfully:" );
   ATH_MSG_INFO( "  cutTrackPtGeV = " << m_cutTrackPtGeV ); 
   ATH_MSG_INFO( "Tool configured for chain/id: " << m_decisionId  );

   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigDisappearingTrackHypoTool::decide( std::vector<DisTrkHypoInfo>& toolInputs ) const 
{
   size_t numTrigger = m_cutTrackPtGeV.size(); 
   size_t numDisTrks = toolInputs.size();
   
   ATH_MSG_VERBOSE( "Number of DisTrks = " << numDisTrks );

   if ( numTrigger == 1 ) {
      ATH_MSG_VERBOSE( "Applying selection of single for " << m_decisionId );
      return inclusiveSelection(toolInputs);
   }
   else {
      ATH_MSG_VERBOSE( "Applying selection of multiplicity for " << m_decisionId );
      return multiplicitySelection(toolInputs);
   }

   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigDisappearingTrackHypoTool::inclusiveSelection(std::vector<DisTrkHypoInfo>& toolInputs) const
{
   bool isPassed = false;
   unsigned int iDisTrk=0;
   for ( auto& input: toolInputs ) {
      ATH_MSG_VERBOSE( "--- iDisTrk=" << iDisTrk << " ---" );
      if ( TrigCompositeUtils::passed( m_decisionId.numeric(), input.previousDecisionsIDs ) ) {
         if ( decideOnSingleObject( input, 0 )==true ) {
            ATH_MSG_VERBOSE( "   Passed selection --> adding DecisionID" );
	    isPassed = true;
            TrigCompositeUtils::addDecisionID(m_decisionId, input.decision);
         }
      } else {
	 ATH_MSG_VERBOSE( "   Not match DecisionID: " << m_decisionId );
      } 
      ++iDisTrk;
   }  

   ATH_MSG_VERBOSE( "Inclusive selection isPassed = " << isPassed );
   return StatusCode::SUCCESS; 
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigDisappearingTrackHypoTool::multiplicitySelection(std::vector<DisTrkHypoInfo>& toolInputs) const
{
   HLT::Index2DVec passingSelection( m_cutTrackPtGeV.size() );

   for ( size_t cutIndex=0; cutIndex < m_cutTrackPtGeV.size(); ++cutIndex ) {
      size_t elementIndex{ 0 };
      for ( auto& input: toolInputs ) {
         if ( TrigCompositeUtils::passed( m_decisionId.numeric(), input.previousDecisionsIDs ) ) {
            if ( decideOnSingleObject( input, cutIndex ) == true ) {
               ATH_MSG_VERBOSE( "Pass through selection " << m_decisionId << " : Event[" << elementIndex << "]" );
               passingSelection[cutIndex].push_back( elementIndex );
            }
         }
	 else {
            ATH_MSG_VERBOSE( "Not match DecisionID " << m_decisionId );
         }
         elementIndex++;
      }
      // If no object passes the selection, multipul selection should stop.
      if ( passingSelection[cutIndex].empty() ) {
         ATH_MSG_VERBOSE( "No object passed selection " << cutIndex << " rejecting" );
         return StatusCode::SUCCESS;
      }
   }

   std::set<size_t> passingIndices;
   HLT::elementsInUniqueCombinations( passingSelection, passingIndices );

   if ( passingIndices.empty() ) {
      ATH_MSG_VERBOSE( "No track passed through selection " << m_decisionId );
      return StatusCode::SUCCESS;
   }

   for ( auto idx: passingIndices ) {
      ATH_MSG_VERBOSE( "track[" << idx << "] passes through Chain/ID " << m_decisionId << " with pT" );
      TrigCompositeUtils::addDecisionID( m_decisionId.numeric(), toolInputs[idx].decision );
   }

   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

bool TrigDisappearingTrackHypoTool::decideOnSingleObject( DisTrkHypoInfo& input, size_t cutIndex ) const
{
   // values
   const std::string prefix = "disTrk";

   auto distrk = input.disTrk;

   int   category  = distrk->getDetail<int>  (prefix+"_category");
   float pt        = distrk->getDetail<float>(prefix+"_pt");
   float refit_pt  = distrk->getDetail<float>(prefix+"_refit_pt");
   float bdt_score = distrk->getDetail<float>(prefix+"_bdtscore");
   pt       /= 1000.0;
   refit_pt /= 1000.0;

   // selection
   bool is_passed = false;
   if( category==1 ) {
      float bdt_threshold_pix4l_sct0 = m_cutBDTPix4Sct0[cutIndex];
      float pt_threshold_pix4l_sct0  = m_cutTrackPtGeV[cutIndex];
      if( pt >= pt_threshold_pix4l_sct0 && bdt_score >= bdt_threshold_pix4l_sct0 ) is_passed = true;
   }
   else if( category==2 ) {
      float bdt_threshold_pix4l_sct1p      = m_cutBDTPix4Sct1p[cutIndex];
      float refit_pt_threshold_pix4l_sct1p = m_cutTrackPtGeV[cutIndex];
      if( refit_pt >= refit_pt_threshold_pix4l_sct1p && bdt_score >= bdt_threshold_pix4l_sct1p ) is_passed = true;
   }
   else if( category==3 ) {
      float bdt_threshold_pix3l_sct0 = m_cutBDTPix3Sct0[cutIndex];
      float pt_threshold_pix3l_sct0  = m_cutTrackPtGeV[cutIndex];
      if( pt >= pt_threshold_pix3l_sct0 && bdt_score >= bdt_threshold_pix3l_sct0 ) is_passed = true;
   }
   else if( category==4 ) {
      float bdt_threshold_pix3l_sct1p      = m_cutBDTPix3Sct1p[cutIndex];
      float refit_pt_threshold_pix3l_sct1p = m_cutTrackPtGeV[cutIndex];
      if( refit_pt >= refit_pt_threshold_pix3l_sct1p && bdt_score >= bdt_threshold_pix3l_sct1p ) is_passed = true;
   }
   else {
      ATH_MSG_VERBOSE( "   category is out of range = " << category );
   }
   ATH_MSG_VERBOSE( "   isPassed = " << is_passed << ", cut index / pT / refitPt / BDT score = " << cutIndex << " / " << pt << " / " << refit_pt << " / " << bdt_score);

   //
   return is_passed;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

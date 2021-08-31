/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

  * Trigger Hypo Tool, that is aimed at triggering large dEdx tracks
  * author Kunihiro Nagano <kunihiro.nagano@cern.ch> - KEK
*/

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "TrigdEdxTrackHypoTool.h"
#include "GaudiKernel/PhysicalConstants.h"

using namespace TrigCompositeUtils;

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

TrigdEdxTrackHypoTool::TrigdEdxTrackHypoTool( const std::string& type, 
					      const std::string& name, 
					      const IInterface* parent ) 
   : AthAlgTool( type, name, parent ),
     m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

TrigdEdxTrackHypoTool::~TrigdEdxTrackHypoTool() {}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigdEdxTrackHypoTool::initialize()
{
   ATH_MSG_INFO( "Initialization completed successfully:" );
   ATH_MSG_INFO( "  cutTrackPtGeV         = " << m_cutTrackPtGeV) ; 
   ATH_MSG_INFO( "  cutTrackEta           = " << m_cutTrackEta );  
   ATH_MSG_INFO( "  cutTrackdEdx          = " << m_cutTrackdEdx );  
   ATH_MSG_INFO( "  cutTrackNhighdEdxHits = " << m_cutTrackNhighdEdxHits ); 
   ATH_MSG_INFO( "  cutTrackHighdEdxDef   = " << m_cutTrackHighdEdxDef ); 
   ATH_MSG_INFO( "Tool configured for chain/id: " << m_decisionId );
   
   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigdEdxTrackHypoTool::decide( std::vector<dEdxTrkHypoInfo>& toolInputs ) const 
{
   size_t numTrigger  = m_cutTrackPtGeV.size(); 
   size_t numdEdxTrks = toolInputs.size();
   
   ATH_MSG_VERBOSE( "Number of dEdxTrks = " << numdEdxTrks );

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

StatusCode TrigdEdxTrackHypoTool::inclusiveSelection(std::vector<dEdxTrkHypoInfo>& toolInputs) const
{
   bool isPassed = false;
   unsigned int idEdxTrk=0;
   for ( auto& input: toolInputs ) {
      ATH_MSG_VERBOSE( "--- idEdxTrk=" << idEdxTrk << " ---");
      if ( TrigCompositeUtils::passed( m_decisionId.numeric(), input.previousDecisionsIDs ) ) {
         if ( decideOnSingleObject( input, 0 )==true ) {
            ATH_MSG_VERBOSE( "   Passed selection --> adding DecisionID");
	    isPassed = true;
            TrigCompositeUtils::addDecisionID(m_decisionId, input.decision);
         }
      } else {
	 ATH_MSG_VERBOSE( "   Not match DecisionID: " << m_decisionId );
      } 
      ++idEdxTrk;
   }  

   ATH_MSG_VERBOSE( "Inclusive selection isPassed = " << isPassed);
   return StatusCode::SUCCESS; 
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigdEdxTrackHypoTool::multiplicitySelection(std::vector<dEdxTrkHypoInfo>& toolInputs) const
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

bool TrigdEdxTrackHypoTool::decideOnSingleObject( dEdxTrkHypoInfo& input, size_t cutIndex ) const
{
   const xAOD::TrigComposite* trk = input.dedxTrk;

   // pT cut
   float trackPt     = trk->getDetail<float>("HPtdEdxTrk_pt");
   float trackPtGeV  = trackPt / 1000.0;
   float ptThreshold = m_cutTrackPtGeV[cutIndex];
   if( trackPtGeV < ptThreshold ) return false;

   // eta cut
   float trackEta      = trk->getDetail<float>("HPtdEdxTrk_eta");
   float etaThreshold  = m_cutTrackEta[cutIndex];
   if( std::abs(trackEta) > etaThreshold ) return false;

   // a0beam cut
   float tracka0beam = trk->getDetail<float>("HPtdEdxTrk_a0beam");
   float a0Threshold = m_cutTracka0beam[cutIndex];
   if( std::abs(tracka0beam) > a0Threshold ) return false;
   
   // dEdx cut
   float trackdEdx     = trk->getDetail<float>("HPtdEdxTrk_dedx");
   float dedxThreshold = m_cutTrackdEdx[cutIndex];
   if( trackdEdx < dedxThreshold ) return false;

   // nr of high dEdx hits
   std::string name = "HPtdEdxTrk_n_hdedx_hits_" + m_cutTrackHighdEdxDef[cutIndex];
   if( ! trk->hasDetail<int16_t>(name) ) ATH_MSG_ERROR( "HPtdEdxTrk container does not have entry for " << name );
   int n_hdedx_hits = (int)trk->getDetail<int16_t>(name);
   int n_hdedx_hits_threshold = m_cutTrackNhighdEdxHits[cutIndex];
   if( n_hdedx_hits < n_hdedx_hits_threshold ) return false;

   //
   ATH_MSG_VERBOSE( "   Selected, cut index / pT / eta / a0 / dEdx / n high dEdx hits = " << cutIndex << " / " << trackPtGeV << " / " << trackEta << " / " << tracka0beam << " / " << trackdEdx << " / " << n_hdedx_hits);

   return true;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

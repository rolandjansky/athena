/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

  * Trigger Hypo Tool, that is aimed at triggering displaced vertex
  * author Kunihiro Nagano <kunihiro.nagano@cern.ch> - KEK
*/

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "TrigHitDVHypoTool.h"

using namespace TrigCompositeUtils;

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

TrigHitDVHypoTool::TrigHitDVHypoTool( const std::string& type, 
				      const std::string& name, 
				      const IInterface* parent ) 
   : AthAlgTool( type, name, parent ),
     m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

TrigHitDVHypoTool::~TrigHitDVHypoTool() {}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigHitDVHypoTool::initialize()
{
   ATH_MSG_INFO( "Initialization completed successfully:" );
   ATH_MSG_INFO( "   cutJetPtGeV = " << m_cutJetPtGeV ); 
   ATH_MSG_INFO( "   cutJetEta   = " << m_cutJetEta );  
   ATH_MSG_INFO( "   doSPseed    = " << m_doSPseed );  

   ATH_MSG_INFO( "Tool configured for chain/id: " << m_decisionId );

   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

float TrigHitDVHypoTool::getBDTthreshold(float mu) const
{
   const float defaultBDTthreshold = 0.03773; // 70% effi at mu=60
   
   if( mu<10 || 60<mu ) return defaultBDTthreshold;

   // pol4 fitting
   const float par0 =  0.404727;
   const float par1 = -0.00344154;
   const float par2 = -6.54218e-05;
   const float par3 = -3.39841e-07;
   const float par4 =  9.93062e-09;

   float thres = par0 + par1*mu + par2*mu*mu + par3*mu*mu*mu + par4*mu*mu*mu*mu;
   return thres;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigHitDVHypoTool::decide( std::vector<HitDVHypoInfo>& toolInputs ) const 
{
   size_t numTrigger = m_cutJetPtGeV.size(); 
   size_t numHitDVs  = toolInputs.size();
   
   ATH_MSG_DEBUG( "   Number of HitDVs = " << numHitDVs );

   if ( numTrigger == 1 ) {
      ATH_MSG_DEBUG( "   Applying selection of single for " << m_decisionId );
      return inclusiveSelection(toolInputs);
   }
   else {
      ATH_MSG_DEBUG( "   Applying selection of multiplicity for " << m_decisionId );
      return multiplicitySelection(toolInputs);
   }

   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigHitDVHypoTool::inclusiveSelection(std::vector<HitDVHypoInfo>& toolInputs) const
{
   bool isJetCutPassed = false;
   for ( auto& input: toolInputs ) {
      if ( TrigCompositeUtils::passed( m_decisionId.numeric(), input.previousDecisionsIDs ) ) {
         if ( decideOnSingleObject( input, 0, true )==true ) {
	    isJetCutPassed = true;
	    break;
	 }
      }
   }
   
   if( ! isJetCutPassed ) return StatusCode::SUCCESS;

   bool isPassed = false;
   unsigned int idv=0;
   for ( auto& input: toolInputs ) {
      ATH_MSG_DEBUG( "   --- idv=" << idv << " ---");
      if ( TrigCompositeUtils::passed( m_decisionId.numeric(), input.previousDecisionsIDs ) ) {
         if ( decideOnSingleObject( input, 0, false )==true ) {
            ATH_MSG_DEBUG( "   Passed selection --> adding DecisionID");
	    isPassed = true;
            TrigCompositeUtils::addDecisionID(m_decisionId, input.decision);
         }
      } else {
	 ATH_MSG_DEBUG( "   Not match DecisionID: " << m_decisionId );
      } 
      ++idv;
   }  

   ATH_MSG_DEBUG( "Inclusive selection isPassed = " << isPassed);
   return StatusCode::SUCCESS; 
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigHitDVHypoTool::multiplicitySelection(std::vector<HitDVHypoInfo>& toolInputs) const
{
   unsigned int n_jetpassed = 0;
   for ( size_t cutIndex=0; cutIndex < m_cutJetPtGeV.size(); ++cutIndex ) {
      bool isJetCutPassed = false;
      for ( auto& input: toolInputs ) {
	 if ( TrigCompositeUtils::passed( m_decisionId.numeric(), input.previousDecisionsIDs ) ) {
	    if ( decideOnSingleObject( input, cutIndex, true )==true ) {
	       isJetCutPassed = true;
	       break;
	    }
	 }
      }
      if( isJetCutPassed ) ++n_jetpassed;
   }
   if( n_jetpassed < m_cutJetPtGeV.size() ) return StatusCode::SUCCESS;

   //
   HLT::Index2DVec passingSelection( m_cutJetPtGeV.size() );

   for ( size_t cutIndex=0; cutIndex < m_cutJetPtGeV.size(); ++cutIndex ) {
      size_t elementIndex{ 0 };
      for ( auto& input: toolInputs ) {
         if ( TrigCompositeUtils::passed( m_decisionId.numeric(), input.previousDecisionsIDs ) ) {
            if ( decideOnSingleObject( input, cutIndex, false ) == true ) {
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

bool TrigHitDVHypoTool::decideOnSingleObject( HitDVHypoInfo& input, size_t cutIndex, bool isOnlyJetCut ) const
{
   float ptThreshold  = m_cutJetPtGeV[cutIndex];
   float etaThreshold = m_cutJetEta[cutIndex];

   const xAOD::TrigComposite* dv = input.hitDV;
   int   seed_type = dv->getDetail<int>  ("hitDV_seed_type");
   float seed_eta  = dv->getDetail<float>("hitDV_seed_eta"); 
   float seed_pt   = dv->getDetail<float>("hitDV_seed_pt"); 

   // if only jet pt/eta cut
   if( isOnlyJetCut ) {
      if( seed_type != 0 )                    return false;
      if( std::abs(seed_eta) > etaThreshold ) return false;
      if( seed_pt < ptThreshold )             return false;
      // passed
      return true;
   }

   if( input.isSPOverflow ) return true;

   // normal cut
   bool  doSPseed     = m_doSPseed[cutIndex];
   float BDTthreshold = getBDTthreshold(input.averageMu);
   float bdt_score    = dv->getDetail<float>("hitDV_bdt_score");

   if( ! doSPseed && seed_type==1 )            return false;
   if( seed_type==0 && seed_pt < ptThreshold ) return false;
   if( std::abs(seed_eta) > etaThreshold )     return false;
   if( bdt_score < BDTthreshold )              return false;

   ATH_MSG_DEBUG( "   Selected, cut index / seed_type / seed_eta / bdt_score = " << cutIndex << " / " << seed_type << " / " << seed_eta << " / " << bdt_score);

   return true;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

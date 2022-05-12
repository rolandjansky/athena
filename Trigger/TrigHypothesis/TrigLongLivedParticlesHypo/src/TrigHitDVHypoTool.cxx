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
   ATH_MSG_INFO( "   effBDT      = " << m_effBDT );
   ATH_MSG_INFO( "   doSPseed    = " << m_doSPseed );

   ATH_MSG_INFO( "Tool configured for chain/id: " << m_decisionId );

   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

float TrigHitDVHypoTool::getBDTthreshold_0eta1(float mu, float eff) const
{
   const float defaultBDTthreshold = 0.33482806; // 70% effi at mu=60

   if( mu<10 || 80<mu ) return defaultBDTthreshold;
   if( eff<0.5 || 1.<eff ) return defaultBDTthreshold;

   // pol4*4 fitting
   const std::vector<std::vector<float>> par = {
      {  2.9044277,     -6.3516751,     -4.0085606,      25.456841,     -18.208683     },
      {  0.066196623,   -0.098157640,    0.027455008,   -0.19900459,     0.16721183    },
      { -0.00091570959,  1.4831281e-05,  0.0028768317,  -0.00053051617,  0.00040427140 },
      {  2.5249073e-06,  1.7882097e-06, -7.1741129e-06, -9.7309257e-07, -3.2151707e-05 },
      { -8.2905419e-08,  3.9337893e-07, -5.8968642e-07,  2.6341441e-07,  2.4551736e-07 }
   };

   float thres = defaultBDTthreshold;
   for (size_t i = 0; i < 5; i++) {
      for (size_t j = 0; j < 5; j++) {
         float tmp = par[i][j];
         for (size_t imu = 0; imu < i; imu++)    tmp *= mu;
         for (size_t jeff = 0; jeff < j; jeff++) tmp *= eff;
         thres += tmp;
      }
   }

   return thres;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

float TrigHitDVHypoTool::getBDTthreshold_1eta2(float mu, float eff) const
{
   const float defaultBDTthreshold = 0.27072057; // 70% effi at mu=60

   if( mu<10 || 80<mu ) return defaultBDTthreshold;
   if( eff<0.5 || 1.<eff ) return defaultBDTthreshold;

   // pol4*4 fitting
   const std::vector<std::vector<float>> par = {
      {  1.8938782,     -1.7096217,     -8.7938548,      19.779355,     -10.656187     },
      {  0.048863505,   -0.18192399,     0.30932576,    -0.11527519,    -0.13591443    },
      { -0.00060239060,  0.00096109052, -0.0015378287,  -0.0030951666,   0.0063958596  },
      {  2.5855329e-06,  7.3128949e-06, -6.9676558e-06,  4.2141737e-05, -7.1813827e-05 },
      { -4.8077006e-08,  1.0646790e-07, -8.0539473e-08, -1.9821312e-07,  3.4713098e-07 }
   };

   float thres = defaultBDTthreshold;
   for (size_t i = 0; i < 5; i++) {
      for (size_t j = 0; j < 5; j++) {
         float tmp = par[i][j];
         for (size_t imu = 0; imu < i; imu++)    tmp *= mu;
         for (size_t jeff = 0; jeff < j; jeff++) tmp *= eff;
         thres += tmp;
      }
   }

   return thres;
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
   float eff = m_effBDT[cutIndex];

   const xAOD::TrigComposite* dv = input.hitDV;
   int   seed_type = dv->getDetail<int>  ("hitDV_seed_type");
   float seed_eta  = dv->getDetail<float>("hitDV_seed_eta");
   float seed_pt   = dv->getDetail<float>("hitDV_seed_pt");

   // if only jet pt/eta cut
   if( isOnlyJetCut ) {
      if( seed_type != SeedType::HLTJet )     return false;
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

   if ( std::abs(seed_eta) < 1 ) {
      BDTthreshold = getBDTthreshold_0eta1(input.averageMu, eff);
   } else if ( std::abs(seed_eta) < 2 ) {
      BDTthreshold = getBDTthreshold_1eta2(input.averageMu, eff);
   }

   if( ! doSPseed                   && seed_type==SeedType::SP ) return false;
   if( seed_type==SeedType::HLTJet  && seed_pt < ptThreshold   ) return false;
   if( std::abs(seed_eta) > etaThreshold )     return false;
   if( bdt_score < BDTthreshold )              return false;

   ATH_MSG_DEBUG( "   Selected, cut index / seed_type / seed_eta / bdt_score = " << cutIndex << " / " << seed_type << " / " << seed_eta << " / " << bdt_score);

   return true;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

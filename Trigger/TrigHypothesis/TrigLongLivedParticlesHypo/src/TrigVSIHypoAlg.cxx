/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigVSIHypoAlg.h"

#include "xAODTracking/TrackParticleAuxContainer.h"
#include "AthViews/ViewHelper.h"

using TrigCompositeUtils::createAndStore;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionAuxContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::newDecisionIn;
using TrigCompositeUtils::linkToPrevious;
using TrigCompositeUtils::viewString;
using TrigCompositeUtils::featureString;
using TrigCompositeUtils::findLink;
using TrigCompositeUtils::LinkInfo;
using TrigCompositeUtils::hypoAlgNodeName;
using TrigCompositeUtils::allFailed;

TrigVSIHypoAlg::TrigVSIHypoAlg(const std::string& name, ISvcLocator* pSvcLocator) :
::HypoBase(name, pSvcLocator)
{
}

StatusCode TrigVSIHypoAlg::initialize()
{
   ATH_CHECK(m_verticesKey.initialize());
   ATH_CHECK(m_vtxCountKey.initialize());
   renounce(m_verticesKey);

   if (m_verticesKey.key() == "Undefined" || m_vtxCountKey.key() == "Undefined") {
      ATH_MSG_ERROR("either vertex Key name or track count key name is undefined " );
      return StatusCode::FAILURE;
   }

   ATH_CHECK(m_hypoTools.retrieve());
   if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());

   return StatusCode::SUCCESS;
}


StatusCode TrigVSIHypoAlg::execute(const EventContext& context) const
{
   // previous decisions

   // Taken from FTFLRT
   ATH_MSG_DEBUG ( "Executing " << name() << "..." );
   auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );

   ATH_CHECK( previousDecisionsHandle.isValid() );

   if (previousDecisionsHandle->size() == 0) {
      ATH_MSG_DEBUG( "No previous decision, nothing to do.");
      return StatusCode::SUCCESS;
   }

   ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() << " implicit ReadHandles for previous decisions. Looking for :" << viewString());
   //end taken section

   // new output decisions
   SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context );
   auto decisions = outputHandle.ptr();

   // monitoring
   auto mon_nVtx        = Monitored::Scalar( "nVtx",        -999. );
   auto mon_preselNVtx  = Monitored::Scalar( "preselNVtx",  -999. );
   auto mon_maxVtxNTrk  = Monitored::Scalar( "maxVtxNTrk",  -999. );
   auto mon_maxVtxMass  = Monitored::Scalar( "maxVtxMass",  -999. );
   auto monitorIt       = Monitored::Group( m_monTool, mon_nVtx, mon_preselNVtx, mon_maxVtxNTrk, mon_maxVtxMass );

   // Recording Data
   auto vtxCountContainer = std::make_unique< xAOD::TrigCompositeContainer>();
   auto vtxCountContainerAux = std::make_unique< xAOD::TrigCompositeAuxContainer>();
   vtxCountContainer->setStore(vtxCountContainerAux.get());

   std::unordered_map<Decision*, size_t> mapDecVtxContIdx;

   for( const Decision* previousDecision : *previousDecisionsHandle ) {

       SG::ReadHandle<xAOD::VertexContainer> vtxHandle;

      if (m_isViewBased) {
         const auto viewELInfo = findLink<ViewContainer>( previousDecision, viewString() );
         ATH_CHECK( viewELInfo.isValid() );

         // get vertices from the key
         ATH_MSG_DEBUG( "Getting Vertex Handle " << m_verticesKey);
         vtxHandle = ViewHelper::makeHandle(*viewELInfo.link, m_verticesKey, context);
      } else {
         ATH_MSG_DEBUG( "Getting Vertex Handle " << m_verticesKey);
         vtxHandle = SG::makeHandle(m_verticesKey, context );
      }

      //ATH_CHECK( vtxHandle.isValid() );
      if ( ! vtxHandle.isValid() ) {
         ATH_MSG_DEBUG ( "Couldn't find " << m_verticesKey << "..." );
         continue;
      }

      ATH_MSG_DEBUG ( "vertex handle size: " << vtxHandle->size() << "..." );
      int nVtx = vtxHandle->size();
      const xAOD::VertexContainer* vtxContainer = vtxHandle.get();

      Decision* d = newDecisionIn(decisions, previousDecision, hypoAlgNodeName(), context);

      DecisionIDContainer prev;
      decisionIDs( previousDecision, prev );

      std::vector<const xAOD::Vertex*> selectedVtx;
      float maxVtxNTrk = 0.;
      float maxVtxMass = 0.;

      // Loop over all vertices and combine them into the std::vector after preselection and create the input for the hypotool
      for ( const xAOD::Vertex* vertex : *vtxContainer ) {
         if (vertex == nullptr) continue;
         const float  mass  = (vertex->isAvailable<float>("vsi_mass"))?  vertex->auxdata<float>("vsi_mass")  : 0.;
         const float  pT    = (vertex->isAvailable<float>("vsi_pT"))?    vertex->auxdata<float>("vsi_pT")    : 0.;
         const size_t ntrk  = vertex->nTrackParticles();

         const float  x_ = vertex->position().x();
         const float  y_ = vertex->position().y();
         const float  r  = std::sqrt(x_*x_+y_*y_);

         if (  pT > m_minPt
            && r  > m_minR
            && ntrk >=  m_requiredNTrks )
         {
            selectedVtx.push_back(vertex);
            maxVtxNTrk = std::max(maxVtxNTrk, static_cast<float>(ntrk));
            maxVtxMass = std::max(maxVtxMass, mass);
         }
      }

      // Monitor
      mon_nVtx = nVtx;
      mon_preselNVtx = selectedVtx.size();
      mon_maxVtxNTrk = maxVtxNTrk;
      mon_maxVtxMass = maxVtxMass;

      xAOD::TrigComposite* vtxCount = new xAOD::TrigComposite();
      vtxCountContainer->push_back(vtxCount);
      vtxCount->setDetail( "vsiHypo_nVtx",    static_cast<float>(nVtx)       );
      vtxCount->setDetail( "vsiHypo_pTcut",   static_cast<float>(m_minPt)    );
      vtxCount->setDetail( "vsiHypo_rCut",    static_cast<float>(m_minR)     );
      vtxCount->setDetail( "vsiHypo_nTrkCut", static_cast<float>(m_requiredNTrks)   );
      vtxCount->setDetail( "vsiHypo_counts",  static_cast<float>(selectedVtx.size()) );

   TrigVSIHypoTool::eventVtxInfo vtxinfo{d, selectedVtx, prev};

   for(auto &tool : m_hypoTools)
   {
      ATH_CHECK(tool->decide(vtxinfo));
   }

      mapDecVtxContIdx.emplace( d, vtxCountContainer->size()-1 );

   }

   // Recording Data
   SG::WriteHandle<xAOD::TrigCompositeContainer> vtxCountHandle(m_vtxCountKey, context);
   ATH_CHECK(vtxCountHandle.record( std::move( vtxCountContainer ), std::move( vtxCountContainerAux ) ) );

   DecisionContainer::iterator it = decisions->begin();
   while(it != decisions->end()) {
      ATH_MSG_DEBUG( "+++++ outputDecision: " << *it << " +++++" );
      if ( allFailed( *it ) ) {
         ATH_MSG_DEBUG( "---> all failed, erasing" );
         it = decisions->erase(it);
      } else {
         ATH_MSG_DEBUG( "---> not all failed" );

         // Link hitDV object
         auto     decision = *it;
         size_t   idx = mapDecVtxContIdx.at(*it);

         ElementLink<xAOD::TrigCompositeContainer> dvEL = ElementLink<xAOD::TrigCompositeContainer>(*vtxCountHandle, idx, context);
         ATH_CHECK( dvEL.isValid() );

         ATH_CHECK( decision->setObjectLink<xAOD::TrigCompositeContainer>( featureString(), dvEL ) );

         ATH_MSG_DEBUG(*decision);
         ++it;
      }
   }

   ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
   ATH_MSG_DEBUG(" TrigVSIHypoAlg:" << name() << " successfully executed. Returning StatusCode::SUCCESS.");
   return StatusCode::SUCCESS;
}

/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigVSIHypoAlg.h"

#include "xAODTracking/TrackParticleAuxContainer.h"
#include "AthViews/ViewHelper.h"

using TrigCompositeUtils::createAndStore;
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
   } else if (previousDecisionsHandle->size() > 1) {
      ATH_MSG_ERROR("Found " << previousDecisionsHandle->size() <<" previous decisions.");
      return StatusCode::FAILURE;
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


   // get vertices from the key
   ATH_MSG_DEBUG( "Getting Vertex Handle " << m_verticesKey);
   auto vtxHandle = SG::makeHandle(m_verticesKey, context );

   ATH_CHECK( vtxHandle.isValid() );
   ATH_MSG_DEBUG ( "vertex handle size: " << vtxHandle->size() << "..." );

   int nVtx = vtxHandle->size();
   const xAOD::VertexContainer* vtxContainer = vtxHandle.get();

   std::vector<const xAOD::Vertex*> selectedVtx;
   float maxVtxNTrk = 0.;
   float maxVtxMass = 0.;

   // Loop over all vertices and combine them into the std::vector after preselection and create the input
   for ( const xAOD::Vertex* vertex : *vtxContainer) {
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

   // Recording Data
   auto vtxCountContainer = std::make_unique< xAOD::TrigCompositeContainer>();
   auto vtxCountContainerAux = std::make_unique< xAOD::TrigCompositeAuxContainer>();
   vtxCountContainer->setStore(vtxCountContainerAux.get());

   xAOD::TrigComposite * vtxCount = new xAOD::TrigComposite();
   vtxCountContainer->push_back(vtxCount);
   vtxCount->setDetail( "vsiHypo_nVtx",    static_cast<float>(nVtx)        );
   vtxCount->setDetail( "vsiHypo_pTcut",   static_cast<float>(m_minPt)    );
   vtxCount->setDetail( "vsiHypo_rCut",    static_cast<float>(m_minR)     );
   vtxCount->setDetail( "vsiHypo_nTrkCut", static_cast<float>(m_requiredNTrks)   );
   vtxCount->setDetail( "vsiHypo_counts",  static_cast<float>(selectedVtx.size()) );

   TrigCompositeUtils::Decision* d = newDecisionIn(decisions, previousDecisionsHandle->at(0), hypoAlgNodeName(), context);

   TrigCompositeUtils::DecisionIDContainer prev;
   TrigCompositeUtils::decisionIDs( previousDecisionsHandle->at(0), prev );

   TrigVSIHypoTool::eventVtxInfo vtxinfo{d, selectedVtx, prev};

   for(auto &tool : m_hypoTools)
   {
      ATH_CHECK(tool->decide(vtxinfo));
   }

   SG::WriteHandle<xAOD::TrigCompositeContainer> vtxCountHandle(m_vtxCountKey, context);
   ATH_CHECK(vtxCountHandle.record( std::move( vtxCountContainer ), std::move( vtxCountContainerAux ) ) );
   d->setObjectLink( featureString(), ElementLink<xAOD::TrigCompositeContainer>( m_vtxCountKey.key(), 0) );
   ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
   return StatusCode::SUCCESS;
}

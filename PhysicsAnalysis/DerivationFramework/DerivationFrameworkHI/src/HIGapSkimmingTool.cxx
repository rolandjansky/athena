/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkHI/HIGapSkimmingTool.h"
#include <HIEventUtils/TopoClusterSelectionTool.h>
#include <HIEventUtils/GapCalculator.h>
#include <xAODCaloEvent/CaloClusterContainer.h>
#include <xAODTracking/TrackParticleContainer.h>
#include <InDetTrackSelectionTool/InDetTrackSelectionTool.h>
#include <xAODJet/JetContainer.h>
#include <JetInterface/IJetSelector.h>

// Constructor
DerivationFramework::HIGapSkimmingTool::HIGapSkimmingTool(const std::string& t,
							const std::string& n,
							const IInterface* p) :
  AthAlgTool(t, n, p),
  m_clusterContainerName("CaloCalTopoClusters"),
  m_trackContainerName("InDetTrackParticles"),
  m_gapMin(0.5),
  m_jetContainerName("AntiKt4EMTopoJets"),
  m_useJetGaps(true),
  m_doJetSelection(false),
  m_maxSumGapCut(0),
  m_minSumGapCut(10),
  m_maxEdgeGapCut(0),
  m_minEdgeGapCut(10)

{
  declareInterface<DerivationFramework::ISkimmingTool>(this);
  declareProperty( "ClusterContainerName", m_clusterContainerName);
  declareProperty( "TrackContainerName", m_trackContainerName);
  declareProperty( "ClusterSelectionTool", m_tcSelectionTool);
  declareProperty( "TrackSelectionTool", m_trackSelectionTool);
  declareProperty( "GapMinimum",m_gapMin);
  declareProperty( "JetContainerName", m_jetContainerName);
  declareProperty( "JetSelectionTool", m_jetSelectionTool);
  declareProperty( "DoJetSelection", m_doJetSelection);
  declareProperty( "UseJetGaps", m_useJetGaps);
  declareProperty( "SumOfGapsMax", m_maxSumGapCut);
  declareProperty( "SumOfGapsMin", m_minSumGapCut);
  declareProperty( "EdgeGapMax", m_maxSumGapCut);
  declareProperty( "EdgeGapMin", m_minSumGapCut);

}
  
// Destructor
DerivationFramework::HIGapSkimmingTool::~HIGapSkimmingTool() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::HIGapSkimmingTool::initialize()
{
  CHECK(m_tcSelectionTool.retrieve());
  CHECK(m_trackSelectionTool.retrieve());
  if(m_useJetGaps && m_doJetSelection)
  {
    CHECK(m_jetSelectionTool.retrieve().isFailure());
  }
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::HIGapSkimmingTool::finalize()
{
  return StatusCode::SUCCESS;
}

// The filter itself
bool DerivationFramework::HIGapSkimmingTool::eventPassesFilter() const
{

  ////
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  const xAOD::CaloClusterContainer* tc=nullptr;
  ATH_CHECK(evtStore()->retrieve(tc,m_clusterContainerName));

  auto etaVals=HI::makeEtaList();
  for(auto cl : *tc)
  {
    if(m_tcSelectionTool->accept(*cl)) etaVals.insert(cl->eta());
  }
  ///
  const xAOD::TrackParticleContainer* trackContainer=nullptr;
  ATH_CHECK(evtStore()->retrieve(trackContainer,m_trackContainerName));

  for(auto track : *trackContainer)
  {
    if(m_trackSelectionTool->accept(*track)) etaVals.insert(track->eta());
  }


  float jetEtaMin=HI::getDetectorEtaMax();
  float jetEtaMax=-jetEtaMin;

  if(m_useJetGaps)
  {
    const xAOD::JetContainer* jetContainer=nullptr;
    ATH_CHECK(evtStore()->retrieve(jetContainer,m_jetContainerName));

    for(auto jet : *jetContainer)
    {
      float jetEta=jet->eta();
      if(m_doJetSelection)
      {
	if(!m_jetSelectionTool->keep(*jet)) continue;
      }
      jetEtaMin=std::min(jetEtaMin,jetEta);
      jetEtaMax=std::min(jetEtaMax,jetEta);
    }
  }
  else
  {
    jetEtaMin=0;
    jetEtaMax=0;
  }

  float sumGapPos=HI::getSumGapPos(etaVals,jetEtaMax,m_gapMin);
  float sumGapNeg=HI::getSumGapNeg(etaVals,jetEtaMin,m_gapMin);
  float maxSumGap=std::max(sumGapPos,sumGapNeg);
  float minSumGap=std::min(sumGapPos,sumGapNeg);
  if(maxSumGap < m_maxSumGapCut ||  minSumGap > m_minSumGapCut) return false;

  
  float edgeGapPos=HI::getEdgeGapPos(etaVals);
  float edgeGapNeg=HI::getEdgeGapNeg(etaVals);
  float maxEdgeGap=std::max(edgeGapPos,edgeGapNeg);
  float minEdgeGap=std::min(edgeGapPos,edgeGapNeg);
  if(maxEdgeGap < m_maxEdgeGapCut ||  minEdgeGap > m_minEdgeGapCut) return false;

  return true;
}


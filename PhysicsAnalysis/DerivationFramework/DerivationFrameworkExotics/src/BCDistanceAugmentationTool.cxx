/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkExotics/BCDistanceAugmentationTool.h"
#include "DerivationFrameworkExotics/TruthHelper.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCore/ShallowCopy.h"

DerivationFramework::BCDistanceAugmentationTool::BCDistanceAugmentationTool(const std::string& t,
                                                            const std::string& n,
                                                            const IInterface* p) :
  AthAlgTool(t, n, p),
  m_bcTool( "Trig::LHCBunchCrossingTool/BunchCrossingTool" ),
  m_PhIsoTool("VGammaORTool/myVGammaORTool")
{

  declareInterface<DerivationFramework::IAugmentationTool>(this);

  declareProperty( "BCTool",        m_bcTool );
  declareProperty( "PhotonIsoTool", m_PhIsoTool );
}

DerivationFramework::BCDistanceAugmentationTool::~BCDistanceAugmentationTool() {
}

StatusCode DerivationFramework::BCDistanceAugmentationTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ..");
  ATH_CHECK( m_bcTool.retrieve() );
  ATH_MSG_INFO("Retrieved tool: " << m_bcTool);

  std::vector<float> ph_pt = std::vector<float>({15e3});
  ATH_CHECK( m_PhIsoTool->setProperty("photon_pt_cuts",ph_pt) );
  ATH_CHECK( m_PhIsoTool.retrieve() );
  ATH_MSG_INFO("Retrieved tool: " << m_PhIsoTool);

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::BCDistanceAugmentationTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::BCDistanceAugmentationTool::addBranches() const {

  const xAOD::EventInfo* eventInfo = evtStore()->retrieve< const xAOD::EventInfo >();
  if(!eventInfo) {
    ATH_MSG_ERROR ("Couldn't retrieve xAOD::EventInfo");
    return StatusCode::FAILURE;
  }

  SG::AuxElement::Decorator< int >  decoratorBCIDDistanceFront("BCIDDistanceFromFront");
  SG::AuxElement::Decorator< int >  decoratorBCIDDistanceTail("BCIDDistanceFromTail");

  decoratorBCIDDistanceFront(*eventInfo) = m_bcTool->distanceFromFront(eventInfo->bcid(), Trig::IBunchCrossingTool::BunchCrossings);
  decoratorBCIDDistanceTail(*eventInfo)  = m_bcTool->distanceFromTail(eventInfo->bcid(), Trig::IBunchCrossingTool::BunchCrossings);

  // add flavour filter
  std::vector<float> truth_results(3,-1.0);
  bool  m_isMC = eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
  if(m_isMC){
    const xAOD::TruthEventContainer* xTruthEventContainer = nullptr;
    CHECK( evtStore()->retrieve( xTruthEventContainer, "TruthEvents"));
    truth_results = DerivationFramework::computeTruthInfo(xTruthEventContainer);
  }

  SG::AuxElement::Decorator< int >  decoratorFlavourFilter("FlavourFilter");
  if(truth_results.size()>0)
    decoratorFlavourFilter(*eventInfo) = int(truth_results.at(0));

  SG::AuxElement::Decorator< float >  decoratorMGVTruthPt("MGVTruthPt");
  if(truth_results.size()>1)
    decoratorMGVTruthPt(*eventInfo) = truth_results.at(1);

  SG::AuxElement::Decorator< float >  decoratorSherpaVTruthPt("SherpaVTruthPt");
  if(truth_results.size()>2)
    decoratorSherpaVTruthPt(*eventInfo) = truth_results.at(2);

  // store the photon overlap variables
  // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/VGammaORTool
  SG::AuxElement::Decorator< bool >  decoratorin_vy_overlap("in_vy_overlap");
  bool in_vy_overlap=false;
  if(m_isMC){
    ATH_CHECK( m_PhIsoTool->setProperty("use_gamma_iso",false) );
    ATH_CHECK( m_PhIsoTool->inOverlap(in_vy_overlap) );
  }
  decoratorin_vy_overlap(*eventInfo) = in_vy_overlap;

  // apply the frixione isolation
  ATH_CHECK( m_PhIsoTool->setProperty("use_gamma_iso",true) );
  SG::AuxElement::Decorator< bool >  decoratorin_vy_overlap_iso("in_vy_overlap_iso");
  bool in_vy_overlap_iso=false;
  if(m_isMC){
    ATH_CHECK(m_PhIsoTool->inOverlap(in_vy_overlap_iso));
  }
  decoratorin_vy_overlap_iso(*eventInfo) = in_vy_overlap_iso;

  // compute the number of clusters
  const xAOD::CaloClusterContainer * clusters = nullptr;
  ATH_CHECK(evtStore()->retrieve(clusters,"CaloCalTopoClusters"));

  //We will now loop over the cluster container counting the number of clusters which pass the criteria
  SG::AuxElement::Decorator< unsigned >  decoratorin_nBatman("nBatman");
  size_t nBatman=0;
  const static SG::AuxElement::ConstAccessor<float>  acc_AVGLARQ("AVG_LAR_Q");
  for ( auto ipart : *clusters ) {
    if (std::fabs(ipart->rawEta())<=2.5) continue;
    if (std::fabs(ipart->rawEta())>=3.2) continue;
    if (ipart->rawE()/cosh(ipart->rawEta())<500.) continue;
    if (acc_AVGLARQ(*ipart)/65535.<=0.2) continue;
    ++nBatman;
  }
  decoratorin_nBatman(*eventInfo) = nBatman;

  return StatusCode::SUCCESS;
}


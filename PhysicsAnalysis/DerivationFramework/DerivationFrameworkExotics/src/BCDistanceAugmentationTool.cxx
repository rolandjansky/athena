/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkExotics/BCDistanceAugmentationTool.h"
//#include "GammaORTools/VGammaORTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthParticle.h"
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
  //declareProperty( "RunNumber", m_runNumber = 0 );
}

DerivationFramework::BCDistanceAugmentationTool::~BCDistanceAugmentationTool() {
}

StatusCode DerivationFramework::BCDistanceAugmentationTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ..");
  ATH_CHECK( m_bcTool.retrieve() );
  ATH_MSG_INFO("Retrieved tool: " << m_bcTool);
  //ATH_CHECK( 
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

  SG::AuxElement::Decorator< int >  decoratorBCIDDistance("BCIDDistanceFromFront");

  decoratorBCIDDistance(*eventInfo) = m_bcTool->distanceFromFront(eventInfo->bcid(), Trig::IBunchCrossingTool::BunchCrossings);

  // add flavour filter
  std::vector<float> truth_results = {-1. -1};
  bool  m_isMC = eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
  if(m_isMC){
    const xAOD::TruthEventContainer* xTruthEventContainer = nullptr;
    CHECK( evtStore()->retrieve( xTruthEventContainer, "TruthEvents"));
    truth_results = computeTruthInfo(xTruthEventContainer);
  }
  SG::AuxElement::Decorator< int >  decoratorFlavourFilter("FlavourFilter");
  decoratorFlavourFilter(*eventInfo) = int(truth_results.at(0));

  SG::AuxElement::Decorator< float >  decoratorMGVTruthPt("MGVTruthPt");
  decoratorMGVTruthPt(*eventInfo) = truth_results.at(1);

  // store the photon overlap variables
  // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/VGammaORTool
  SG::AuxElement::Decorator< bool >  decoratorin_vy_overlap("in_vy_overlap");
  bool in_vy_overlap=false;
  ATH_CHECK( m_PhIsoTool->setProperty("use_gamma_iso",false) );
  ATH_CHECK(m_PhIsoTool->inOverlap(in_vy_overlap));
  decoratorin_vy_overlap(*eventInfo) = in_vy_overlap;

  // apply the frixione isolation
  ATH_CHECK( m_PhIsoTool->setProperty("use_gamma_iso",true) );
  SG::AuxElement::Decorator< bool >  decoratorin_vy_overlap_iso("in_vy_overlap_iso");
  bool in_vy_overlap_iso=false;
  ATH_CHECK(m_PhIsoTool->inOverlap(in_vy_overlap_iso));
  decoratorin_vy_overlap_iso(*eventInfo) = in_vy_overlap_iso;

  return StatusCode::SUCCESS;
}

const std::vector<float> DerivationFramework::BCDistanceAugmentationTool::computeTruthInfo(const xAOD::TruthEventContainer* xTruthEventContainer) const{

  float flav=0;
  float ptv62=-1.0;
  TLorentzVector lep23; 
  bool isCharm=false;
  bool isBottom=false;
  xAOD::TruthEventContainer::const_iterator itr;
  for (itr = xTruthEventContainer->begin(); itr!=xTruthEventContainer->end(); ++itr) {
    unsigned nPart = (*itr)->nTruthParticles();
    for(unsigned iPart=0; iPart<nPart;++iPart){
      const xAOD::TruthParticle* particle = (*itr)->truthParticle(iPart);

      if(particle && ((fabs(particle->pdgId())>=11 && fabs(particle->pdgId())<=18)) && particle->status()==23){ // these are the events without a status 62 W or Z
	lep23+=particle->p4();
      }
      if(particle && ((fabs(particle->pdgId())>=11 && fabs(particle->pdgId())<=18) || fabs(particle->pdgId())==24 || fabs(particle->pdgId())==23) && fabs(particle->eta())<10.0){
	if(particle->status()==62 && (particle->pdgId()==23 || fabs(particle->pdgId())==24)){ ptv62 = particle->pt(); }	
      }

      if(particle && (isBwithWeakDK(particle->pdgId())) && particle->pt()>0.0 && fabs(particle->eta())<4.0){ isBottom=true; }
      if(particle && (isDwithWeakDK(particle->pdgId())) && particle->pt()>4.0e3 && fabs(particle->eta())<3.0) isCharm=true;

    }// end particle loop
  }

  if(isCharm) flav=2;
  if(isBottom) flav=1;

   float ptvFilter=ptv62;
   if(ptv62<0.0) ptvFilter = lep23.Pt();

  std::vector<float> out;
  out.push_back(flav);
  out.push_back(ptvFilter);
  return out;
}

bool DerivationFramework::BCDistanceAugmentationTool::isBwithWeakDK(const int pID) const {
  int id = abs(pID);
  return ( id == 511   || // B+
	   id == 521   || // B0
	   id == 531   || // Bs
	   id == 541   || // Bc
	   id == 5122  || // Lambda_B
	   id == 5132  || // Xi_b-
	   id == 5232  || // X_b0
	   id == 5112  || // Sigma_b-
	   id == 5212  || // Sigma_b0
	   id == 5222  || // Sigma_b+
	   id == 5332 );  // Omega_B
}
	
	
bool DerivationFramework::BCDistanceAugmentationTool::isDwithWeakDK(const int pID) const {
  int id = abs(pID);
  return ( id == 411   || // D+
	   id == 421   || // D0
	   id == 431   || // Ds
	   id == 4122  || // Lambda_C
	   id == 4132  || // Xi_C^0
	   id == 4232  || // Xi_C^+
	   id == 4212  || // Xi_C^0
	   id == 4322  || // Xi'_C+  This is in fact EM not weak
	   id == 4332); // Omega_C
  
}


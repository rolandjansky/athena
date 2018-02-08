/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */


#include "TrigEgammaEmulationTool/TrigEgammaSelectorBaseTool.h"

using namespace Trig;

/*****************************************************************************************/
TrigEgammaSelectorBaseTool::TrigEgammaSelectorBaseTool(const std::string& myname):
  AsgTool(myname),
  m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool"),
  m_lumiTool("LuminosityTool/OnlLuminosityTool"),
  m_storeGate(nullptr),
  m_trigdec(nullptr)
{
  declareProperty("LuminosityToolOnline"    , m_lumiTool              , "Luminosity Tool Online" );
  declareProperty("LuminosityTool"          , m_lumiBlockMuTool       , "Luminosity Tool Offline");

  // just for compile
  HLT::TriggerElement* t = NULL;
  const xAOD::TrigElectronContainer* a = getFeature<xAOD::TrigElectronContainer>(t);
  const xAOD::ElectronContainer* b = getFeature<xAOD::ElectronContainer>(t);
  bool a1 = ancestorPassed<xAOD::TrigElectronContainer>(t);
  bool b1 = ancestorPassed<xAOD::ElectronContainer>(t);
  (void)a; (void)b;
  (void)a1; (void)b1;
  m_te=nullptr;
}
//!==========================================================================
StatusCode TrigEgammaSelectorBaseTool::initialize(){

  if (m_lumiTool.retrieve().isFailure()) {
      ATH_MSG_WARNING("Unable to retrieve LuminosityToolOnline");
      return StatusCode::FAILURE;
  } else {
      ATH_MSG_INFO("Successfully retrieved LuminosityToolOnline");
  }

  if (m_lumiBlockMuTool.retrieve().isFailure()) {                                     
      ATH_MSG_WARNING("Unable to retrieve LumiBlockMuTool");
      return StatusCode::FAILURE;
  } else {                                                                     
      ATH_MSG_INFO("Successfully retrieved LumiBlockMuTool");
  } 
  
  
  return StatusCode::SUCCESS;
}
//!==========================================================================
StatusCode TrigEgammaSelectorBaseTool::finalize(){
  return StatusCode::SUCCESS;
}

//!==========================================================================
float TrigEgammaSelectorBaseTool::dR(const float eta1, const float phi1, const float eta2, const float phi2){
    float deta = fabs(eta1 - eta2);
    float dphi = fabs(phi1 - phi2) < TMath::Pi() ? fabs(phi1 - phi2) : 2*TMath:: \
                 Pi() - fabs(phi1 - phi2);
    return sqrt(deta*deta + dphi*dphi);
}
//!==========================================================================
void TrigEgammaSelectorBaseTool::dressDecision(const SG::AuxElement * /*aux*/, std::string /*label*/, bool /*pass*/)
{
  //aux->auxdecor<bool>("TrigEgammaEmulationTool_"+label) = pass; 
}
//!==========================================================================
float TrigEgammaSelectorBaseTool::getOnlAverageMu(){
  if(m_lumiBlockMuTool){
    return m_lumiBlockMuTool->averageInteractionsPerCrossing(); // (retrieve mu for the current BCID)
  }
  return 0.0;
}
//!==========================================================================
float TrigEgammaSelectorBaseTool::getAverageMu(){
  if(m_lumiTool){
    return m_lumiTool->lbAverageInteractionsPerCrossing(); // (retrieve mu for the current BCID)
  }
  return 0.0;
}
//!==========================================================================
const xAOD::TrigRingerRings* TrigEgammaSelectorBaseTool::getTrigCaloRings( const xAOD::TrigEMCluster *emCluster ){

  const xAOD::TrigRingerRings *ringer = nullptr;
  if(!emCluster)  return nullptr;

  Trig::FeatureContainer fc = (m_trigdec->features("HLT_.*",TrigDefs::alsoDeactivateTEs));
  const std::vector< Trig::Feature<xAOD::TrigRingerRings > > vec_featRinger = fc.get< xAOD::TrigRingerRings >("",TrigDefs::alsoDeactivateTEs);


  ATH_MSG_DEBUG("L2Calo Rings FC Size " << vec_featRinger.size());
  for( Trig::Feature<xAOD::TrigRingerRings > featRinger : vec_featRinger ){
    ringer = featRinger.cptr();
    if(emCluster->RoIword() ==  (getFeature<xAOD::TrigEMCluster>(featRinger.te()))->RoIword() ){
      ATH_MSG_DEBUG("L2Calo Rings matched with TrigEMCluster object.");
      return ringer;
    }
  }
  return nullptr;
}

//!==========================================================================
/*
bool TrigEgammaSelectorBaseTool::getCaloRings( const xAOD::Electron *el, std::vector<float> &ringsE ){
  if(!el) return false;
  ringsE.clear();
  auto m_ringsELReader = xAOD::getCaloRingsReader();

  // First, check if we can retrieve decoration:
  const xAOD::CaloRingsELVec *caloRingsELVec(nullptr);
  try {
    caloRingsELVec = &(m_ringsELReader->operator()(*el));
  } catch ( const std::exception &e) {
    ATH_MSG_WARNING("Couldn't retrieve CaloRingsELVec. Reason: " << e.what());
  }

  if ( caloRingsELVec->empty() ){
    ATH_MSG_WARNING("Particle does not have CaloRings decoratorion.");
    return false;
  }

  // For now, we are using only the first cluster
  const xAOD::CaloRings *clrings = *(caloRingsELVec->at(0));
  // For now, we are using only the first cluster
 
  if(clrings) clrings->exportRingsTo(ringsE);
  else{
    ATH_MSG_WARNING("There is a problem when try to attack the rings vector using exportRigsTo() method.");
    return false;
  }
  return true;
}
#else
bool TrigEgammaSelectorBaseTool::getCaloRings( const xAOD::Electron *el, std::vector<float> & ){
  if(!el) return false;
  return true;
}
*/



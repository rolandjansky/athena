/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigEgammaEmulationTool/TrigEgammaSelectorBaseTool.h"
using namespace Trig;

/*****************************************************************************************/
TrigEgammaSelectorBaseTool::TrigEgammaSelectorBaseTool(const std::string& myname):
  AsgTool(myname),
  m_storeGate(nullptr),
  m_trigdec(nullptr)
{
  // just for compile
  HLT::TriggerElement* t = NULL;
  const xAOD::TrigElectronContainer* a = getFeature<xAOD::TrigElectronContainer>(t);
  const xAOD::ElectronContainer* b = getFeature<xAOD::ElectronContainer>(t);
  bool a1 = ancestorPassed<xAOD::TrigElectronContainer>(t);
  bool b1 = ancestorPassed<xAOD::ElectronContainer>(t);
  (void)a; (void)b;
  (void)a1; (void)b1;
}
//!==========================================================================
StatusCode TrigEgammaSelectorBaseTool::initialize(){
  return StatusCode::SUCCESS;
}
//!==========================================================================
StatusCode TrigEgammaSelectorBaseTool::finalize(){
  return StatusCode::SUCCESS;
}
//!==========================================================================
const xAOD::TrigRingerRings* TrigEgammaSelectorBaseTool::getTrigCaloRings( const xAOD::TrigEMCluster *emCluster,  const xAOD::TrigRNNOutput *& rnnOutput){

  const xAOD::TrigRingerRings *ringer = nullptr;
  if(!emCluster)  return nullptr;

  Trig::FeatureContainer fc = (m_trigdec->features("HLT_.*",TrigDefs::alsoDeactivateTEs));
  const std::vector< Trig::Feature<xAOD::TrigRingerRings > > vec_featRinger = fc.get< xAOD::TrigRingerRings >("",TrigDefs::alsoDeactivateTEs);


  ATH_MSG_DEBUG("L2Calo Rings FC Size " << vec_featRinger.size());
  for( Trig::Feature<xAOD::TrigRingerRings > featRinger : vec_featRinger ){
    ringer = featRinger.cptr();
    if(emCluster->RoIword() ==  (getFeature<xAOD::TrigEMCluster>(featRinger.te()))->RoIword() ){
      ATH_MSG_DEBUG("L2Calo Rings matched with TrigEMCluster object.");
      //rnnOutput = getTrigRNNOutput(emCluster);
      rnnOutput = nullptr;
      return ringer;
    }
  }
  return nullptr;
}

const xAOD::TrigRNNOutput* TrigEgammaSelectorBaseTool::getTrigRNNOutput(const xAOD::TrigEMCluster *emCluster){
  const xAOD::TrigRNNOutput *rnnOutput = nullptr;
  if(!emCluster)  return nullptr;

  Trig::FeatureContainer fc = (m_trigdec->features("HLT_.*",TrigDefs::alsoDeactivateTEs));
  const std::vector< Trig::Feature<xAOD::TrigRNNOutput > >   vec_featRnnOutput    = fc.get< xAOD::TrigRNNOutput >("",TrigDefs::alsoDeactivateTEs);

  ATH_MSG_DEBUG("L2Calo RNNOutput FC Size " << vec_featRnnOutput.size());
  for(Trig::Feature<xAOD::TrigRNNOutput> featRnnOutput : vec_featRnnOutput){
    rnnOutput = featRnnOutput.cptr();
    //auto  *a  = rnnOutput->ringer()->emCluster();
    //if(!a){
    //  ATH_MSG_DEBUG("NAO ENTROU");
    // }

    //ATH_MSG_DEBUG("aki");
    ATH_MSG_DEBUG( "RNNoutput = " <<rnnOutput->rnnDecision().at(0) );
    //ATH_MSG_DEBUG("aki");


    //if(emCluster->RoIword() ==  a->RoIword() ){
    //  ATH_MSG_DEBUG("L2Calo RNN matched with RNNOutput object.");
    //  return rnnOutput;
    //}
  }

  return nullptr;
}


//!==========================================================================
bool TrigEgammaSelectorBaseTool::getCaloRings( const xAOD::Electron *el, std::vector<float> &ringsE ){
  if(!el) return false;
  ringsE.clear();
 
  /*auto m_ringsELReader = xAOD::getCaloRingsReader();

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
  */
  return true;
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

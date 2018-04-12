/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/JetSJCalibrationGroomer.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/Selector.hh"
#include "fastjet/tools/Filter.hh"
#include "JetEDM/PseudoJetVector.h"
#include "JetCalibTools/JetCalibrationTool.h"
#include "xAODJet/JetAuxContainer.h"

// ====================================================================================
//
// ====================================================================================

JetSJCalibrationGroomer::JetSJCalibrationGroomer(std::string name)
: AsgTool(name), m_jetFromPJTool("JetFromPseudojetTool"), m_subjetCalTool(""), m_groomingMethod(invalid)
{
  declareProperty("RClus",                m_rClus=0.2,      "R for reclustering (0 for none)");
  declareProperty("PTFrac",               m_pTFrac=0.05,    "pT fraction for retaining subjets");
  declareProperty("NFiltJets",            m_nFiltJets=0,    "n jets kept in filtering (0 -> trimming)");
  declareProperty("ReclustAlgo",          m_reclustAlgo=1,  "algorithm used for reclustering subjets - 0: kt, 1: CA. 2: AntiKt");
  declareProperty("JetFromPseudojetTool", m_jetFromPJTool,  "tool to build jets");
  declareProperty("SubjetCalTool",        m_subjetCalTool,  "tool to calibrate jets");
}

// ====================================================================================
//
// ====================================================================================
JetSJCalibrationGroomer::~JetSJCalibrationGroomer() {
}

// ====================================================================================
//
// ====================================================================================
StatusCode JetSJCalibrationGroomer::initialize() {
  if( m_rClus <= 0.0 || m_rClus > 10.0 ){
    ATH_MSG_ERROR("Invalid value for RClus " << m_rClus);
    return StatusCode::FAILURE;
  }
  else if( (m_nFiltJets !=0 && m_pTFrac != 0.0) || (m_nFiltJets ==0 && m_pTFrac == 0.0) ){
    ATH_MSG_ERROR("Incompatible values for m_nFiltJets=" << m_nFiltJets << " and m_pTFrac=" << m_pTFrac << ". Exactly one of them must be !=0, the other one ==0.");
    return StatusCode::FAILURE;
  }
  else if( m_nFiltJets !=0 ){
    ATH_MSG_INFO("Set up to filter jets with " << m_nFiltJets << " hardest jets being kept.");
    m_groomingMethod = filtering ;
  }
  else{ 
    if( m_pTFrac <= 1){
      ATH_MSG_INFO("Set up to trimm jets with pT_sj/pT_jet>=" << m_pTFrac << " subjets being kept.");
      m_groomingMethod = trimming ;
    }
    else{
      ATH_MSG_ERROR("Invalid value for m_pTFrac=" << m_pTFrac << ". Must be in range [0, 1]");
      return StatusCode::FAILURE;
    }
  }


  ATH_MSG_INFO("Trying to get m_jetFromPJTool " << m_jetFromPJTool.name());
  ATH_CHECK( m_jetFromPJTool.retrieve() );
  ATH_MSG_INFO("Trying to get m_subjetCalTool " << m_subjetCalTool.name());
  ATH_CHECK( m_subjetCalTool.retrieve() );
  return StatusCode::SUCCESS;
}

// ====================================================================================
//
// ====================================================================================
int JetSJCalibrationGroomer::groom(const xAOD::Jet& jin, xAOD::JetContainer& jcout) const {
  // get PseudoJet
  const fastjet::PseudoJet* pjin = jin.getPseudoJet(); // "atlas" jet -> "fastjet" jet
  if ( pjin == 0 ) {
    ATH_MSG_WARNING("Jet does not have a pseudojet.");
    return 1;
  }

  // set method and groom jet
  fastjet::Filter filter;
  if( m_groomingMethod == filtering ){
    filter = fastjet::Filter( fastjet::JetDefinition( fastjet::JetAlgorithm(m_reclustAlgo) , m_rClus ), fastjet::SelectorNHardest( m_nFiltJets ) );
  }
  else if( m_groomingMethod == trimming ){
    filter = fastjet::Filter( fastjet::JetDefinition( fastjet::JetAlgorithm(m_reclustAlgo) , m_rClus ), fastjet::SelectorPtFractionMin( m_pTFrac ) );
  }
  else{
    ATH_MSG_ERROR("Invalid grooming method " << m_groomingMethod << ". Check config." );
    return 1;
  }
  fastjet::PseudoJet groomedPjet = filter( *pjin );
  std::vector<fastjet::PseudoJet> keptSubPjets = groomedPjet.pieces(); // get N PJ subjets from filtering procedure. These subjets we can calibrate.

  // build a temporary JetContainer
  xAOD::JetContainer groomedCont;
  // SET an AUX STORE !
  // convert PJ's into xAOD::Jet's
  fillJetContainer( keptSubPjets, groomedCont );

  // calibrate subjets resulting from filtering procedure
  fastjet::PseudoJet groomCalibedPjet;
  for (size_t i=0; i<groomedCont.size(); i++) {
    xAOD::Jet *sj = groomedCont[i];
    StatusCode sc = m_subjetCalTool->applyCalibration(*sj ) ; if(sc.isFailure()) continue;
    keptSubPjets[i].reset_momentum( sj->px(),sj->py(),sj->pz(),sj->e() );
    groomCalibedPjet += keptSubPjets[i]; // add them to get calibrated groomed fat jet
  }
  delete groomedCont.getStore();

  // fill groomed xAOD::Jet container with uncalibrated jets
  // might cause trouble, I don't understand where and why exactly!
  xAOD::Jet* groomCalibedJet = m_jetFromPJTool->add( groomedPjet, jcout, &jin );

  // a trick to keep internal structure from groomedPjet, but rescale 4-momentum to groomCalibedPjet
  xAOD::JetFourMom_t calibP4;
  calibP4.SetPxPyPzE( groomCalibedPjet.px(), groomCalibedPjet.py(), groomCalibedPjet.pz(), groomCalibedPjet.E() );
  groomCalibedJet->setJetP4(calibP4);

  int npieces = groomedPjet.pieces().size();
  groomCalibedJet->setAttribute("RClus", m_rClus);
  groomCalibedJet->setAttribute<int>("NRemainSubjets", npieces);
  if( m_groomingMethod == filtering ){
    groomCalibedJet->setAttribute<int>("TransformType", xAOD::JetTransform::UnknownTransform);
    groomCalibedJet->setAttribute("NFiltJets", m_nFiltJets);
  }
  else if( m_groomingMethod == trimming ){
    groomCalibedJet->setAttribute<int>("TransformType", xAOD::JetTransform::Trim);
    groomCalibedJet->setAttribute("PTFrac", m_pTFrac);
  }

  return 0;
}
// ====================================================================================
//
// ====================================================================================
void JetSJCalibrationGroomer::fillJetContainer(
    const std::vector<fastjet::PseudoJet>& inPjVec,
    xAOD::JetContainer& outJetCont) const
{
  xAOD::JetAuxContainer* aux = new xAOD::JetAuxContainer();
  outJetCont.setStore( aux );
  outJetCont.reserve( inPjVec.size() );
  std::vector<std::string> tmpv;
  for( const fastjet::PseudoJet& pj : inPjVec){
    m_jetFromPJTool->add( pj, outJetCont, xAOD::JetInput::LCTopo, tmpv ) ;
    xAOD::Jet & j = * (outJetCont.back());
    j.setAttribute<xAOD::JetFourMom_t>("JetPileupScaleMomentum", j.jetP4()); // This is to workaound JetCalibTools issue
  }
}
// ====================================================================================
//
// ====================================================================================
void JetSJCalibrationGroomer::print() const 
{
  ATH_MSG_INFO("=========== " << name() << " parameters: ===========" );
  ATH_MSG_INFO("     Recluster R: " << m_rClus);
  ATH_MSG_INFO("  pT faction min: " << m_pTFrac);
  ATH_MSG_INFO("      nFilt jets: " << m_nFiltJets);
  ATH_MSG_INFO("     Jet builder: " << m_jetFromPJTool.name());
  ATH_MSG_INFO(" subjet cal tool: " << m_subjetCalTool.name());
}
// ====================================================================================
// 
// ====================================================================================


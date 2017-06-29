/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRecTools/SoftKillerWeightTool.h"

#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/Selector.hh"
#include "fastjet/RectangularGrid.hh"
#include "fastjet/contrib/SoftKiller.hh"

using namespace std;

namespace {
  const static SG::AuxElement::ConstAccessor<float> acc_clambda("CENTER_LAMBDA");
}

SoftKillerWeightTool::SoftKillerWeightTool(const std::string& name) : JetConstituentModifierBase(name)
                                                                    , m_lambdaCalDivide(317)
                                                                    , m_isCaloSplit(false)
                                                                    , m_gridSpacing(0.6)
                                                                    , m_eCalGrid(0.5)
                                                                    , m_hCalGrid(0.5)
                                                                    , m_rapmin(0.0)
                                                                    , m_rapmax(2.5)
                                                                    , m_rapminApplied(0)
                                                                    , m_rapmaxApplied(10)
                                                                      
{

#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif

  declareProperty("SKGridSize", m_gridSpacing);
  declareProperty("SKRapMin", m_rapmin);
  declareProperty("SKRapMax", m_rapmax); 
  declareProperty("SKRapMinApplied", m_rapminApplied); 
  declareProperty("SKRapMaxApplied", m_rapmaxApplied); 
  declareProperty("isCaloSplit", m_isCaloSplit);
  declareProperty("ECalGridSize", m_eCalGrid);
  declareProperty("HCalGridSize", m_hCalGrid);
}


StatusCode SoftKillerWeightTool::initialize() {
  if(m_isCaloSplit && (m_inputType!=xAOD::Type::ParticleFlow && m_inputType!=xAOD::Type::CaloCluster)) {
    ATH_MSG_ERROR("SoftKillerWeightTool requires CaloCluster or PFO inputs when isCaloSplit is true."
		  << " It cannot apply split calorimeters on objects of type "
		  << m_inputType);
  }
  return StatusCode::SUCCESS;
}

StatusCode SoftKillerWeightTool::process_impl(xAOD::IParticleContainer* cont) const {
  const static SG::AuxElement::Accessor<float> weightAcc("PUWeight"); // Handle for PU weighting here
  if(m_isCaloSplit == false) RunSoftKiller(*cont);
  else RunSoftKillerSplit(*cont);

  for(xAOD::IParticle* part : *cont) {
    float w = 1;
    if(m_isCaloSplit == false) w = calculateWeight(*part);
    else w = calculateSplitWeight(*part);
    // use parent class's type-sensitive setter
    ATH_CHECK(setEnergyPt(part, part->e()*w, part->pt()*w));
    weightAcc(*part) = w; // Weight decoration of the container
  }
  return StatusCode::SUCCESS;
}


// Finds the pT cut for this event based on the SK results
// The partSK collection contains all particles that aren't cut, so particles below 
// its min pT are cut
double SoftKillerWeightTool::findMinPt(const vector<fastjet::PseudoJet> &partSK) const {
  double minPt = 999999999999;

  for(unsigned int i=0; i < partSK.size(); i++){
    if( partSK[i].pt() < minPt) minPt = partSK[i].pt();
  }

  // There is a small rounding error which I account for this way
  return (minPt - 1e-12);
}



// Reweights particles (when calo isn't split)
void SoftKillerWeightTool::RunSoftKiller(xAOD::IParticleContainer& cont) const {
  vector<fastjet::PseudoJet> partPJ;
  partPJ.reserve(cont.size());

  for(xAOD::IParticle* part : cont){
    if(part->e() > 0) partPJ.push_back( fastjet::PseudoJet( part->p4() ));
  }

  fastjet::Selector selector = fastjet::SelectorAbsRapRange(m_rapmin, m_rapmax);
  fastjet::RectangularGrid SKgrid(-m_rapmax, m_rapmax, m_gridSpacing, m_gridSpacing, selector);
  fastjet::contrib::SoftKiller softkiller(SKgrid);
  std::vector<fastjet::PseudoJet> partSK = softkiller(selector(partPJ));

  m_minPt = findMinPt(partSK);
}

void SoftKillerWeightTool::RunSoftKillerSplit(xAOD::IParticleContainer& cont) const {
  vector<fastjet::PseudoJet> partPJ_ECal;
  partPJ_ECal.reserve(cont.size());
  vector<fastjet::PseudoJet> partPJ_HCal;
  partPJ_HCal.reserve(cont.size());

  for(xAOD::IParticle* part : cont){
    double center_lambda = acc_clambda.isAvailable(*part) ? acc_clambda(*part) : 0.;
    if( center_lambda < m_lambdaCalDivide && part->e() > 0) partPJ_ECal.push_back( fastjet::PseudoJet( part->p4() ));
    if( center_lambda >= m_lambdaCalDivide && part->e() > 0) partPJ_HCal.push_back( fastjet::PseudoJet( part->p4() ));
  }

  fastjet::Selector selector = fastjet::SelectorAbsRapRange(m_rapmin, m_rapmax);
  fastjet::RectangularGrid SKgridECal(-m_rapmax, m_rapmax, m_eCalGrid, m_eCalGrid, selector);
  fastjet::contrib::SoftKiller softkillerECal(SKgridECal);
  std::vector<fastjet::PseudoJet> partSK_ECal = softkillerECal(selector(partPJ_ECal));
  m_minPtECal = findMinPt(partSK_ECal);

  fastjet::RectangularGrid SKgridHCal(-m_rapmax, m_rapmax, m_hCalGrid, m_hCalGrid, selector);
  fastjet::contrib::SoftKiller softkillerHCal(SKgridHCal);
  std::vector<fastjet::PseudoJet> partSK_HCal = softkillerHCal(selector(partPJ_HCal));
  m_minPtHCal = findMinPt(partSK_HCal);
}

float SoftKillerWeightTool::calculateWeight(xAOD::IParticle& part) const{
  // If the particle pT is below the SoftKiller pT cut, rescale 4-momentum to 0
  if( abs(part.eta()) < m_rapminApplied || abs(part.eta()) > m_rapmaxApplied) return 1;
  if( part.pt() < m_minPt) return 0;
  return 1;
}


float SoftKillerWeightTool::calculateSplitWeight(xAOD::IParticle& part) const{
  if( abs(part.eta()) < m_rapminApplied || abs(part.eta()) > m_rapmaxApplied) return 1;
  double center_lambda = acc_clambda.isAvailable(part) ? acc_clambda(part) : 0.;

  //Make a separate pT cut for the ECal and HCal
  if( center_lambda < m_lambdaCalDivide && part.pt() < m_minPtECal) return 0;
  if( part.pt() < m_minPtHCal) return 0;
  return 1;
}




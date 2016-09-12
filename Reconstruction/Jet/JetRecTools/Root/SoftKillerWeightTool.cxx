/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>

#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/Selector.hh"
#include "fastjet/RectangularGrid.hh"
#include "fastjet/contrib/SoftKiller.hh"

#include "JetRecTools/SoftKillerWeightTool.h"
#include "fastjet/contrib/SoftKiller.hh"
#include "xAODCore/ShallowCopy.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODCore/ShallowAuxContainer.h"

#include "JetRecTools/SoftKillerWeightTool.h"



using namespace std;


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

  declareProperty("SKGridSize", m_gridSpacing = 0.6);
  declareProperty("SKRapMin", m_rapmin = 0);
  declareProperty("SKRapMax", m_rapmax = 2.5); 
  declareProperty("SKRapMinApplied", m_rapminApplied = 0); 
  declareProperty("SKRapMaxApplied", m_rapmaxApplied = 10); 
  declareProperty("isCaloSplit", m_isCaloSplit = false);
  declareProperty("ECalGridSize", m_eCalGrid = 0.5);
  declareProperty("HCalGridSize", m_hCalGrid = 0.5);
}


StatusCode SoftKillerWeightTool::process(xAOD::CaloClusterContainer* cont) const {
  SG::AuxElement::Accessor<float> weightAcc("PUWeight"); // Handle for PU weighting here
  if(m_isCaloSplit == false) RunClusters(cont);
  else RunSplitClusters(cont);

  for(xAOD::CaloCluster* cl : *cont) {
    float w = 1;
    if(m_isCaloSplit == false) w = calculateWeight(cl);
    else w = calculateSplitWeight(cl);
    float clust_e = cl->e(); // Cluster energy
    cl->setE(clust_e * w);
    weightAcc(*cl) = w; // Weight decoration of the container
  }
  return StatusCode::SUCCESS;
}

StatusCode SoftKillerWeightTool::process(xAOD::IParticleContainer* cont) const {
  xAOD::CaloClusterContainer* clust = dynamic_cast<xAOD::CaloClusterContainer*> (cont); // Get CaloCluster container
  if(clust) return process(clust);
  return StatusCode::FAILURE;
}


// Finds the pT cut for this event based on the SK results
// The clustSK collection contains all clusters that aren't cut, so clusters below 
// its min pT are cut
double SoftKillerWeightTool::findMinPt(vector<fastjet::PseudoJet> *clustSK) const {
  double minPt = 999999999999;

  for(unsigned int i=0; i < clustSK->size(); i++){
    if( (*clustSK)[i].pt() < minPt) minPt = (*clustSK)[i].pt();
  }

  // There is a small rounding error which I account for this way
  return (minPt - 1e-12);
}



// Reweights clusters (when calo isn't split)
void SoftKillerWeightTool::RunClusters(xAOD::CaloClusterContainer* clust) const {
  vector<fastjet::PseudoJet> clustPJ;

  for(xAOD::CaloCluster* cl : *clust){
    if((cl)->e() > 0) clustPJ.push_back( fastjet::PseudoJet( (cl)->p4() ));
  }

  fastjet::Selector selector = fastjet::SelectorAbsRapRange(m_rapmin, m_rapmax);
  fastjet::RectangularGrid SKgrid(-m_rapmax, m_rapmax, m_gridSpacing, m_gridSpacing, selector);
  fastjet::contrib::SoftKiller softkiller(SKgrid);
  std::vector<fastjet::PseudoJet> clustSK = softkiller(selector(clustPJ));

  m_minPt = findMinPt(&clustSK);
}

void SoftKillerWeightTool::RunSplitClusters(xAOD::CaloClusterContainer* clust) const {
  vector<fastjet::PseudoJet> clustPJ_ECal;
  vector<fastjet::PseudoJet> clustPJ_HCal;

  for(xAOD::CaloCluster* cl : *clust){
    double center_lambda;
    (cl)->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA,center_lambda);
    if( center_lambda < m_lambdaCalDivide && (cl)->e() > 0) clustPJ_ECal.push_back( fastjet::PseudoJet( (cl)->p4() ));
    if( center_lambda >= m_lambdaCalDivide && (cl)->e() > 0) clustPJ_HCal.push_back( fastjet::PseudoJet( (cl)->p4() ));
  }

  fastjet::Selector selector = fastjet::SelectorAbsRapRange(m_rapmin, m_rapmax);
  fastjet::RectangularGrid SKgridECal(-m_rapmax, m_rapmax, m_eCalGrid, m_eCalGrid, selector);
  fastjet::contrib::SoftKiller softkillerECal(SKgridECal);
  std::vector<fastjet::PseudoJet> clustSK_ECal = softkillerECal(selector(clustPJ_ECal));
  m_minPtECal = findMinPt(&clustSK_ECal);

  fastjet::RectangularGrid SKgridHCal(-m_rapmax, m_rapmax, m_hCalGrid, m_hCalGrid, selector);
  fastjet::contrib::SoftKiller softkillerHCal(SKgridHCal);
  std::vector<fastjet::PseudoJet> clustSK_HCal = softkillerHCal(selector(clustPJ_HCal));
  m_minPtHCal = findMinPt(&clustSK_HCal);
}

float SoftKillerWeightTool::calculateWeight(xAOD::CaloCluster* cl) const{
  // If the cluster pT is below the SoftKiller pT cut, rescale 4-momentum to 0
  if( abs(cl->eta()) < m_rapminApplied || abs(cl->eta()) > m_rapmaxApplied) return 1;
  if( cl->pt() < m_minPt) return 0;
  return 1;
}


float SoftKillerWeightTool::calculateSplitWeight(xAOD::CaloCluster* cl) const{
  if( abs(cl->eta()) < m_rapminApplied || abs(cl->eta()) > m_rapmaxApplied) return 1;
  double center_lambda;

  if(!cl->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA,center_lambda)) center_lambda = 0;

  //Make a separate pT cut for the ECal and HCal
  if( center_lambda < m_lambdaCalDivide && cl->pt() < m_minPtECal) return 0;
  if( cl->pt() < m_minPtHCal) return 0;
  return 1;
}




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
                                                                      //m_initCount(0)
                                                                    , m_lambdaCalDivide(317)
                                                                    , m_isCaloSplit(false)
                                                                    , m_gridSpacing(0.45)
                                                                      
{

#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif

  declareProperty("SKGridSize", m_gridSpacing = 0.45);
  declareProperty("SKRapMin", m_rapmin = 0);
  declareProperty("SKRapMax", m_rapmax = 2.5); 
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
double SoftKillerWeightTool::findMinPt(vector<fastjet::PseudoJet> *m_clustSK) const {
  double m_minPt = 999999999999;

  for(unsigned int i=0; i < m_clustSK->size(); i++){
    if( (*m_clustSK)[i].pt() < m_minPt) m_minPt = (*m_clustSK)[i].pt();
  }

  // There is a small rounding error which I account for this way
  return (m_minPt - 1e-12);
}



// Reweights clusters (when calo isn't split)
void SoftKillerWeightTool::RunClusters(xAOD::CaloClusterContainer* m_clust) const {
  vector<fastjet::PseudoJet> m_clustPJ;

  for(xAOD::CaloCluster* cl : *m_clust){
    if((cl)->e() > 0) m_clustPJ.push_back( fastjet::PseudoJet( (cl)->p4() ));
  }

  fastjet::Selector m_selector = fastjet::SelectorAbsRapRange(m_rapmin, m_rapmax);
  fastjet::RectangularGrid m_SKgrid(-m_rapmax, m_rapmax, m_gridSpacing, m_gridSpacing, m_selector);
  fastjet::contrib::SoftKiller m_softkiller(m_SKgrid);
  std::vector<fastjet::PseudoJet> m_clustSK = m_softkiller(m_selector(m_clustPJ));

  m_minPt = findMinPt(&m_clustSK);
}

void SoftKillerWeightTool::RunSplitClusters(xAOD::CaloClusterContainer* m_clust) const {
  vector<fastjet::PseudoJet> m_clustPJ_ECal;
  vector<fastjet::PseudoJet> m_clustPJ_HCal;

  for(xAOD::CaloCluster* cl : *m_clust){
    double m_center_lambda;
    (cl)->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA,m_center_lambda);
    if( m_center_lambda < m_lambdaCalDivide && (cl)->e() > 0) m_clustPJ_ECal.push_back( fastjet::PseudoJet( (cl)->p4() ));
    if( m_center_lambda >= m_lambdaCalDivide && (cl)->e() > 0) m_clustPJ_HCal.push_back( fastjet::PseudoJet( (cl)->p4() ));
  }

  fastjet::Selector m_selector = fastjet::SelectorAbsRapRange(m_rapmin, m_rapmax);
  fastjet::RectangularGrid m_SKgridECal(-m_rapmax, m_rapmax, m_eCalGrid, m_eCalGrid, m_selector);
  fastjet::contrib::SoftKiller m_softkillerECal(m_SKgridECal);
  std::vector<fastjet::PseudoJet> m_clustSK_ECal = m_softkillerECal(m_selector(m_clustPJ_ECal));
  m_minPtECal = findMinPt(&m_clustSK_ECal);

  fastjet::RectangularGrid m_SKgridHCal(-m_rapmax, m_rapmax, m_hCalGrid, m_hCalGrid, m_selector);
  fastjet::contrib::SoftKiller m_softkillerHCal(m_SKgridHCal);
  std::vector<fastjet::PseudoJet> m_clustSK_HCal = m_softkillerHCal(m_selector(m_clustPJ_HCal));
  m_minPtHCal = findMinPt(&m_clustSK_HCal);
}

float SoftKillerWeightTool::calculateWeight(xAOD::CaloCluster* cl) const{
  // If the cluster pT is below the SoftKiller pT cut, rescale 4-momentum to 0
  if( abs(cl->eta()) < m_rapmin || abs(cl->eta()) > m_rapmax) return 1;
  if( cl->pt() < m_minPt) return 0;
  return 1;
}


float SoftKillerWeightTool::calculateSplitWeight(xAOD::CaloCluster* cl) const{
  if( abs(cl->eta()) < m_rapmin || abs(cl->eta()) > m_rapmax) return 1;
  double m_center_lambda;

  cl->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA,m_center_lambda);

  //Make a separate pT cut for the ECal and HCal
  if( m_center_lambda < m_lambdaCalDivide && cl->pt() < m_minPtECal) return 0;
  if( cl->pt() < m_minPtHCal) return 0;
  return 1;
}




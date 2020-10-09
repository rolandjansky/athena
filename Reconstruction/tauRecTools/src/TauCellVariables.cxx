/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "TauCellVariables.h"
#include "tauRecTools/KineUtils.h"
#include "tauRecTools/HelperFunctions.h"

#include "xAODTau/TauJet.h"
#include "xAODJet/Jet.h"
#include "CaloUtils/CaloVertexedCell.h"

#include <cmath>
#include <vector>

//______________________________________________________________________________
TauCellVariables::TauCellVariables(const std::string& name) :
  TauRecToolBase(name) {
}

//______________________________________________________________________________
TauCellVariables::~TauCellVariables() {
}

//______________________________________________________________________________
StatusCode TauCellVariables::execute(xAOD::TauJet& pTau) const {

  if (! pTau.jetLink().isValid()) {
    ATH_MSG_ERROR("tau does not have jet seed for cell variable calculation");
    return StatusCode::FAILURE;
  }
  const xAOD::Jet* pJetSeed = pTau.jet();
  
  ATH_MSG_VERBOSE("cluster position is eta=" << pTau.eta() << " phi=" << pTau.phi() );

  int numStripCell = 0;
  int numEMCell = 0;

  double sumCellET = 0.;
  double sumCellET01 = 0;
  double sumCellET12 = 0.;
  double sumStripET = 0.;
  double sumEMCellET = 0.;
  double sumHadCellET = 0.;
  double stripEta = 0.;
  double stripEta2 = 0.;
  double EMRadius = 0.;
  double HadRadius = 0.;

  std::vector<double> cellRingEnergys(7,0.);

  std::vector<const xAOD::CaloCluster*> clusterList;
  ATH_CHECK(tauRecTools::GetJetClusterList(pJetSeed, clusterList, m_useSubtractedCluster));

  int numCells = 0;
  std::bitset<200000> cellSeen;

  // loop over cells in all the clusters and calculate the variables
  for (auto cluster : clusterList){
    const CaloClusterCellLink* cellLinks = cluster->getCellLinks();
    
    for (const CaloCell* cell : *cellLinks) {
      ++numCells;
     
      // cells could be used by more than one cluster, only count the cell one time 
      if (cellSeen.test(cell->caloDDE()->calo_hash())) {
        continue;
      }
      else {
        cellSeen.set(cell->caloDDE()->calo_hash());
      }
      
      double cellPhi = cell->phi();
      double cellEta = cell->eta();
      double cellET = cell->et();
      double cellEnergy = cell->energy();
      TLorentzVector tauAxis = pTau.p4(xAOD::TauJetParameters::DetectorAxis);

      // correct cell four momentum based on tau vertex
      if (m_doVertexCorrection && pTau.vertexLink().isValid()) {
        CaloVertexedCell vxCell (*cell, pTau.vertex()->position());
        cellPhi = vxCell.phi();
        cellEta = vxCell.eta();
        cellET = vxCell.et();
        cellEnergy = vxCell.energy();
        tauAxis = pTau.p4(xAOD::TauJetParameters::IntermediateAxis);
      }
      
      double dR = Tau1P3PKineUtils::deltaR(tauAxis.Eta(), tauAxis.Phi(), cellEta, cellPhi);
      CaloSampling::CaloSample calo = cell->caloDDE()->getSampling();
      
      // use cells with dR < m_cellCone relative to tau intermediate axis:
      if (dR < m_cellCone) {
        sumCellET += cellET;
      
        if (dR < 0.1) sumCellET01 += cellET;
        if (dR > 0.1 && dR < 0.2) sumCellET12 += cellET;

        // EM layer: PreSamplerB, PreSamplerE, EMB1, EME1, EMB2, EME2
        // Most energy of neutral particles are deposited in the first two EM laywers
        // The third layer is regarded as HAD layber
        if (isEMLayer(calo)) {
          EMRadius += dR*cellET;
          sumEMCellET += cellET;
          if (cellEnergy > m_cellEthr) numEMCell += 1;

          // Strip layer: EMB1 and EME1 
          if (isStripLayer(calo) && (std::abs(cellEta) < 2.5)) {
            sumStripET += cellET;
            stripEta += cellEta * cellET;
            stripEta2 += pow(cellEta, 2) * cellET;
            if (cellEnergy > m_stripEthr) numStripCell += 1;
          } 
        } // end of EM cells
        else { 
            HadRadius += dR*cellET;
            sumHadCellET += cellET;
        } // end of HAD cells
      } // end of dR <  m_cellCone

      if (dR < 0.05) cellRingEnergys[0] += cellET;
      if (dR >= 0.05 && dR < 0.075) cellRingEnergys[1] += cellET;
      if (dR >= 0.075 && dR < 0.1) cellRingEnergys[2] += cellET;
      if (dR >= 0.1 && dR < 0.125) cellRingEnergys[3] += cellET;
      if (dR >= 0.125 && dR < 0.15) cellRingEnergys[4] += cellET;
      if (dR >= 0.15 && dR < 0.2) cellRingEnergys[5] += cellET;
      if (dR >= 0.2 && dR < 0.4) cellRingEnergys[6] += cellET;
    } // end of loop over cells
  } // end of loop over clusters

  ATH_MSG_DEBUG(numCells << " cells in seed");
  
  pTau.setDetail(xAOD::TauJetParameters::numCells ,  static_cast<int>  (numCells));
  pTau.setDetail(xAOD::TauJetParameters::nStrip , numStripCell );
  pTau.setDetail(xAOD::TauJetParameters::etEMAtEMScale , static_cast<float>( sumEMCellET ));
  pTau.setDetail(xAOD::TauJetParameters::etHadAtEMScale , static_cast<float>( sumHadCellET ));
  pTau.setDetail(xAOD::TauJetParameters::cellBasedEnergyRing1 , static_cast<float>( cellRingEnergys[0] ));
  pTau.setDetail(xAOD::TauJetParameters::cellBasedEnergyRing2 , static_cast<float>( cellRingEnergys[1] ));
  pTau.setDetail(xAOD::TauJetParameters::cellBasedEnergyRing3 , static_cast<float>( cellRingEnergys[2] ));
  pTau.setDetail(xAOD::TauJetParameters::cellBasedEnergyRing4 , static_cast<float>( cellRingEnergys[3] ));
  pTau.setDetail(xAOD::TauJetParameters::cellBasedEnergyRing5 , static_cast<float>( cellRingEnergys[4] ));
  pTau.setDetail(xAOD::TauJetParameters::cellBasedEnergyRing6 , static_cast<float>( cellRingEnergys[5] ));
  pTau.setDetail(xAOD::TauJetParameters::cellBasedEnergyRing7 , static_cast<float>( cellRingEnergys[6] ));

  // take care of the variables with division
  // -- fraction of cell energy within [0,0.1] and [0.1,0.2]
  if (std::abs(sumCellET) > 1e-6) {
    pTau.setDetail(xAOD::TauJetParameters::centFrac , static_cast<float>( sumCellET01 / sumCellET ));
    pTau.setDetail(xAOD::TauJetParameters::isolFrac , static_cast<float>( sumCellET12 / sumCellET ));
  } 
  else {
    pTau.setDetail(xAOD::TauJetParameters::centFrac , static_cast<float>( 0.0 ));
    pTau.setDetail(xAOD::TauJetParameters::isolFrac , static_cast<float>( -1.0 ));
  }
  
  // -- width of strip cells
  double stripWidth2 = 0.0;
  if (std::abs(sumStripET) > 1e-6) {
    stripEta = stripEta / sumStripET;
    stripEta2 = stripEta2 / sumStripET;
    stripWidth2 = stripEta2 - stripEta * stripEta;
  } 
  else {
    stripWidth2 = -1.0;
  }
  pTau.setDetail(xAOD::TauJetParameters::stripWidth2 , static_cast<float>( stripWidth2));
  
  // -- cell weighted radius of EM cells
  if (std::abs(sumEMCellET) > 1e-6) {
    EMRadius = EMRadius / sumEMCellET;
  } 
  else {
    EMRadius = -1.0;
  }
  pTau.setDetail(xAOD::TauJetParameters::EMRadius , static_cast<float>( EMRadius ));
  
  // -- cell weighted radius of HAD cells
  if (std::abs(sumHadCellET) > 1e-6) {
    HadRadius = HadRadius / sumHadCellET;
  } 
  else {
    HadRadius = -1.0;
  }
  pTau.setDetail(xAOD::TauJetParameters::hadRadius , static_cast<float>( HadRadius ));
 
  return StatusCode::SUCCESS;
}

#endif

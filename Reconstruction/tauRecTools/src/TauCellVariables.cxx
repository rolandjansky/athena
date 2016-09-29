/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
/********************************************************************
NAME:     TauCellVariables.cxx
PACKAGE:  offline/Reconstruction/tauRec
AUTHORS:  S. Rajagopalan
CREATED:  March 15, 2001

Aug 2001: Add (e,px,py,pz) for sum of EM cells (FEP)
Veto cell if track is within 0.8*size. Hence 1 cell if track
is in center, more otherwise.
 
Sep 2001: Take account of larger cells in third layer

Nov 2001: Omit third layer of EM calorimeter from EM sums. This layer
can become quite thick in the barrel, so it is more likely to
be hadronic, while photons are mainly contained in the first
two layers. (D. Lissauer suggested this.)

Dec 2002: Add tau likelihood calculation.
Feb 2003: Fix phi wrapping for et-weighted phi calculation.
Jan 2004: Use CLHEP units. Use phi = (-pi,pi].
Feb 2004: Fix identifiers, add taucmsdrdR
March 2004: Fix CaloCluster -> CaloEnergyCluster
Jul 2004: force -PI < phi < PI
Jul 2004: move to I4Momentum
Aug 2004: admit arbitrary seeds
23/10/2006 - (AK) fixing some compilation warnings (unused parameter)
18/04/2007 - (AK) fixing some compilation warnings (unused parameter)
18/01/2008 - (NM) use etaCalo/phiCalo for merged algo, set tauJet eta/phi if not set by tau1p3p
16/03/2010 - (AK) use the cell id instead of the pointer
17/03/2010 - (AK) change to P4Helpers
16/05/2011 - (FF) introduce possibility to correct cell origin wrt to primary vertex and beamspot
Jan 2012   - (FF) add cellEnergyRing variables
 ********************************************************************/

#include <algorithm> 
#include <math.h>
#include <vector>
#include <sstream>

//#include "GaudiKernel/Property.h"
#include "GaudiKernel/SystemOfUnits.h"

//#include "AtlasDetDescr/AtlasDetectorID.h"
//#include "CaloUtils/CaloCellList.h"
//#include "CaloEvent/CaloCluster.h"
//#include "CaloEvent/CaloCell.h"
//#include "CaloEvent/CaloSamplingHelper.h"
#include "CaloUtils/CaloVertexedCell.h"
//#include "CaloIdentifier/CaloID.h"
//#include "CaloIdentifier/CaloCell_ID.h"
//#include "CaloGeoHelpers/CaloSampling.h"
//#include "Particle/TrackParticle.h"
//#include "FourMom/P4EEtaPhiM.h"

#include "xAODTau/TauJet.h"
#include "xAODJet/Jet.h"
#include "tauRecTools/KineUtils.h"
#include "TauCellVariables.h"

using CLHEP::GeV;

TauCellVariables::TauCellVariables(const std::string& name) :
  TauRecToolBase(name),
m_cellEthr(0.2 * GeV),
m_stripEthr(0.2 * GeV),
m_EMSumThr(0.5 * GeV),
m_EMSumR(0.2),
m_cellCone(0.2),
m_emid(0),
m_tileid(0),
m_doCellCorrection(false) //FF: don't do cell correction by default
{

    declareProperty("CellEthreshold", m_cellEthr);
    declareProperty("StripEthreshold", m_stripEthr);
    declareProperty("EMSumThreshold", m_EMSumThr);
    declareProperty("EMSumRadius", m_EMSumR);
    declareProperty("CellCone", m_cellCone);
    declareProperty("CellCorrection", m_doCellCorrection);
}

TauCellVariables::~TauCellVariables() {
}

StatusCode TauCellVariables::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode TauCellVariables::initialize() {
    
    ATH_MSG_VERBOSE("TauCellVariables::initialize"); // DEBUG
    
    StatusCode sc;

    // retrieve all helpers from det store
    sc = detStore()->retrieve(m_emid);
    if (sc.isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve LArEM_ID helper from DetectorStore");
        return sc;
    }
    ATH_MSG_VERBOSE("Storegate retrieved"); // DEBUG

    sc = detStore()->retrieve(m_tileid);
    if (sc.isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve TileID helper from DetectorStore");
        return sc;
    }
    ATH_MSG_VERBOSE("TileID from DetectorStore"); // DEBUG

    return StatusCode::SUCCESS;
}

StatusCode TauCellVariables::eventInitialize() {
    return StatusCode::SUCCESS;
}

StatusCode TauCellVariables::execute(xAOD::TauJet& pTau) {

    ATH_MSG_VERBOSE("execute"); // DEBUG

    AtlasDetectorID AtlasID;

    int numStripCell = 0;
    int numEMCell = 0;

    double sumCellE = 0.;
    double sumEMCellE = 0.;

    double eta = 0.;
    double phi = 0.;
    double stripEta = 0.;
    double stripEta2 = 0.;

    double EMRadius = 0.;

    double sumStripET = 0.;
    double sumCellET = 0.;
    double sumEMCellET = 0.;
    double sumCellET12 = 0.;

    double ET1Sum = 0;

    double EMET1Sum = 0;
    double EMET4Sum = 0;

    double HadRadius = 0.;
    double sumHadCellET = 0.;

    double cellEta, cellPhi, cellET, cellEnergy;
    
    std::vector<double> vCellRingEnergy(8,0.); //size=8, init with 0.

    ATH_MSG_VERBOSE("get cluster position, use for cell loop");

    ATH_MSG_VERBOSE("position is eta=" << pTau.eta() << " phi=" << pTau.phi() );

    //use tau vertex to correct cell position
    bool applyCellCorrection = false;
    if (m_doCellCorrection && pTau.vertexLink()) {
       applyCellCorrection = true;
    }

    ///////////////////////////////////////////////////////////////////////
    // loop over all cells of the tau (placed there by the TauSeedBuilder)
  
    const xAOD::Jet* pJetSeed = (*pTau.jetLink());
    if (!pJetSeed) {
      ATH_MSG_WARNING("tau does not have jet seed for cell variable calculation");
      return StatusCode::SUCCESS;
    }

    xAOD::JetConstituentVector::const_iterator cItr = pJetSeed->getConstituents().begin();
    xAOD::JetConstituentVector::const_iterator cItrE = pJetSeed->getConstituents().end();

    unsigned int num_cells = 0;

    std::bitset<200000> cellSeen;

    for (; cItr != cItrE; ++cItr) {
      
      const xAOD::CaloCluster* cluster = dynamic_cast<const xAOD::CaloCluster*>( (*cItr)->rawConstituent() ); 

      CaloClusterCellLink::const_iterator firstcell = cluster->getCellLinks()->begin();
      CaloClusterCellLink::const_iterator lastcell = cluster->getCellLinks()->end();
      

      // ATH_MSG_VERBOSE( "in loop over clusters and cells : cluster  phi= " << cluster->phi() << ", eta= " << cluster->eta()<< ", energy= " << cluster->e() << ", et= " <<cluster->pt() );
    
    const CaloCell *cell;
    double dR;
    
    //loop over cells and calculate the variables
    for (; firstcell != lastcell; ++firstcell) {
        cell = *firstcell;


	if (cellSeen.test(cell->caloDDE()->calo_hash())) {
	  //already encountered this cell
	  continue;
	}
	else {
	  //New cell
	  cellSeen.set(cell->caloDDE()->calo_hash());
	}
        ++num_cells;

	// ATH_MSG_VERBOSE( "in loop over clusters and cells : phi= " << cell->phi() << ", eta= " << cell->eta()<< ", energy= " << cell->energy() << ", et= " <<cell->et() );

        // correct cell for tau vertex
        if (applyCellCorrection) {
          //ATH_MSG_INFO( "before cell correction: phi= " << cell->phi() << ", eta= " << cell->eta()<< ", energy= " << cell->energy() << ", et= " <<cell->et() );
          CaloVertexedCell vxCell (*cell, (*pTau.vertexLink())->position());
          cellPhi = vxCell.phi();
          cellEta = vxCell.eta();
          cellET = vxCell.et();
          cellEnergy = vxCell.energy();
        }
        else {
          cellPhi = cell->phi();
          cellEta = cell->eta();
          cellET = cell->et();
          cellEnergy = cell->energy();          
        }
        
        CaloSampling::CaloSample calo = cell->caloDDE()->getSampling();

        // Use cells those are in DR < m_cellCone of eta,phi of tau intermediate axis:
	dR = Tau1P3PKineUtils::deltaR(pTau.eta(),pTau.phi(),cellEta,cellPhi);
        
        if (dR < m_cellCone) {
            // Global sums
            sumCellE += cellEnergy;

            eta += cellEnergy * cellEta;

            // Must handle phi wrapping! Compute relative to cluster phi:
            double dphicc = cellPhi - pTau.phi();
            if (dphicc > M_PI) dphicc = dphicc - 2 * M_PI;
            if (dphicc < -M_PI) dphicc = dphicc + 2 * M_PI;
            
            phi += cellEnergy*dphicc;

            // If cell is an EM cell, include in sum for EM radius and for
            // total EM (e,px,py,pz)
            // Nov 2000: Only include first 2 layers in EM

            if (dR < 0.1) ET1Sum += cellET;

            if ((calo == CaloSampling::PreSamplerB) ||
                (calo == CaloSampling::PreSamplerE) ||

                (calo == CaloSampling::EMB1) ||
                (calo == CaloSampling::EME1) ||

                (calo == CaloSampling::EMB2) ||
                (calo == CaloSampling::EME2) ||

                (calo == CaloSampling::EMB3) ||
                (calo == CaloSampling::EME3)) {
                if (dR < 0.1) EMET1Sum += cellET;
                if (dR < m_cellCone) EMET4Sum += cellET;
            }

            if ((calo == CaloSampling::PreSamplerB) ||
                (calo == CaloSampling::PreSamplerE) ||

                (calo == CaloSampling::EMB1) ||
                (calo == CaloSampling::EME1) ||

                (calo == CaloSampling::EMB2) ||
                (calo == CaloSampling::EME2)) {

                sumEMCellE += cellEnergy;

                // If cell is a strip cell, sum for stripET calculation:
                if (((calo == CaloSampling::EMB1) ||
                    (calo == CaloSampling::EME1)) // to be investigated
                    && (fabs(cellEta) < 2.5)) {
                    sumStripET += cellET;
                    stripEta += cellEta * cellET;
                    stripEta2 += pow(cellEta, 2) * cellET;
                    if (cellEnergy > m_stripEthr) numStripCell += 1;
                } // end of strip cells

                EMRadius += dR*cellET;
                sumEMCellET += cellET;
                if (cellEnergy > m_cellEthr) numEMCell += 1;

            }// end of EM cells
            else { // HAD cells
                HadRadius += dR*cellET;
                sumHadCellET += cellET;
            }

            //  Sum cells in 0.1 < DR < 0.2 for Econe calculation
            sumCellET += cellET;
            if (dR > 0.1 && dR < 0.2) sumCellET12 += cellET;

        }// end of dR <  m_cellCone
        else {
            // nothing
        }

        // vCellRingEnergy[0] is a dummy value
        if (dR < 0.05) vCellRingEnergy[1] += cellET;
        if (dR >= 0.05 && dR < 0.075) vCellRingEnergy[2] += cellET;
        if (dR >= 0.075 && dR < 0.1) vCellRingEnergy[3] += cellET;
        if (dR >= 0.1 && dR < 0.125) vCellRingEnergy[4] += cellET;
        if (dR >= 0.125 && dR < 0.15) vCellRingEnergy[5] += cellET;
        if (dR >= 0.15 && dR < 0.2) vCellRingEnergy[6] += cellET;
        if (dR >= 0.2 && dR < 0.4) vCellRingEnergy[7] += cellET;

    } // end of loop over CaloCells

    }// end of loop over seed jet constituents

    ATH_MSG_DEBUG(num_cells << " cells in seed");

    //////////////////////////////////////////////////////////////////////////
    // save variables
    // keep this here until we know nobody complains that these were removed together with all extra details variables
    // pExtraDetails->setSeedCalo_sumCellEnergy(sumCellE);
    // pExtraDetails->setSeedCalo_sumEMCellEnergy(sumEMCellE);
    // pExtraDetails->setSeedCalo_nEMCell(numEMCell);
    // pExtraDetails->setSeedCalo_stripEt(sumStripET);
    //

    pTau.setDetail(xAOD::TauJetParameters::nStrip , numStripCell );

    // if (fabs(EMET4Sum) > 0.000001)
    //     pExtraDetails->setSeedCalo_EMCentFrac(EMET1Sum / EMET4Sum);
    // else
    //     pExtraDetails->setSeedCalo_EMCentFrac(0);

    if (fabs(sumStripET) > 0.000001) {
        stripEta = stripEta / sumStripET;
        stripEta2 = stripEta2 / sumStripET;
    } else {
        stripEta = 0;
        stripEta2 = -1.0;
    }

    pTau.setDetail(xAOD::TauJetParameters::stripWidth2 , static_cast<float>(stripEta2 - stripEta * stripEta) );

    if (fabs(sumEMCellET) > 0.000001) {
        EMRadius = EMRadius / sumEMCellET;
    } else {
        EMRadius = -1.0;
    }
    if (fabs(sumHadCellET) > 0.000001) {
        HadRadius = HadRadius / sumHadCellET;
    } else {
        HadRadius = -1.0;
    }
    
    pTau.setDetail(xAOD::TauJetParameters::EMRadius , static_cast<float>( EMRadius ) );
    pTau.setDetail(xAOD::TauJetParameters::etEMAtEMScale , static_cast<float>( sumEMCellET ) );
    pTau.setDetail(xAOD::TauJetParameters::hadRadius , static_cast<float>( HadRadius ) );
    pTau.setDetail(xAOD::TauJetParameters::etHadAtEMScale , static_cast<float>( sumHadCellET ) );
    
    if (fabs(sumCellET) > 0.000001) {
      pTau.setDetail(xAOD::TauJetParameters::centFrac , static_cast<float>( ET1Sum / sumCellET ) );
      pTau.setDetail(xAOD::TauJetParameters::isolFrac , static_cast<float>( sumCellET12 / sumCellET ) );
    } else {
      pTau.setDetail(xAOD::TauJetParameters::centFrac , static_cast<float>( 0.0 ) );
      pTau.setDetail(xAOD::TauJetParameters::isolFrac , static_cast<float>( -1.0 ) );
    }

    //save cell ring energies

    pTau.setDetail(xAOD::TauJetParameters::cellBasedEnergyRing1 , static_cast<float>( vCellRingEnergy[1] ) );

    pTau.setDetail(xAOD::TauJetParameters::cellBasedEnergyRing2 , static_cast<float>( vCellRingEnergy[2] ) );

    pTau.setDetail(xAOD::TauJetParameters::cellBasedEnergyRing3 , static_cast<float>( vCellRingEnergy[3] ) );

    pTau.setDetail(xAOD::TauJetParameters::cellBasedEnergyRing4 , static_cast<float>( vCellRingEnergy[4] ) );

    pTau.setDetail(xAOD::TauJetParameters::cellBasedEnergyRing5 , static_cast<float>( vCellRingEnergy[5] ) );

    pTau.setDetail(xAOD::TauJetParameters::cellBasedEnergyRing6 , static_cast<float>( vCellRingEnergy[6] ) );

    pTau.setDetail(xAOD::TauJetParameters::cellBasedEnergyRing7 , static_cast<float>( vCellRingEnergy[7] ) );

    return StatusCode::SUCCESS;
}




#endif

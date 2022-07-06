/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     EgammaReSamp2Fex.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma
//
// AUTHOR:   M.P. Casado, D.O. Damazio
//
//
// ********************************************************************

#include "EgammaReSamp2Fex.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "TrigT2CaloCommon/Calo_Def.h"
#include "xAODTrigCalo/TrigEMCluster.h"

EgammaReSamp2Fex::EgammaReSamp2Fex(const std::string& type, const std::string& name,
                                   const IInterface* parent) :
    IReAlgToolCalo(type, name, parent)
{
}

StatusCode EgammaReSamp2Fex::execute(xAOD::TrigEMCluster& rtrigEmCluster, const IRoiDescriptor& roi,
                                     const CaloDetDescrElement*& caloDDE,
                                     const EventContext& context) const
{
  ATH_MSG_DEBUG("in execute(TrigEMCluster&)");

  // Region Selector, sampling 2
  int sampling = 2;

  LArTT_Selector<LArCellCont> sel;
  ATH_CHECK( m_dataSvc->loadCollections(context, roi, TTEM, sampling, sel) );

  double energyEta = 0.;
  double energyPhi = 0.;

  // add these variables to take care of phi wrap-around
  double energyNegPhi = 0.;     // SRA
  double energyNegPhiConv = 0.; // SRA
  double energyPosPhi = 0.;     // SRA

  // 1. Find seed cell (highest Et in ROI .. layer 2)
  // 2. Find Et weighted eta, phi in 3*7 cell (layer 2) (photon + e id)
  // 3. Find Et in cells of sizes 3*3, 3*7, 7*7 (layer 2 + strips)
  //	    				        (3*7 for photon + e id)
  // 4. Find cluster width in 3*5 cell, layer 2 (photon id, needs
  //		 			         parabolic parametrisation)
  // 5. Find strip energies and eta (2*5 window)
  // 6. Find frac73 (photon id), (E1-E2)/(E1+E2) (e + photon id)

  double seedEnergy = 0.;
  double seedPhi = 999.;
  double seedEta = 999.;
  double hotPhi = 999.;
  double hotEta = 999.;
  int ncells = 0;
  // LVL1 positions
  float etaL1 = rtrigEmCluster.eta();
  float phiL1 = rtrigEmCluster.phi();

  const LArEM_ID* emID = m_larMgr->getEM_ID();
  const LArCell* seedCell = nullptr;
  const LArCell* hotCell = nullptr;
  for (const LArCell* larcell : sel) {
    if (larcell->energy() > seedEnergy) { // Hottest cell seach
      float deta = std::abs(etaL1 - larcell->eta());
      if (deta < m_maxHotCellDeta) { // Eta check is faster. Do it First
        float dphi = std::abs(phiL1 - larcell->phi());
        dphi = std::abs(M_PI - dphi);
        dphi = std::abs(M_PI - dphi);
        if (dphi < m_maxHotCellDphi) {
          seedEnergy = larcell->energy();
          seedCell = larcell;
        } // End of dphi check
      }   // End of deta check
    }     // End of if energy
    ncells++;
  }
  if (seedCell != nullptr) {
    seedEta = seedCell->eta();
    seedPhi = seedCell->phi();
    // For the S-shape correction, we store the caloDDE of the hottest cell
    caloDDE = (seedCell->caloDDE());
    hotCell = seedCell;
    hotEta = hotCell->eta();
    hotPhi = hotCell->phi();
  }
  else {
    return StatusCode::SUCCESS;
  }

  std::map<const LArCell*, float> windows;
  for (const LArCell* larcell : sel) {
    float deta = std::abs(seedEta - larcell->eta());
    if (deta < 0.025 + 0.002) { // Eta check is faster.
				// Do it First 0.025 is cell, plus a little loose
      float dphi = std::abs(seedPhi - larcell->phi());
      dphi = std::abs(M_PI - dphi);
      dphi = std::abs(M_PI - dphi);
      if (dphi < 0.025 + 0.002) { // the same here (around 2*pi/64/4, but ok)
        if (windows.find(larcell) == windows.end()) {
          windows[larcell] = 0.0;
        } // end of windows check
      }   // End of dphi check
    }     // End of deta check
    ncells++;
  }

  for (const LArCell* larcell : sel) {
    double etaCell = larcell->eta();
    double phiCell = larcell->phi();
    double energyCell = larcell->et();

    // Find position of current cell w.r.t. seed
    float deta = std::abs(etaCell - seedEta);
    if (deta > 0.05 + 0.002) continue; // find the energy weighted position with larger cluster
    for (auto& [cell, energy] : windows) {
      float deta1 = std::abs(etaCell - cell->eta());
      float dphi = std::abs(phiCell - cell->phi());
      if (dphi > M_PI) dphi = 2. * M_PI - dphi; // wrap (pi - 2pi)->(-pi - 0)

      // Electromagnetic measurements, done by layer
      // layer 2: Et weighted eta, phi, summed Et in 3*7,
      //          cluster width in 3*5 cell
      // emEnergy in 3*7 cell is also calculated, all samplings

      double cellSizeEta;
      double cellSizePhi;
      if (emID->is_em_barrel(larcell->ID())) {
        cellSizeEta = etaSizeLArEMSamp2(etaCell, Calorimeter::EMBAR);
        cellSizePhi = phiSizeLArEMSamp2(etaCell, Calorimeter::EMBAR);
      }
      else {
        cellSizeEta = etaSizeLArEMSamp2(etaCell, Calorimeter::EMEND);
        cellSizePhi = phiSizeLArEMSamp2(etaCell, Calorimeter::EMEND);
      }

      // 3. do the 3*5 stuff
      int nCellsEta = 3;
      int nCellsPhi = 5;
      if (deta1 <= 0.5 * double(nCellsEta - 1) * cellSizeEta + 0.01 &&
          dphi <= 0.5 * double(nCellsPhi - 1) * cellSizePhi + 0.01) {
        energy += energyCell;
      } // End of do the 3*5 stuff
    }   // End of windows map loop
  }     // end of the loop

  float max = -999;
  for (auto const& [cell, energy] : windows) {
    if (energy > max) {
      max = energy;
      seedCell = cell;
    }
  }

  if (seedCell != nullptr) {
    seedEta = seedCell->eta();
    seedPhi = seedCell->phi();
    // For the S-shape correction, we store the caloDDE of the hottest cell
    caloDDE = (seedCell->caloDDE());
  }
  else {
    return StatusCode::SUCCESS;
  }

  bool cluster_in_barrel = true;
  if (seedCell && seedCell->caloDDE()) cluster_in_barrel = seedCell->caloDDE()->is_lar_em_barrel();

  // Build desired quantities using the values previously obtained.

  double energy37Lay2 = 0.;       // sum Et in 3*7 cell layer 2
  double energy37Lay2NegPhi = 0.; // SRA for phi wrap-around
  double energy37Lay2PosPhi = 0.; // SRA for phi wrap-around
  double energy77Lay2 = 0.;       // sum Et in 7*7 cell layer 2
  double energy35Lay2 = 0.;       // sum Et in 3*5 cell layer 2
  double energy33Lay2 = 0.;       // sum Et in 3*3 cell layer 2
  double weightEta = 0.;          // temporary variable for cluster width
  double weightEta2 = 0.;         // ditto
  double clusterWidth35 = 0.;     // cluster width in eta, 3*5 cell, layer 2
  int nCellsEta = 0;              // size of cell array in eta
  int nCellsPhi = 0;              // size of cell array in phi
  double deta = 0.;               // eta difference current cell - seed
  double dphi = 0.;               // phi difference current cell - seed

  double totalEnergy = 0;
  CaloSampling::CaloSample samp;

  for (const LArCell* larcell : sel) {
    double etaCell = larcell->eta();
    double phiCell = larcell->phi();
    double energyCell = larcell->energy();

    // Find position of current cell w.r.t. seed
    deta = std::abs(etaCell - seedEta);
    dphi = std::abs(phiCell - seedPhi);
    if (dphi > M_PI) dphi = 2. * M_PI - dphi; // wrap (pi - 2pi)->(-pi - 0)

    double detaH = std::abs(etaCell - hotEta); // eta difference current cell - seed
    double dphiH = std::abs(phiCell - hotPhi); // phi difference current cell - seed
    if (dphiH > M_PI) dphiH = 2. * M_PI - dphiH; // wrap (pi - 2pi)->(-pi - 0)

    // Electromagnetic measurements, done by layer
    // layer 2: Et weighted eta, phi, summed Et in 3*7,
    //          cluster width in 3*5 cell
    // emEnergy in 3*7 cell is also calculated, all samplings

    double cellSizeEta;
    double cellSizePhi;
    if (emID->is_em_barrel(larcell->ID())) {
      cellSizeEta = etaSizeLArEMSamp2(etaCell, Calorimeter::EMBAR);
      cellSizePhi = phiSizeLArEMSamp2(etaCell, Calorimeter::EMBAR);
    }
    else {
      cellSizeEta = etaSizeLArEMSamp2(etaCell, Calorimeter::EMEND);
      cellSizePhi = phiSizeLArEMSamp2(etaCell, Calorimeter::EMEND);
    }

    // hot cell stuff first
    // 1. do the 7*7 stuff
    nCellsEta = 7;
    nCellsPhi = 7;
    if (detaH <= 0.5 * double(nCellsEta - 1) * cellSizeEta + 0.01 &&
        dphiH <= 0.5 * double(nCellsPhi - 1) * cellSizePhi + 0.01) {
      // 2. do the 3*7 stuff
      nCellsEta = 3;
      nCellsPhi = 7;
      if (detaH <= 0.5 * double(nCellsEta - 1) * cellSizeEta + 0.01 &&
          dphiH <= 0.5 * double(nCellsPhi - 1) * cellSizePhi + 0.01) {
        energy37Lay2 += energyCell;
      } // End of do the 3*7 stuff
      energy77Lay2 += energyCell;
    } // End of do the 7*7 stuff

    // 1. do the 7*7 stuff
    nCellsEta = 7;
    nCellsPhi = 7;
    if (deta <= 0.5 * double(nCellsEta - 1) * cellSizeEta + 0.005 &&
        dphi <= 0.5 * double(nCellsPhi - 1) * cellSizePhi + 0.005) {
      // 2. do the 3*7 stuff
      nCellsEta = 3;
      nCellsPhi = 7;
      if (deta <= 0.5 * double(nCellsEta - 1) * cellSizeEta + 0.002 &&
          dphi <= 0.5 * double(nCellsPhi - 1) * cellSizePhi + 0.002) {
        energyEta += energyCell * etaCell;
        // energy37Lay2 += energyCell;

        if (cluster_in_barrel) {
          samp = larcell->caloDDE()->getSampling();
        }

        if (phiCell > 0.) { // SRA phi wrap-around
          energyPosPhi += double(energyCell) * phiCell;
          energy37Lay2PosPhi += energyCell;
        }
        else {
          energyNegPhi += double(energyCell) * phiCell;
          energyNegPhiConv += double(energyCell) * (phiCell + 2.0 * M_PI);
          energy37Lay2NegPhi += energyCell;
        }
        // 3. do the 3*5 stuff
        nCellsEta = 3;
        nCellsPhi = 5;
        if (deta <= 0.5 * double(nCellsEta - 1) * cellSizeEta + 0.01 &&
            dphi <= 0.5 * double(nCellsPhi - 1) * cellSizePhi + 0.01) {
          weightEta += energyCell * etaCell;
          weightEta2 += energyCell * etaCell * etaCell;
          energy35Lay2 += energyCell;
          // 3a. do the 3*3 stuff
          // nCellsEta = 3;
          nCellsPhi = 3;
          if (deta <= 0.5 * double(nCellsEta - 1) * cellSizeEta + 0.01 &&
              dphi <= 0.5 * double(nCellsPhi - 1) * cellSizePhi + 0.01) {
            energy33Lay2 += energyCell;
          } // End of do the 3*3 stuff
        }   // End of do the 3*5 stuff

      } // End of do the 3x7 stuff
      // 4. do the 5*5 stuff
      nCellsEta = 5;
      nCellsPhi = 5;
      if ((!cluster_in_barrel) && deta <= 0.5 * double(nCellsEta - 1) * cellSizeEta + 0.005 &&
          dphi <= 0.5 * double(nCellsPhi - 1) * cellSizePhi + 0.005) {
        samp = larcell->caloDDE()->getSampling();
      } // End of do the 5*5 stuff
    }   // End of do the 7*7 stuff

  } // end of loop over sampling 2

  // Normalise energy weighted angles and calculate values

  if ((energy37Lay2PosPhi + energy37Lay2NegPhi) > 0.) { // dont divide by zero
    // SRA phi wrap-around
    double AvgNegPhi = 0.;
    double AvgPosPhi = 0.;

    energyEta /= (energy37Lay2PosPhi + energy37Lay2NegPhi);

    if (energy37Lay2NegPhi > 0.) {
      AvgNegPhi = energyNegPhi / energy37Lay2NegPhi;
    }
    else {
      AvgNegPhi = -999.0;
    }

    if (energy37Lay2PosPhi > 0.) {
      AvgPosPhi = energyPosPhi / energy37Lay2PosPhi;
    }
    else {
      AvgPosPhi = -999.0;
    }

    if (AvgPosPhi == -999.0) {
      if (AvgNegPhi != -999.0) {
        energyPhi = AvgNegPhi;
      }
    }

    if (AvgNegPhi == -999.0) {
      if (AvgPosPhi != -999.0) {
        energyPhi = AvgPosPhi;
      }
    }

    if (AvgNegPhi != -999.0 && AvgPosPhi != -999.0) {
      if ((AvgNegPhi > (-M_PI / 2.0)) && (AvgPosPhi < (M_PI / 2.0))) {
        energyPhi = (energyNegPhi + energyPosPhi) / (energy37Lay2NegPhi + energy37Lay2PosPhi);
      }
      else {
        if ((AvgNegPhi < (-M_PI / 2.0)) && (AvgPosPhi > (M_PI / 2.0))) {
          energyPhi = (energyNegPhiConv + energyPosPhi) / (energy37Lay2NegPhi + energy37Lay2PosPhi);
          if (energyPhi > M_PI) {
            energyPhi = energyPhi - 2 * M_PI;
          }
        }
      }
    }
  }
  else {
    energyEta = 99.;
    energyPhi = 0.;
  }

  for (const LArCell* larcell : sel) {
    double etaCell = larcell->eta();
    double phiCell = larcell->phi();
    double energyCell = larcell->energy();

    // Find position of current cell w.r.t. seed
    deta = std::abs(etaCell - energyEta);
    dphi = std::abs(phiCell - energyPhi);
    if (dphi > M_PI) dphi = 2. * M_PI - dphi; // wrap (pi - 2pi)->(-pi - 0)

    double cellSizeEta;
    double cellSizePhi;
    if (emID->is_em_barrel(larcell->ID())) {
      cellSizeEta = etaSizeLArEMSamp2(etaCell, Calorimeter::EMBAR);
      cellSizePhi = phiSizeLArEMSamp2(etaCell, Calorimeter::EMBAR);
    }
    else {
      cellSizeEta = etaSizeLArEMSamp2(etaCell, Calorimeter::EMEND);
      cellSizePhi = phiSizeLArEMSamp2(etaCell, Calorimeter::EMEND);
    }

    nCellsEta = 3;
    nCellsPhi = 7;
    if ((deta <= 0.5 * double(nCellsEta - 1) * cellSizeEta + 0.01) &&
        (dphi <= 0.5 * double(nCellsPhi - 1) * cellSizePhi + 0.01)) {

      if (cluster_in_barrel) {
        totalEnergy += energyCell;
        // samp = CaloSampling::getSampling(*larcell);
        samp = larcell->caloDDE()->getSampling();
        rtrigEmCluster.setEnergy(samp, rtrigEmCluster.energy(samp) + energyCell);
        rtrigEmCluster.setRawEnergy(samp, rtrigEmCluster.rawEnergy(samp) + energyCell);
      }
    } // End of do the 3x7 stuff

    nCellsEta = 5;
    nCellsPhi = 5;
    if ((!cluster_in_barrel) && (deta <= 0.5 * double(nCellsEta - 1) * cellSizeEta + 0.01) &&
        (dphi <= 0.5 * double(nCellsPhi - 1) * cellSizePhi + 0.01)) {
      totalEnergy += energyCell;
      // samp = CaloSampling::getSampling(*larcell);
      samp = larcell->caloDDE()->getSampling();
      rtrigEmCluster.setEnergy(samp, rtrigEmCluster.energy(samp) + energyCell);
      rtrigEmCluster.setRawEnergy(samp, rtrigEmCluster.rawEnergy(samp) + energyCell);
    } // End of do the 5*5 stuff

  } // end of loop over sampling 2

  // calculate cluster width

  if (energy35Lay2 > 0.) {
    const double inv_energy35Lay2 = 1. / energy35Lay2;
    clusterWidth35 = (weightEta2 * inv_energy35Lay2) -
                     (weightEta * inv_energy35Lay2) * (weightEta * inv_energy35Lay2);
    clusterWidth35 > 0. ? clusterWidth35 = sqrt(clusterWidth35) : clusterWidth35 = 99.;
  }
  else {
    clusterWidth35 = 99.;
  }

  // Update cluster Variables

  // update stored variables
  rtrigEmCluster.setE233(energy33Lay2);
  rtrigEmCluster.setE237(energy37Lay2);
  rtrigEmCluster.setE277(energy77Lay2);
  rtrigEmCluster.setWeta2(clusterWidth35);
  rtrigEmCluster.setRawEnergy(rtrigEmCluster.rawEnergy() + totalEnergy);
  rtrigEmCluster.setEta(energyEta);
  rtrigEmCluster.setPhi(energyPhi);
  rtrigEmCluster.setRawEta(energyEta);
  rtrigEmCluster.setRawPhi(energyPhi);
  rtrigEmCluster.setNCells(ncells);

  return StatusCode::SUCCESS;
}

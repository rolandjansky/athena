/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     EgammaReEmEnFex.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma
//
// AUTHOR:   M.P. Casado
//
// ********************************************************************

#include "CaloGeoHelpers/CaloSampling.h"
#include "xAODTrigCalo/TrigEMCluster.h"

#include "EgammaReEmEnFex.h"
#include "TrigT2CaloCommon/Calo_Def.h"

EgammaReEmEnFex::EgammaReEmEnFex(const std::string& type, const std::string& name,
                                 const IInterface* parent) :
    IReAlgToolCalo(type, name, parent)
{
}

EgammaReEmEnFex::~EgammaReEmEnFex()
{
}

StatusCode EgammaReEmEnFex::execute(xAOD::TrigEMCluster& rtrigEmCluster, const IRoiDescriptor& roi,
                                    const CaloDetDescrElement*& caloDDE,
                                    const EventContext& context) const
{
  bool clusterInBarrel = true;
  if (caloDDE) clusterInBarrel = caloDDE->is_lar_em_barrel();

  ATH_MSG_DEBUG("in execute(TrigEMCluster &)");

  // Region Selector, sampling 0
  int sampling = 0;

  LArTT_Selector<LArCellCont> sel;
  ATH_CHECK( m_dataSvc->loadCollections(context, roi, TTEM, sampling, sel) );

  double deta = 0.; // eta difference current cell - seed
  double dphi = 0.; // phi difference current cell - seed

  double totalEnergy = 0;
  CaloSampling::CaloSample samp;

  double energyEta = rtrigEmCluster.eta();
  double energyPhi = rtrigEmCluster.phi();
  if (caloDDE) {
    energyEta = caloDDE->eta();
    energyPhi = caloDDE->phi();
  }

  int ncells = 0;

  for (const LArCell* larcell : sel) { // Should be revised for London scheme
    ncells++;

    double etaCell = larcell->eta();
    double phiCell = larcell->phi();
    double energyCell = larcell->energy();

    // find the standard em cluster energy (3*7 cell, now sampling 0)
    // Find position of current cell w.r.t. seed
    deta = std::abs(etaCell - energyEta);
    dphi = std::abs(phiCell - energyPhi);

    if (dphi > M_PI) dphi = 2. * M_PI - dphi; // wrap (pi - 2pi)->(-pi - 0)
    // 3x7 means three cells per 7 in the second layer 0.025*3/2, 0.025*7/2, for instance
    bool condition37 = clusterInBarrel &&
                       ((deta <= 0.0375 + 0.0005) && (dphi <= 0.0875 + 0.0005));
    bool condition55 = (!clusterInBarrel) &&
                       ((deta <= 0.0625 + 0.0005) && (dphi <= 0.0625 + 0.0005));

    if (condition37 || condition55) {

      totalEnergy += energyCell;
      // samp = CaloSampling::getSampling(*larcell);
      samp = larcell->caloDDE()->getSampling();
      rtrigEmCluster.setEnergy(samp, rtrigEmCluster.energy(samp) + energyCell);
      rtrigEmCluster.setRawEnergy(samp, rtrigEmCluster.rawEnergy(samp) + energyCell);
    }

  } // end of loop over sampling 0

  // Region Selector, sampling 3
  sampling = 3;

  LArTT_Selector<LArCellCont> sel3;
  ATH_CHECK( m_dataSvc->loadCollections(context, roi, TTEM, sampling, sel3) );

  for (const LArCell* larcell : sel3) { // Should be revised for London scheme
    ncells++;
    double etaCell = larcell->eta();
    double phiCell = larcell->phi();
    double energyCell = larcell->energy();

    // find the standard em cluster energy (3*7 cell, now sampling 0)
    // Find position of current cell w.r.t. seed
    deta = std::abs(etaCell - energyEta);
    dphi = std::abs(phiCell - energyPhi);
    if (dphi > M_PI) dphi = 2. * M_PI - dphi; // wrap (pi - 2pi)->(-pi - 0)

    // 3x7 means three cells per 7 in the second layer 0.025*3/2, 0.025*7/2, for instance
    bool condition37 = clusterInBarrel && ((deta <= 0.0375 + 0.001) && (dphi <= 0.0875 + 0.001));
    bool condition55 = (!clusterInBarrel) &&
                       ((deta <= 0.0625 + 0.001) && (dphi <= 0.0625 + 0.001));

    if (condition37 || condition55) {

      totalEnergy += energyCell;
      samp = larcell->caloDDE()->getSampling();
      rtrigEmCluster.setEnergy(samp, rtrigEmCluster.energy(samp) + energyCell);
      rtrigEmCluster.setRawEnergy(samp, rtrigEmCluster.rawEnergy(samp) + energyCell);
    }

  } // end of loop over sampling 3

  // Update Cluster Variables
  rtrigEmCluster.setNCells(ncells + rtrigEmCluster.nCells());
  rtrigEmCluster.setRawEnergy(rtrigEmCluster.rawEnergy() + totalEnergy);
  // The dependency with energy is not defined yet
  rtrigEmCluster.setEnergy(rtrigEmCluster.rawEnergy() );

  rtrigEmCluster.setEt(rtrigEmCluster.energy() / cosh(energyEta));
  rtrigEmCluster.setRawEt(rtrigEmCluster.rawEnergy() / cosh(energyEta));

  return StatusCode::SUCCESS;
}

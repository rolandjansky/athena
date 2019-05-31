/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     EgammaReHadEnFex.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma
//
// AUTHOR:   M.P. Casado
//
// REFERENCES: Based on Trigger/TrigT2CaloEgamma/T2EmHadAlgorithm.cxx
//
// ********************************************************************

#include "EgammaReHadEnFex.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "TrigT2CaloCommon/Calo_Def.h"
#include <math.h>

EgammaReHadEnFex::EgammaReHadEnFex(const std::string& type, const std::string& name,
                                   const IInterface* parent) :
    IReAlgToolCalo(type, name, parent)
{}

StatusCode EgammaReHadEnFex::execute(xAOD::TrigEMCluster& rtrigEmCluster, const IRoiDescriptor& roi,
                                     const CaloDetDescrElement*& /*caloDDE*/,
                                     const EventContext& context) const
{
  double deta = 0.; // eta difference current cell - seed
  double dphi = 0.; // phi difference current cell - seed

  CaloSampling::CaloSample samp;

  double energyEta = rtrigEmCluster.eta();
  double energyPhi = rtrigEmCluster.phi();
  rtrigEmCluster.setEhad1(0.);

  // Dump eta-phi strip ranges into arrays
  // Define variables here so that their scope covers both the Tile and HEC loops
  // Use arrays of length 3 since there is no samp 0 in the HAD calorimeters
  // this avoids wasting memory but makes the code slightly less clear

  int ncells = 0;

  for (unsigned int sampling = 0; sampling < 3; sampling++) {

    LArTT_Selector<LArCellCont> sel;
    ATH_CHECK( m_dataSvc->loadCollections(context, roi, TTHEC, sampling, sel) );

    for (const LArCell* larcell : sel) {

      ncells++;
      double etaCell = larcell->eta();
      double phiCell = larcell->phi();
      double energyCell = larcell->energy();

      // find position of current cell w.r.t. seed
      deta = fabs(etaCell - energyEta);
      dphi = fabs(phiCell - energyPhi);
      if (dphi > M_PI) dphi = 2. * M_PI - dphi; // wrap 0 -> 6.28
      // hadronic measurements: energy sum in 0.1*0.1
      if (deta <= 0.12 && dphi <= 0.12) {
        // samp = CaloSampling::getSampling(*larcell);
        samp = larcell->caloDDE()->getSampling();
        rtrigEmCluster.setEnergy(samp, rtrigEmCluster.energy(samp) + energyCell);
        rtrigEmCluster.setRawEnergy(samp, rtrigEmCluster.rawEnergy(samp) + energyCell);
        if (CaloSampling::HEC0 == samp) {
          rtrigEmCluster.setEhad1(rtrigEmCluster.ehad1() + energyCell);
        }
      }

    } // end of loop over cells

  } // End sampling loop
#ifndef NDEBUG
  if (msgLvl(MSG::DEBUG)) {
    for (int sampling = 0; sampling < 3; sampling++) {
      if (m_geometryTool->EtaPhiRange(1, sampling, energyEta, energyPhi)) {
        ATH_MSG_ERROR("problems with EtaPhiRange");
      }
    }
    PrintCluster(rtrigEmCluster.energy(CaloSampling::HEC0), 1, 0, CaloSampling::HEC0, CaloSampling::HEC0);
    PrintCluster(rtrigEmCluster.energy(CaloSampling::HEC1), 1, 1, CaloSampling::HEC1, CaloSampling::HEC1);
    PrintCluster(rtrigEmCluster.energy(CaloSampling::HEC2), 1, 2, CaloSampling::HEC2, CaloSampling::HEC2);
  }
#endif

  // Next TILECAL

  TileCellCollection seltile;
  ATH_CHECK( m_dataSvc->loadCollections(context, roi, seltile) );

  for (const TileCell* tilecell : seltile) { // loop over cells

    ncells++;
    double etaCell = tilecell->eta();
    double phiCell = tilecell->phi();
    double energyCell = tilecell->energy();

    // find position of current cell w.r.t. seed
    deta = fabs(etaCell - energyEta);
    dphi = fabs(phiCell - energyPhi);
    if (dphi > M_PI) dphi = 2. * M_PI - dphi; // wrap 0 -> 6.28
    // hadronic measurements: energy sum in 0.1*0.1
    if (deta <= 0.12 && dphi <= 0.12) {
      // samp = CaloSampling::getSampling(*tilecell);
      samp = tilecell->caloDDE()->getSampling();
      rtrigEmCluster.setEnergy(samp, rtrigEmCluster.energy(samp) + energyCell);
      rtrigEmCluster.setRawEnergy(samp, rtrigEmCluster.rawEnergy(samp) + energyCell);
      if ((CaloSampling::TileBar0 == samp) || (CaloSampling::TileExt0 == samp) ||
          (CaloSampling::TileGap1 == samp) || (CaloSampling::TileGap2 == samp)) {
        rtrigEmCluster.setEhad1(rtrigEmCluster.ehad1() + energyCell);
      }
    }

  } // end of loop over cells

  rtrigEmCluster.setNCells(ncells + rtrigEmCluster.nCells());

#ifndef NDEBUG
  if (msgLvl(MSG::DEBUG)) {
    PrintCluster(rtrigEmCluster.energy(CaloSampling::TileBar0) + rtrigEmCluster.energy(CaloSampling::TileExt0),
                 1, 0, CaloSampling::TileBar0, CaloSampling::TileExt0);
    PrintCluster(rtrigEmCluster.energy(CaloSampling::TileBar1) + rtrigEmCluster.energy(CaloSampling::TileExt1),
                 1, 1, CaloSampling::TileBar1, CaloSampling::TileExt1);
    PrintCluster(rtrigEmCluster.energy(CaloSampling::TileBar2) + rtrigEmCluster.energy(CaloSampling::TileExt2),
                 1, 2, CaloSampling::TileBar2, CaloSampling::TileExt2);
  }
#endif

  return StatusCode::SUCCESS;
}

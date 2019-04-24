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
#include "T2CalibrationEgamma.h"
#include "TrigT2CaloCommon/Calo_Def.h"

EgammaReEmEnFex::EgammaReEmEnFex(const std::string& type, const std::string& name,
                                 const IInterface* parent) :
    IReAlgToolCalo(type, name, parent)
{
  declareProperty("QlCorrectionLimit", m_limit);
  declareProperty("QlCorrectionDimension", m_dimension);
  declareProperty("QlCorrection", m_correction);
  // Calibration object
  m_calib = new T2CalibrationEgamma();
}

EgammaReEmEnFex::~EgammaReEmEnFex()
{
  delete m_calib;
}

StatusCode EgammaReEmEnFex::execute(xAOD::TrigEMCluster& rtrigEmCluster, const IRoiDescriptor& roi,
                                    const CaloDetDescrElement*& caloDDE,
                                    const EventContext& context) const
{
  bool cluster_in_barrel = true;
  if (caloDDE) cluster_in_barrel = caloDDE->is_lar_em_barrel();

  ATH_MSG_DEBUG("in execute(TrigEMCluster &)");

  // Region Selector, sampling 0
  int sampling = 0;

  LArTT_Selector<LArCellCont> sel;
  LArTT_Selector<LArCellCont>::const_iterator iBegin, iEnd, it;
  ATH_CHECK( m_dataSvc->loadCollections(context, roi, TTEM, sampling, sel) );
  iBegin = sel.begin();
  iEnd = sel.end();

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

  for (it = iBegin; it != iEnd; ++it) { // Should be revised for London scheme
    ncells++;

    const LArCell* larcell = (*it);
    double etaCell = larcell->eta();
    double phiCell = larcell->phi();
    double energyCell = larcell->energy();

    // find the standard em cluster energy (3*7 cell, now sampling 0)
    // Find position of current cell w.r.t. seed
    deta = fabs(etaCell - energyEta);
    dphi = fabs(phiCell - energyPhi);

    if (dphi > M_PI) dphi = 2. * M_PI - dphi; // wrap 0 -> 6.28
    // 3x7 means three cells per 7 in the second layer 0.025*3/2, 0.025*7/2, for instance
    bool condition37 = cluster_in_barrel &&
                       ((deta <= 0.0375 + 0.0005) && (dphi <= 0.0875 + 0.0005));
    bool condition55 = (!cluster_in_barrel) &&
                       ((deta <= 0.0625 + 0.0005) && (dphi <= 0.0625 + 0.0005));

    if (condition37 || condition55) {

      totalEnergy += energyCell;
      // samp = CaloSampling::getSampling(*larcell);
      samp = larcell->caloDDE()->getSampling();
      rtrigEmCluster.setEnergy(samp, rtrigEmCluster.energy(samp) + energyCell);
      rtrigEmCluster.setRawEnergy(samp, rtrigEmCluster.rawEnergy(samp) + energyCell);
    }

  } // end of loop over sampling 0

#ifndef NDEBUG
  // This will internaly define normal, narrow and large clusters
  if (msgLvl(MSG::ERROR)) {
    if (m_geometryTool->EtaPhiRange(0, 0, energyEta, energyPhi))
      ATH_MSG_ERROR("problems with EtaPhiRange");
    PrintCluster(totalEnergy, 0, 0, CaloSampling::PreSamplerB, CaloSampling::PreSamplerE);
  }
#endif

  // Region Selector, sampling 3
  sampling = 3;

  LArTT_Selector<LArCellCont> sel3;
  ATH_CHECK( m_dataSvc->loadCollections(context, roi, TTEM, sampling, sel3) );
  iBegin = sel3.begin();
  iEnd = sel3.end();
  /*
          if ( m_saveCells ){
             m_data->storeCells(iBegin,iEnd,*m_CaloCellContPoint,m_cellkeepthr);
          }
  */

  for (it = iBegin; it != iEnd; ++it) { // Should be revised for London scheme
    ncells++;
    const LArCell* larcell = (*it);
    double etaCell = larcell->eta();
    double phiCell = larcell->phi();
    double energyCell = larcell->energy();

    // find the standard em cluster energy (3*7 cell, now sampling 0)
    // Find position of current cell w.r.t. seed
    deta = fabs(etaCell - energyEta);
    dphi = fabs(phiCell - energyPhi);
    if (dphi > M_PI) dphi = 2. * M_PI - dphi; // wrap 0 -> 6.28

    // 3x7 means three cells per 7 in the second layer 0.025*3/2, 0.025*7/2, for instance
    bool condition37 = cluster_in_barrel && ((deta <= 0.0375 + 0.001) && (dphi <= 0.0875 + 0.001));
    bool condition55 = (!cluster_in_barrel) &&
                       ((deta <= 0.0625 + 0.001) && (dphi <= 0.0625 + 0.001));

    if (condition37 || condition55) {

      totalEnergy += energyCell;
      // samp = CaloSampling::getSampling(*larcell);
      samp = larcell->caloDDE()->getSampling();
      rtrigEmCluster.setEnergy(samp, rtrigEmCluster.energy(samp) + energyCell);
      rtrigEmCluster.setRawEnergy(samp, rtrigEmCluster.rawEnergy(samp) + energyCell);
    }

  } // end of loop over sampling 3

  // Update Cluster Variables
  rtrigEmCluster.setNCells(ncells + rtrigEmCluster.nCells());
  rtrigEmCluster.setRawEnergy(rtrigEmCluster.rawEnergy() + totalEnergy);
  // The dependency with energy is not defined yet
  rtrigEmCluster.setEnergy(rtrigEmCluster.rawEnergy() *
                           m_calib->Calib(energyEta, rtrigEmCluster.energy()));

  rtrigEmCluster.setEt(rtrigEmCluster.energy() / cosh(energyEta));
  rtrigEmCluster.setRawEt(rtrigEmCluster.rawEnergy() / cosh(energyEta));

#ifndef NDEBUG
  // This will internaly define normal, narrow and large clusters
  if (msgLvl(MSG::ERROR)) {
    if (m_geometryTool->EtaPhiRange(0, 3, energyEta, energyPhi))
      ATH_MSG_ERROR("problems with EtaPhiRange");

    PrintCluster(totalEnergy, 0, 3, CaloSampling::EMB3, CaloSampling::EME3);
  }
#endif

  return StatusCode::SUCCESS;
}

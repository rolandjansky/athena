// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PFODefs.h 792675 2017-01-18 10:39:20Z mhodgkin $
#ifndef XAODPFLOW_PFODEFS_H
#define XAODPFLOW_PFODEFS_H

// Local include(s):
#include "Math/Vector4D.h"


/// Namespace holding all the xAOD EDM classes
namespace xAOD {
  typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > PtEtaPhiMVectorF;


namespace PFODetails
{
    //-------------------------------------------------------------------------
    // This is were the details variables will go
    // DO NOT CHANGE THE ORDER OF THE ENUMS!
    // You can add, but changing the order may lead to disaster!
    //-------------------------------------------------------------------------

  enum PFOAttributes {
    nPi0 = 0,
    //If a downstream algorithms overrides nPi0, nPi0Proto is used to store the original value
    nPi0Proto,  
    eflowRec_EM_FRAC_ENHANCED = 200,
    eflowRec_ENG_FRAC_CORE,
    eflowRec_FIRST_ENG_DENS,
    eflowRec_CENTER_LAMBDA,
    eflowRec_SECOND_R,
    eflowRec_DELTA_ALPHA,
    eflowRec_HOT_STRIP_FRAC,
    eflowRec_THREE_CELL_STRIP_FRAC,
    eflowRec_LATERAL,
    eflowRec_LONGITUDINAL,
    eflowRec_SECOND_LAMBDA,
    eflowRec_ISOLATION,
    eflowRec_ENG_FRAC_MAX, 
    eflowRec_ENG_BAD_CELLS,
    eflowRec_N_BAD_CELLS,
    eflowRec_BADLARQ_FRAC,
    eflowRec_ENG_POS,
    eflowRec_SIGNIFICANCE,
    eflowRec_CELL_SIGNIFICANCE,
    eflowRec_CELL_SIG_SAMPLING,
    eflowRec_AVG_LAR_Q,
    eflowRec_AVG_TILE_Q,
    eflowRec_LAYERENERGY_EM3,
    eflowRec_LAYERENERGY_HEC0,
    eflowRec_LAYERENERGY_Tile0,
    eflowRec_LAYERENERGY_HEC,
    eflowRec_TIMING,
    eflowRec_tracksExpectedEnergyDeposit,
    eflowRec_isInDenseEnvironment,
    eflowRec_LAYERENERGY_EM,
    eflowRec_LAYERENERGY_PreSamplerB,
    eflowRec_LAYERENERGY_EMB1,
    eflowRec_LAYERENERGY_EMB2,
    eflowRec_LAYERENERGY_EMB3,
    eflowRec_LAYERENERGY_PreSamplerE,
    eflowRec_LAYERENERGY_EME1,
    eflowRec_LAYERENERGY_EME2,
    eflowRec_LAYERENERGY_EME3,
    eflowRec_LAYERENERGY_HEC1,
    eflowRec_LAYERENERGY_HEC2,
    eflowRec_LAYERENERGY_HEC3,
    eflowRec_LAYERENERGY_TileBar0,
    eflowRec_LAYERENERGY_TileBar1,
    eflowRec_LAYERENERGY_TileBar2,
    eflowRec_LAYERENERGY_TileGap1,
    eflowRec_LAYERENERGY_TileGap2,
    eflowRec_LAYERENERGY_TileGap3,
    eflowRec_LAYERENERGY_TileExt0,
    eflowRec_LAYERENERGY_TileExt1,
    eflowRec_LAYERENERGY_TileExt2,
    eflowRec_LAYERENERGY_FCAL0,
    eflowRec_LAYERENERGY_FCAL1,
    eflowRec_LAYERENERGY_FCAL2,
    eflowRec_LAYERENERGY_MINIFCAL0,
    eflowRec_LAYERENERGY_MINIFCAL1,
    eflowRec_LAYERENERGY_MINIFCAL2,
    eflowRec_LAYERENERGY_MINIFCAL3,
    eflowRec_EM_PROBABILITY,
    /** These variables belong to the cell-based particle flow algorithm */
    cellBased_FIRST_ETA = 400, 
    cellBased_SECOND_R, 
    cellBased_SECOND_LAMBDA, 
    cellBased_DELTA_PHI, 
    cellBased_DELTA_THETA, 
    cellBased_CENTER_LAMBDA, 
    cellBased_LATERAL, 
    cellBased_LONGITUDINAL, 
    cellBased_ENG_FRAC_EM, 
    cellBased_ENG_FRAC_MAX, 
    cellBased_ENG_FRAC_CORE, 
    cellBased_SECOND_ENG_DENS, 
    cellBased_EM1CoreFrac, 
    cellBased_asymmetryInEM1WRTTrk, 
    cellBased_NHitsInEM1, 
    cellBased_NPosECells_PS, 
    cellBased_NPosECells_EM1, 
    cellBased_NPosECells_EM2, 
    cellBased_firstEtaWRTClusterPosition_EM1, 
    cellBased_firstEtaWRTClusterPosition_EM2, 
    cellBased_secondEtaWRTClusterPosition_EM1, 
    cellBased_secondEtaWRTClusterPosition_EM2, 
    cellBased_energy_EM1, 
    cellBased_energy_EM2, 
    /** These are the variables describing Tau Shot objects, which are built from EM1 cells */
    tauShots_nCellsInEta = 600,
    tauShots_pt1, 
    tauShots_pt3, 
    tauShots_pt5, 
    tauShots_ws5, 
    tauShots_sdevEta5_WRTmean, 
    tauShots_sdevEta5_WRTmode, 
    tauShots_sdevPt5, 
    tauShots_deltaPt12_min, 
    tauShots_Fside_3not1, 
    tauShots_Fside_5not1, 
    tauShots_Fside_5not3, 
    tauShots_fracSide_3not1, 
    tauShots_fracSide_5not1, 
    tauShots_fracSide_5not3, 
    tauShots_pt1OverPt3, 
    tauShots_pt3OverPt5, 
    tauShots_mergedScore, 
    tauShots_signalScore, 
    tauShots_nPhotons,
    tauShots_seedHash
  };

  /** This enum is used to label the associated particles to the PFO object */

  enum PFOParticleType{
    CaloCluster = 0,
    Track,
    TauShot,
    HadronicCalo,
    ChargedPFO,
    NeutralPFO,
    TauTrack
  };

  /** This enum is used to label the lepton type of the neutral PFO */
  enum PFOLeptonType{
    PFO_electron = 0,
    PFO_muon,
    PFO_tau,
    PFO_photon,
    PFO_nonLeptonic
  };


}//end namespace PFODetails

}

#endif // XAODPFLOW_PFODEFS_H

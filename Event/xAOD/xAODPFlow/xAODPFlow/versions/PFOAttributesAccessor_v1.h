// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PFOAttributesAccessor_v1.h 792675 2017-01-18 10:39:20Z mhodgkin $
#ifndef PFOATTRIBUTESACCESSOR_H
#define PFOATTRIBUTESACCESSOR_H

/* Author Mark Hodgkinson
This is pretty much based on the class xAODJets/JetMomentsAccessor.h - i.e I copied the code from there
*/

// System include(s):
#include <iostream>

// Local include(s):
#include "xAODPFlow/PFODefs.h"

#define DEFINE_PFO_CONSTACCESSOR( NAME )                                    \
  case xAOD::PFODetails::NAME:				\
  {									\
    static const SG::AuxElement::Accessor< T > a( #NAME );			\
    return &a;								\
  }									\
  break

namespace xAOD{

  template<class T> struct PFOAttributesAccessor_v1 {
    static const SG::AuxElement::Accessor<T>* constAccessor (xAOD::PFODetails::PFOAttributes variable){
      switch (variable){
	DEFINE_PFO_CONSTACCESSOR(nPi0);
	DEFINE_PFO_CONSTACCESSOR(nPi0Proto);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_EM_FRAC_ENHANCED);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_ENG_FRAC_CORE);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_FIRST_ENG_DENS);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_CENTER_LAMBDA);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_SECOND_R);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_DELTA_ALPHA);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_HOT_STRIP_FRAC);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_THREE_CELL_STRIP_FRAC);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_LATERAL);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_LONGITUDINAL);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_SECOND_LAMBDA);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_ISOLATION);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_ENG_FRAC_MAX); 
	DEFINE_PFO_CONSTACCESSOR(eflowRec_ENG_BAD_CELLS);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_N_BAD_CELLS);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_BADLARQ_FRAC);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_ENG_POS);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_SIGNIFICANCE);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_CELL_SIGNIFICANCE);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_CELL_SIG_SAMPLING);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_AVG_LAR_Q);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_AVG_TILE_Q);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_EM3);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_HEC0);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_Tile0);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_HEC);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_TIMING);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_tracksExpectedEnergyDeposit);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_isInDenseEnvironment);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_EM);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_PreSamplerB);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_EMB1);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_EMB2);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_EMB3);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_PreSamplerE);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_EME1);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_EME2);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_EME3);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_HEC1);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_HEC2);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_HEC3);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_TileBar0);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_TileBar1);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_TileBar2);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_TileGap1);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_TileGap2);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_TileGap3);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_TileExt0);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_TileExt1);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_TileExt2);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_FCAL0);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_FCAL1);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_FCAL2);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_MINIFCAL0);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_MINIFCAL1);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_MINIFCAL2);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_LAYERENERGY_MINIFCAL3);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_layerVectorCellOrdering);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_radiusVectorCellOrdering);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_avgEdensityVectorCellOrdering);
        DEFINE_PFO_CONSTACCESSOR(eflowRec_layerHED);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_EM_PROBABILITY);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_ENG_CALIB_TOT);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_ENG_CALIB_FRAC_EM);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_ENG_CALIB_FRAC_HAD);
	DEFINE_PFO_CONSTACCESSOR(eflowRec_ENG_CALIB_FRAC_REST);
	DEFINE_PFO_CONSTACCESSOR(cellBased_FIRST_ETA); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_SECOND_R); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_SECOND_LAMBDA); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_DELTA_PHI); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_DELTA_THETA); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_CENTER_LAMBDA); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_LATERAL); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_LONGITUDINAL); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_ENG_FRAC_EM); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_ENG_FRAC_MAX); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_ENG_FRAC_CORE); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_SECOND_ENG_DENS); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_EM1CoreFrac); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_asymmetryInEM1WRTTrk); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_NHitsInEM1); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_NPosECells_PS); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_NPosECells_EM1); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_NPosECells_EM2); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_firstEtaWRTClusterPosition_EM1); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_firstEtaWRTClusterPosition_EM2); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_secondEtaWRTClusterPosition_EM1); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_secondEtaWRTClusterPosition_EM2); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_energy_EM1); 
	DEFINE_PFO_CONSTACCESSOR(cellBased_energy_EM2); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_nCellsInEta);
	DEFINE_PFO_CONSTACCESSOR(tauShots_pt1); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_pt3); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_pt5); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_ws5); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_sdevEta5_WRTmean); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_sdevEta5_WRTmode); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_sdevPt5); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_deltaPt12_min); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_Fside_3not1); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_Fside_5not1); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_Fside_5not3); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_fracSide_3not1); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_fracSide_5not1); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_fracSide_5not3); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_pt1OverPt3); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_pt3OverPt5); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_mergedScore); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_signalScore); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_nPhotons); 
	DEFINE_PFO_CONSTACCESSOR(tauShots_seedHash);
	default:
	  std::cerr << "xAOD::PFO_v1 ERROR Unknown float/double variable (" << static_cast<int>(variable) << ") requested" << std::endl;
	  return 0;
      }//switch on variable names
    }//const accessor
    
  }//struct
  ;
  
  struct PFOParticleTypeMapper_v1 {
    bool getValue(PFODetails::PFOParticleType type, std::string& stringName){
      switch (type){
      case PFODetails::CaloCluster:
	stringName = "pfo_ClusterLinks";
	return true;
      case PFODetails::Track:
	stringName = "pfo_TrackLinks";
	return true;
	// Note - clients should NEVER set both links to TauShot and HadroniCaloCluster - Tau WG agreed they NEVER will do such a thing (and they are only client)
      case PFODetails::TauShot:
	stringName = "pfo_TauShotLinks";
	return true;
      case PFODetails::HadronicCalo:
        stringName = "pfo_TauShotLinks";
      case PFODetails::ChargedPFO:
	stringName = "pfo_Charged";
      case PFODetails::NeutralPFO:
	stringName = "pfo_Neutral";
      case PFODetails::TauTrack:
	stringName = "pfo_TauTrack";
      }//switch
      return false;
    }//getValue

  };

}//xAOD namespace

#endif

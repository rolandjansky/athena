// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PFOAttributesAccessor_v1.h 601670 2014-06-13 13:27:35Z mhodgkin $
#ifndef PFOATTRIBUTESACCESSOR_H
#define PFOATTRIBUTESACCESSOR_H

/* Author Mark Hodgkinson
This is pretty much based on the class xAODJets/JetMomentsAccessor.h - i.e I copied the code from there
*/

// System include(s):
#include <iostream>

// Local include(s):
#include "xAODPFlow/PFODefs.h"

#define DEFINE_PFO_ACCESSOR( NAME )                                    \
  case xAOD::PFODetails::NAME:				\
  {									\
    static SG::AuxElement::Accessor< T > a( #NAME );			\
    return &a;								\
  }									\
  break

namespace xAOD{

  template<class T> struct PFOAttributesAccessor_v1 {
    static SG::AuxElement::Accessor<T>* accessor (xAOD::PFODetails::PFOAttributes variable){
      switch (variable){
	DEFINE_PFO_ACCESSOR(nPi0);
	DEFINE_PFO_ACCESSOR(nPi0Proto);
	DEFINE_PFO_ACCESSOR(eflowRec_EM_FRAC_ENHANCED);
	DEFINE_PFO_ACCESSOR(eflowRec_ENG_FRAC_CORE);
	DEFINE_PFO_ACCESSOR(eflowRec_FIRST_ENG_DENS);
	DEFINE_PFO_ACCESSOR(eflowRec_CENTER_LAMBDA);
	DEFINE_PFO_ACCESSOR(eflowRec_SECOND_R);
	DEFINE_PFO_ACCESSOR(eflowRec_DELTA_ALPHA);
	DEFINE_PFO_ACCESSOR(eflowRec_HOT_STRIP_FRAC);
	DEFINE_PFO_ACCESSOR(eflowRec_THREE_CELL_STRIP_FRAC);
	DEFINE_PFO_ACCESSOR(eflowRec_LATERAL);
	DEFINE_PFO_ACCESSOR(eflowRec_LONGITUDINAL);
	DEFINE_PFO_ACCESSOR(eflowRec_SECOND_LAMBDA);
	DEFINE_PFO_ACCESSOR(eflowRec_ISOLATION);
	DEFINE_PFO_ACCESSOR(eflowRec_ENG_FRAC_MAX); 
	DEFINE_PFO_ACCESSOR(eflowRec_ENG_BAD_CELLS);
	DEFINE_PFO_ACCESSOR(eflowRec_N_BAD_CELLS);
	DEFINE_PFO_ACCESSOR(eflowRec_BADLARQ_FRAC);
	DEFINE_PFO_ACCESSOR(eflowRec_ENG_POS);
	DEFINE_PFO_ACCESSOR(eflowRec_SIGNIFICANCE);
	DEFINE_PFO_ACCESSOR(eflowRec_CELL_SIGNIFICANCE);
	DEFINE_PFO_ACCESSOR(eflowRec_CELL_SIG_SAMPLING);
	DEFINE_PFO_ACCESSOR(eflowRec_AVG_LAR_Q);
	DEFINE_PFO_ACCESSOR(eflowRec_AVG_TILE_Q);
	DEFINE_PFO_ACCESSOR(eflowRec_LAYERENERGY_EM3);
	DEFINE_PFO_ACCESSOR(eflowRec_LAYERENERGY_HEC0);
	DEFINE_PFO_ACCESSOR(eflowRec_LAYERENERGY_Tile0);
	DEFINE_PFO_ACCESSOR(cellBased_FIRST_ETA); 
	DEFINE_PFO_ACCESSOR(cellBased_SECOND_R); 
	DEFINE_PFO_ACCESSOR(cellBased_SECOND_LAMBDA); 
	DEFINE_PFO_ACCESSOR(cellBased_DELTA_PHI); 
	DEFINE_PFO_ACCESSOR(cellBased_DELTA_THETA); 
	DEFINE_PFO_ACCESSOR(cellBased_CENTER_LAMBDA); 
	DEFINE_PFO_ACCESSOR(cellBased_LATERAL); 
	DEFINE_PFO_ACCESSOR(cellBased_LONGITUDINAL); 
	DEFINE_PFO_ACCESSOR(cellBased_ENG_FRAC_EM); 
	DEFINE_PFO_ACCESSOR(cellBased_ENG_FRAC_MAX); 
	DEFINE_PFO_ACCESSOR(cellBased_ENG_FRAC_CORE); 
	DEFINE_PFO_ACCESSOR(cellBased_SECOND_ENG_DENS); 
	DEFINE_PFO_ACCESSOR(cellBased_EM1CoreFrac); 
	DEFINE_PFO_ACCESSOR(cellBased_asymmetryInEM1WRTTrk); 
	DEFINE_PFO_ACCESSOR(cellBased_NHitsInEM1); 
	DEFINE_PFO_ACCESSOR(cellBased_NPosECells_PS); 
	DEFINE_PFO_ACCESSOR(cellBased_NPosECells_EM1); 
	DEFINE_PFO_ACCESSOR(cellBased_NPosECells_EM2); 
	DEFINE_PFO_ACCESSOR(cellBased_firstEtaWRTClusterPosition_EM1); 
	DEFINE_PFO_ACCESSOR(cellBased_firstEtaWRTClusterPosition_EM2); 
	DEFINE_PFO_ACCESSOR(cellBased_secondEtaWRTClusterPosition_EM1); 
	DEFINE_PFO_ACCESSOR(cellBased_secondEtaWRTClusterPosition_EM2); 
	DEFINE_PFO_ACCESSOR(cellBased_energy_EM1); 
	DEFINE_PFO_ACCESSOR(cellBased_energy_EM2); 
	DEFINE_PFO_ACCESSOR(tauShots_nCellsInEta);
	DEFINE_PFO_ACCESSOR(tauShots_pt1); 
	DEFINE_PFO_ACCESSOR(tauShots_pt3); 
	DEFINE_PFO_ACCESSOR(tauShots_pt5); 
	DEFINE_PFO_ACCESSOR(tauShots_ws5); 
	DEFINE_PFO_ACCESSOR(tauShots_sdevEta5_WRTmean); 
	DEFINE_PFO_ACCESSOR(tauShots_sdevEta5_WRTmode); 
	DEFINE_PFO_ACCESSOR(tauShots_sdevPt5); 
	DEFINE_PFO_ACCESSOR(tauShots_deltaPt12_min); 
	DEFINE_PFO_ACCESSOR(tauShots_Fside_3not1); 
	DEFINE_PFO_ACCESSOR(tauShots_Fside_5not1); 
	DEFINE_PFO_ACCESSOR(tauShots_Fside_5not3); 
	DEFINE_PFO_ACCESSOR(tauShots_fracSide_3not1); 
	DEFINE_PFO_ACCESSOR(tauShots_fracSide_5not1); 
	DEFINE_PFO_ACCESSOR(tauShots_fracSide_5not3); 
	DEFINE_PFO_ACCESSOR(tauShots_pt1OverPt3); 
	DEFINE_PFO_ACCESSOR(tauShots_pt3OverPt5); 
	DEFINE_PFO_ACCESSOR(tauShots_mergedScore); 
	DEFINE_PFO_ACCESSOR(tauShots_signalScore); 
	DEFINE_PFO_ACCESSOR(tauShots_nPhotons); 
	DEFINE_PFO_ACCESSOR(tauShots_seedHash);
	default:
	  std::cerr << "xAOD::PFO_v1 ERROR Unknown float/double variable (" << static_cast<int>(variable) << ") requested" << std::endl;
	  return 0;
      }//switch on variable names
    }//accessor
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
      case PFODetails::TauShot:
	stringName = "pfo_TauShotLinks";
	return true;
      }//switch
      return false;
    }//getValue

  };

}//xAOD namespace

#endif

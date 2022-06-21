/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     EgammaReSamp2Fex.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma
 
 AUTHOR:   M.P. Casado
 
 PURPOSE:  Calculates energy weighted cluster position around
	   hottest cell in RoI. Also, calculates energy in
	   a 3x7 and in a 7x7 cluster (rCore = 3x7/7x7).
 *******************************************************************/

#ifndef TRIGT2CALOEGAMMA_CALOSAMP2FEXEGAMMARE_H 
#define TRIGT2CALOEGAMMA_CALOSAMP2FEXEGAMMARE_H


#include "TrigT2CaloCommon/IReAlgToolCalo.h"
#include "GaudiKernel/AlgTool.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "TrigT2CaloCommon/Calo_Def.h"

class IRoiDesciptor;

/** Feature extraction Tool for LVL2 Calo. Second EM Calorimeter sample. */
class EgammaReSamp2Fex: public IReAlgToolCalo {
  public:
    EgammaReSamp2Fex(const std::string & type, const std::string & name,
                     const IInterface* parent);

    /** @brief execute feature extraction for the EM Calorimeter
    *	second layer 
    *   @param[out] rtrigEmCluster is the output cluster.
    *   @param[in] eta/phi-min/max = RoI definition.
    */
    virtual StatusCode execute(xAOD::TrigEMCluster &rtrigEmCluster, const IRoiDescriptor& roi,
                               const CaloDetDescrElement*& caloDDE,
                               const EventContext& context) const override;

  private:
    inline double etaSizeLArEMSamp2(const double eta, const int calo) const;
    inline double phiSizeLArEMSamp2(const double eta, const int calo) const;

    Gaudi::Property<float> m_maxHotCellDeta{this, "MaxDetaHotCell", 1.0};
    Gaudi::Property<float> m_maxHotCellDphi{this, "MaxDphiHotCell", 1.0};
};

inline double EgammaReSamp2Fex::etaSizeLArEMSamp2(double cellEta, int calo) const{
  double sizeEta;
  if( calo == Calorimeter::EMBAR ){
      if ( std::abs(cellEta) < 1.4 ){
        sizeEta = 0.025;
      }else{
        sizeEta = 0.075;
      }
  } else {
      if ( std::abs(cellEta) < 2.5 ){
        sizeEta = 0.025;
      } else{
        sizeEta = 0.1;
      }
  }
  return sizeEta;
}

inline double EgammaReSamp2Fex::phiSizeLArEMSamp2(double cellEta, int calo) const{
  double sizePhi;
  if( calo == Calorimeter::EMBAR ){
    sizePhi = 0.025;
  } else {
      if ( std::abs(cellEta) < 2.5 ){
        sizePhi = 0.025;
      } else {
        sizePhi = 0.1;
      }
  }
  return sizePhi;
}

#endif

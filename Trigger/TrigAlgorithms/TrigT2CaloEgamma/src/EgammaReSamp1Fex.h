/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     EgammaReSamp1Fex.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma
 
 AUTHOR:   M.P. Casado
 
 PURPOSE:  Based on Cluster position defined in the second
	   EM Calorimeter layer, calculates strip layer energy
	   profile variable (eRatio) to discriminate clusters
	   with 1 energy peak in the strip layer from clusters
	   with multiple peaks. Calculates Eta position in
	   this layer.
 *******************************************************************/

#ifndef TRIGT2CALOEGAMMA_CALOSAMP1FEXEGAMMARE_H 
#define TRIGT2CALOEGAMMA_CALOSAMP1FEXEGAMMARE_H

#include "TrigT2CaloCommon/IReAlgToolCalo.h"

class IRoiDescriptor;

/** Feature extraction Tool for LVL2 Calo. First EM Calorimeter sample. */
class EgammaReSamp1Fex: public IReAlgToolCalo {
  public:
    EgammaReSamp1Fex(const std::string & type, const std::string & name,
                     const IInterface* parent);

    /** @brief execute feature extraction for the EM Calorimeter
    *   second layer 
    *   @param[out] rtrigEmCluster is the output cluster.
    *   @param[in] eta/phi-min/max = RoI definition.
    */
    virtual StatusCode execute(xAOD::TrigEMCluster &rtrigEmCluster,
                               const IRoiDescriptor& roi,
                               const CaloDetDescrElement*& caloDDE,
                               const EventContext& context ) const override;
};

#endif

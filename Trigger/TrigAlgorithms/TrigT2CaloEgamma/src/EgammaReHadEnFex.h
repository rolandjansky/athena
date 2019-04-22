/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     EgammaReHadEnFex.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma
 
 AUTHOR:   M.P. Casado
 
 PURPOSE:  Based on Cluster position defined in the second
           EM Calorimeter layer, calculates total Had cluster
           energy.
 *******************************************************************/

#ifndef TRIGT2CALOEGAMMA_EGAMMAHADENFEXRE_H 
#define TRIGT2CALOEGAMMA_EGAMMAHADENFEXRE_H

#include "TrigT2CaloCommon/IReAlgToolCalo.h"

/** Feature extraction Tool for LVL2 Calo. Hadronic EndCaps
	and Tile Calorimeter. All Samples */
class EgammaReHadEnFex: public IReAlgToolCalo {
public:
    EgammaReHadEnFex(const std::string & type, const std::string & name, 
                     const IInterface* parent);

    /** @brief execute feature extraction for the EM Calorimeter
    *   second layer 
    *   @param[out] rtrigEmCluster is the output cluster.
    *   @param[in] eta/phi-min/max = RoI definition.
    */
    virtual StatusCode execute(xAOD::TrigEMCluster &rtrigEmCluster,
                               const IRoiDescriptor& roi,
                               const CaloDetDescrElement*& caloDDE,
                               const EventContext& context) const override;
};


#endif

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     EgammaReEmEnFex.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma
 
 AUTHOR:   M.P. Casado
 
 PURPOSE:  Based on Cluster position defined in the second
           EM Calorimeter layer, calculates total EM cluster
	   energy.
 *******************************************************************/

#ifndef TRIGT2CALOEGAMMA_EGAMMAEMENFEXRE_H 
#define TRIGT2CALOEGAMMA_EGAMMAEMENFEXRE_H

#include "TrigT2CaloCommon/IReAlgToolCalo.h"

/** Feature extraction Tool for LVL2 Calo. EM Presampler and
	third EM Calorimeter sample. */
class EgammaReEmEnFex: public IReAlgToolCalo {
  public:
    EgammaReEmEnFex(const std::string & type, const std::string & name,
                    const IInterface* parent);

    virtual ~EgammaReEmEnFex();

    /** @brief execute feature extraction for the EM Calorimeter
    *   second layer 
    *   @param[out] rtrigEmCluster is the output cluster.
    *   @param[in] roi RoI definition.
    */
    virtual StatusCode execute(xAOD::TrigEMCluster &rtrigEmCluster,
                               const IRoiDescriptor& roi,
                               const CaloDetDescrElement*& caloDDE,
                               const EventContext& context) const override;
};

#endif

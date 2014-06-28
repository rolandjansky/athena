/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     CosmicHadEnFex.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCosmic
 
 AUTHOR:   D.O.Damazio
 
 PURPOSE:  Based on Cluster position defined in the second
           EM Calorimeter layer, calculates total Had cluster
           energy.
 *******************************************************************/

#ifndef TRIGT2CALOCOSMIC_COSMICHADENFEX_H 
#define TRIGT2CALOCOSMIC_COSMICHADENFEX_H

#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "GaudiKernel/AlgTool.h"

/** Feature extraction Tool for LVL2 Calo. Hadronic EndCaps
	and Tile Calorimeter. All Samples */
class CosmicHadEnFex: public IAlgToolCalo {
  public:
    /** Constructor */
    CosmicHadEnFex(const std::string & type, const std::string & name, 
                 const IInterface* parent);
    /** Destructor */
    virtual ~CosmicHadEnFex();
    /** @brief execute feature extraction for the EM Calorimeter
    *   second layer 
    *   @param[out] rtrigEmCluster is the output cluster.
    *   @param[in] eta/phi-min/max = RoI definition.
    */
    using IAlgToolCalo::execute;
    StatusCode execute(TrigEMCluster &rtrigEmCluster,double etamin,
			double etamax, double phimin, double phimax);
   private:
    float m_THR_1;
    float m_THR_2;

};


#endif

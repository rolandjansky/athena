/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     EgammaEmEnFex.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma
 
 AUTHOR:   M.P. Casado
 
 PURPOSE:  Based on Cluster position defined in the second
           EM Calorimeter layer, calculates total EM cluster
	   energy.
 *******************************************************************/

#ifndef TRIGT2CALOEGAMMA_EGAMMAEMENFEX_H 
#define TRIGT2CALOEGAMMA_EGAMMAEMENFEX_H

#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "CxxUtils/phihelper.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "GaudiKernel/AlgTool.h"

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;  // legacy trigger code

/** Feature extraction Tool for LVL2 Calo. EM Presampler and
	third EM Calorimeter sample. */
class EgammaEmEnFex: public IAlgToolCalo {
  public:
    // to avoid compiler warning about hidden virtuals
    using IAlgToolCalo::execute;
  
    /** Constructor */
    EgammaEmEnFex(const std::string & type, const std::string & name, 
                 const IInterface* parent);
    /** Destructor */
    virtual ~EgammaEmEnFex();
    /** @brief execute feature extraction for the EM Calorimeter
    *   second layer 
    *   @param[out] rtrigEmCluster is the output cluster.
    *   @param[in] roi RoI definition.
    */
    StatusCode execute(xAOD::TrigEMCluster &rtrigEmCluster,
		       const IRoiDescriptor& roi,
		       const CaloDetDescrElement*& caloDDE = caloDDENull);

    /// OBSOLETE!! DO NOT USE

    /** @brief execute feature extraction for the EM Calorimeter
    *   second layer 
    *   @param[out] rtrigEmCluster is the output cluster.
    *   @param[in] eta/phi-min/max = RoI definition.
    */
    StatusCode execute(xAOD::TrigEMCluster &rtrigEmCluster,
		       double etamin, double etamax, 
		       double phimin, double phimax) {
      TrigRoiDescriptor roi( 0.5*(etamin+etamax), etamin, etamax,
			     CxxUtils::phiMean(phimin,phimax), phimin, phimax);
      return execute( rtrigEmCluster, roi, caloDDENull );
    }


};


#endif

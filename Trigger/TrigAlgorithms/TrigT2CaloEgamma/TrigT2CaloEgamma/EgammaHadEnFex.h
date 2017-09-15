/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     EgammaHadEnFex.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma
 
 AUTHOR:   M.P. Casado
 
 PURPOSE:  Based on Cluster position defined in the second
           EM Calorimeter layer, calculates total Had cluster
           energy.
 *******************************************************************/

#ifndef TRIGT2CALOEGAMMA_EGAMMAHADENFEX_H 
#define TRIGT2CALOEGAMMA_EGAMMAHADENFEX_H

#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "GaudiKernel/AlgTool.h"

/** Feature extraction Tool for LVL2 Calo. Hadronic EndCaps
	and Tile Calorimeter. All Samples */
class EgammaHadEnFex: public IAlgToolCalo {
  public:
    // to avoid compiler warning about hidden virtuals
    using IAlgToolCalo::execute;
  
    /** Constructor */
    EgammaHadEnFex(const std::string & type, const std::string & name, 
                 const IInterface* parent);
    /** Destructor */
    virtual ~EgammaHadEnFex();
    /** @brief execute feature extraction for the EM Calorimeter
    *   second layer 
    *   @param[out] rtrigEmCluster is the output cluster.
    *   @param[in] eta/phi-min/max = RoI definition.
    */
    StatusCode execute(xAOD::TrigEMCluster &rtrigEmCluster,
		       const IRoiDescriptor& roi );

    //    StatusCode execute(TrigEMCluster &rtrigEmCluster,double etamin,
    //			double etamax, double phimin, double phimax);


    StatusCode initialize() {
                // Very important to call base class initialize
                if ( IAlgToolCalo::initialize().isFailure() ) {
                        *(new MsgStream(AlgTool::msgSvc(), name()))
                        << MSG::FATAL
                        << "Could not init base class IAlgTooCalo" << endmsg;
                }
                if (!m_timersvc.empty()) {
                        m_timer[0]->propName("T2CaEm_NCells");
                }
                return StatusCode::SUCCESS;
    }

};


#endif

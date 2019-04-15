/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/AlgTool.h"

/** Feature extraction Tool for LVL2 Calo. Hadronic EndCaps
	and Tile Calorimeter. All Samples */
class EgammaReHadEnFex: public IReAlgToolCalo {
  public:
    // to avoid compiler warning about hidden virtuals
    using IReAlgToolCalo::execute;
  
    /** Constructor */
    EgammaReHadEnFex(const std::string & type, const std::string & name, 
                 const IInterface* parent);
    /** Destructor */
    virtual ~EgammaReHadEnFex();
    /** @brief execute feature extraction for the EM Calorimeter
    *   second layer 
    *   @param[out] rtrigEmCluster is the output cluster.
    *   @param[in] eta/phi-min/max = RoI definition.
    */
    StatusCode execute(xAOD::TrigEMCluster &rtrigEmCluster,
		       const IRoiDescriptor& roi,
		       const CaloDetDescrElement*& caloDDE = caloReDDENull,
                       const EventContext* context = nullptr ) const;

    //    StatusCode execute(TrigEMCluster &rtrigEmCluster,double etamin,
    //			double etamax, double phimin, double phimax);


    StatusCode initialize() {
                // Very important to call base class initialize
                if ( IReAlgToolCalo::initialize().isFailure() ) {
                        *(new MsgStream(AlgTool::msgSvc(), name()))
                        << MSG::FATAL
                        << "Could not init base class IReAlgTooCalo" << endmsg;
                }
                if (!m_timersvc.empty()) {
                        m_timer[0]->propName("T2CaEm_NCells");
                }
                return StatusCode::SUCCESS;
    }

};


#endif

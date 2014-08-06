/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     EgammaSamp2Fex.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma
 
 AUTHOR:   M.P. Casado
 
 PURPOSE:  Calculates energy weighted cluster position around
	   hottest cell in RoI. Also, calculates energy in
	   a 3x7 and in a 7x7 cluster (rCore = 3x7/7x7).
 *******************************************************************/

#ifndef TRIGT2CALOEGAMMA_CALOALLFEXEGAMMA_H 
#define TRIGT2CALOEGAMMA_CALOALLFEXEGAMMA_H


#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "GaudiKernel/AlgTool.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "TrigT2CaloCommon/Calo_Def.h"
#include "TrigT2CaloCommon/phiutils.h"

class IRoiDescriptor;

/** Feature extraction Tool for LVL2 Calo. Second EM Calorimeter sample. */
class EgammaAllFex: public IAlgToolCalo {
  public:
    // to avoid compiler warning about hidden virtuals
    using IAlgToolCalo::execute;
  
    /** Constructor */
    EgammaAllFex(const std::string & type, const std::string & name, 
                 const IInterface* parent);
    /** Destructor */
    virtual ~EgammaAllFex();

   /** @brief execute feature extraction for the EM Calorimeter
    *	second layer 
    *   @param[out] rtrigEmCluster is the output cluster.
    *   @param[in] eta/phi-min/max = RoI definition.
    */
    StatusCode execute(xAOD::TrigEMCluster &rtrigEmCluster,
		       const IRoiDescriptor& roi );

    /// OBSOLETE, DO NOT USE!!
    /** @brief execute feature extraction for the EM Calorimeter
    *	second layer 
    *   @param[out] rtrigEmCluster is the output cluster.
    *   @param[in] eta/phi-min/max = RoI definition.
    */
    StatusCode execute(xAOD::TrigEMCluster &rtrigEmCluster,
		       double etamin, double etamax, 
		       double phimin, double phimax) { 
      TrigRoiDescriptor roi( 0.5*(etamin+etamax), etamin, etamax,
                             HLT::phimean(phimin,phimax), phimin, phimax);
      return execute( rtrigEmCluster, roi );
    }


    /** Special initialize for All to include eta as a
	trigger timer item monitored parameter. Important
	to compare time performance as a function of cluster
	position.
    */

    StatusCode initialize() {
		// Very important to call base class initialize
                if ( IAlgToolCalo::initialize().isFailure() ) {
                	*(new MsgStream(AlgTool::msgSvc(), name()))
			<< MSG::FATAL 
			<< "Could not init base class IAlgTooCalo" << endreq;
                }
                std::string basename(name().substr(25,5)+".");
		//std::string basename(name().substr(6,1)+name().substr(name().find("Fex",0)-5,5));
                //basename+=(name().substr(6,1)+name().substr(name().find("Fex",0)-5,5));
		if (m_timersvc) {
                	m_timer[0]->propName(basename+"Eta");
		}
                return StatusCode::SUCCESS;
    }
   private:
     bool  m_includeHad;

};


#endif

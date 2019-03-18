/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     CosmicSamp2Fex.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCosmic
 
 AUTHOR:   D.O. Damazio
 
 PURPOSE:  Calculates energy weighted cluster position around
	   hottest cell in RoI for LAr second layer.
 *******************************************************************/

#ifndef TRIGT2CALOCOSMIC_CALOSAMP2FEXCOSMIC_H 
#define TRIGT2CALOCOSMIC_CALOSAMP2FEXCOSMIC_H


#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "GaudiKernel/AlgTool.h"

#include "IRegionSelector/IRoiDescriptor.h"

/** Feature extraction Tool for LVL2 Calo. Second EM Calorimeter sample. */
class CosmicSamp2Fex: public IAlgToolCalo {
  public:
    /** Constructor */
    CosmicSamp2Fex(const std::string & type, const std::string & name, 
                 const IInterface* parent);
    /** Destructor */
    virtual ~CosmicSamp2Fex();
    /** @brief execute feature extraction for the EM Calorimeter
    *	second layer 
    *   @param[out] rtrigEmCluster is the output cluster.
    *   @param[in] eta/phi-min/max = RoI definition.
    */
    using IAlgToolCalo::execute;
    //    StatusCode execute(TrigEMCluster &rtrigEmCluster,double etamin,
    //	  		     double etamax, double phimin, double phimax);
    StatusCode execute(TrigEMCluster &rtrigEmCluster,
		       const IRoiDescriptor& roi,
		       const CaloDetDescrElement*& /*caloDDE*/);

    /** Special initialize for Samp2 to include eta as a
	trigger timer item monitored parameter. Important
	to compare time performance as a function of cluster
	position.
    */
    StatusCode initialize() {
		// Very important to call base class initialize
                if ( IAlgToolCalo::initialize().isFailure() ) {
                	*(new MsgStream(AlgTool::msgSvc(), name()))
			<< MSG::FATAL 
			<< "Could not init base class IAlgTooCalo" << endmsg;
                }
                std::string basename(name().substr(25,5)+".");
		if (m_timersvc) {
                	m_timer[0]->propName(basename+"Eta");
		}
                return StatusCode::SUCCESS;
    }

  private:
    double etaSizeLArEMSamp2(const double eta, const int calo) const;
    double phiSizeLArEMSamp2(const double eta, const int calo) const;
    float m_THR_1;
    float m_THR_2;

};


#endif

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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


#include "TrigT2CaloCommon/IReAlgToolCalo.h"
#include "TrigT2CaloCommon/Calo_Def.h"

class IRoiDescriptor;

/** Feature extraction Tool for LVL2 Calo. Second EM Calorimeter sample. */
class EgammaAllFex: public IReAlgToolCalo {
  public:
  
    /** Constructor */
    EgammaAllFex(const std::string & type, const std::string & name, 
                 const IInterface* parent);

   /** @brief execute feature extraction for the EM Calorimeter
    *	second layer 
    *   @param[out] rtrigEmCluster is the output cluster.
    *   @param[in] eta/phi-min/max = RoI definition.
    */
    virtual StatusCode execute(xAOD::TrigEMCluster &rtrigEmCluster,
		       const IRoiDescriptor& roi,
		       const CaloDetDescrElement*& /*caloDDE*/,
                       const EventContext& context ) const override;

   private:
     bool  m_includeHad;

};


#endif

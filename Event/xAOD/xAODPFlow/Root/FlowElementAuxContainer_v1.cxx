/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODPFlow/versions/FlowElementAuxContainer_v1.h"
 
namespace xAOD {
 
   FlowElementAuxContainer_v1::FlowElementAuxContainer_v1()
      : AuxContainerBase() {
	AUX_VARIABLE(pt);
	AUX_VARIABLE(eta);
	AUX_VARIABLE(phi);
	AUX_VARIABLE(m);
	AUX_VARIABLE(charge);
	AUX_VARIABLE(signalType);
	AUX_VARIABLE(vertexType);
	AUX_VARIABLE( chargedObjectLinks );
	AUX_VARIABLE( chargedObjectWeights );
	AUX_VARIABLE( otherObjectLinks );
	AUX_VARIABLE( otherObjectWeights );
      }   
   
} // namespace xAOD

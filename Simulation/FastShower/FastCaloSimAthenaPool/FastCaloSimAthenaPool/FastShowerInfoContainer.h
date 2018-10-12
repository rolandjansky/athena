/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTSHOWERINFOCONTAINER_H
#define FASTSHOWERINFOCONTAINER_H

//#include <vector>
// #include <iostream>
#include "AthContainers/DataVector.h"
#include "FastCaloSimAthenaPool/FastShowerInfo.h"



/** 
*	@class FastShowerInfoContainer
*	@brief Container for FastShowerInfo objects
*/
class FastShowerInfoContainer : public DataVector<FastShowerInfo>
{

	
	public:
	
		FastShowerInfoContainer(SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS) : DataVector<FastShowerInfo>(ownPolicy) {};
		
		~FastShowerInfoContainer(){};
			
};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(FastShowerInfoContainer, 666778, 1)

#endif

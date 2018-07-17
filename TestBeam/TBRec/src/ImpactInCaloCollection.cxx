/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "ImpactInCaloCollection.h"
#include "ImpactInCalo.h"

void ImpactInCaloCollection::print() const{
	ImpactInCaloCollection::const_iterator iter;
	for( iter=begin(); iter!=end(); ++iter ) (*iter)->print();
}


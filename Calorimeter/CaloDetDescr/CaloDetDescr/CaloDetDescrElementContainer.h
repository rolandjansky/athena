/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALODETECTORELEMENTCONTAINER_H
#define CALODETECTORELEMENTCONTAINER_H

#include "CaloDetDescr/CaloDetDescrElement.h"
#include "DataModel/DataVector.h"

class CaloDetDescrElementContainer : public DataVector<CaloDetDescrElement> 
{
 public:
  CaloDetDescrElementContainer(SG::OwnershipPolicy ownPolicy = SG::VIEW_ELEMENTS) : DataVector<CaloDetDescrElement>(ownPolicy){};
};

CLASS_DEF(CaloDetDescrElementContainer, 1225484576 , 1)
SG_BASE(CaloDetDescrElementContainer, DataVector<CaloDetDescrElement> );

#endif

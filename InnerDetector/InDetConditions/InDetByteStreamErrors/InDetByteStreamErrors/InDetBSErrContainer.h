/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDetByteStreamErrors package
 -----------------------------------------
 ***************************************************************************/


#ifndef INDETBYTESTREAMERRORS_INDETBSERRCONTAINER_H
#define INDETBYTESTREAMERRORS_INDETBSERRCONTAINER_H


#include "Identifier/IdentifierHash.h"
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

typedef DataVector<std::pair<IdentifierHash, int> > InDetBSErrContainer; 

CLASS_DEF(InDetBSErrContainer,1318436494,1)


#endif // INDETBYTESTREAMERRORS_INDETBSERRCONTAINER_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_COMBLINKSCONTAINER_H
#define TRIGGERMENUANALYSIS_COMBLINKSCONTAINER_H
/*
  CombLinksContainer.h
*/
#include "AthContainers/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "TriggerMenuNtuple/CombLinks.h"

typedef DataVector<CombLinks> CombLinksContainer;
//CLASS_DEF( DataVector<CombLinks> , 163738626 , 1 )
CLASS_DEF( CombLinksContainer , 1275088149 , 1 )

#endif // TRIGGERMENUANALYSIS_COMBLINKSCONTAINER_H

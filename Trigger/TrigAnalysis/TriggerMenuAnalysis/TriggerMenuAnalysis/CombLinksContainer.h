/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CombLinksContainer_h__
#define __CombLinksContainer_h__
/*
  CombLinksContainer.h
*/
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "TriggerMenuNtuple/CombLinks.h"

typedef DataVector<CombLinks> CombLinksContainer;
//CLASS_DEF( DataVector<CombLinks> , 163738626 , 1 )
CLASS_DEF( CombLinksContainer , 1275088149 , 1 )

#endif // __CombLinksContainer_h__

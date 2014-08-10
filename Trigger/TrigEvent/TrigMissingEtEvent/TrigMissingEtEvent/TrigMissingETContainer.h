/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMISSINGETEVENT_TRIGMISSINGETCONTAINER_H 
#define TRIGMISSINGETEVENT_TRIGMISSINGETCONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "SGTools/BaseInfo.h"

class TrigMissingETContainer : public DataVector<TrigMissingET> { }; 

CLASS_DEF( TrigMissingETContainer , 1271945976 , 1 )

SG_BASE(TrigMissingETContainer, DataVector<TrigMissingET>);

#endif

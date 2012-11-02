/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTCONDITIONSDATA_FLOATARRAYSTORECONTAINER_H
#define TRTCONDITIONSDATA_FLOATARRAYSTORECONTAINER_H
/** Container for FloatArrayStore objects using the COOL
    CondMultChannCollection */
 
#ifndef CLIDSVC_CLASSDEF_H
#include "CLIDSvc/CLASS_DEF.h"
#endif
#include "AthenaPoolUtilities/CondMultChanCollection.h"
#include "TRT_ConditionsData/FloatArrayStore.h"

typedef CondMultChanCollection<FloatArrayStore> FloatArrayStoreContainer;

CLASS_DEF( FloatArrayStoreContainer ,1073923649, 1 )
 
#endif // TRTCONDITIONSDATA_FLOATARRAYSTORECONTAINER_H

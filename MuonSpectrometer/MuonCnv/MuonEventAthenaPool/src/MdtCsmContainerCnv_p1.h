/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_MDTCSMCONTAINERCNV_P1_H
#define MUONEVENTATHENAPOOL_MDTCSMCONTAINERCNV_P1_H

/*

Transient/Persistent converter for a container of collections of CSC Raw Hits,
@author edward.moyse@cern.ch
*/
#include "MuonRDO/MdtCsmContainer.h"
#include "MuonEventAthenaPool/MdtCsmContainer_p1.h"
#include "MdtCsmCnv_p1.h"

class MdtCsmContainerCnv_p1
    : public T_AthenaPoolTPCnvIDContFromIdentifier< MdtCsmContainer, MdtCsmContainer_p1, MdtCsmCnv_p1>
{
public:
  MdtCsmContainerCnv_p1() {}

};


#endif





/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXCONTAINER_CNV_P1_TRK_H
#define VXCONTAINER_CNV_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TrackCollectionCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "VxVertex/VxContainer.h"
#include "TrkEventTPCnv/VxVertex/VxContainer_p1.h"

#include "TrkEventTPCnv/VxVertex/VxCandidateCnv_p1.h"


class MsgStream;


class VxContainerCnv_p1 : public T_AthenaPoolTPPolyVectorCnv< VxContainer, Trk::VxContainer_p1, ITPConverterFor<Trk::VxCandidate> >

{
public:
  VxContainerCnv_p1() {}

};


#endif //VXCONTAINER_CNV_P1_TRK_H

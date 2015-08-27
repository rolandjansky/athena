/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MVFVXCONTAINER_CNV_P1_TRK_H
#define MVFVXCONTAINER_CNV_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TrackCollectionCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "VxMultiVertex/MVFVxContainer.h"
#include "TrkEventTPCnv/VxMultiVertex/MVFVxContainer_p1.h"

#include "TrkEventTPCnv/VxMultiVertex/MVFVxCandidateCnv_p1.h"


class MsgStream;

class MVFVxContainerCnv_p1
 : public T_AthenaPoolTPPolyVectorCnv< MVFVxContainer, Trk::MVFVxContainer_p1, MVFVxCandidateCnv_p1 >
{
public:
  MVFVxContainerCnv_p1() {}

};


#endif //MVFVXCONTAINER_CNV_P1_TRK_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef V0CONTAINER_CNV_P1_TRK_H
#define V0CONTAINER_CNV_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   V0ContainerCnv_p1.h
//
// March 2006 
//
//  Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkV0Vertex/V0Container.h"
#include "TrkEventTPCnv/TrkV0Vertex/V0Container_p1.h"
//#include "TrkEventTPCnv/TrkV0Vertex/V0CandidateCnv_p1.h"

class MsgStream;

class V0ContainerCnv_p1 : public T_AthenaPoolTPPolyVectorCnv< V0Container, Trk::V0Container_p1, ITPConverterFor<Trk::V0Candidate> >
{

 public:
   V0ContainerCnv_p1() {}

}; //end of class definitions

#endif

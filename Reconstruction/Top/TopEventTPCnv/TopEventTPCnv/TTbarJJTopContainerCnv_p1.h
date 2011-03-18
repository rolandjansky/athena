/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TTBARJJTOPCONTAINERCNV_P1_H
#define TTBARJJTOPCONTAINERCNV_P1_H

#include "TopEvent/TTbarJJTopContainer.h"
#include "TopEventTPCnv/TTbarJJTopCnv_p1.h"
#include "TopEventTPCnv/TTbarJJTopContainer_p1.h"

#include "TopEventTPCnv/VectorCnv_p1.h"


// VectorCnv_p1
typedef VectorCnv_p1<TopRec::TTbarJJTopContainer,TTbarJJTopContainer_p1,TTbarJJTopCnv_p1>  TTbarJJTopContainerCnv_p1;

// T_TPCnv
template <> class T_TPCnv<TopRec::TTbarJJTopContainer,TTbarJJTopContainer_p1> : 
public TTbarJJTopContainerCnv_p1 
{
public:
};

#endif //TTBARJJCONTAINERCNV_P1_H

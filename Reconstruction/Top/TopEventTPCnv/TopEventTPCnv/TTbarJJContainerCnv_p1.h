/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TTBARJJCONTAINERCNV_P1_H
#define TTBARJJCONTAINERCNV_P1_H

#include "TopEvent/TTbarJJContainer.h"
#include "TopEventTPCnv/TTbarJJCnv_p1.h"
#include "TopEventTPCnv/TTbarJJContainer_p1.h"

#include "TopEventTPCnv/VectorCnv_p1.h"

// VectorCnv_p1
typedef VectorCnv_p1<TopRec::TTbarJJContainer,TTbarJJContainer_p1,TTbarJJCnv_p1> TTbarJJContainerCnv_p1;

// T_TPCnv
template <> class T_TPCnv<TopRec::TTbarJJContainer,TTbarJJContainer_p1> : 
public TTbarJJContainerCnv_p1 
{
public:
};

#endif //TTBARJJCONTAINERCNV_P1_H

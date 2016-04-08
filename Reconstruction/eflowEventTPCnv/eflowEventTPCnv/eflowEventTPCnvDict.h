/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWEVENTTPCNVDICT_H
#define EFLOWEVENTTPCNVDICT_H

#include "eflowEventTPCnv/eflowObject_p1.h"
#include "eflowEventTPCnv/eflowObjectContainer_p1.h"

#include "eflowEventTPCnv/eflowObject_p2.h"
#include "eflowEventTPCnv/eflowObjectContainer_p2.h"

#include "eflowEventTPCnv/eflowObject_p3.h"
#include "eflowEventTPCnv/eflowObjectContainer_p3.h"

#include "eflowEventTPCnv/eflowObject_p4.h"
#include "eflowEventTPCnv/eflowObjectContainer_p4.h"

#include "eflowEventTPCnv/eflowObject_p5.h"
#include "eflowEventTPCnv/eflowObjectContainer_p5.h"

namespace eflowEventTPCnv {
  struct tmp {
    std::vector<eflowObject_p1> m_dummyEFlowVectorA;
    std::vector<eflowObject_p2> m_dummyEFlowVectorB;
    std::vector<eflowObject_p3> m_dummyEFlowVectorC;
    std::vector<eflowObject_p4> m_dummyEFlowVectorD;
    std::vector<eflowObject_p5> m_dummyEFlowVectorE;
  };
}
#endif

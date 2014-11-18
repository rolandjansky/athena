/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWEVENTTPCNVDICT_H
#define EFLOWEVENTTPCNVDICT_H

#include "eflowEventTPCnv/eflowObject_p1.h"
#include "eflowEventTPCnv/eflowObjectContainer_p1.h"
#include "eflowEventTPCnv/eflowObjectContainerCnv_p1.h"

#include "eflowEventTPCnv/eflowObject_p2.h"
#include "eflowEventTPCnv/eflowObjectContainer_p2.h"
#include "eflowEventTPCnv/eflowObjectContainerCnv_p2.h"

#include "eflowEventTPCnv/eflowObject_p3.h"
#include "eflowEventTPCnv/eflowObjectContainer_p3.h"
#include "eflowEventTPCnv/eflowObjectContainerCnv_p3.h"

#include "eflowEventTPCnv/eflowObject_p4.h"
#include "eflowEventTPCnv/eflowObjectContainer_p4.h"
#include "eflowEventTPCnv/eflowObjectContainerCnv_p4.h"

#include "eflowEventTPCnv/eflowObject_p5.h"
#include "eflowEventTPCnv/eflowObjectContainer_p5.h"
#include "eflowEventTPCnv/eflowObjectContainerCnv_p5.h"

namespace eflowEventTPCnv {
  struct tmp {
    std::vector<eflowObject_p1> m_dummyEFlowVector;
    T_TPCnv<eflowObjectContainer, eflowObjectContainer_p1> m_dummyEFlowCnv;

    std::vector<eflowObject_p2> m_dummyEFlowVectorB;
    T_TPCnv<eflowObjectContainer, eflowObjectContainer_p2> m_dummyEFlowCnvB;

    std::vector<eflowObject_p3> m_dummyEFlowVectorC;
    T_TPCnv<eflowObjectContainer, eflowObjectContainer_p3> m_dummyEFlowCnvC;

    std::vector<eflowObject_p4> m_dummyEFlowVectorD;
    T_TPCnv<eflowObjectContainer, eflowObjectContainer_p4> m_dummyEFlowCnvD;

    std::vector<eflowObject_p5> m_dummyEFlowVectorE;
    T_TPCnv<eflowObjectContainer, eflowObjectContainer_p5> m_dummyEFlowCnvE;

  };
}
#endif


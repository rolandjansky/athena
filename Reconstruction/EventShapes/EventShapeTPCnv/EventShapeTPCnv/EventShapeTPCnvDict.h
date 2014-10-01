/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPETPCNV_EVENTSHAPETPCNVCNVDICT_H
#define EVENTSHAPETPCNV_EVENTSHAPETPCNVCNVDICT_H

#include "EventShapeTPCnv/EventShapeStore_p1.h"
#include "EventShapeTPCnv/EventShapeStore_p2.h"
#include "EventShapeTPCnv/EventShapeStoreCnv_p1.h"
#include "EventShapeTPCnv/EventShapeStoreCnv_p2.h"

namespace {
  struct GCCXML_DUMMY_INSTANTIATION_EVENTSHAPETPCNV {
    T_TPCnv<EventShapeStore, EventShapeStore_p1> m_esscnv_p1;
    T_TPCnv<EventShapeStore, EventShapeStore_p2> m_esscnv_p2;
  };
}

#endif

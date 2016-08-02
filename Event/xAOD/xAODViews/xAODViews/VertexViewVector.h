/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODVIEWS_VERTEXVIEWVECTOR_H
#define XAODVIEWS_VERTEXVIEWVECTOR_H
#include "xAODTracking/VertexContainer.h"
#include "AthContainers/ViewVector.h"
#include "xAODCore/CLASS_DEF.h"
namespace xAOD {
  typedef ViewVector<VertexContainer> VertexViewVector;
}
VIEWVECTOR_CLASS_DEF(xAOD::VertexViewVector, 115116346)

#endif // XAODVIEWS_VERTEXVIEWVECTOR_H

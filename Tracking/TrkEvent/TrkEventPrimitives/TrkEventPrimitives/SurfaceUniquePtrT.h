// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrkParametersBase/SurfaceUniquePtrT.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2015
 * @brief Handle conditional ownership of surfaces.
 */


#ifndef TRKEVENTPRIMITIVES_SURFACEUNIQUEPTRT_H
#define TRKEVENTPRIMITIVES_SURFACEUNIQUEPTRT_H
#include <memory>

namespace Trk {

template <class S>
class SurfaceDeleter
{
public:
  void operator() (const S* p) { 
    if (p && p->isFree()) { delete p; }
  }
};

template <class S>
using SurfaceUniquePtrT = std::unique_ptr<S, SurfaceDeleter<S> >;

} // namespace Trk

#endif 

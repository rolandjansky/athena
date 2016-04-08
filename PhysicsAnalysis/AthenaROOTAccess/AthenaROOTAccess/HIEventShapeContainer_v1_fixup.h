// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaROOTAccess/HIEventShapeContainer_v1_fixup.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2015
 * @brief Run fixup for HIEventShapeContainer_v1.
 *
 * Persistency for HIEventShapeContainer_v1 is totally FUBAR.
 */


#ifndef ATHENAROOTACCESS_HIEVENTSHAPECONTAINER_V1_FIXUP_H
#define ATHENAROOTACCESS_HIEVENTSHAPECONTAINER_V1_FIXUP_H


#include "AthenaROOTAccess/IARAFixup.h"


namespace AthenaROOTAccess {


class HIEventShapeContainer_v1_fixup
  : public IARAFixup
{
public:
  /// Run fixup on an object that has just been read in, before associating
  /// aux data.
  virtual void fixupBeforeAux (void* data, long long entry) override;
};


} // namespace AthenaROOTAccess


#endif // not ATHENAROOTACCESS_HIEVENTSHAPECONTAINER_V1_FIXUP_H

//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* author: Walter Lampl */
/* Container Class to hold a LArRawRamp Object */
/* date of creation : 26/02/2004 */

#ifndef LARRAWRAMPCONTAINER_H
#define LARRAWRAMPCONTAINER_H


#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "LArRawConditions/LArRawRamp.h"


class LArRawRampContainer : public DataVector<LArRawRamp> {
 public:
  LArRawRampContainer() : DataVector<LArRawRamp> () {};

  virtual ~LArRawRampContainer() {};

};

CLASS_DEF( LArRawRampContainer, 1167083373,1)
#endif


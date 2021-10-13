/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_MUONCALIBPATTERNCOLLECTION_H
#define MUONCALIB_MUONCALIBPATTERNCOLLECTION_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "MuonCalibEventBase/MuonCalibPattern.h"

/**
   @class MuonCalibPatternCollection
   This typedef represents a collection of MuonCalibPattern objects.
   It is a DataVector. It can be saved
   to storegate and persistified using POOL.
*/
typedef DataVector<MuonCalib::MuonCalibPattern> MuonCalibPatternCollection;
CLASS_DEF(MuonCalibPatternCollection, 1196210546, 1)

#endif

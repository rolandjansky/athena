/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MdtTubeCalibContainerCollection.h
//   Header file for class MdtTubeCalibContainerCollection
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_MDTTUBECALIBCONTAINERCOLLECTION_H
#define MUONCALIB_MDTTUBECALIBCONTAINERCOLLECTION_H

#include "AthContainers/DataVector.h"
#include "MdtCalibData/MdtTubeCalibContainer.h"
#include "AthenaKernel/CLASS_DEF.h"

/**
This typedef representing:
- a collection of MdtTubeCalibContainer objects. 
It is a DataVector. It can be saved to storegate.
*/
typedef DataVector<MuonCalib::MdtTubeCalibContainer> MdtTubeCalibContainerCollection;
CLASS_DEF(MdtTubeCalibContainerCollection, 1221928754, 1 )

#endif

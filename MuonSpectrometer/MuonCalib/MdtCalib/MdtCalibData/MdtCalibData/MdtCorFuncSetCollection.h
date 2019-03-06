/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MdtCalibDataCollection.h
//   Header file for class MdtCalibDataCollection
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_MDTCORFUNCSETCOLLECTION_H
#define MUONCALIB_MDTCORFUNCSETCOLLECTION_H

#include "AthContainers/DataVector.h"
#include "MdtCalibData/MdtCorFuncSet.h"
#include "AthenaKernel/CLASS_DEF.h"

/**
This typedef representing:
- a collection of MdtCorFuncSet objects. 
It is a DataVector. It can be saved to storegate.
*/
typedef DataVector<MuonCalib::MdtCorFuncSet> MdtCorFuncSetCollection;
CLASS_DEF(MdtCorFuncSetCollection , 1176811704 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( MdtCorFuncSetCollection, 1338477494 );

#endif

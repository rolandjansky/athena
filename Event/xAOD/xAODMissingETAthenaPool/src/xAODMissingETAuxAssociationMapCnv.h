// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMissingETAuxAssociationMapCnv.h 692089 2015-08-30 14:36:16Z khoo $
#ifndef XAODMISSINGETATHENAPOOL_XAODMISSINGETAUXASSOCIATIONMAPCNV_H
#define XAODMISSINGETATHENAPOOL_XAODMISSINGETAUXASSOCIATIONMAPCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODMissingET/MissingETAuxAssociationMap.h"
#include "xAODMissingETAuxAssociationMapCnv_v1.h"


/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::MissingETAuxAssociationMap,
                                     xAODMissingETAuxAssociationMapCnv_v1 >
   xAODMissingETAuxAssociationMapCnv;


#endif

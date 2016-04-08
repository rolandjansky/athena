/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Gaudi/Athena include(s):
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s):
#include "xAODMissingET/MissingETAuxAssociationMap.h"
#include "xAODMissingET/versions/MissingETAuxAssociationMap_v1.h"

// Local include(s):
#include "xAODMissingETAuxAssociationMapCnv_v1.h"

// Declare the T/P converter(s):
DECLARE_TPCNV_FACTORY( xAODMissingETAuxAssociationMapCnv_v1,
                       xAOD::MissingETAuxAssociationMap,
                       xAOD::MissingETAuxAssociationMap_v1,
                       Athena::TPCnvVers::Old )

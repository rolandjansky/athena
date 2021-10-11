/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/TPCnvFactory.h"
#include "DetDescrCondTPCnv/DetCondCFloatCnv_p1.h"

DECLARE_TPCNV_FACTORY(DetCondCFloatCnv_p1,
                      DetCondCFloat,
                      DetCondCFloat_p1,
                      Athena::TPCnvVers::Current)

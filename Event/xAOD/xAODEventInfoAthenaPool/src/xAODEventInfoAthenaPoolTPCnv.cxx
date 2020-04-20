// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

// Gaudi/Athena include(s).
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s).
#include "xAODEventInfo/EventAuxInfo.h"
#include "xAODEventInfo/versions/EventAuxInfo_v1.h"

// Local include(s).
#include "xAODEventAuxInfoCnv_v1.h"

// Declare the T/P converter(s).
DECLARE_TPCNV_FACTORY( xAODEventAuxInfoCnv_v1,
                       xAOD::EventAuxInfo,
                       xAOD::EventAuxInfo_v1,
                       Athena::TPCnvVers::Old )

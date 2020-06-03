// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

// Gaudi/Athena include(s).
#include "AthenaKernel/TPCnvFactory.h"

// EDM include(s).
#include "xAODEventInfo/EventAuxInfo.h"
#include "xAODEventInfo/versions/EventAuxInfo_v2.h"

// Local include(s).
#include "xAODEventAuxInfoCnv_v2.h"

// Declare the T/P converter(s).
DECLARE_TPCNV_FACTORY( xAODEventAuxInfoCnv_v2,
                       xAOD::EventAuxInfo,
                       xAOD::EventAuxInfo_v2,
                       Athena::TPCnvVers::Old )

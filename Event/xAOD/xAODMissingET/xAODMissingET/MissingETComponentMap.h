// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMISSINGET_MISSINGETCOMPONENTMAP_H
#define XAODMISSINGET_MISSINGETCOMPONENTMAP_H

#include "xAODMissingET/MissingETComponent.h"
#include "xAODMissingET/versions/MissingETComponentMap_v1.h"

namespace xAOD
{
  /*! @brief Version control by type definition */
  typedef MissingETComponentMap_v1 MissingETComponentMap;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF(xAOD::MissingETComponentMap,36625032,1 )

#endif

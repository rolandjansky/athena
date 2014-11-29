// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMISSINGET_MISSINGETASSOCIATIONMAP_H
#define XAODMISSINGET_MISSINGETASSOCIATIONMAP_H

#include "xAODMissingET/MissingETAssociation.h"
#include "xAODMissingET/versions/MissingETAssociationMap_v1.h"

namespace xAOD
{
  /*! @brief Version control by type defintion */
  typedef MissingETAssociationMap_v1 MissingETAssociationMap;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::MissingETAssociationMap, 135890658, 1 )

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 619407 2014-10-01 15:55:09Z khoo $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODMissingET/versions/MissingETContainer_v1.h"
#include "xAODMissingET/versions/MissingETComponentMap_v1.h"
#include "xAODMissingET/versions/MissingETAssociationMap_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, MissingETContainer_v1 );
ADD_NS_DV_PROXY( xAOD, MissingETComponentMap_v1 );
ADD_NS_DV_PROXY( xAOD, MissingETAssociationMap_v1 );

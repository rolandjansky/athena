/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 575944 2013-12-17 15:07:13Z christos $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODEgamma/versions/ElectronContainer_v1.h"
#include "xAODEgamma/versions/PhotonContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, ElectronContainer_v1 );
ADD_NS_DV_PROXY( xAOD, PhotonContainer_v1 );

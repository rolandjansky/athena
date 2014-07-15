// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigEgammaDict.h 602825 2014-06-20 07:29:53Z krasznaa $
#ifndef XAODTRIGEGAMMA_XAODTRIGEGAMMADICT_H
#define XAODTRIGEGAMMA_XAODTRIGEGAMMADICT_H

// STL include(s):
#include <vector>
	
// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

//Local include(s)
#include "xAODTrigEgamma/versions/TrigPhoton_v1.h"
#include "xAODTrigEgamma/versions/TrigPhotonContainer_v1.h"
#include "xAODTrigEgamma/versions/TrigPhotonAuxContainer_v1.h"

#include "xAODTrigEgamma/versions/TrigElectron_v1.h"
#include "xAODTrigEgamma/versions/TrigElectronContainer_v1.h"
#include "xAODTrigEgamma/versions/TrigElectronAuxContainer_v1.h"

namespace{
   struct GCCXML_DUMMY_INSTANTIATION_XAODTRIGEGAMMA {
	
      xAOD::TrigElectronContainer_v1 electron_c1;
      DataLink< xAOD::TrigElectronContainer_v1 > electron_dl1;
      std::vector< DataLink< xAOD::TrigElectronContainer_v1 > > electron_dl2;
      ElementLink< xAOD::TrigElectronContainer_v1 > electron_el1;
      std::vector< ElementLink< xAOD::TrigElectronContainer_v1 > > electron_el2;
      std::vector< std::vector< ElementLink< xAOD::TrigElectronContainer_v1 > > > electron_el3;

      xAOD::TrigPhotonContainer_v1 photon_c1;
      DataLink< xAOD::TrigPhotonContainer_v1 > photon_dl1;
      std::vector< DataLink< xAOD::TrigPhotonContainer_v1 > > photon_dl2;
      ElementLink< xAOD::TrigPhotonContainer_v1 > photon_el1;
      std::vector< ElementLink< xAOD::TrigPhotonContainer_v1 > > photon_el2;
      std::vector< std::vector< ElementLink< xAOD::TrigPhotonContainer_v1 > > > photon_el3;

   };
} // private namespace

#endif // XAODTRIGEGAMMA_XAODTRIGEGAMMADICT_H

// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigMuonDict.h 698126 2015-10-02 12:59:53Z mishitsu $
#ifndef XAODTRIGMUON_XAODTRIGMUONDICT_H
#define XAODTRIGMUON_XAODTRIGMUONDICT_H

// Needed to successfully generate the dictionary in standalone mode:
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#   define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__

// STL include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/L2IsoMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/versions/L2StandAloneMuon_v1.h"
#include "xAODTrigMuon/versions/L2StandAloneMuonContainer_v1.h"
#include "xAODTrigMuon/versions/L2StandAloneMuonAuxContainer_v1.h"
#include "xAODTrigMuon/versions/L2StandAloneMuon_v2.h"
#include "xAODTrigMuon/versions/L2StandAloneMuonContainer_v2.h"
#include "xAODTrigMuon/versions/L2StandAloneMuonAuxContainer_v2.h"
#include "xAODTrigMuon/versions/L2IsoMuon_v1.h"
#include "xAODTrigMuon/versions/L2IsoMuonContainer_v1.h"
#include "xAODTrigMuon/versions/L2IsoMuonAuxContainer_v1.h"
#include "xAODTrigMuon/versions/L2CombinedMuon_v1.h"
#include "xAODTrigMuon/versions/L2CombinedMuonContainer_v1.h"
#include "xAODTrigMuon/versions/L2CombinedMuonAuxContainer_v1.h"

namespace{
   struct GCCXML_DUMMY_INSTANTIATION_XAODTRIGMUON {

      xAOD::L2StandAloneMuonContainer_v1 muonsa_c1;
      DataLink< xAOD::L2StandAloneMuonContainer_v1 > muonsa_dl1;
      std::vector< DataLink< xAOD::L2StandAloneMuonContainer_v1 > > muonsa_dl2;
      ElementLink< xAOD::L2StandAloneMuonContainer_v1 > muonsa_el1;
      std::vector< ElementLink< xAOD::L2StandAloneMuonContainer_v1 > > muonsa_el2;
      std::vector< std::vector< ElementLink< xAOD::L2StandAloneMuonContainer_v1 > > > muonsa_el3;

      xAOD::L2StandAloneMuonContainer_v2 muonsa_c2;
      DataLink< xAOD::L2StandAloneMuonContainer_v2 > muonsa_dl4;
      std::vector< DataLink< xAOD::L2StandAloneMuonContainer_v2 > > muonsa_dl5;
      ElementLink< xAOD::L2StandAloneMuonContainer_v2 > muonsa_el4;
      std::vector< ElementLink< xAOD::L2StandAloneMuonContainer_v2 > > muonsa_el5;
      std::vector< std::vector< ElementLink< xAOD::L2StandAloneMuonContainer_v2 > > > muonsa_el6;

      xAOD::L2IsoMuonContainer_v1 muoniso_c1;
      DataLink< xAOD::L2IsoMuonContainer_v1 > muoniso_dl1;
      std::vector< DataLink< xAOD::L2IsoMuonContainer_v1 > > muoniso_dl2;
      ElementLink< xAOD::L2IsoMuonContainer_v1 > muoniso_el1;
      std::vector< ElementLink< xAOD::L2IsoMuonContainer_v1 > > muoniso_el2;
      std::vector< std::vector< ElementLink< xAOD::L2IsoMuonContainer_v1 > > > muoniso_el3;

      xAOD::L2CombinedMuonContainer_v1 muoncb_c1;
      DataLink< xAOD::L2CombinedMuonContainer_v1 > muoncb_dl1;
      std::vector< DataLink< xAOD::L2CombinedMuonContainer_v1 > > muoncb_dl2;
      ElementLink< xAOD::L2CombinedMuonContainer_v1 > muoncb_el1;
      std::vector< ElementLink< xAOD::L2CombinedMuonContainer_v1 > > muoncb_el2;
      std::vector< std::vector< ElementLink< xAOD::L2CombinedMuonContainer_v1 > > > muoncb_el3;

   };
}

#endif // XAODTRIGMUON_XAODTRIGMUONDICT_H

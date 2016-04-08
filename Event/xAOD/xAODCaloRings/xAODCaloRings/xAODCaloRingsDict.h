/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloRingsDict.h 707323 2015-11-12 02:45:01Z wsfreund $ 
#ifndef XAODCALORINGS_XAODCALORINGSDICT_H
#define XAODCALORINGS_XAODCALORINGSDICT_H

// Needed to successfully generate the dictionary in standalone mode:
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#  define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__

// STL include(s):
#include <vector>
#include <memory>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"

// Local include(s):

#include "xAODCaloRings/tools/AtlasGeoPoint.h"

#include "xAODCaloRings/RingSetContainer.h"
#include "xAODCaloRings/versions/RingSetContainer_v1.h"
#include "xAODCaloRings/versions/RingSetAuxContainer_v1.h"

#include "xAODCaloRings/RingSetConfContainer.h"
#include "xAODCaloRings/versions/RingSetConfContainer_v1.h"
#include "xAODCaloRings/versions/RingSetConfAuxContainer_v1.h"

#include "xAODCaloRings/CaloRingsContainer.h"
#include "xAODCaloRings/versions/CaloRingsContainer_v1.h"
#include "xAODCaloRings/versions/CaloRingsAuxContainer_v1.h"

#include "xAODCaloRings/CaloRingsDefs.h"

namespace {

struct GCCXML_DUMMY_INSTANTIATION_XAODCALORINGS {

  DataLink< xAOD::RingSetContainer_v1 >                                             ringset_v1_l1;
  ElementLink< xAOD::RingSetContainer_v1 >                                          ringset_v1_l2;
  ElementLinkVector< xAOD::RingSetContainer_v1 >                                    ringset_v1_l3;
  std::vector< DataLink< xAOD::RingSetContainer_v1 > >                              ringset_v1_l4;
  std::vector< ElementLink< xAOD::RingSetContainer_v1 > >                           ringset_v1_l5;
  std::vector< ElementLinkVector< xAOD::RingSetContainer_v1 > >                     ringset_v1_l6;
  std::vector< std::vector< ElementLink< DataVector< xAOD::RingSet_v1 > > > >       ringset_v1_l7;

  DataLink< xAOD::RingSetConfContainer_v1 >                                         ringsetconf_v1_l1;
  ElementLink< xAOD::RingSetConfContainer_v1 >                                      ringsetconf_v1_l2;
  ElementLinkVector< xAOD::RingSetConfContainer_v1 >                                ringsetconf_v1_l3;
  std::vector< DataLink< xAOD::RingSetConfContainer_v1 > >                          ringsetconf_v1_l4;
  std::vector< ElementLink< xAOD::RingSetConfContainer_v1 > >                       ringsetconf_v1_l5;
  std::vector< ElementLinkVector< xAOD::RingSetConfContainer_v1 > >                 ringsetconf_v1_l6;
  std::vector< std::vector< ElementLink< DataVector< xAOD::RingSetConf_v1 > > > >   ringsetconf_v1_l7;

  DataLink< xAOD::CaloRingsContainer_v1 >                                           clrings_v1_l1;
  ElementLink< xAOD::CaloRingsContainer_v1 >                                        clrings_v1_l2;
  ElementLinkVector< xAOD::CaloRingsContainer_v1 >                                  clrings_v1_l3;
  std::vector< DataLink< xAOD::CaloRingsContainer_v1 > >                            clrings_v1_l4;
  std::vector< ElementLink< xAOD::CaloRingsContainer_v1 > >                         clrings_v1_l5;
  std::vector< ElementLinkVector< xAOD::CaloRingsContainer_v1 > >                   clrings_v1_l6;
  std::vector< std::vector< ElementLink< DataVector< xAOD::CaloRings_v1 > > > >     clrings_v1_l7;

  // Instantiate sample used by xAOD::RingSet, so that
  // Reflex would see them with their "correct type". Note that the
  // dictionary for these types comes from other places. This instantiation
  // is just needed for "Reflex related technical reasons"...
  std::vector< CaloSampling::CaloSample > sample_vec;

};

} // private namespace

#endif // XAODCALORINGS_XAODCALORINGSDICT_H

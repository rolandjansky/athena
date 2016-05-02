// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODCUTFLOW_XAODCUTFLOWDICT_H
#define XAODCUTFLOW_XAODCUTFLOWDICT_H


// STL
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"

// Local include(s):
#include "xAODCutFlow/CutBookkeeperContainer.h"
#include "xAODCutFlow/CutBookkeeperAuxContainer.h"
#include "xAODCutFlow/xAODCutFlowHelpers.h"



namespace {
  struct GCCXML_DUMMY_INSTANTIATION_XAODCUTFLOW {
    xAOD::CutBookkeeperContainer_v1                                              ebkl_c1;
    DataLink< xAOD::CutBookkeeperContainer_v1 >                                  ebkl_l1;
    ElementLink< xAOD::CutBookkeeperContainer_v1 >                               ebkl_l2;
    ElementLinkVector< xAOD::CutBookkeeperContainer_v1 >                         ebkl_l3;
    std::vector< DataLink< xAOD::CutBookkeeperContainer_v1 > >                   ebkl_l4;
    std::vector< ElementLink< xAOD::CutBookkeeperContainer_v1 > >                ebkl_l5;
    std::vector< ElementLinkVector< xAOD::CutBookkeeperContainer_v1 > >          ebkl_l6;
    std::vector< std::vector< ElementLink< xAOD::CutBookkeeperContainer_v1 > > > ebkl_l7;

  };
}

#endif // XAODCUTFLOW_XAODCUTFLOWDICT_H

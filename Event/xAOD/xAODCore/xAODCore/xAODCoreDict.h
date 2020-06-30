// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCORE_XAODCOREDICT_H
#define XAODCORE_XAODCOREDICT_H

// System include(s):
#include <vector>
extern "C" {
#   include <stdint.h>
}

// ROOT include(s):
#include <Rtypes.h>
#include <Math/Vector4D.h>

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODCore/AuxContainerBase.h"
#include "xAODCore/AuxInfoBase.h"
#include "xAODCore/ShallowAuxContainer.h"
#include "xAODCore/ShallowAuxInfo.h"
#include "xAODCore/tools/TDVCollectionProxy.h"
#include "xAODCore/tools/PrintHelpers.h"

namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODCORE {
      DataVector< xAOD::TDVCollectionProxyDummy > dummy1;
      // Instantiations necessary for the std::vector<ROOT::Math::LorentzVector>
      // dictionary:
      ROOT::Math::PtEtaPhiM4D< Double_t > root1;
      ROOT::Math::LorentzVector< ROOT::Math::PtEtaPhiM4D< Double_t > > root2;
      std::vector< ROOT::Math::LorentzVector< ROOT::Math::PtEtaPhiM4D< Double_t > > > root3;
      // Additional ROOT-only dictionaries:
      std::vector< unsigned char > root4;
      std::vector< std::vector< unsigned char > > root5;
      std::vector<signed char> root6;
      std::vector< std::vector<signed char> > root7;
      std::vector< std::vector<char> > root8;
      std::vector< uint32_t > root9;
      std::vector< std::vector< uint32_t > > root10;
   };
} // private namespace

#endif // XAODCORE_XAODCOREDICT_H

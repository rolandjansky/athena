// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCoreAthenaDict.h 591472 2014-04-05 11:23:29Z krasznaa $
#ifndef XAODCORE_XAODCOREATHENADICT_H
#define XAODCORE_XAODCOREATHENADICT_H

// These are classes for which we need Reflex dictionaries just in Athena.

// ROOT include(s):
#include <Rtypes.h>
#include <Math/Vector4D.h>

#ifdef __GCCXML__
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODCORE2 {
      ROOT::Math::PtEtaPhiM4D< Double_t > dummy1;
      ROOT::Math::LorentzVector< ROOT::Math::PtEtaPhiM4D< Double_t > > dummy2;
   };
}
#endif // __GCCXML__
#endif // XAODCORE_XAODCOREATHENADICT_H

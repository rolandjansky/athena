// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DiTauDefs.h 631921 2015-09-23 23:30:59Z dkirchme $
#ifndef XAODDITAU_DITAUDEFS_H
#define XAODDITAU_DITAUDEFS_H

// Local include(s):
#include "Math/Vector4D.h"
#include <bitset>

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
  typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > PtEtaPhiMVectorF;

    template <class T>
    struct xAODDiTAU_return_type;

    // specialization for ints
    template <>
    struct xAODDiTAU_return_type<int> { typedef int type; };

    // specialization for floats
    template <>
    struct xAODDiTAU_return_type<float> { typedef float type; };


namespace DiTauJetParameters
{

    //-------------------------------------------------------------------------
    // Enum for ditau parameters 
    //-------------------------------------------------------------------------
    enum Detail
    {
        TauJetVtxFraction,

        R_jet,
        R_subjet,
        R_core,
        BDT

    };



}//end namespace DiTauJetParameters

}

#endif // XAODDITAU_TAUDEFS_H

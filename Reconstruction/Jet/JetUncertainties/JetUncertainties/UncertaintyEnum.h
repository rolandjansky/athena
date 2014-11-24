/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_UNCERTAINTYENUM_H
#define JETUNCERTAINTIES_UNCERTAINTYENUM_H

//#include "xAODJet/Jet.h"

#include "TString.h"

namespace jet
{

namespace CompCategory
{
    enum TypeEnum
    {
        UNKNOWN=0,  // Failure/unset/etc
        Statistical,// Statistical and method uncertainties
        Detector,   // Detector uncertainties
        Modelling,  // Modelling uncertainties
        Mixed,      // Mixed detector/modelling uncertainties
        Effective,  // Effective nuisance parameters (category is nonsensical after global reduction)
        Other       // Other uncertainties that don't fit in the above (MC nonclosure, ...)
    };
    TString EnumToString(const TypeEnum type);
    TypeEnum StringToEnum(const TString type);
}

namespace CompCorrelation
{
    enum TypeEnum
    {
        UNKNOWN=0,      // Failure/unset/etc
        Uncorrelated,   // Uncorrelated systematic - add in quadrature
        Correlated      // Correlated systematic - add linearly
    };
    TString EnumToString(const TypeEnum type);
    TypeEnum StringToEnum(const TString type);
}


namespace CompParametrization
{
    enum TypeEnum
    {
        UNKNOWN=0,      // Failure/unset/etc
        Pt,             // 1D, (pT) dependence
        PtEta,          // 2D, (pT,eta) dependence
        PtAbsEta,       // 2D, (pT,|eta|) dependence
        PtMassEta,      // 3D, (pT,m/pT,eta) dependence
        PtMassAbsEta    // 3D, (pT,m/pT,|eta|) dependence
    };
    TString EnumToString(const TypeEnum type);
    TypeEnum StringToEnum(const TString type);
    bool IsAbsEta(const TypeEnum type);
}

namespace CompScaleVar
{
    enum TypeEnum
    {
        UNKNOWN=0,      // Failure/unset/etc
        FourVec,        // The full jet 4-vector
        Pt,             // Just the jet pT
        Mass,           // Just the jet mass
        D12,            // Just the 1,2 splitting distance
        D23,            // Just the 2,3 splitting distance
        Tau21,          // Just the ratio on n-subjettiness 2/1
        Tau32           // Just the ratio on n-subjettiness 3/2
    };
    TString EnumToString(const TypeEnum type);
    TypeEnum StringToEnum(const TString type);
}

//namespace CompVariable
//{
//    enum TypeEnum
//    {
//        UNKNOWN=0,  // Failure/unset/etc
//        pt,         // calibrated pT
//        eta,        // calibrated eta
//        absEta,     // calibrated |eta|
//        mOverPt     // calibrated m/pT
//    };
//    TString EnumToString(const TypeEnum type);
//    TypeEnum StringToEnum(const TString type);
//    TString EnumToFormattedString(const TypeEnum type);
//    float EnumToValue(const TypeEnum type, const xAOD::Jet& jet);
//}

namespace PileupComp
{
    enum TypeEnum
    {
        UNKNOWN=0,  // Failure/unset/etc
        OffsetNPV,  // NPV based pileup offset
        OffsetMu,   // mu based pileup offset
        PtTerm,     // pT based systematic (both NPV and mu dependence)
        RhoTopology // Rho topology systematic (no NPV or mu dependence, but pileup anyway)
    };
    TString EnumToString(const TypeEnum type);
    TypeEnum StringToEnum(const TString type);
}

namespace FlavourComp
{
    enum TypeEnum
    {
        UNKNOWN=0,      // Failure/unset/etc
        Response,       // Flavour response
        Composition,    // Flavour compositon
        bJES            // bJES response
    };
    TString EnumToString(const TypeEnum type);
    TypeEnum StringToEnum(const TString type);
}

} // end jet namespace

#endif

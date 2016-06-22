/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_UNCERTAINTYENUM_H
#define JETUNCERTAINTIES_UNCERTAINTYENUM_H

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
    TString enumToString(const TypeEnum type);
    TypeEnum stringToEnum(const TString type);
}

namespace CompCorrelation
{
    enum TypeEnum
    {
        UNKNOWN=0,      // Failure/unset/etc
        Uncorrelated,   // Uncorrelated systematic - add in quadrature
        Correlated      // Correlated systematic - add linearly
    };
    TString enumToString(const TypeEnum type);
    TypeEnum stringToEnum(const TString type);
}


namespace CompParametrization
{
    enum TypeEnum
    {
        UNKNOWN=0,      // Failure/unset/etc
        Pt,             // 1D, (pT) dependence
        PtEta,          // 2D, (pT,eta) dependence
        PtAbsEta,       // 2D, (pT,|eta|) dependence
        PtMass,         // 2D, (pt,m/pT) dependence
        PtMassEta,      // 3D, (pT,m/pT,eta) dependence
        PtMassAbsEta    // 3D, (pT,m/pT,|eta|) dependence
    };

    TString enumToString(const TypeEnum type);
    TypeEnum stringToEnum(const TString type);
    bool isAbsEta(const TypeEnum type);
}

namespace CompMassDef
{
    enum TypeEnum
    {
        UNKNOWN=0,      // Failure/unset/etc
        CaloMass,       // Calorimeter jet mass
        TAMass,         // Track-assisted jet mass = (mTrack/pTtrack) * pTcalo
        CombMassQCD,    // Combined jet mass = mCalo*wCalo + mTA*wTA, QCD weights
        CombMassWZ,     // Combined jet mass = mCalo*wCalo + mTA*wTA, W/Z weights
        CombMassHbb,    // Combined jet mass = mCalo*wCalo + mTA*wTA, Hbb weights
        CombMassTop     // Combined jet mass = mCalo*wCalo + mTA*wTA, Top weights
    };

    TString enumToString(const TypeEnum type);
    TypeEnum stringToEnum(const TString type);
    TString getJetScaleString(const TypeEnum type);

}

namespace CompScaleVar
{
    enum TypeEnum
    {
        UNKNOWN=0,      // Failure/unset/etc
        FourVec,        // The full jet 4-vector
        Pt,             // The jet pT
        Mass,           // The jet mass (the default four-vector mass)
        D12,            // The 1,2 splitting distance
        D23,            // The 2,3 splitting distance
        Tau21,          // The ratio on n-subjettiness 2/1
        Tau32,          // The ratio on n-subjettiness 3/2
        Tau32WTA,       // The ratio on n-subjettiness 3/2 with WTA axis
        D2Beta1,        // The value of D2^{beta=1} (ECF ratio)
        
        MassRes         // The jet mass resolution
    };
    TString enumToString(const TypeEnum type);
    TypeEnum stringToEnum(const TString type);
}

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
    TString enumToString(const TypeEnum type);
    TypeEnum stringToEnum(const TString type);
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
    TString enumToString(const TypeEnum type);
    TypeEnum stringToEnum(const TString type);
}

namespace CombMassComp
{
    enum TypeEnum
    {
        UNKNOWN=0,      // Failure/unset/setc
        Calo,           // Calorimeter-only uncertainty
        TA,             // Track-assisted-only uncertainty
        Both            // Coherent calo and TA uncertainties
    };
    TString enumToString(const TypeEnum type);
    TypeEnum stringToEnum(const TString type);
}

namespace JetTopology
{
    enum TypeEnum
    {
        UNKNOWN=0,      // Failure/unset/etc
        QCD,            // QCD jet topology
        WZ,             // W/Z jet topology
        Hbb,            // Hbb jet topology
        Top             // Top jet topology
    };
    TString enumToString(const TypeEnum type);
    TypeEnum stringToEnum(const TString type);
}

} // end jet namespace

#endif

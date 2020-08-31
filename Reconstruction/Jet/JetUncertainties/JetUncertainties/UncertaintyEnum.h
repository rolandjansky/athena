/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
        PtMassAbsEta,   // 3D, (pT,m/pT,|eta|) dependence
        eLOGmOe,        // 2D, (E,log(m/E)) dependence
        eLOGmOeEta,     // 3D, (E,log(m/E),eta) dependence
        eLOGmOeAbsEta,  // 3D, (E,log(m/E),|eta|) dependence
        PtLOGPtMassForTagSF,// 2D, (pt,log(m/pT)) dependence, only for tagging SF
    };

    TString enumToString(const TypeEnum type);
    TypeEnum stringToEnum(const TString type);
    bool isAbsEta(const TypeEnum type);
    bool includesMass(const TypeEnum type);
}

namespace CompMassDef
{
    enum TypeEnum
    {
        UNKNOWN=0,      // Failure/unset/etc
        FourVecMass,    // Mass directly from the four-vector
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
        // Error state
        UNKNOWN=0,      // Failure/unset/etc
        
        // Scale uncertainties
        FourVec,        // The full jet 4-vector
        Pt,             // The jet pT
        Mass,           // The jet mass (the default four-vector mass)
        D12,            // The 1,2 splitting distance
        D23,            // The 2,3 splitting distance
        Tau21,          // The ratio on n-subjettiness 2/1
        Tau32,          // The ratio on n-subjettiness 3/2
        Tau21WTA,       // The ratio on n-subjettiness 2/1 with WTA axis
        Tau32WTA,       // The ratio on n-subjettiness 3/2 with WTA axis
        D2Beta1,        // The value of D_2^{beta=1} (ECF ratio)
        C2Beta1,        // The value of C_2^{beta=1} (ECF ratio)
        Qw,             // The value of Qw
        TagScaleFactor, // Tagging efficiency SF
        
        // Resolution uncertainties
        MassRes,        // The jet mass resolution, relative
        MassResAbs,     // The jet mass resolution, absolute
        PtRes,          // The jet pT resolution, relative
        PtResAbs,       // The jet pT resolution, absolute
        FourVecRes,     // The jet energy resolution applied to the full four-vector, relative
        FourVecResAbs   // The jet energy resolution applied to the full four-vector, absolute
    };
    TString enumToString(const TypeEnum type);
    TypeEnum stringToEnum(const TString type);

    bool isScaleType(const TypeEnum type);
    bool isResolutionType(const TypeEnum type);
    bool isAbsResolutionType(const TypeEnum type);
    bool isRelResolutionType(const TypeEnum type);
}

namespace CompFlavorLabelVar
{
    enum TypeEnum // used to identify the jet flavor relevant for this NP
    {
        UNKNOWN=0,  // error state
        t_qqb,      // full-contained top
        t,          // inclusive top
        V_qq,       // W/Z->qq
        W_qq,       // W->qq
        Z_qq,       // Z->qq
	W_qq_From_t,// t->W->qq
	t_other,    // inclusive top but not contain t_qqb
	q           // background jet
    };
    TString enumToString(const TypeEnum type);
    TypeEnum stringToEnum(const TString type);
}

namespace CompTaggerRegionVar
{
    enum TypeEnum // used to identify the region relevant for this NP
    {
        UNKNOWN=0,  // error state
        passMpassD2_2Var,// passing both mass and D2 cuts
        passMfailD2_2Var,// passing mass cut but failing D2
        failMpassD2_2Var,// failing mass cut but passing D2
	failMfailD2_2Var // failing both mass and D2 cuts
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
        bJES,           // bJES response
        PerJetResponse  // Per-jet flavour response
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
        Top,            // Top jet topology
        MIXED           // Mixed topology (multiple of the above)
    };
    TString enumToString(const TypeEnum type);
    TypeEnum stringToEnum(const TString type);
}

namespace ExtendedBool
{
    enum TypeEnum
    {
        UNSET = -1,
        FALSE = 0,
        TRUE  = 1
    };
}

namespace Interpolate
{
    enum TypeEnum
    {
        UNKNOWN=0,      // Failure/unset/etc
        None,           // No interpolation
        Full,           // Full interpolation
        OnlyX,          // Interpolate only in the x-direction (hold y,z fixed)
        OnlyY           // Interpolate only in the y-direction (hold x,z fixed)
    };
    TString enumToString(const TypeEnum type);
    TypeEnum stringToEnum(const TString type);
}

} // end jet namespace

#endif

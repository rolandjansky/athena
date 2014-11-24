/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetUncertainties/UncertaintyEnum.h"

#include <math.h>

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Component category enumeration methods      //
//                                              //
//////////////////////////////////////////////////

namespace CompCategory
{
    TString EnumToString(const TypeEnum type)
    {
        switch (type)
        {
            case Statistical:   return "Statistical";
            case Detector:      return "Detector";
            case Modelling:     return "Modelling";
            case Mixed:         return "Mixed";
            case Effective:     return "Effective";
            case Other:         return "Other";
            default:            return "UNKNOWN";
        }
    }
    
    TypeEnum StringToEnum(const TString type)
    {
        if (!type.CompareTo("Statistical"))
            return Statistical;
        if (!type.CompareTo("Detector"))
            return Detector;
        if (!type.CompareTo("Modelling"))
            return Modelling;
        if (!type.CompareTo("Mixed"))
            return Mixed;
        if (!type.CompareTo("Effective"))
            return Effective;
        if (!type.CompareTo("Other"))
            return Other;
        return UNKNOWN;
    }
} // end CompCategory namespace


//////////////////////////////////////////////////
//                                              //
//  Component correlation enumeration methods   //
//                                              //
//////////////////////////////////////////////////

namespace CompCorrelation
{
    TString EnumToString(const TypeEnum type)
    {
        switch (type)
        {
            case Uncorrelated:  return "Uncorrelated";
            case Correlated:    return "Correlated";
            default:            return "UNKNOWN";
        }
    }

    TypeEnum StringToEnum(const TString type)
    {
        if (!type.CompareTo("Uncorrelated"))
            return Uncorrelated;
        if (!type.CompareTo("Correlated"))
            return Correlated;
        return UNKNOWN;
    }
} // end CompCorrelation namespace


//////////////////////////////////////////////////
//                                              //
//  Component parametrization type enum methods //
//                                              //
//////////////////////////////////////////////////

namespace CompParametrization
{
    TString EnumToString(const TypeEnum type)
    {
        switch(type)
        {
            case Pt:            return "Pt";
            case PtEta:         return "PtEta";
            case PtAbsEta:      return "PtAbsEta";
            case PtMassEta:     return "PtMassEta";
            case PtMassAbsEta:  return "PtMassAbsEta";
            default:            return "UNKNOWN";
        }
    }

    TypeEnum StringToEnum(const TString type)
    {
        if (!type.CompareTo("Pt",TString::kIgnoreCase))
            return Pt;
        if (!type.CompareTo("PtEta",TString::kIgnoreCase))
            return PtEta;
        if (!type.CompareTo("PtAbsEta",TString::kIgnoreCase))
            return PtAbsEta;
        if (!type.CompareTo("PtMassEta",TString::kIgnoreCase))
            return PtMassEta;
        if (!type.CompareTo("PtMassAbsEta",TString::kIgnoreCase))
            return PtMassAbsEta;
        return UNKNOWN;
    }

    bool IsAbsEta(const TypeEnum type)
    {
        switch(type)
        {
            case PtAbsEta:
            case PtMassAbsEta:  return true;
            
            default:            return false;
        }
    }
} // end CompParametrization namespace


//////////////////////////////////////////////////
//                                              //
//  Component parametrization type enum methods //
//                                              //
//////////////////////////////////////////////////

namespace CompScaleVar
{
    TString EnumToString(const TypeEnum type)
    {
        switch(type)
        {
            case FourVec:       return "FourVec";
            case Pt:            return "Pt";
            case Mass:          return "Mass";
            case D12:           return "D12";
            case D23:           return "D23";
            case Tau21:         return "Tau21";
            case Tau32:         return "Tau32";
            default:            return "UNKNOWN";
        }
    }

    TypeEnum StringToEnum(const TString type)
    {
        if (!type.CompareTo("FourVec",TString::kIgnoreCase) || !type.CompareTo("4Vec",TString::kIgnoreCase) || !type.CompareTo("FourVector",TString::kIgnoreCase))
            return FourVec;
        if (!type.CompareTo("Pt",TString::kIgnoreCase))
            return Pt;
        if (!type.CompareTo("Mass",TString::kIgnoreCase))
            return Mass;
        if (!type.CompareTo("D12",TString::kIgnoreCase))
            return D12;
        if (!type.CompareTo("D23",TString::kIgnoreCase))
            return D23;
        if (!type.CompareTo("Tau21",TString::kIgnoreCase))
            return Tau21;
        if (!type.CompareTo("Tau32",TString::kIgnoreCase))
            return Tau32;
        return UNKNOWN;
    }
}


//////////////////////////////////////////////////
//                                              //
//  Component variable type enum methods        //
//                                              //
//////////////////////////////////////////////////

//namespace CompVariable
//{
//    TString EnumToString(const TypeEnum type)
//    {
//        switch (type)
//        {
//            case pt:        return "pT";
//            case eta:       return "eta";
//            case absEta:    return "absEta";
//            case mOverPt:   return "mOverPt";
//            default:        return "UNKNOWN";
//        }
//    }
//
//    TypeEnum StringToEnum(const TString type)
//    {
//        if (!type.CompareTo("pt",TString::kIgnoreCase)      || !type.CompareTo("p_{T}"))
//            return pt;
//        if (!type.CompareTo("eta",TString::kIgnoreCase)     || !type.CompareTo("#eta"))
//            return eta;
//        if (!type.CompareTo("absEta",TString::kIgnoreCase)  || !type.CompareTo("|#eta|"))
//            return absEta;
//        if (!type.CompareTo("mOverPt",TString::kIgnoreCase) || !type.CompareTo("m/p_{T}"))
//            return mOverPt;
//        return UNKNOWN;
//    }
//
//    TString EnumToFormattedString(const TypeEnum type)
//    {
//        switch (type)
//        {
//            case pt:        return "p_{T}";
//            case eta:       return "#eta";
//            case absEta:    return "|#eta|";
//            case mOverPt:   return "m/p_{T}";
//            default:        return "UNKNOWN";
//        }
//    }
//
//    float EnumToValue(const TypeEnum type, const xAOD::Jet& jet)
//    {
//        switch (type)
//        {
//            case pt:        return jet.pt();
//            case eta:       return jet.eta();
//            case absEta:    return fabs(jet.eta());
//            case mOverPt:   return jet.m()/jet.pt();
//            default:        return NAN;
//        }
//    }
//}


//////////////////////////////////////////////////
//                                              //
//  Pileup component enumeration methods        //
//                                              //
//////////////////////////////////////////////////

namespace PileupComp
{
    TString EnumToString(const TypeEnum type)
    {
        switch (type)
        {
            case OffsetNPV:     return "Pileup_OffsetNPV";
            case OffsetMu:      return "Pileup_OffsetMu";
            case PtTerm:        return "Pileup_PtTerm";
            case RhoTopology:   return "Pileup_RhoTopology";
            default:            return "UNKNOWN";
        }
    }

    TypeEnum StringToEnum(const TString type)
    {
        if (type.Contains("Pileup",TString::kIgnoreCase))
        {
            if (type.Contains("OffsetNPV",TString::kIgnoreCase))
                return OffsetNPV;
            if (type.Contains("OffsetMu",TString::kIgnoreCase))
                return OffsetMu;
            if (type.Contains("PtTerm",TString::kIgnoreCase))
                return PtTerm;
            if (type.Contains("RhoTopology",TString::kIgnoreCase))
                return RhoTopology;
        }
        return UNKNOWN;
    }
}


//////////////////////////////////////////////////
//                                              //
//  Pileup component enumeration methods        //
//                                              //
//////////////////////////////////////////////////

namespace FlavourComp
{
    TString EnumToString(const TypeEnum type)
    {
        switch (type)
        {
            case Response:      return "FlavourResponse";
            case Composition:   return "FlavourComposition";
            case bJES:          return "bJES";
            default:            return "UNKNOWN";
        }
    }

    TypeEnum StringToEnum(const TString type)
    {
        if (type.Contains("Flavour",TString::kIgnoreCase) || type.Contains("Flavor",TString::kIgnoreCase))
        {
            if (type.Contains("Response",TString::kIgnoreCase))
                return Response;
            if (type.Contains("Composition",TString::kIgnoreCase))
                return Composition;
        }
        if (type.Contains("bJES",TString::kIgnoreCase))
            return bJES;
        return UNKNOWN;
    }
}

} // end jet namespace


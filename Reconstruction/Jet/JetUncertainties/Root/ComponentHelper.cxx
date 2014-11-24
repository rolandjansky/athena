/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetUncertainties/ComponentHelper.h"

namespace jet
{

ComponentHelper::ComponentHelper(const TString& compName)
{
    name        = compName;
    desc        = "";
    cat         = "";
    corr        = "";
    param       = "";
    scale       = "";
    interpolate = "";
    numSplit    = -1;
    special     = "";
    hists       = "";
    validHists  = "";

    category        = CompCategory::UNKNOWN;
    correlation     = CompCorrelation::UNKNOWN;
    parametrization = CompParametrization::UNKNOWN;
    scaleVar        = CompScaleVar::UNKNOWN;
    isSpecial       = false;
    pileupType      = PileupComp::UNKNOWN;
    flavourType     = FlavourComp::UNKNOWN;
}

ComponentHelper::ComponentHelper(TEnv& settings, const TString& compPrefix, const TString& MCtype)
{
    // Read in information on the uncertainty component
    //      - Name: name
    //      - Desc: description
    //      - Type: category
    //      - Corr: correlation type (only need to specify if >1 histogram)
    //      - Param: parametrization
    //      - Scale: the variable to scale (fourvec, pt, D12, etc), default fourvec
    //      - Split: Number of sub-components to split this component into (default 1, no split)
    //      - Interp: If false, uses fixed bin content insted of interpolation
    //      - Special: Whether this component requires special treatment, "false" if not specified
    //      - Hists: histogram name(s), comma/space-separated list (if not specified, uses name)
    //      - VHists: validity histogram name(s), comma/space-separated list (optional)
    // Overwrite MCTYPE with the specified type if applicable
    name        = TString(settings.GetValue(compPrefix+"Name","")).ReplaceAll("MCTYPE",MCtype);
    desc        = settings.GetValue(compPrefix+"Desc","");
    cat         = settings.GetValue(compPrefix+"Type","");
    corr        = settings.GetValue(compPrefix+"Corr","");
    param       = settings.GetValue(compPrefix+"Param","");
    scale       = settings.GetValue(compPrefix+"Scale","FourVec");
    interpolate = settings.GetValue(compPrefix+"Interp","true");
    numSplit    = settings.GetValue(compPrefix+"Split",1);
    special     = settings.GetValue(compPrefix+"Special","");
    hists       = TString(settings.GetValue(compPrefix+"Hists","")).ReplaceAll("MCTYPE",MCtype);
    validHists  = TString(settings.GetValue(compPrefix+"VHists","")).ReplaceAll("MCType",MCtype);

    // Get enums where appropriate
    // Leave interpreting/checking the enums to others
    category        = CompCategory::StringToEnum(cat);
    correlation     = CompCorrelation::StringToEnum(corr);
    parametrization = CompParametrization::StringToEnum(param);
    scaleVar        = CompScaleVar::StringToEnum(scale);
    isSpecial       = (!special.CompareTo("true",TString::kIgnoreCase)) || (!special.CompareTo("yes",TString::kIgnoreCase));
    pileupType      = PileupComp::StringToEnum(name);
    flavourType     = FlavourComp::StringToEnum(name);
}

} // end jet namespace


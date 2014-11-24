/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_COMPONENTHELPER_H
#define JETUNCERTAINTIES_COMPONENTHELPER_H

#include "JetUncertainties/UncertaintyEnum.h"

#include "TString.h"
#include "TEnv.h"

namespace jet
{

class ComponentHelper
{
    public:
        ComponentHelper(const TString& name="");
        ComponentHelper(TEnv& settings, const TString& compPrefix, const TString& MCtype);
        virtual ~ComponentHelper() {}

        // Raw component values to read from a file
        TString name;
        TString desc;
        TString cat;
        TString corr;
        TString param;
        TString scale;
        TString interpolate;
        int numSplit;
        TString special;
        TString hists;
        TString validHists;

        // Derived values to parse from the raw values
        CompCategory::TypeEnum category;
        CompCorrelation::TypeEnum correlation;
        CompParametrization::TypeEnum parametrization;
        CompScaleVar::TypeEnum scaleVar;
        bool isSpecial;
        PileupComp::TypeEnum pileupType;
        FlavourComp::TypeEnum flavourType;
};

} // end jet namespace


#endif

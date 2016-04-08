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
        ComponentHelper(const TString& name="", const float energyScaleVal=1.e-3);
        ComponentHelper(TEnv& settings, const TString& compPrefix, const TString& MCtype, const float energyScaleVal);
        virtual ~ComponentHelper() {}
        
        // Values fixed by arguments
        const float energyScale;

        // Raw component values to read from a file
        TString name;
        TString desc;
        TString cat;
        TString corr;
        TString param;
        TString scale;
        TString isRed; 
        TString interpolStr;
        int splitNum;
        TString special;
        TString hists;
        TString validHists;
        int group;

        // Derived values to parse from the raw values
        CompCategory::TypeEnum category;
        CompCorrelation::TypeEnum correlation;
        CompParametrization::TypeEnum parametrization;
        CompScaleVar::TypeEnum scaleVar;
        bool isSpecial;
        PileupComp::TypeEnum pileupType;
        FlavourComp::TypeEnum flavourType;
        bool interpolate;
        bool reducible; 
};

} // end jet namespace


#endif

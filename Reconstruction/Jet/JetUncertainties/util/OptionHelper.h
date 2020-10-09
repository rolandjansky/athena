/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_OPTIONHELPER_H
#define JETUNCERTAINTIES_OPTIONHELPER_H

#include "AsgMessaging/AsgMessaging.h"
#include <vector>
#include <utility>
#include "TString.h"
#include "JetUncertainties/Helpers.h"
#include "JetUncertainties/UncertaintyEnum.h"

namespace jet
{

class OptionHelper : public asg::AsgMessaging
{
    public:
        // Constructor/destructor/initialization
        OptionHelper(const std::string& name = "OptionHelper");
        ~OptionHelper() {}
        bool Initialize(const std::vector<TString>& options);
        
        //
        // Info retrieval
        //
        
        // General control
        bool    IsPublicFormat()  const { checkInit(); return m_isPublic;       }
        bool    IsEtaDepPileup()  const { checkInit(); return m_isEtaDepPU;     }
        bool    IgnorePT()        const { checkInit(); return m_ignorePT;       }
        TString GetNamePrefix()   const { checkInit(); return m_compNamePrefix; }

        // Plot control
        bool    DoATLASLabel()    const { checkInit(); return m_doATLASLabel;  }
        TString GetATLASLabel()   const { checkInit(); return m_ATLASLabel;    }
        TString GetBunchSpacing() const { checkInit(); return m_bunchSpacing;  }
        bool    DrawTotal()       const { checkInit(); return m_doTotalUnc;    }
        TString TotalUncName()    const { checkInit(); return m_totalUncName;  }
        bool    SpecifyTagger()   const { checkInit(); return m_specifyTagger; }
        double  AxisMax()         const { checkInit(); return m_axisMax < 0 ? (IsLargeR()? 0.2 : 0.1) : m_axisMax; }
        double  AxisMin()         const { checkInit(); return m_axisMin;       }
        std::pair<double,double> xAxisRange() const { checkInit(); return m_xAxisRange; }
        bool    AbsValue()        const { checkInit(); return m_absVal;        }
        bool    LogPt()           const { checkInit(); return m_logPt;         }
        TString getMassType()     const { checkInit(); return m_massType;      }

        std::vector<double> GetPtBins()             const;
        std::vector<double> GetEtaBins()            const;
        std::vector<double> GetFixedPtVals()        const;
        std::vector<double> GetFixedEtaVals()       const;
        std::vector<double> GetFixedMoverPtVals()   const;
        
        // Uncertainty types
        bool IsSmallR() const { checkInit(); return m_isSmallR; }
        bool IsLargeR() const { checkInit(); return m_isLargeR; }
        bool IsJER()    const { checkInit(); return m_isJER;    }
        bool IsTLA()    const { checkInit(); return m_isTLA;         }

        // Compositions
        bool IsUnknownComposition() const { checkInit(); return m_isUnknownComp; }
        bool IsDijetComposition()   const { checkInit(); return m_isDijetComp;   }
        bool IsGinosComposition()   const { checkInit(); return m_isGinosComp;   }
        bool IsReginasComposition() const { checkInit(); return m_isReginasComp; }

        // Comparison helpers
        bool                 CompareOnly()    const { checkInit(); return m_onlyCompare; }
        TString              DoCompare()      const { checkInit(); return m_doCompare;   }
        std::vector<TString> GetCompareVals() const { checkInit(); return m_compareVals; }
        
        // Variable control
        std::vector<CompScaleVar::TypeEnum> GetScaleVars() const { checkInit(); return m_scaleVars; }
        const std::vector<std::string> VariablesToShift() const { checkInit(); return m_systFilters; }
  
        // Layout control
        TString GetInputsDir() const {  checkInit(); return m_inputsDir; }

    private:
        bool    m_isInit;

        bool    m_isPublic;
        bool    m_isEtaDepPU;
        bool    m_ignorePT;
        TString m_compNamePrefix;

        bool    m_doATLASLabel;
        TString m_ATLASLabel;
        TString m_bunchSpacing;
        bool    m_doTotalUnc;
        TString m_totalUncName;
        bool    m_specifyTagger;
        double  m_axisMax;
        double  m_axisMin;
        std::pair<double,double> m_xAxisRange;
        bool    m_absVal;
        bool    m_logPt;
        TString m_massType;
        
        TString m_ptBins;
        TString m_etaBins;
        TString m_fixedPtVals;
        TString m_fixedEtaVals;
        TString m_fixedMoverPtVals;

        bool    m_isSmallR;
        bool    m_isLargeR;
        bool    m_isJER;
        bool    m_isTLA;

        bool    m_isUnknownComp;
        bool    m_isDijetComp;
        bool    m_isGinosComp;
        bool    m_isReginasComp;

        bool    m_onlyCompare;
        TString m_doCompare;
        std::vector<TString> m_compareVals;
        
        std::vector<CompScaleVar::TypeEnum> m_scaleVars;
        std::vector<std::string> m_systFilters;
  
        // allowing MakeUncertaintyPlots to be run from outside
        // of the testInputs/run/ directory
        TString m_inputsDir;

        TString getOptionValue(const std::vector<TString>& options, const TString optionName) const;
        template <typename T>
        T getOptionValueWithDefault(const std::vector<TString>& options, const TString optionName, const T defaultVal);
        std::vector<TString> getCompareVals(const std::vector<TString>& options) const;
        
        void checkInit() const;

        std::vector<double> getBins(const TString& toParse) const;
};

OptionHelper::OptionHelper(const std::string& name)
    : asg::AsgMessaging(name)
    , m_isInit(false)

    , m_isPublic(false)
    , m_isEtaDepPU(false)
    , m_ignorePT(false)
    , m_compNamePrefix("JET_")

    , m_doATLASLabel(true)
    , m_ATLASLabel("Internal")
    , m_bunchSpacing("")
    , m_doTotalUnc(true)
    , m_totalUncName("Total uncertainty")
    , m_specifyTagger(true)
    , m_axisMax(-1)
    , m_axisMin(0)
    , m_xAxisRange(0,0)
    , m_absVal(true)
    , m_logPt(true)
    , m_massType("")
    
    , m_ptBins("")
    , m_etaBins("")
    , m_fixedPtVals("")
    , m_fixedEtaVals("")
    , m_fixedMoverPtVals("")

    , m_isSmallR(true)
    , m_isLargeR(false)
    , m_isJER(false)
    , m_isTLA(false)

    , m_isUnknownComp(true)
    , m_isDijetComp(false)
    , m_isGinosComp(false)
    , m_isReginasComp(false)

    , m_onlyCompare(false)
    , m_doCompare("")
    , m_compareVals()

    , m_scaleVars()
    , m_systFilters()
  
    , m_inputsDir("./")
{ }

bool OptionHelper::Initialize(const std::vector<TString>& options)
{
    if (m_isInit)
    {
        ATH_MSG_ERROR("The tool has already been initialized, blocking re-initialization");
        return false;
    }
    m_isInit = true;

    // Check for the no-options case
    if (!options.size()) return true;

    // Options exist, let's parse them
    m_isPublic       = getOptionValueWithDefault(options,"isPublic",m_isPublic);
    m_isEtaDepPU     = getOptionValueWithDefault(options,"isEtaDepPileup",m_isEtaDepPU);
    m_ignorePT       = getOptionValueWithDefault(options,"ignorePT",m_ignorePT);
    m_compNamePrefix = getOptionValueWithDefault(options,"prefix",m_compNamePrefix);

    m_doATLASLabel   = getOptionValueWithDefault(options,"DoATLASLabel",m_doATLASLabel);
    m_ATLASLabel     = getOptionValueWithDefault(options,"ATLASLabel",m_ATLASLabel);
    m_bunchSpacing   = getOptionValueWithDefault(options,"bunchSpacing",m_bunchSpacing);
    m_doTotalUnc     = getOptionValueWithDefault(options,"drawTotal",m_doTotalUnc);
    m_totalUncName   = getOptionValueWithDefault(options,"totalUncName",m_totalUncName).Strip(TString::kBoth,'"');
    m_specifyTagger  = getOptionValueWithDefault(options,"specifyTagger",m_specifyTagger);
    m_axisMax        = getOptionValueWithDefault(options,"axisMax",m_axisMax);
    m_axisMin        = getOptionValueWithDefault(options,"axisMin",m_axisMin);
    TString xAxisRange  = getOptionValue(options,"xAxisRange");
    if (xAxisRange != "")
    {
        std::vector<double> range = jet::utils::vectorize<double>(xAxisRange,"&");
        if (range.size() != 2)
            ATH_MSG_WARNING("xAxisRange doesn't match expected format of \"val1&val2\". Skipping.");
        else
        {
            const double lowX = range.at(0);
            const double highX = range.at(1);
            m_xAxisRange = std::make_pair(lowX,highX);
        }
    }
    m_absVal         = getOptionValueWithDefault(options,"absVal",m_absVal);
    m_logPt          = getOptionValueWithDefault(options,"logPt",m_logPt);
    m_massType       = getOptionValueWithDefault(options,"massDef",m_massType);
    
    m_ptBins         = getOptionValueWithDefault(options,"ptBins",m_ptBins);
    m_etaBins        = getOptionValueWithDefault(options,"etaBins",m_etaBins);
    m_fixedPtVals    = getOptionValueWithDefault(options,"fixedPtVals",m_fixedPtVals);
    m_fixedEtaVals   = getOptionValueWithDefault(options,"fixedEtaVals",m_fixedEtaVals);
    m_fixedMoverPtVals = getOptionValueWithDefault(options,"fixedMoverPtVals",m_fixedMoverPtVals);

    m_isLargeR       = getOptionValueWithDefault(options,"isLargeR",m_isLargeR);
    m_isJER          = getOptionValueWithDefault(options,"isJER",m_isJER);
    m_isTLA          = getOptionValueWithDefault(options,"isTLA",m_isTLA);
    m_isSmallR       = !(m_isLargeR || m_isJER);
    
    m_isDijetComp    = getOptionValueWithDefault(options,"isDijet",m_isDijetComp);
    m_isGinosComp    = getOptionValueWithDefault(options,"isGinos",m_isGinosComp);
    m_isReginasComp  = getOptionValueWithDefault(options,"isReginas",m_isReginasComp);
    m_isUnknownComp  = !m_isDijetComp;
    if (m_isGinosComp) {
      m_isDijetComp = false;
      m_isUnknownComp = false;
    }

    m_onlyCompare    = getOptionValueWithDefault(options,"compareOnly",m_onlyCompare);
    m_doCompare      = getOptionValueWithDefault(options,"doCompare",m_doCompare);
    m_compareVals    = getCompareVals(options);

    m_inputsDir      = getOptionValueWithDefault(options,"inputsDir",m_inputsDir);

    const TString localScaleVar = getOptionValue(options,"scaleVar");
    if (localScaleVar == "")
    {
        if (m_isSmallR || m_isJER) m_scaleVars.push_back(CompScaleVar::FourVec);
        else if (m_isLargeR)       m_scaleVars.push_back(CompScaleVar::Pt);
    }
    else
    {
        std::vector<TString> localScaleVarVec = jet::utils::vectorize<TString>(localScaleVar,"&");
        for (size_t iVar = 0; iVar < localScaleVarVec.size(); ++iVar)
            m_scaleVars.push_back(CompScaleVar::stringToEnum(localScaleVarVec.at(iVar)));
    }

    const TString systFilterString = getOptionValue(options,"VariablesToShift");
    if (systFilterString != "")
        m_systFilters = jet::utils::vectorize<std::string>(systFilterString,",");

    return true;
}


TString OptionHelper::getOptionValue(const std::vector<TString>& options, const TString optionName) const
{
    for (size_t iOpt = 0; iOpt < options.size(); ++iOpt)
    {
        std::vector<TString> option = jet::utils::vectorize<TString>(options.at(iOpt),"=");
        if (option.size() != 2)
        {
            printf("Bad option, check formatting: %s\n",options.at(iOpt).Data());
            exit(-1);
        }
        if (!option.at(0).CompareTo(optionName,TString::kIgnoreCase))
            return option.at(1);
    }
    return "";
}

template <typename T>
T OptionHelper::getOptionValueWithDefault(const std::vector<TString>& options, const TString optionName, const T defaultVal)
{
    const TString optionVal = getOptionValue(options,optionName);
    if (optionVal == "")
        return defaultVal;
    return jet::utils::getTypeObjFromString<T>(optionVal);
}

std::vector<TString> OptionHelper::getCompareVals(const std::vector<TString>& options) const
{
   std::vector<TString> returnVal;
   for (int i=1; i> 0; i++) {
     TString optname = TString::Format("compareConfig%i",i);
     const TString thisname = getOptionValue(options,optname);
     if (thisname != "") returnVal.push_back(thisname);
     else break;
   }
   return returnVal;
}


void OptionHelper::checkInit() const
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Asked for a value before initializing the tool");
        throw std::string("Initialization failure");
    }
}





std::vector<double> OptionHelper::getBins(const TString& toParse) const
{
    // Ensure this is in the expected format
    // type&numBins&minVal&maxVal
    // type is either "U" for uniform or "L" for logarithmic
    std::vector<TString> tokens = jet::utils::vectorize<TString>(toParse,"&");
    
    // Check the number of tokens
    if (tokens.size() != 4)
    {
        ATH_MSG_FATAL("Unexpected format for bins: " << toParse.Data());
        throw std::string("Parse failure");
    }

    // Check the type
    const bool isUniform = (tokens.at(0) == "U") || (tokens.at(0) == "u");
    const bool isLog     = (tokens.at(0) == "L") || (tokens.at(0) == "l");
    if (!isUniform && !isLog)
    {
        ATH_MSG_FATAL("Unexpected binning type (token 0), only U/u and L/l are currently supported: " << toParse.Data());
        throw std::string("Parse failure");
    }

    // Check the number of bins
    unsigned numBins = 0;
    if (!jet::utils::getTypeObjFromString(tokens.at(1),numBins))
    {
        ATH_MSG_FATAL("Number of bins (token 1) was not an unsigned int: " << toParse.Data());
        throw std::string("Parse failure");
    }

    // Check the min and max
    double minVal = 0, maxVal = 0;
    if (!jet::utils::getTypeObjFromString(tokens.at(2),minVal))
    {
        ATH_MSG_FATAL("Number of bins (token 2) was not a double: " << toParse.Data());
        throw std::string("Parse failure");
    }
    if (!jet::utils::getTypeObjFromString(tokens.at(3),maxVal))
    {
        ATH_MSG_FATAL("Number of bins (token 3) was not a double: " << toParse.Data());
        throw std::string("Parse failure");
    }
    if (maxVal < minVal)
    {
        ATH_MSG_FATAL("The maximum value is smaller than the minimum: " << toParse.Data());
        throw std::string("Parse failure");
    }

    // Done checking, finally return the bins
    if (isUniform)
        return jet::utils::getUniformBins(numBins,minVal,maxVal);
    return jet::utils::getLogBins(numBins,minVal,maxVal);
}



std::vector<double> OptionHelper::GetPtBins() const
{
    checkInit();
    std::vector<double> bins;

    // Check if the user has provided something
    if (m_ptBins == "")
    {
        if (IsLargeR())
            bins = jet::utils::getLogBins(2000,200,3000);
        else
            bins = jet::utils::getLogBins(2000,15,3000);
    }
    else
        bins = getBins(m_ptBins);
    
    return bins;
}

std::vector<double> OptionHelper::GetEtaBins() const
{
    checkInit();
    std::vector<double> bins;

    // Check if the user has provided something
    if (m_etaBins == "")
        bins = jet::utils::getUniformBins(1800,-4.5,4.5);
    else
        bins = getBins(m_etaBins);
    
    return bins;
}

std::vector<double> OptionHelper::GetFixedPtVals() const
{
    checkInit();
    std::vector<double> bins;

    if (m_fixedPtVals != "")
        bins = jet::utils::vectorize<double>(m_fixedPtVals,",");
    else
        bins = jet::utils::vectorize<double>("25,40,60,80,120",",");

    return bins;
}

std::vector<double> OptionHelper::GetFixedEtaVals() const
{
    checkInit();
    std::vector<double> bins;

    if (m_fixedEtaVals != "")
        bins = jet::utils::vectorize<double>(m_fixedEtaVals,",");
    else if (IsLargeR())
        bins = jet::utils::vectorize<double>("0",",");
    else if (IsJER())
        bins = jet::utils::vectorize<double>("0,0.8,1.2,2.1,2.8,3.2,3.6",",");
    else
        bins = jet::utils::vectorize<double>("0,0.5,1,2,2.5,3,4",",");

    return bins;
}

std::vector<double> OptionHelper::GetFixedMoverPtVals() const
{
    checkInit();
    std::vector<double> bins;

    if (m_fixedMoverPtVals != "")
        bins = jet::utils::vectorize<double>(m_fixedMoverPtVals,",");
    else if (!IsLargeR())
        bins = jet::utils::vectorize<double>("0",",");
    else if (IsPublicFormat())
        bins = jet::utils::vectorize<double>("0.101",",");
    else
        bins = jet::utils::vectorize<double>("0.001,0.05,0.101,0.15,0.201,0.25,0.301,0.35,0.401,0.45,0.501,0.55,0.601,0.65,0.701,0.75,0.801,0.85,0.901,0.95,1.001",",");
    
    return bins;
}


} // end jet namespace

#endif


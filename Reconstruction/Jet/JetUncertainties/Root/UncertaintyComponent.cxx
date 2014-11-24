/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/UncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

#include "TFile.h"

namespace jet
{

bool operator <  (const UncertaintyComponent& compA, const UncertaintyComponent& compB)
{
    return compA.getName() < compB.getName();
}

bool operator == (const UncertaintyComponent& compA, const UncertaintyComponent& compB)
{
    return compA.getName() == compB.getName();
}

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

UncertaintyComponent::UncertaintyComponent(const std::string& name)
    : asg::AsgMessaging(name)
    , m_isInit(false)
    , m_name(name.c_str())
    , m_desc("")
    , m_category(CompCategory::UNKNOWN)
    , m_corrType(CompCorrelation::UNKNOWN)
    , m_scaleVar(CompScaleVar::UNKNOWN)
    , m_interpolate(true)
    , m_splitNumber(0)
    , m_histos()
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

UncertaintyComponent::UncertaintyComponent(const ComponentHelper& component)
    : asg::AsgMessaging(component.name.Data())
    , m_isInit(false)
    , m_name(component.name)
    , m_desc(component.desc)
    , m_category(component.category)
    , m_corrType(component.correlation)
    , m_scaleVar(component.scaleVar)
    , m_interpolate(component.interpolate)
    , m_splitNumber(0)
    , m_histos()
{
    ATH_MSG_DEBUG(Form("Creating UncertaintyComponent named %s",m_name.Data()));
    if (m_category == CompCategory::UNKNOWN)
        ATH_MSG_FATAL("UNKNOWN category for UncertaintyComponent named " << m_name.Data());
    // Acceptible to not exist if it's only one component, check during Initialize()
    //if (m_corrType == CompCorrelation::UNKNOWN)
    //    ATH_MSG_FATAL(Form("UNKNOWN correlation for UncertaintyComponent named %s",m_name.Data()));
    if (m_scaleVar == CompScaleVar::UNKNOWN)
        ATH_MSG_FATAL("UNKNOWN scale variable for UncertaintyComponent named " << m_name.Data());
}

UncertaintyComponent::UncertaintyComponent(const UncertaintyComponent& toCopy)
    : asg::AsgMessaging(Form("%s_copy",toCopy.m_name.Data()))
    , m_isInit(toCopy.m_isInit)
    , m_name(toCopy.m_name)
    , m_desc(toCopy.m_desc)
    , m_category(toCopy.m_category)
    , m_corrType(toCopy.m_corrType)
    , m_scaleVar(toCopy.m_scaleVar)
    , m_interpolate(toCopy.m_interpolate)
    , m_splitNumber(toCopy.m_splitNumber)
    , m_histos()
{
    for (size_t iHist = 0; iHist < toCopy.m_histos.size(); ++iHist)
        m_histos.push_back(new UncertaintyHistogram(*toCopy.m_histos.at(iHist)));
}

UncertaintyComponent::~UncertaintyComponent()
{
    ATH_MSG_DEBUG(Form("Deleting UncertaintyComponent named %s",m_name.Data()));
    for (size_t iHisto = 0; iHisto < m_histos.size(); ++iHisto)
        JESUNC_SAFE_DELETE(m_histos.at(iHisto));
    m_histos.clear();
}

StatusCode UncertaintyComponent::SetSplitFactor(const int splitCompNumber)
{
    // Must set this before initialization
    if (m_isInit)
    {
        ATH_MSG_ERROR("Component is already initialized, this must be set before initialization");
        return StatusCode::FAILURE;
    }

    // Must be a recognized value
    if (splitCompNumber < 0 || splitCompNumber > 2)
    {
        ATH_MSG_ERROR("Asked for split number " << splitCompNumber << ", which is unrecognized (expected 0, 1, or 2)");
        return StatusCode::FAILURE;
    }

    m_splitNumber = splitCompNumber;
    ATH_MSG_DEBUG("Set split factor of " << m_name.Data() << " to " << m_splitNumber);
    return StatusCode::SUCCESS;
}

StatusCode UncertaintyComponent::Initialize(const std::vector<TString>& histNames, TFile* histFile)
{
    std::vector<TString> validHistNames;
    return Initialize(histNames,validHistNames,histFile);
}

StatusCode UncertaintyComponent::Initialize(const std::vector<TString>& histNames, const std::vector<TString>& validHistNames, TFile* histFile)
{
    // Prevent double-initialization
    if (m_isInit)
    {
        ATH_MSG_ERROR("Component is already initialized: " << m_name.Data());
        return StatusCode::FAILURE;
    }
    
    // Ensure that the type of component is sensible
    if (m_category == CompCategory::UNKNOWN)
    {
        ATH_MSG_ERROR("Category type is set to UNKNOWN for component: " << m_name.Data());
        return StatusCode::FAILURE;
    }
    //if (m_corrType == CompCorrelation::UNKNOWN)
    //{
    //    ATH_MSG_ERROR("Correlation type is set to UNKNOWN for component: " << m_name.Data());
    //    return StatusCode::FAILURE;
    //}

    // Ensure there was at least one histogram specified
    if (histNames.size() < 1)
    {
        ATH_MSG_ERROR(Form("No histograms specified for a component (%s)",m_name.Data()));
        return StatusCode::FAILURE;
    }

    // If there was more than one histogram, ensure correlation was specified
    if (histNames.size() > 1 && m_corrType == CompCorrelation::UNKNOWN)
    {
        ATH_MSG_ERROR(Form("Specified %zu histograms for a component (%s), but didn't specify their correlation",histNames.size(),m_name.Data()));
        return StatusCode::FAILURE;
    }

    // Ensure either no valid histograms were specified OR an equal number of uncertainty and validity
    if (validHistNames.size() != 0 && validHistNames.size() != histNames.size())
    {
        ATH_MSG_ERROR(Form("Specified %zu uncertainty histograms, but a %zu validity histograms (expected either 0 or the same number) for component: %s",histNames.size(),validHistNames.size(),m_name.Data()));
        return StatusCode::FAILURE;
    }

    // Add the histograms
    for (size_t iHisto = 0; iHisto < histNames.size(); ++iHisto)
    {
        if (!validHistNames.size())
            m_histos.push_back(new UncertaintyHistogram(histNames.at(iHisto),"",m_interpolate));
        else
            m_histos.push_back(new UncertaintyHistogram(histNames.at(iHisto),validHistNames.at(iHisto),m_interpolate));
        if (m_histos.back()->Initialize(histFile).isFailure())
            return StatusCode::FAILURE;
    }
    
    // Print a summary
    ATH_MSG_DEBUG(Form("%s: successfully initialized",m_name.Data()));

    m_isInit = true;
    return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////
//                                              //
//  Validitty and uncertainty retrieval         //
//                                              //
//////////////////////////////////////////////////

bool UncertaintyComponent::getValidity(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    // Valid only if all histogram(s) are valid
    for (size_t iHisto = 0; iHisto < m_histos.size(); ++iHisto)
        if (!getValidity(m_histos.at(iHisto),jet,eInfo))
            return false;
    return true;
}

double UncertaintyComponent::getUncertainty(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    // Simple if it's just one histogram
    if (m_histos.size() == 1)
        return getUncertainty(m_histos.at(0),jet,eInfo) * (m_splitNumber == 0 ? 1 : getSplitFactorLinear(m_histos.at(0),jet));

    // Quad sum or linear sum depending on correlation type specified
    double unc = 0;
    if (m_corrType == CompCorrelation::Uncorrelated)
    {
        for (size_t iHisto = 0; iHisto < m_histos.size(); ++iHisto)
            unc += pow(getUncertainty(m_histos.at(iHisto),jet,eInfo),2);
        unc = sqrt(unc);
    }
    else if (m_corrType == CompCorrelation::Correlated)
    {
        for (size_t iHisto = 0; iHisto < m_histos.size(); ++iHisto)
            unc += getUncertainty(m_histos.at(iHisto),jet,eInfo);
    }
    else
    {
        ATH_MSG_ERROR(Form("Unexpected correlation type for %s",m_name.Data()));
        return JESUNC_ERROR_CODE;
    }

    return m_splitNumber == 0 ? unc : unc*getSplitFactorLinear(m_histos.at(0),jet);
}

bool UncertaintyComponent::getValidUncertainty(double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    // Simple if it's just one histogram
    if (m_histos.size() == 1)
        return getValidUncertainty(m_histos.at(0),unc,jet,eInfo);

    // Quad sum or linear sum depending on correlation type specified
    if (m_corrType == CompCorrelation::Uncorrelated)
    {
        double localUnc = 0;
        for (size_t iHisto = 0; iHisto < m_histos.size(); ++iHisto)
        {
            double uncVal = 0;
            if (!getValidUncertainty(m_histos.at(iHisto),uncVal,jet,eInfo))
                return false;
            localUnc += pow(uncVal,2);
        }
        unc = sqrt(localUnc);
    }
    else if (m_corrType == CompCorrelation::Correlated)
    {
        double localUnc = 0;
        for (size_t iHisto = 0; iHisto < m_histos.size(); ++iHisto)
        {
            double uncVal = 0;
            if (!getValidUncertainty(m_histos.at(iHisto),uncVal,jet,eInfo))
                return false;
            localUnc += uncVal;
        }
        unc = localUnc;
    }
    else
    {
        ATH_MSG_ERROR(Form("Unexpected correlation type for %s",m_name.Data()));
        return false;
    }
    if (m_splitNumber > 0)
        unc *= getSplitFactorLinear(m_histos.at(0),jet);

    return true;
}

double UncertaintyComponent::getSplitFactorLinear(const UncertaintyHistogram* histo, const xAOD::Jet& jet) const
{
    // Straight line in log(pT) from 0 to 1
    // y = mx+b
    // m = 1/[log(max)-log(min)]
    // x = log(min) --> b = -m*log(min)

    const double minPt = histo->getHisto()->GetXaxis()->GetBinLowEdge(1);
    const double maxPt = histo->getHisto()->GetXaxis()->GetBinLowEdge(histo->getHisto()->GetNbinsX()+1);
    const double valPt = jet.pt();

    const double slope = 1./(log(maxPt)-log(minPt));
    const double intercept = -slope*log(minPt);

    const double factor = slope*log(valPt <= minPt ? minPt+1.e-3 : valPt >= maxPt ? maxPt-1.e-3 : valPt)+intercept;
    
    return m_splitNumber == 1 ? factor : sqrt(1-factor*factor);
}

} // end jet namespace


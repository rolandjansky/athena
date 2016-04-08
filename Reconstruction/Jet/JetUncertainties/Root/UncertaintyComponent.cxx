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
    , m_energyScale(1)
    , m_interpolate(true)
    , m_groupNum(-1)
    , m_splitNumber(0)
    , m_histos()
    , m_isReducible(false)
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
    , m_energyScale(component.energyScale)
    , m_interpolate(component.interpolate)
    , m_groupNum(component.group)
    , m_splitNumber(component.splitNum)
    , m_histos()
    , m_isReducible(component.reducible)
{
    ATH_MSG_DEBUG(Form("Creating UncertaintyComponent named %s",m_name.Data()));
    if (m_category == CompCategory::UNKNOWN)
        ATH_MSG_FATAL("UNKNOWN category for UncertaintyComponent named " << m_name.Data());
    // Acceptible to not exist if it's only one component, check during initialize()
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
    , m_energyScale(toCopy.m_energyScale)
    , m_interpolate(toCopy.m_interpolate)
    , m_groupNum(toCopy.m_groupNum)
    , m_splitNumber(toCopy.m_splitNumber)
    , m_histos()
    , m_isReducible(toCopy.m_isReducible)
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

StatusCode UncertaintyComponent::initialize(const std::vector<TString>& histNames, TFile* histFile)
{
    std::vector<TString> validHistNames;
    return initialize(histNames,validHistNames,histFile);
}

StatusCode UncertaintyComponent::initialize(const std::vector<TString>& histNames, const std::vector<TString>& validHistNames, TFile* histFile)
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
        if (m_histos.back()->initialize(histFile).isFailure())
            return StatusCode::FAILURE;
    }
    
    // Print a summary
    ATH_MSG_DEBUG(Form("%s: successfully initialized",m_name.Data()));

    m_isInit = true;
    return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////
//                                              //
//  Methods to test for special cases           //
//                                              //
//////////////////////////////////////////////////

bool UncertaintyComponent::isAlwaysZero() const
{
    if (!m_isInit)
    {
        ATH_MSG_ERROR("Cannot call method before initialization, component: "<<m_name.Data());
        return false;
    }

    for (size_t iHisto = 0; iHisto < m_histos.size(); ++iHisto)
    {
        const TH1* histo = m_histos.at(iHisto)->getHisto();
        if (fabs(histo->GetMinimum()) > 1.e-5 || fabs(histo->GetMaximum()) > 1.e-5)
            return false;
    }
    return true;
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
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
        return getUncertainty(m_histos.at(0),jet,eInfo) * (m_splitNumber == 0 ? 1 : getSplitFactor(m_histos.at(0),jet));

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

    return m_splitNumber == 0 ? unc : unc*getSplitFactor(m_histos.at(0),jet);
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
    if (m_splitNumber)
        unc *= getSplitFactor(m_histos.at(0),jet);

    return true;
}

double UncertaintyComponent::getSplitFactor(const UncertaintyHistogram* uncHisto, const xAOD::Jet& jet) const
{
    double splitFactor = 1;
    const TH1* histo = uncHisto->getHisto();

    if (m_splitNumber == 1 || m_splitNumber == -1)
    {
        // Straight line in log(pT) from 0 to 1
        // y = mx+b
        // m = 1/[log(max)-log(min)]
        // x = log(min) --> b = -m*log(min)

        const double minPt = histo->GetXaxis()->GetBinLowEdge(1);
        const double maxPt = histo->GetXaxis()->GetBinLowEdge(histo->GetNbinsX()+1);
        const double valPt = jet.pt()*m_energyScale;

        const double slope = 1./(log(maxPt)-log(minPt));
        const double intercept = -slope*log(minPt);

        splitFactor = slope*log(valPt <= minPt ? minPt+1.e-3 : valPt >= maxPt ? maxPt-1.e-3 : valPt)+intercept;
    }
    else if (m_splitNumber == 2 || m_splitNumber == -2)
    {
        // Straight line in |eta| from 0 to 1
        // y = mx + b
        // m = 1/(max - min)
        // x = min --> b = -m*min
        const double minEta = 0;
        const double maxEta = 4.5;
        const double absEta = fabs(jet.eta());

        const double slope = 1./(maxEta - minEta);
        const double intercept = -slope*minEta;

        splitFactor = slope*(absEta <= minEta ? minEta+1.e-3 : absEta >= maxEta ? maxEta-1.e-3 : absEta)+intercept;
    }
    else if (m_splitNumber == 3 || m_splitNumber == -3)
    {
        // Increasing with log(pT) and increasing with |eta|
        // z = mx + ny + c
        // z(min,min) = 0
        // z(max,max) = 1
        // Linear in both dimensions means need factor of 1/2 in a single dimension
        // m = 0.5/[log(maxPt)-log(minPt)]
        // n = 0.5/(maxEta - minEta)
        // c = -minPt*m - minEta*n
        // 2*z = (logx-logxmin)/(logxmax-logxmin) + (y-ymin)/(ymax-ymin)
        const double minPt = histo->GetXaxis()->GetBinLowEdge(1);
        const double maxPt = histo->GetXaxis()->GetBinLowEdge(histo->GetNbinsX()+1);
        const double valPt = jet.pt()*m_energyScale;

        const double minEta = 0;
        const double maxEta = 4.5;
        const double absEta = fabs(jet.eta());

        const double slopePt = 1./(log(maxPt)-log(minPt));
        const double slopeEta = 1./(maxEta-minEta);
        
        const double fixedPt  = valPt <= minPt ? minPt+1.e-3 : valPt >= maxPt ? maxPt - 1.e-3 : valPt;
        const double fixedEta = absEta <= minEta ? minEta+1.e-3 : absEta >= maxEta ? maxEta-1.e-3 : absEta;

        splitFactor = (slopePt*(log(fixedPt)-log(minPt)) + slopeEta*(fixedEta-minEta))/2.;
    }
    else if (m_splitNumber == 4 || m_splitNumber == -4)
    {
        // Increasing with log(pT) and decreasing with |eta|
        // See description above, follows similarly
        // 2*z = (logx-logxmin)/(logxmax-logxmin) + (ymax-y)/(ymax-ymin)
        const double minPt = histo->GetXaxis()->GetBinLowEdge(1);
        const double maxPt = histo->GetXaxis()->GetBinLowEdge(histo->GetNbinsX()+1);
        const double valPt = jet.pt()*m_energyScale;

        const double minEta = 0;
        const double maxEta = 4.5;
        const double absEta = fabs(jet.eta());

        const double slopePt = 1./(log(maxPt)-log(minPt));
        const double slopeEta = 1./(maxEta-minEta);
        
        const double fixedPt  = valPt <= minPt ? minPt+1.e-3 : valPt >= maxPt ? maxPt - 1.e-3 : valPt;
        const double fixedEta = absEta <= minEta ? minEta+1.e-3 : absEta >= maxEta ? maxEta-1.e-3 : absEta;

        splitFactor = (slopePt*(log(fixedPt)-log(minPt)) + slopeEta*(maxEta-fixedEta))/2.;
    }


    // Now check if this is the functional part or the complementary part
    if (m_splitNumber < 0)
        splitFactor = sqrt(1-splitFactor*splitFactor);

    return splitFactor;
}

} // end jet namespace


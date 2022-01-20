/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/UncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

#include "TFile.h"

namespace jet
{

bool operator <  (const UncertaintyComponent& componentA, const UncertaintyComponent& componentB)
{
    return componentA.getName() < componentB.getName();
}

bool operator == (const UncertaintyComponent& componentA, const UncertaintyComponent& componentB)
{
    return componentA.getName() == componentB.getName();
}


//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

UncertaintyComponent::UncertaintyComponent(const std::string& name)
    : asg::AsgMessaging(name)
    , m_isInit(false)
    , m_uncHistName("")
    , m_validHistName("")
    , m_scaleVar(CompScaleVar::UNKNOWN)
    , m_topology(JetTopology::UNKNOWN)
    , m_energyScale(1)
    , m_interpolate(Interpolate::UNKNOWN)
    , m_splitNumber(0)
    , m_uncHist(NULL)
    , m_validHist(NULL)
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

UncertaintyComponent::UncertaintyComponent(const ComponentHelper& component, const size_t numHist)
    : asg::AsgMessaging(component.name.Data())
    , m_isInit(false)
    , m_uncHistName(component.uncNames.size()!=0?component.uncNames.at(0):"NONE")
    , m_validHistName(component.validName)
    , m_scaleVar(component.scaleVar)
    , m_topology(component.topology)
    , m_energyScale(component.energyScale)
    , m_interpolate(component.interpolate)
    , m_splitNumber(component.splitNum)
    , m_uncHist(NULL)
    , m_validHist(NULL)
{
    ATH_MSG_DEBUG("Creating UncertaintyComponent named " << m_uncHistName.Data());
    if (m_uncHistName == "")
        ATH_MSG_FATAL("Cannot create an UncertaintyComponent with an empty uncHistName");
    if (m_scaleVar == CompScaleVar::UNKNOWN)
        ATH_MSG_FATAL("Cannot create an UncertaintyComponent scaling an UNKNOWN variable");
    if (numHist != component.uncNames.size())
        ATH_MSG_FATAL("Expected " << numHist << " uncertainty histograms, but received " << component.uncNames.size() << " : " << m_uncHistName.Data());
}

UncertaintyComponent::UncertaintyComponent(const UncertaintyComponent& toCopy)
    : asg::AsgMessaging(Form("%s_copy",toCopy.m_uncHistName.Data()))
    , m_isInit(toCopy.m_isInit)
    , m_uncHistName(toCopy.m_uncHistName)
    , m_validHistName(toCopy.m_validHistName)
    , m_scaleVar(toCopy.m_scaleVar)
    , m_topology(toCopy.m_topology)
    , m_energyScale(toCopy.m_energyScale)
    , m_interpolate(toCopy.m_interpolate)
    , m_splitNumber(toCopy.m_splitNumber)
    , m_uncHist(NULL)
    , m_validHist(NULL)
{

    if (toCopy.m_uncHist)
        m_uncHist = new UncertaintyHistogram(*toCopy.m_uncHist);
    if (toCopy.m_validHist)
        m_validHist = new UncertaintyHistogram(*toCopy.m_validHist);
}

UncertaintyComponent::~UncertaintyComponent()
{
    ATH_MSG_DEBUG("Deleting UncertaintyComponent named " << getName().Data());
    JESUNC_SAFE_DELETE(m_uncHist);
    JESUNC_SAFE_DELETE(m_validHist);
}

StatusCode UncertaintyComponent::initialize(TFile* histFile)
{
    // Prevent double-initialization
    if (m_isInit)
    {
        ATH_MSG_ERROR("Component is already initialized: " << getName().Data());
        return StatusCode::FAILURE;
    }

    // Create the histograms
    m_uncHist = new UncertaintyHistogram(m_uncHistName,m_interpolate);
    if (!m_uncHist)
    {
        ATH_MSG_ERROR("Failed to create uncertainty histogram for component: " << getName().Data());
        return StatusCode::FAILURE;
    }
    if (m_validHistName != "")
    {
        m_validHist = new UncertaintyHistogram(m_validHistName,Interpolate::None);
        if (!m_validHist)
        {
            ATH_MSG_ERROR("Failed to create validity histogram of name \"" << getValidName().Data() << "\" for component: " << getName().Data());
            return StatusCode::FAILURE;
        }
    }

    // Initialize the histograms
    if (m_uncHist->initialize(histFile).isFailure())   return StatusCode::FAILURE;
    if (m_validHist && m_validHist->initialize(histFile).isFailure()) return StatusCode::FAILURE;

    // Print a summary
    ATH_MSG_DEBUG("Successfully initialized UncertaintyComponent named " << getName().Data());
    
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
        ATH_MSG_ERROR("Cannot call method before initialization, component: " << getName().Data());
        return false;
    }

    const TH1* histo = m_uncHist->getHisto();
    if (fabs(histo->GetMinimum()) > 1.e-8 || fabs(histo->GetMaximum()) > 1.e-8)
        return false;
    return true;
}


//////////////////////////////////////////////////
//                                              //
//  Uncertainty/validity retrieval methods      //
//                                              //
//////////////////////////////////////////////////

bool UncertaintyComponent::getValidity(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    if (!m_isInit)
    {
        ATH_MSG_ERROR("Component hasn't been initialized: " << getName().Data());
        return false;
    }
    return getValidityImpl(jet,eInfo);
}

double UncertaintyComponent::getUncertainty(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    if (!m_isInit)
    {
        ATH_MSG_ERROR("Component hasn't been initialized: " << getName().Data());
        return JESUNC_ERROR_CODE;
    }
    return getUncertaintyImpl(jet,eInfo)*getSplitFactor(jet);
}

bool UncertaintyComponent::getValidUncertainty(double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    if (getValidity(jet,eInfo))
    {
        unc = getUncertainty(jet,eInfo);
        return true;
    }
    return false;
}


//////////////////////////////////////////////////
//                                              //
//  Split factor methods                        //
//                                              //
//////////////////////////////////////////////////

double UncertaintyComponent::getSplitFactor(const xAOD::Jet& jet) const
{
    // Immediate return for the most common case
    if (!m_splitNumber)
        return 1;

    // SplitNumber was specified, we have to calculate the factor
    double splitFactor = 1;
    const TH1* histo = m_uncHist->getHisto();

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


//////////////////////////////////////////////////
//                                              //
//  Helper methods                              //
//                                              //
//////////////////////////////////////////////////

bool UncertaintyComponent::getValidBool(const double validity) const
{
    if (validity < 1.e-5 && validity > -1.e-5) return false;
    if (validity < 1+1.e-5 && validity > 1-1.e-5) return true;
    ATH_MSG_ERROR(Form("Validity value not in expected range: %lf for histogram %s",validity,getValidName().Data()));
    return false;
}

double UncertaintyComponent::getAbsMass(const xAOD::Jet& jet, const CompMassDef::TypeEnum massDef) const
{
    // Check for the simple case (where we want the default four-vector itself)
    if (massDef == CompMassDef::UNKNOWN || massDef == CompMassDef::FourVecMass)
        return jet.m();
    
    // Not the simple case, check for the specified four-vector instead and return it if it is available
    JetFourMomAccessor scale(CompMassDef::getJetScaleString(massDef).Data());
    if (scale.isAvailable(jet))
        return scale(jet).M();

    // Fall-back on the TA moment as a float if applicable (legacy support)
    SG::AuxElement::ConstAccessor<float> scaleTAMoment("JetTrackAssistedMassCalibrated");
    if (massDef == CompMassDef::TAMass && scaleTAMoment.isAvailable(jet))
        return scaleTAMoment(jet);

    // Fall-back on the calo mass as the 4-vec if applicable (legacy support)
    if (massDef == CompMassDef::CaloMass)
        return jet.m();

    // Specified scale is not available, error
    ATH_MSG_ERROR("Failed to retrieve the " << CompMassDef::enumToString(massDef).Data() << " mass from the jet");
    return JESUNC_ERROR_CODE;
}

double UncertaintyComponent::getMassOverPt(const xAOD::Jet& jet, const CompMassDef::TypeEnum massDef) const
{
    // Check for the simple case (where we want the default four-vector itself)
    if (massDef == CompMassDef::UNKNOWN || massDef == CompMassDef::FourVecMass)
        return jet.m()/jet.pt();
    
    // Not the simple case, check for the specified four-vector instead and return it if it is available
    JetFourMomAccessor scale(CompMassDef::getJetScaleString(massDef).Data());
    if (scale.isAvailable(jet))
        return scale(jet).M()/scale(jet).Pt();
    
    // Fall-back on the TA moment as a float if applicable (legacy support)
    SG::AuxElement::ConstAccessor<float> scaleTAMoment("JetTrackAssistedMassCalibrated");
    if (massDef == CompMassDef::TAMass && scaleTAMoment.isAvailable(jet))
        return scaleTAMoment(jet)/jet.pt();
    
    // Fall-back on the calo mass as the 4-vec if applicable (legacy support)
    if (massDef == CompMassDef::CaloMass)
        return jet.m()/jet.pt();

    // Specified scale is not available, error
    ATH_MSG_ERROR("Failed to retrieve the " << CompMassDef::enumToString(massDef).Data() << " mass from the jet");
    return JESUNC_ERROR_CODE;

}

double UncertaintyComponent::getMassOverE(const xAOD::Jet& jet, const CompMassDef::TypeEnum massDef) const
{
    // Check for the simple case (where we want the default four-vector itself)
    if (massDef == CompMassDef::UNKNOWN || massDef == CompMassDef::FourVecMass)
        return jet.m()/jet.e();

    // Not the simple case, check for the specified four-vector instead and return it if it is available
    JetFourMomAccessor scale(CompMassDef::getJetScaleString(massDef).Data());
    if (scale.isAvailable(jet))
        return scale(jet).M()/scale(jet).E();
    
    // Fall-back on the TA moment as a float if applicable (legacy support)
    SG::AuxElement::ConstAccessor<float> scaleTAMoment("JetTrackAssistedMassCalibrated");
    if (massDef == CompMassDef::TAMass && scaleTAMoment.isAvailable(jet))
        return scaleTAMoment(jet)/jet.e();
    
    // Fall-back on the calo mass as the 4-vec if applicable (legacy support)
    if (massDef == CompMassDef::CaloMass)
        return jet.m()/jet.e();

    // Specified scale is not available, error
    ATH_MSG_ERROR("Failed to retrieve the " << CompMassDef::enumToString(massDef).Data() << " mass from the jet");
    return JESUNC_ERROR_CODE;

}

} // end jet namespace


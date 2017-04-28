/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/CombinedMassUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"
#include "JetUncertainties/UncertaintyGroup.h"

#include "TFile.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

CombinedMassUncertaintyComponent::CombinedMassUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name),0)
    , m_combMassType(CombMassComp::UNKNOWN)
    , m_setWeightMassDefs(false)
    , m_caloMassComp(NULL)
    , m_TAMassComp(NULL)
    , m_caloMassWeight(NULL)
    , m_TAMassWeight(NULL)
    , m_caloMassScale_weights("")
    , m_TAMassScale_weights("")
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

CombinedMassUncertaintyComponent::CombinedMassUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component,0)
    , m_combMassType(component.combMassType)
    , m_setWeightMassDefs(false)
    , m_caloMassComp(NULL)
    , m_TAMassComp(NULL)
    , m_caloMassWeight(NULL)
    , m_TAMassWeight(NULL)
    , m_caloMassScale_weights("")
    , m_TAMassScale_weights("")
{
    ATH_MSG_DEBUG("Created CombinedMassUncertaintyComponent named " << getName().Data());
}

CombinedMassUncertaintyComponent::CombinedMassUncertaintyComponent(const CombinedMassUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_combMassType(toCopy.m_combMassType)
    , m_setWeightMassDefs(toCopy.m_setWeightMassDefs)
    , m_caloMassComp(NULL)
    , m_TAMassComp(NULL)
    , m_caloMassWeight(toCopy.m_caloMassWeight)
    , m_TAMassWeight(toCopy.m_TAMassWeight)
    , m_caloMassScale_weights(toCopy.m_caloMassScale_weights)
    , m_TAMassScale_weights(toCopy.m_TAMassScale_weights)
{
    ATH_MSG_DEBUG("Creating copy of CombinedMassUncertaintyComponent named " << getName().Data());
    if (toCopy.m_caloMassComp)
        m_caloMassComp = new UncertaintyGroup(*toCopy.m_caloMassComp);
    if (toCopy.m_TAMassComp)
        m_TAMassComp = new UncertaintyGroup(*toCopy.m_TAMassComp);
}

CombinedMassUncertaintyComponent* CombinedMassUncertaintyComponent::clone() const
{
    return new CombinedMassUncertaintyComponent(*this);
}

CombinedMassUncertaintyComponent::~CombinedMassUncertaintyComponent()
{
    JESUNC_SAFE_DELETE(m_caloMassComp);
    JESUNC_SAFE_DELETE(m_TAMassComp);
}

StatusCode CombinedMassUncertaintyComponent::setCaloTerm(UncertaintyGroup* caloComp)
{
    if (m_isInit)
    {
        ATH_MSG_ERROR("Can only set the calo mass term before initialization: " << getName().Data());
        return StatusCode::FAILURE;
    }
    if (m_caloMassComp != NULL)
    {
        ATH_MSG_ERROR("Calo mass term has already been set, blocking double-init: " << getName().Data());
        return StatusCode::FAILURE;
    }
    m_caloMassComp = caloComp;

    return StatusCode::SUCCESS;
}

StatusCode CombinedMassUncertaintyComponent::setTATerm(UncertaintyGroup* TAComp)
{
    if (m_isInit)
    {
        ATH_MSG_ERROR("Can only set the TA mass term before initialization: " << getName().Data());
        return StatusCode::FAILURE;
    }
    if (m_TAMassComp != NULL)
    {
        ATH_MSG_ERROR("TA mass term has already been set, blocking double-init: " << getName().Data());
        return StatusCode::FAILURE;
    }
    m_TAMassComp = TAComp;

    return StatusCode::SUCCESS;
}

StatusCode CombinedMassUncertaintyComponent::setCaloWeights(const UncertaintyHistogram* caloWeights)
{
    if (m_isInit)
    {
        ATH_MSG_ERROR("Can only set the calo mass weights before initialization: " << getName().Data());
        return StatusCode::FAILURE;
    }
    if (m_caloMassWeight != NULL)
    {
        ATH_MSG_ERROR("Calo mass weights has already been set, blocking double-init: " << getName().Data());
        return StatusCode::FAILURE;
    }
    m_caloMassWeight = caloWeights;

    return StatusCode::SUCCESS;
}

StatusCode CombinedMassUncertaintyComponent::setTAWeights(const UncertaintyHistogram* TAWeights)
{
    if (m_isInit)
    {
        ATH_MSG_ERROR("Can only set the TA mass weights before initialization: " << getName().Data());
        return StatusCode::FAILURE;
    }
    if (m_TAMassWeight != NULL)
    {
        ATH_MSG_ERROR("TA mass weights has already been set, blocking double-init: " << getName().Data());
        return StatusCode::FAILURE;
    }
    m_TAMassWeight = TAWeights;

    return StatusCode::SUCCESS;
}

StatusCode CombinedMassUncertaintyComponent::setCombWeightMassDefs(const CompMassDef::TypeEnum caloMassDef, const CompMassDef::TypeEnum TAMassDef)
{
    if (m_isInit)
    {
        ATH_MSG_ERROR("Can only set the weight mass definitions before initialization: " << getName().Data());
        return StatusCode::FAILURE;
    }
    if (m_setWeightMassDefs)
    {
        ATH_MSG_ERROR("Already set the weights, blocking double-setting: " << getName().Data());
        return StatusCode::FAILURE;
    }
    switch (caloMassDef)
    {
        case CompMassDef::CaloMass:
        case CompMassDef::TAMass:
            m_caloMassScale_weights = JetFourMomAccessor(CompMassDef::getJetScaleString(caloMassDef).Data());
            break;
        default:
            ATH_MSG_ERROR("Unsupported mass parametrization for the combined mass calo weights: " << getName().Data());
            return StatusCode::FAILURE;
    }
    switch (TAMassDef)
    {
        case CompMassDef::CaloMass:
        case CompMassDef::TAMass:
            m_TAMassScale_weights = JetFourMomAccessor(CompMassDef::getJetScaleString(TAMassDef).Data());
            break;
        default:
            ATH_MSG_ERROR("Unsupported mass parametrization for the combined mass TA weights: " << getName().Data());
            return StatusCode::FAILURE;
    }

    m_setWeightMassDefs = true;
    return StatusCode::SUCCESS;
}

StatusCode CombinedMassUncertaintyComponent::initialize(TFile* histFile)
{
    // We are completely different here than the normal case
    // Ignore the base component initialization procedure
    
    // Prevent double-initialization
    if (m_isInit)
    {
        ATH_MSG_ERROR("Component is already initialized: " << getName().Data());
        return StatusCode::FAILURE;
    }
    
    // Ensure that the combination weights exist
    if (!m_caloMassWeight)
    {
        ATH_MSG_ERROR("Calorimeter mass weights were not defined: " << getName().Data());
        return StatusCode::FAILURE;
    }
    if (!m_TAMassWeight)
    {
        ATH_MSG_ERROR("Track-assisted mass weights were not defined: " << getName().Data());
        return StatusCode::FAILURE;
    }

    // Ensure that the weight mass definitions were set
    if (!m_setWeightMassDefs)
    {
        ATH_MSG_ERROR("The mass definitions for the combination weight factors were not set: " << getName().Data());
        return StatusCode::FAILURE;
    }

    // Understand what situation we are in
    if      (m_combMassType == CombMassComp::UNKNOWN)
    {
        ATH_MSG_ERROR("Unknown combined mass uncertainty type: " << getName().Data());
        return StatusCode::FAILURE;
    }
    else if (m_combMassType == CombMassComp::Calo)
    {
        // Only the calo mass component should be specified
        if (!m_caloMassComp)
        {
            ATH_MSG_ERROR("Calo mass term was not specified for a CombMass_calo component: " << getName().Data());
            return StatusCode::FAILURE;
        }
        if (m_TAMassComp)
        {
            ATH_MSG_ERROR("TA mass term was specified for a CombMass_calo component: " << getName().Data());
            return StatusCode::FAILURE;
        }
    }
    else if (m_combMassType == CombMassComp::TA)
    {
        // Only the TA mass component should be specified
        if (m_caloMassComp)
        {
            ATH_MSG_ERROR("Calo mass term was specified for a CombMass_TA component: " << getName().Data());
            return StatusCode::FAILURE;
        }
        if (!m_TAMassComp)
        {
            ATH_MSG_ERROR("TA mass term was not specified for a CombMass_TA component: " << getName().Data());
            return StatusCode::FAILURE;
        }
    }
    else if (m_combMassType == CombMassComp::Both)
    {
        // Both components should be specified
        if (!m_caloMassComp)
        {
            ATH_MSG_ERROR("Calo mass term was not specified for a CombMass_both component: " << getName().Data());
            return StatusCode::FAILURE;
        }
        if (!m_TAMassComp)
        {
            ATH_MSG_ERROR("TA mass term was not specified for a CombMass_both component: " << getName().Data());
            return StatusCode::FAILURE;
        }
    }

    // Initialize the component(s)
    if (m_caloMassComp && m_caloMassComp->initialize(histFile).isFailure())
    {
        ATH_MSG_ERROR("Failed to initialize calo mass portion of a comb mass uncertainty: " << getName().Data());
        return StatusCode::FAILURE;
    }
    if (m_TAMassComp && m_TAMassComp->initialize(histFile).isFailure())
    {
        ATH_MSG_ERROR("Failed to initialize TA mass portion of a comb mass uncertainty: " << getName().Data());
        return StatusCode::FAILURE;
    }


    // Done!
    m_isInit = true;
    return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool CombinedMassUncertaintyComponent::getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    return getValidityCalo(jet,eInfo) && getValidityTA(jet,eInfo);
}

StatusCode CombinedMassUncertaintyComponent::calculateCombinedMass(const xAOD::Jet& jet, const double shiftFactorCalo, const double shiftFactorTA, double& combMass) const
{
    // Accessors for the scales we need
    static JetFourMomAccessor caloMassScale(CompMassDef::getJetScaleString(CompMassDef::CaloMass).Data());
    static JetFourMomAccessor TAMassScale(CompMassDef::getJetScaleString(CompMassDef::TAMass).Data());

    // Get the weight factors
    const double factorCalo = getWeightFactorCalo(jet,shiftFactorCalo);
    const double factorTA   = getWeightFactorTA(jet,shiftFactorTA);

    // Watch for division by zero
    if (factorCalo+factorTA == 0)
    {
        ATH_MSG_ERROR("Encountered division by zero when calculating weights: " << getName().Data());
        return StatusCode::FAILURE;
    }

    // Calculate the weights
    const double caloWeight = factorCalo/(factorCalo+factorTA);
    const double TAWeight   = factorTA/(factorCalo+factorTA);

    // Watch for zero masses
    // If one mass is zero, use the other without a weight
    // If both are zero, it doesn't matter, the combined mass is zero
    if (caloMassScale.m(jet) == 0)
        combMass = TAMassScale.m(jet)*shiftFactorTA;
    else if (TAMassScale.m(jet) == 0)
        combMass = caloMassScale.m(jet)*shiftFactorCalo;
    else
        combMass = (caloMassScale.m(jet)*shiftFactorCalo*caloWeight) + (TAMassScale.m(jet)*shiftFactorTA*TAWeight);

    //if (fabs(jet.pt()*m_energyScale-700)<1 && fabs(jet.m()*m_energyScale-70)<1) ATH_MSG_INFO(Form("CombMass: %f*%f*%f + %f*%f*%f = %f",caloMassScale(jet).M()*m_energyScale,shiftFactorCalo,caloWeight,TAMassScale(jet).M()*m_energyScale,shiftFactorTA,TAWeight,combMass*m_energyScale));

    return true;
}

double CombinedMassUncertaintyComponent::getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{

    // Get the per-part uncertainties
    double uncCalo = 0;
    if (m_caloMassComp && !m_caloMassComp->getValidUncertainty(uncCalo,jet,eInfo,CompScaleVar::Mass))
    {
        // Not valid
        ATH_MSG_ERROR("Combined mass is outside of the validity range (calo part)");
        return JESUNC_ERROR_CODE;
    }
    double uncTA = 0;
    if (m_TAMassComp && !m_TAMassComp->getValidUncertainty(uncTA,jet,eInfo,CompScaleVar::Mass))
    {
        // Not valid
        ATH_MSG_ERROR("Combined mass is outside of the validity range (TA part)");
        return JESUNC_ERROR_CODE;
    }

    // Get the mass values (both up and down shifts)
    // The resolution functions evaluated for masses shifted up and down may not be symmetric
    // For now, take the average of the up and down shifts
    // Note that we also only consider correlated shifts here
    // Uncorrelated shifts can be handled as two separate components
    // Anticorelated shifts are currently not supported (no need so far)
    double massDefault = 0;
    if (!calculateCombinedMass(jet,1,1,massDefault))            return JESUNC_ERROR_CODE;
    if (massDefault == 0) return 0; // This is a relative uncertainty
    double massUp = 0;
    if (!calculateCombinedMass(jet,1+uncCalo,1+uncTA,massUp))   return JESUNC_ERROR_CODE;
    double massDown = 0;
    if (!calculateCombinedMass(jet,1-uncCalo,1-uncTA,massDown)) return JESUNC_ERROR_CODE;
    
    const double massUncUp   = fabs((massUp-massDefault)/massDefault);
    const double massUncDown = fabs((massDown-massDefault)/massDefault);

    //if (fabs(jet.pt()*m_energyScale-700)<1 && fabs(jet.m()*m_energyScale-70)<1) ATH_MSG_INFO(Form("Combined mass: pT = %.0f, m = %.1f, mComb = %.1f, mCaloUnc = %.1f%%, mTAUnc = %.1f%%, m+1s = %.1f (%.1f%%), m-1s = %.1f (%.1f%%)",jet.pt()*m_energyScale,jet.m()*m_energyScale,massDefault*m_energyScale,100*uncCalo,100*uncTA,massUp*m_energyScale,100*massUncUp,massDown*m_energyScale,100*massUncDown));

    return (massUncUp+massUncDown)/2.;
}



bool CombinedMassUncertaintyComponent::getValidityCalo(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    return !m_caloMassComp ? true : m_caloMassComp->getValidity(jet,eInfo,CompScaleVar::Mass);
}

bool CombinedMassUncertaintyComponent::getValidityTA(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    return !m_TAMassComp ? true : m_TAMassComp->getValidity(jet,eInfo,CompScaleVar::Mass);
}

double CombinedMassUncertaintyComponent::getUncertaintyCalo(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    return !m_caloMassComp ? 0 : m_caloMassComp->getUncertainty(jet,eInfo,CompScaleVar::Mass);
}

double CombinedMassUncertaintyComponent::getUncertaintyTA(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    return !m_TAMassComp ? 0 : m_TAMassComp->getUncertainty(jet,eInfo,CompScaleVar::Mass);
}

bool CombinedMassUncertaintyComponent::getValidUncertaintyCalo(double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    return !m_caloMassComp ? true : m_caloMassComp->getValidUncertainty(unc,jet,eInfo,CompScaleVar::Mass);
}

bool CombinedMassUncertaintyComponent::getValidUncertaintyTA(double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    return !m_TAMassComp ? true : m_TAMassComp->getValidUncertainty(unc,jet,eInfo,CompScaleVar::Mass);
}

double CombinedMassUncertaintyComponent::getWeightFactorCalo(const xAOD::Jet& jet, const double shiftFactor) const
{
    const double resolution = !m_caloMassWeight ? 0 : m_caloMassWeight->getValue(m_caloMassScale_weights(jet).Pt()*m_energyScale,
                                                                                 (m_caloMassScale_weights(jet).M()*shiftFactor)/m_caloMassScale_weights(jet).Pt()
                                                                                );
    return resolution == 0 ? 0 : 1./(resolution*resolution);
}

double CombinedMassUncertaintyComponent::getWeightFactorTA(const xAOD::Jet& jet, const double shiftFactor) const
{
    const double resolution = !m_TAMassWeight ? 0 : m_TAMassWeight->getValue(m_TAMassScale_weights(jet).Pt()*m_energyScale,
                                                                             (m_TAMassScale_weights(jet).M()*shiftFactor)/m_TAMassScale_weights(jet).Pt()
                                                                            );
    return resolution == 0 ? 0 : 1./(resolution*resolution);
}



//////////////////////////////////////////////////
//                                              //
//  Base class method overrides                 //
//                                              //
//////////////////////////////////////////////////

bool CombinedMassUncertaintyComponent::isAlwaysZero() const
{
    if (!m_isInit)
    {
        ATH_MSG_ERROR("Cannot call method before initialization, component: " << getName().Data());
        return false;
    }

    return (!m_caloMassComp || m_caloMassComp->isAlwaysZero()) && (!m_TAMassComp || m_TAMassComp->isAlwaysZero());
}




} // end jet namespace

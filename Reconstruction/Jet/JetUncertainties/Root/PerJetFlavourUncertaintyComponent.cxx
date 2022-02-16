/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/PerJetFlavourUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

PerJetFlavourUncertaintyComponent::PerJetFlavourUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name),0)
    , m_absEta(false)
    , m_labels()
    , m_flavourType(FlavourComp::UNKNOWN)

{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

PerJetFlavourUncertaintyComponent::PerJetFlavourUncertaintyComponent(const ComponentHelper& component)
    : UncertaintyComponent(component,1)
    , m_absEta(CompParametrization::isAbsEta(component.parametrization))
    , m_labels(component.truthLabels)
    , m_flavourType(component.flavourType)

{
    ATH_MSG_DEBUG("Created PerJetFlavourUncertaintyComponent named " << m_uncHistName.Data());
}

PerJetFlavourUncertaintyComponent::PerJetFlavourUncertaintyComponent(const PerJetFlavourUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_absEta(toCopy.m_absEta)
    , m_labels(toCopy.m_labels)
    , m_flavourType(toCopy.m_flavourType)

{
    ATH_MSG_DEBUG("Creating copy of PerJetFlavourUncertaintyComponent named " << m_uncHistName.Data());
}

PerJetFlavourUncertaintyComponent* PerJetFlavourUncertaintyComponent::clone() const
{
    return new PerJetFlavourUncertaintyComponent(*this);
}

StatusCode PerJetFlavourUncertaintyComponent::initialize(TFile* histFile)
{
    // Call the base class first
    if (UncertaintyComponent::initialize(histFile).isFailure())
        return StatusCode::FAILURE;

    // Ensure that the labels are sane
    if (!m_labels.size())
    {
        ATH_MSG_ERROR("Specified a PerJetFlavourUncertaintyComponent without the associated labels to use: " << getName().Data());
        return StatusCode::FAILURE;
    }
    for (int aLabel : m_labels)
    {
        if (!isSupportedLabel(aLabel))
        {
            ATH_MSG_ERROR("Unsupported label ID of " << aLabel << " in " << getName().Data());
            return StatusCode::FAILURE;
        }
    }

    // Done checking the configuration
    return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool PerJetFlavourUncertaintyComponent::getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return !m_validHist ? true : getValidBool(m_validHist->getValue(jet.pt()*m_energyScale,m_absEta ? fabs(jet.eta()) : jet.eta()));
}

double PerJetFlavourUncertaintyComponent::getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    return getFlavourResponseUncertainty(jet,eInfo);
}

double PerJetFlavourUncertaintyComponent::getFlavourResponseUncertainty(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    // sigma_x = l_x * dR_x(x response modelling uncertainty)
    //      sigma_x is the flavour response uncertainty for jet type x
    //      l_x is the jet flavour label, and is 0 or 1
    //      dR_x is the response modelling uncertainty for jets of type x
    //      dR_x is typically taken as the difference in x response between Pythia and Herwig
    
    // Check the jet label (is this uncertainty relevant?)
    if (!checkTruthLabel(jet))
        return 0;

    // This is a jet of the relevant type
    // Now calculate and return the uncertainty
    const double pT  = jet.pt()*m_energyScale;
    const double eta = m_absEta ? fabs(jet.eta()) : jet.eta();
    
    FlavourComp::TypeEnum ThisJetFlavourType = FlavourComp::UNKNOWN;
    if(m_labels.at(0)==21 || m_labels.at(0)==0)   ThisJetFlavourType = FlavourComp::PerJetResponse_Gluon;
    else if(m_labels.at(0)==1 || m_labels.at(0)==2 || m_labels.at(0)==3) ThisJetFlavourType = FlavourComp::PerJetResponse_LQ;
    else if(m_labels.at(0)==5)  ThisJetFlavourType = FlavourComp::PerJetResponse_B;
    else if(m_labels.at(0)==4 )  ThisJetFlavourType = FlavourComp::PerJetResponse_C;
    
    // bool DoesItPass = false;
    if(m_flavourType == ThisJetFlavourType){
        // Return the uncertainty
        return m_uncHist->getValue(pT,eta);

    }else{ return 0;}


}



bool PerJetFlavourUncertaintyComponent::isSupportedLabel(const int label) const
{
    switch (abs(label))
    {
        case 1:  // parton-label, up
        case 2:  // parton-label, down
        case 3:  // parton-label, strange
        case 4:  // parton-label, charm
        case 5:  // parton-label, bottom
        case 21: // parton-label, gluon
        case 0: // parton-label, PileUp/non-identified
            return true;
        default:
            return false;
    }
}

bool PerJetFlavourUncertaintyComponent::checkTruthLabel(const xAOD::Jet& jet) const
{
    static SG::AuxElement::ConstAccessor<int> truthLabelAccessor ("PartonTruthLabelID");
    if (!truthLabelAccessor.isAvailable(jet))
    {
        ATH_MSG_ERROR("Unable to find PartonTruthLabelID on the jet");
        return JESUNC_ERROR_CODE;
    }
    const int truthHighestEparton = abs(truthLabelAccessor(jet));

    for (const int label : m_labels)
        if (label == truthHighestEparton)
            return true;
    return false;
}

} // end jet namespace


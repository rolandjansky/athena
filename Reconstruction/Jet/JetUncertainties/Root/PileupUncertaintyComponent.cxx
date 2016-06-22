/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/PileupUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

#include "TFile.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

PileupUncertaintyComponent::PileupUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name),0)
    , m_pileupType(PileupComp::UNKNOWN)
    , m_refNPV(-1)
    , m_refMu(-1)
    , m_refNPVHist(NULL)
    , m_refMuHist(NULL)
    , m_absEta(false)
    , m_secondUncName("")
    , m_secondUncHist(NULL)
    , m_refType(PileupRef_UNKNOWN)
    , m_secondRefType(PileupRef_UNKNOWN)
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

PileupUncertaintyComponent::PileupUncertaintyComponent( const ComponentHelper& component,
                                                        const float refNPV,
                                                        const float refMu
                                                        )
    : UncertaintyComponent(component,component.pileupType == PileupComp::PtTerm ? 2 : 1)
    , m_pileupType(component.pileupType)
    , m_refNPV(refNPV)
    , m_refMu(refMu)
    , m_refNPVHist(NULL)
    , m_refMuHist(NULL)
    , m_absEta(CompParametrization::isAbsEta(component.parametrization))
    , m_secondUncName(component.uncNames.size()>1?component.uncNames.at(1):"")
    , m_secondUncHist(NULL)
    , m_refType(PileupRef_UNKNOWN)
    , m_secondRefType(PileupRef_UNKNOWN)
{
    ATH_MSG_DEBUG("Created PileupUncertaintyComponent named" << m_uncHistName.Data());
    
    // Ensure that the pileup type and ref values are sensible
    if (m_pileupType == PileupComp::UNKNOWN)
        ATH_MSG_FATAL("Pileup type is UNKNOWN: " << m_uncHistName.Data());
    if (m_refNPV <= 0 || m_refMu <= 0)
        ATH_MSG_FATAL(Form("Unexpected pileup reference values.  (NPV,mu)=(%.1f,%.1f) for %s",m_refNPV,m_refMu,m_uncHistName.Data()));
}

PileupUncertaintyComponent::PileupUncertaintyComponent( const ComponentHelper& component,
                                                        const UncertaintyHistogram* refNPV,
                                                        const UncertaintyHistogram* refMu
                                                        )
    : UncertaintyComponent(component,component.pileupType == PileupComp::PtTerm ? 2 : 1)
    , m_pileupType(component.pileupType)
    , m_refNPV(-1)
    , m_refMu(-1)
    , m_refNPVHist(refNPV)
    , m_refMuHist(refMu)
    , m_absEta(CompParametrization::isAbsEta(component.parametrization))
    , m_secondUncName(component.uncNames.size()>1?component.uncNames.at(1):"")
    , m_secondUncHist(NULL)
    , m_refType(PileupRef_UNKNOWN)
    , m_secondRefType(PileupRef_UNKNOWN)
{
    ATH_MSG_DEBUG("Created PileupUncertaintyComponent named" << m_uncHistName.Data());
    
    // Ensure that the pileup type and ref values are sensible
    if (m_pileupType == PileupComp::UNKNOWN)
        ATH_MSG_FATAL("Pileup type is UNKNOWN: " << m_uncHistName.Data());
    if (!m_refNPV || !m_refMu)
        ATH_MSG_FATAL(Form("Unexpected pileup reference values.  (NPV,mu)=(%s,%s) for %s",m_refNPVHist?"OK":"NULL",m_refMuHist?"OK":"NULL",m_uncHistName.Data()));
}

PileupUncertaintyComponent::PileupUncertaintyComponent( const ComponentHelper& component,
                                                        const UncertaintyHistogram* refNPV,
                                                        const float refMu
                                                        )
    : UncertaintyComponent(component,component.pileupType == PileupComp::PtTerm ? 2 : 1)
    , m_pileupType(component.pileupType)
    , m_refNPV(-1)
    , m_refMu(refMu)
    , m_refNPVHist(refNPV)
    , m_refMuHist(NULL)
    , m_absEta(CompParametrization::isAbsEta(component.parametrization))
    , m_secondUncName(component.uncNames.size()>1?component.uncNames.at(1):"")
    , m_secondUncHist(NULL)
    , m_refType(PileupRef_UNKNOWN)
    , m_secondRefType(PileupRef_UNKNOWN)
{
    ATH_MSG_DEBUG("Created PileupUncertaintyComponent named" << m_uncHistName.Data());
    
    // Ensure that the pileup type and ref values are sensible
    if (m_pileupType == PileupComp::UNKNOWN)
        ATH_MSG_FATAL("Pileup type is UNKNOWN: " << m_uncHistName.Data());
    if (!m_refNPV || m_refMu <= 0)
        ATH_MSG_FATAL(Form("Unexpected pileup reference values.  (NPV,mu)=(%s,%.1f) for %s",m_refNPVHist?"OK":"NULL",m_refMu,m_uncHistName.Data()));
}

PileupUncertaintyComponent::PileupUncertaintyComponent( const ComponentHelper& component,
                                                        const float refNPV,
                                                        const UncertaintyHistogram* refMu
                                                        )
    : UncertaintyComponent(component,component.pileupType == PileupComp::PtTerm ? 2 : 1)
    , m_pileupType(component.pileupType)
    , m_refNPV(refNPV)
    , m_refMu(-1)
    , m_refNPVHist(NULL)
    , m_refMuHist(refMu)
    , m_absEta(CompParametrization::isAbsEta(component.parametrization))
    , m_secondUncName(component.uncNames.size()>1?component.uncNames.at(1):"")
    , m_secondUncHist(NULL)
    , m_refType(PileupRef_UNKNOWN)
    , m_secondRefType(PileupRef_UNKNOWN)
{
    ATH_MSG_DEBUG("Created PileupUncertaintyComponent named" << m_uncHistName.Data());
    
    // Ensure that the pileup type and ref values are sensible
    if (m_pileupType == PileupComp::UNKNOWN)
        ATH_MSG_FATAL("Pileup type is UNKNOWN: " << m_uncHistName.Data());
    if (m_refNPV <= 0 || !m_refMu)
        ATH_MSG_FATAL(Form("Unusual pileup reference values.  (NPV,mu)=(%.1f,%s) for %s",m_refNPV,m_refMuHist?"OK":"NULL",m_uncHistName.Data()));
}

PileupUncertaintyComponent::PileupUncertaintyComponent(const PileupUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_pileupType(toCopy.m_pileupType)
    , m_refNPV(toCopy.m_refNPV)
    , m_refMu(toCopy.m_refMu)
    , m_refNPVHist(toCopy.m_refNPVHist)
    , m_refMuHist(toCopy.m_refMuHist)
    , m_absEta(toCopy.m_absEta)
    , m_secondUncName(toCopy.m_secondUncName)
    , m_secondUncHist(NULL)
    , m_refType(toCopy.m_refType)
    , m_secondRefType(toCopy.m_secondRefType)
{
    ATH_MSG_DEBUG(Form("Creating copy of PileupUncertaintyComponent named %s",m_uncHistName.Data()));
    if (toCopy.m_secondUncHist)
        m_secondUncHist = new UncertaintyHistogram(*toCopy.m_secondUncHist);
}

PileupUncertaintyComponent* PileupUncertaintyComponent::clone() const
{
    return new PileupUncertaintyComponent(*this);
}

PileupUncertaintyComponent::~PileupUncertaintyComponent()
{
    JESUNC_SAFE_DELETE(m_secondUncHist);
}

StatusCode PileupUncertaintyComponent::initialize(TFile* histFile)
{
    // Call the base class first
    if (UncertaintyComponent::initialize(histFile).isFailure())
        return StatusCode::FAILURE;

    
    // Then ensure that the number of histograms matches what is expected for Pileup components
    if (m_pileupType == PileupComp::OffsetNPV && m_secondUncName != "")
    {
        ATH_MSG_ERROR("Expected a single histogram for OffsetNPV: " << getName().Data());
        return StatusCode::FAILURE;
    }
    else if (m_pileupType == PileupComp::OffsetMu && m_secondUncName != "")
    {
        ATH_MSG_ERROR("Expected a single histogram for OffsetMu: " << getName().Data());
        return StatusCode::FAILURE;
    }
    else if (m_pileupType == PileupComp::PtTerm && m_secondUncName == "")
    {
        ATH_MSG_ERROR("Expected two histograms for PtTerm: " << getName().Data());
        return StatusCode::FAILURE;
    }

    // Get the reference types
    if      (m_pileupType == PileupComp::OffsetNPV)
        m_refType = PileupRef_NPV;
    else if (m_pileupType == PileupComp::OffsetMu)
        m_refType = PileupRef_MU;
    else if (m_pileupType == PileupComp::RhoTopology)
        m_refType = PileupRef_NONE;
    else if (m_pileupType == PileupComp::PtTerm)
    {
        if      (m_uncHistName.Contains("NPV") && m_secondUncName.Contains("Mu"))
        {
            m_refType = PileupRef_NPV;
            m_secondRefType = PileupRef_MU;
        }
        else if (m_uncHistName.Contains("Mu") && m_secondUncName.Contains("NPV"))
        {
            m_refType = PileupRef_MU;
            m_secondRefType = PileupRef_NPV;
        }
        else
        {
            ATH_MSG_ERROR("Unexpected histogram naming scheme for PtTerm");
            return StatusCode::FAILURE;
        }
    }
    
    // Create the second histogram if applicable
    if (m_pileupType == PileupComp::PtTerm)
    {
        m_secondUncHist = new UncertaintyHistogram(m_secondUncName,m_interpolate);
        if (!m_secondUncHist)
        {
            ATH_MSG_ERROR("Failed to create second uncertainty histogram for component: " << getName().Data());
            return StatusCode::FAILURE;
        }
        if (m_secondUncHist->initialize(histFile).isFailure()) return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool PileupUncertaintyComponent::getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return !m_validHist ? true : getValidBool(m_validHist->getValue(jet.pt()*m_energyScale,m_absEta ? fabs(jet.eta()) : jet.eta()));
}

double PileupUncertaintyComponent::getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    double unc = getPileupWeight(jet,eInfo,m_refType)*m_uncHist->getValue(jet.pt()*m_energyScale,m_absEta ? fabs(jet.eta()) : jet.eta());
    if (m_secondUncHist) unc += getPileupWeight(jet,eInfo,m_secondRefType)*m_secondUncHist->getValue(jet.pt()*m_energyScale,m_absEta ? fabs(jet.eta()) : jet.eta());

    return unc;
}

double PileupUncertaintyComponent::getPileupWeight(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo, const PileupRefType refType) const
{
    static SG::AuxElement::Accessor<float> accNPV("NPV");

    double weight;
    const float mu  = eInfo.averageInteractionsPerCrossing();
    const float NPV = accNPV.isAvailable(eInfo) ? accNPV(eInfo) : -1;

    if (NPV <= 0)
    {
        ATH_MSG_ERROR("Unexpected number of primary vertices, does EventInfo contain NPV? (NPV="<<NPV<<")");
        return JESUNC_ERROR_CODE;
    }
    
    if      (refType == PileupRef_NPV)
        weight = NPV - (m_refNPVHist?m_refNPVHist->getValue(fabs(jet.eta())):m_refNPV);
    else if (refType == PileupRef_MU)
        weight = mu  - (m_refMuHist?m_refMuHist->getValue(fabs(jet.eta())):m_refMu);
    else if (refType == PileupRef_NONE)
        weight = 1;
    else
    {
        ATH_MSG_ERROR(Form("Unknown pileup referencetype for component %s",getName().Data()));
        return JESUNC_ERROR_CODE;
    }

    return weight;
}


} // end jet namespace


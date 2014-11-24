/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/PileupUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

#include "TFile.h"

namespace jet
{

const size_t PileupUncertaintyComponent::PT_TERM_NPV = 0;
const size_t PileupUncertaintyComponent::PT_TERM_MU  = 1;


//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

PileupUncertaintyComponent::PileupUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name))
    , m_pileupType(PileupComp::UNKNOWN)
    , m_refNPV(0)
    , m_refMu(0)
    , m_absEta(false)
    , m_NPVaccessor("NPV")
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

PileupUncertaintyComponent::PileupUncertaintyComponent( const ComponentHelper& component,
                                                        const float refNPV,
                                                        const float refMu
                                                        )
    : UncertaintyComponent(component)
    , m_pileupType(component.pileupType)
    , m_refNPV(refNPV)
    , m_refMu(refMu)
    , m_absEta(CompParametrization::IsAbsEta(component.parametrization))
    , m_NPVaccessor("NPV")
{
    ATH_MSG_DEBUG("Created PileupUncertaintyComponent named" << m_name.Data());
    
    // Ensure that the pileup type and ref values are sensible
    if (m_pileupType == PileupComp::UNKNOWN)
        ATH_MSG_FATAL("Pileup type is UNKNOWN: " << m_name.Data());
    if (m_refNPV <= 0 || m_refMu <= 0)
        ATH_MSG_FATAL(Form("Unusual pileup reference values.  (NPV,mu)=(%.1f,%.1f) for %s",m_refNPV,m_refMu,m_name.Data()));
}

PileupUncertaintyComponent::PileupUncertaintyComponent(const PileupUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_pileupType(toCopy.m_pileupType)
    , m_refNPV(toCopy.m_refNPV)
    , m_refMu(toCopy.m_refMu)
    , m_absEta(toCopy.m_absEta)
    , m_NPVaccessor(toCopy.m_NPVaccessor)
{
    ATH_MSG_DEBUG(Form("Creating copy of PileupUncertaintyComponent named %s",m_name.Data()));
}

PileupUncertaintyComponent* PileupUncertaintyComponent::clone() const
{
    return new PileupUncertaintyComponent(*this);
}

StatusCode PileupUncertaintyComponent::Initialize(const std::vector<TString>& histNames, TFile* histFile)
{
    std::vector<TString> validHistNames;
    return Initialize(histNames,validHistNames,histFile);
}

StatusCode PileupUncertaintyComponent::Initialize(const std::vector<TString>& histNames, const std::vector<TString>& validHistNames, TFile* histFile)
{
    // Call the base class first
    if (UncertaintyComponent::Initialize(histNames,validHistNames,histFile).isFailure())
        return StatusCode::FAILURE;

    // Then ensure that the number of histograms matches what is expected for Pileup components
    if (m_pileupType == PileupComp::OffsetNPV && m_histos.size() != 1)
    {
        ATH_MSG_ERROR("Expected a single histogram for OffsetNPV: " << m_name.Data());
        return StatusCode::FAILURE;
    }
    else if (m_pileupType == PileupComp::OffsetMu && m_histos.size() != 1)
    {
        ATH_MSG_ERROR("Expected a single histogram for OffsetMu: " << m_name.Data());
        return StatusCode::FAILURE;
    }
    else if (m_pileupType == PileupComp::PtTerm && m_histos.size() != 2)
    {
        ATH_MSG_ERROR("Expected two histograms for PtTerm: " << m_name.Data());
        return StatusCode::FAILURE;
    }

    // If this is the pT term, make sure the histograms are in the expected order (NPV, then mu)
    if (m_pileupType == PileupComp::PtTerm)
    {
        const TString name1 = m_histos.at(PT_TERM_NPV)->getName();
        const TString name2 = m_histos.at(PT_TERM_MU)->getName();
        if (name1.Contains("NPV") && name2.Contains("Mu"))
        {
            // Nothing to do - correct order
        }
        else if (name1.Contains("Mu") && name2.Contains("NPV"))
        {
            // Switch the order
            UncertaintyHistogram* temp = m_histos[PT_TERM_NPV];
            m_histos[PT_TERM_NPV] = m_histos[PT_TERM_MU];
            m_histos[PT_TERM_MU] = temp;
        }
        else
        {
            // Unexpected inputs
            ATH_MSG_ERROR("Component is PtTerm, but histogram names are unexpected (need to discriminate NPV vs Mu histograms): " << name1.Data() << " and " << name2.Data());
            return StatusCode::FAILURE;
        }
    }

    return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool PileupUncertaintyComponent::getValidity(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    return histo->getValidity(jet.pt()/1.e3,m_absEta ? fabs(jet.eta()) : jet.eta());
}

double PileupUncertaintyComponent::getUncertainty(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    return getPileupWeight(histo,eInfo) * histo->getUncertainty(jet.pt()/1.e3,m_absEta ? fabs(jet.eta()) : jet.eta());
}

bool PileupUncertaintyComponent::getValidUncertainty(const UncertaintyHistogram* histo, double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    bool success = histo->getValidUncertainty(unc,jet.pt()/1.e3,m_absEta ? fabs(jet.eta()) : jet.eta());
    if (success)
        unc *= getPileupWeight(histo,eInfo);
    return success;
}

double PileupUncertaintyComponent::getPileupWeight(const UncertaintyHistogram* histo, const xAOD::EventInfo& eInfo) const
{
    double weight;
    const float mu  = eInfo.averageInteractionsPerCrossing();
    const float NPV = m_NPVaccessor(eInfo);
    //const float NPV = eInfo.isAvailable<float>("NPV") ? eInfo.auxdata<float>("NPV") : getDefaultNPV();
    //const float NPV = eInfo.isAvailable<float>("NPV") ? eInfo.auxdata<float>("NPV") : -1;

    if (NPV <= 0)
    {
        ATH_MSG_ERROR("Unexpected number of primary vertices, does EventInfo contain NPV? (NPV="<<NPV<<")");
        return JESUNC_ERROR_CODE;
    }


    if (m_pileupType == PileupComp::OffsetNPV)
        weight = NPV - m_refNPV;
    else if (m_pileupType == PileupComp::OffsetMu)
        weight = mu - m_refMu;
    else if (m_pileupType == PileupComp::PtTerm)
    {
        if (histo == m_histos.at(PT_TERM_NPV))
            weight = NPV - m_refNPV;
        else if (histo == m_histos.at(PT_TERM_MU))
            weight = mu - m_refMu;
        else
        {
            ATH_MSG_ERROR(Form("Unexpected histogram %s for component %s",histo->getName().Data(),m_name.Data()));
            return JESUNC_ERROR_CODE;
        }
    }
    else if (m_pileupType == PileupComp::RhoTopology)
        weight = 1;
    else
    {
        ATH_MSG_ERROR(Form("Unknown pileup type for component %s",m_name.Data()));
        return JESUNC_ERROR_CODE;
    }

    return weight;
}

//unsigned PileupUncertaintyComponent::getDefaultNPV() const
//{
//    const xAOD::VertexContainer* vertices = NULL;
//    if (evtStore()->retrieve(vertices,"PrimaryVertices").isFailure())
//    {
//        ATH_MSG_ERROR("Failed to retrieve default NPV value from PrimaryVertices");
//        return 0;
//    }
//
//    unsigned NPV = 0;
//    xAOD::VertexContainer::const_iterator itr;
//    for (itr = vertices->begin(); itr != vertices->end(); ++itr)
//        if ( (*itr)->nTrackParticles() > 1)
//            NPV++;
//
//    return NPV;
//}


} // end jet namespace


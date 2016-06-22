/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/ValidityHistogram.h"
#include "JetUncertainties/Helpers.h"

#include <math.h>

namespace jet
{


//////////////////////////////////////////////////
//                                              //
//  Validity information helpers                //
//                                              //
//////////////////////////////////////////////////

class InfoHelper
{
    public:
        InfoHelper(const ValidityHistogram& validHist, const float energyScale, const CompMassDef::TypeEnum massDef)
            : m_validHist(validHist), m_energyScale(energyScale), m_massDef(massDef) {}
        virtual ~InfoHelper() {}
        virtual InfoHelper* clone() const = 0;
        
        virtual bool isValid(const xAOD::Jet& jet) const = 0;

    protected:
        const ValidityHistogram& m_validHist;
        const float m_energyScale;
        const CompMassDef::TypeEnum m_massDef;

        double getMassOverPt(const xAOD::Jet& jet) const;
};

class InfoHelperPt : public InfoHelper
{
    public:
        InfoHelperPt(const ValidityHistogram& validHist, const float energyScale)
            : InfoHelper(validHist,energyScale,CompMassDef::UNKNOWN) {}
        virtual InfoHelperPt* clone() const { return new InfoHelperPt(*this); }
        
        virtual bool isValid(const xAOD::Jet& jet) const
        {
            return m_validHist.getValue(jet.pt()*m_energyScale);
        }
};

class InfoHelperPtEta : public InfoHelper
{
    public:
        InfoHelperPtEta(const ValidityHistogram& validHist, const float energyScale)
            : InfoHelper(validHist,energyScale,CompMassDef::UNKNOWN) {}
        virtual InfoHelperPtEta* clone() const { return new InfoHelperPtEta(*this); }

        virtual bool isValid(const xAOD::Jet& jet) const
        {
            return m_validHist.getValue(jet.pt()*m_energyScale,jet.eta());
        }
};

class InfoHelperPtAbsEta : public InfoHelper
{
    public:
        InfoHelperPtAbsEta(const ValidityHistogram& validHist, const float energyScale)
            : InfoHelper(validHist,energyScale,CompMassDef::UNKNOWN) {}
        virtual InfoHelperPtAbsEta* clone() const { return new InfoHelperPtAbsEta(*this); }

        virtual bool isValid(const xAOD::Jet& jet) const
        {
            return m_validHist.getValue(jet.pt()*m_energyScale,fabs(jet.eta()));
        }
};

class InfoHelperPtMass : public InfoHelper
{
    public:
        InfoHelperPtMass(const ValidityHistogram& validHist, const float energyScale, const CompMassDef::TypeEnum massDef)
            : InfoHelper(validHist,energyScale,massDef) {}
        virtual InfoHelperPtMass* clone() const { return new InfoHelperPtMass(*this); }

        virtual bool isValid(const xAOD::Jet& jet) const
        {
            return m_validHist.getValue(jet.pt()*m_energyScale,getMassOverPt(jet));
        }
};

class InfoHelperPtMassEta : public InfoHelper
{
    public:
        InfoHelperPtMassEta(const ValidityHistogram& validHist, const float energyScale, const CompMassDef::TypeEnum massDef)
            : InfoHelper(validHist,energyScale,massDef) {}
        virtual InfoHelperPtMassEta* clone() const { return new InfoHelperPtMassEta(*this); }

        virtual bool isValid(const xAOD::Jet& jet) const
        {
            return m_validHist.getValue(jet.pt()*m_energyScale,getMassOverPt(jet),jet.eta());
        }
};

class InfoHelperPtMassAbsEta : public InfoHelper
{
    public:
        InfoHelperPtMassAbsEta(const ValidityHistogram& validHist, const float energyScale, const CompMassDef::TypeEnum massDef)
            : InfoHelper(validHist,energyScale,massDef) {}
        virtual InfoHelperPtMassAbsEta* clone() const { return new InfoHelperPtMassAbsEta(*this); }

        virtual bool isValid(const xAOD::Jet& jet) const
        {
            return m_validHist.getValue(jet.pt()*m_energyScale,getMassOverPt(jet),fabs(jet.eta()));
        }
};

double InfoHelper::getMassOverPt(const xAOD::Jet& jet) const
{
    static SG::AuxElement::ConstAccessor<xAOD::JetFourMom_t> scale(CompMassDef::getJetScaleString(m_massDef).Data());
    static SG::AuxElement::ConstAccessor<float> scaleTAMoment("JetTrackAssistedMassCalibrated");

    // UNKNOWN is just use the assigned scale
    if (m_massDef == CompMassDef::UNKNOWN)
        return jet.m()/jet.pt();
    
    // Check if the specified scale is available and return it if so
    if (scale.isAvailable(jet))
        return scale(jet).M()/scale(jet).Pt();
    // Fall-back on the TA moment as a float if applicable (TODO: temporary until JetCalibTools updated)
    if (m_massDef == CompMassDef::TAMass && scaleTAMoment.isAvailable(jet))
        return scaleTAMoment(jet)/jet.pt();
    // Fall-back on the calo mass as the 4-vec if applicable (TODO: temporary until JetCalibTools updated)
    if (m_massDef == CompMassDef::CaloMass)
        return jet.m()/jet.pt();

    // Specified scale is not available, error
    return JESUNC_ERROR_CODE;

}




//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

ValidityHistogram::ValidityHistogram(const std::string& histName, const CompParametrization::TypeEnum parametrization, const float energyScale, const CompMassDef::TypeEnum massDef)
    : UncertaintyHistogram(histName,false)
    , m_isInit(false)
    , m_param(parametrization)
    , m_energyScale(energyScale)
    , m_massDef(massDef)
    , m_helper(NULL)
{
    ATH_MSG_DEBUG(Form("Creating ValidityHistogram named %s",getName().Data()));
}

ValidityHistogram::ValidityHistogram(const TString& histName, const CompParametrization::TypeEnum parametrization, const float energyScale, const CompMassDef::TypeEnum massDef)
    : ValidityHistogram(std::string(histName.Data()),parametrization,energyScale,massDef)
{ }

ValidityHistogram::ValidityHistogram(const char* histName, const CompParametrization::TypeEnum parametrization, const float energyScale, const CompMassDef::TypeEnum massDef)
    : ValidityHistogram(std::string(histName),parametrization,energyScale,massDef)
{ }

ValidityHistogram::ValidityHistogram(const ValidityHistogram& toCopy)
    : UncertaintyHistogram(toCopy)
    , m_isInit(toCopy.m_isInit)
    , m_param(toCopy.m_param)
    , m_energyScale(toCopy.m_energyScale)
    , m_massDef(toCopy.m_massDef)
    , m_helper(NULL)
{
    ATH_MSG_DEBUG("Creating copy of ValidityHistogram named " << getName().Data());
    m_helper = toCopy.m_helper->clone();
}

ValidityHistogram::~ValidityHistogram()
{
    JESUNC_SAFE_DELETE(m_helper);
}

StatusCode ValidityHistogram::initialize(TFile* histFile)
{
    // Ensure it wasn't already initialized
    if (m_isInit)
    {
        ATH_MSG_ERROR("ValidityHistogram was already initialized: " << getName().Data());
        return StatusCode::FAILURE;
    }

    // Initialize the base class
    if (UncertaintyHistogram::initialize(histFile).isFailure())
        return StatusCode::FAILURE;

    // Ensure that the parametrization is sensible
    switch (m_param)
    {
        case CompParametrization::UNKNOWN:
            ATH_MSG_ERROR("ValidityHistogram parametrization is UNKNOWN: " << getName().Data());
            return StatusCode::FAILURE;
        case CompParametrization::Pt:
            //1D
            if (getNumDim() != 1)
            {
                ATH_MSG_ERROR("ValidityHistogram is " << getNumDim() << "D, but parametrization is 1D: " << CompParametrization::enumToString(m_param).Data());
                return StatusCode::FAILURE;
            }
            break;
        case CompParametrization::PtEta:
        case CompParametrization::PtAbsEta:
        case CompParametrization::PtMass:
            // 2D
            if (getNumDim() != 2)
            {
                ATH_MSG_ERROR("ValidityHistogram is " << getNumDim() << "D, but parametrization is 2D: " << CompParametrization::enumToString(m_param).Data());
                return StatusCode::FAILURE;
            }
            break;
        case CompParametrization::PtMassEta:
        case CompParametrization::PtMassAbsEta:
            // 3D
            if (getNumDim() != 3)
            {
                ATH_MSG_ERROR("ValidityHistogram is " << getNumDim() << "D, but parametrization is 3D: " << CompParametrization::enumToString(m_param).Data());
                return StatusCode::FAILURE;
            }
            break;
        default:
            ATH_MSG_ERROR("ValidityHistogram named \"" << getName().Data() << "\" had an unpexted parametrization: " << CompParametrization::enumToString(m_param).Data());
            return StatusCode::FAILURE;
    }

    // Figure out which function helper we want once, right now, so we don't need to switch every time
    switch (m_param)
    {
        case CompParametrization::Pt:
            m_helper = new InfoHelperPt(*this,m_energyScale);
            break;
        case CompParametrization::PtEta:
            m_helper = new InfoHelperPtEta(*this,m_energyScale);
            break;
        case CompParametrization::PtAbsEta:
            m_helper = new InfoHelperPtAbsEta(*this,m_energyScale);
            break;
        case CompParametrization::PtMass:
            m_helper = new InfoHelperPtMass(*this,m_energyScale,m_massDef);
            break;
        case CompParametrization::PtMassEta:
            m_helper = new InfoHelperPtMassEta(*this,m_energyScale,m_massDef);
            break;
        case CompParametrization::PtMassAbsEta:
            m_helper = new InfoHelperPtMassAbsEta(*this,m_energyScale,m_massDef);
            break;
        default:
            ATH_MSG_ERROR("ValidityHistogram named \"" << getName().Data() << "\" was not prepared to handle the provided parametrization: " << CompParametrization::enumToString(m_param).Data());
            return StatusCode::FAILURE;
    }

    // Done
    m_isInit = true;
    return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////
//                                              //
//  Validity information access                 //
//                                              //
//////////////////////////////////////////////////


bool ValidityHistogram::getValidity(const xAOD::Jet& jet) const
{
    if (!m_isInit)
    {
        ATH_MSG_ERROR("You must initialize the ValidityHistogram before calling getIsValid: " << getName().Data());
        return false;
    }

    const double validity = m_helper->isValid(jet);
    if (validity < 1.e-5 && validity > -1.e-5)
        return false;
    if (validity < 1+1.e-5 && validity > 1-1.e-5)
        return true;
    
    // Validity is neither 0 nor 1
    ATH_MSG_ERROR("ValidityHistogram \"" << getName().Data() << "\" out of expected range: " << validity);
    return false;
}


} // end jet namespace


/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

        double getAbsMass(const xAOD::Jet& jet) const;
        double getMassOverPt(const xAOD::Jet& jet) const;
        double getMassOverE(const xAOD::Jet& jet) const;
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

class InfoHelperPtAbsMass : public InfoHelper
{
    public:
        InfoHelperPtAbsMass(const ValidityHistogram& validHist, const float energyScale, const CompMassDef::TypeEnum massDef)
            : InfoHelper(validHist,energyScale,massDef) {}
        virtual InfoHelperPtAbsMass* clone() const { return new InfoHelperPtAbsMass(*this); }

        virtual bool isValid(const xAOD::Jet& jet) const
        {
            return m_validHist.getValue(jet.pt()*m_energyScale,getAbsMass(jet));
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

class InfoHelperPtAbsMassEta : public InfoHelper
{
    public:
        InfoHelperPtAbsMassEta(const ValidityHistogram& validHist, const float energyScale, const CompMassDef::TypeEnum massDef)
            : InfoHelper(validHist,energyScale,massDef) {}
        virtual InfoHelperPtAbsMassEta* clone() const { return new InfoHelperPtAbsMassEta(*this); }

        virtual bool isValid(const xAOD::Jet& jet) const
        {
            return m_validHist.getValue(jet.pt()*m_energyScale,getAbsMass(jet)*m_energyScale,jet.eta());
        }
};

class InfoHelperPtAbsMassAbsEta : public InfoHelper
{
    public:
        InfoHelperPtAbsMassAbsEta(const ValidityHistogram& validHist, const float energyScale, const CompMassDef::TypeEnum massDef)
            : InfoHelper(validHist,energyScale,massDef) {}
        virtual InfoHelperPtAbsMassAbsEta* clone() const { return new InfoHelperPtAbsMassAbsEta(*this); }

        virtual bool isValid(const xAOD::Jet& jet) const
        {
            return m_validHist.getValue(jet.pt()*m_energyScale,getAbsMass(jet)*m_energyScale,fabs(jet.eta()));
        }
};

class InfoHelpereLOGmOe : public InfoHelper
{
    public:
        InfoHelpereLOGmOe(const ValidityHistogram& validHist, const float energyScale, const CompMassDef::TypeEnum massDef)
            : InfoHelper(validHist,energyScale,massDef) {}
        virtual InfoHelpereLOGmOe* clone() const { return new InfoHelpereLOGmOe(*this); }

        virtual bool isValid(const xAOD::Jet& jet) const
        {
            return m_validHist.getValue(jet.e()*m_energyScale,log(getMassOverE(jet)));
        }
};

class InfoHelpereLOGmOeEta : public InfoHelper
{
    public:
        InfoHelpereLOGmOeEta(const ValidityHistogram& validHist, const float energyScale, const CompMassDef::TypeEnum massDef)
            : InfoHelper(validHist,energyScale,massDef) {}
        virtual InfoHelpereLOGmOeEta* clone() const { return new InfoHelpereLOGmOeEta(*this); }

        virtual bool isValid(const xAOD::Jet& jet) const
        {
            return m_validHist.getValue(jet.e()*m_energyScale,log(getMassOverE(jet)),jet.eta());
        }
};

class InfoHelpereLOGmOeAbsEta : public InfoHelper
{
    public:
        InfoHelpereLOGmOeAbsEta(const ValidityHistogram& validHist, const float energyScale, const CompMassDef::TypeEnum massDef)
            : InfoHelper(validHist,energyScale,massDef) {}
        virtual InfoHelpereLOGmOeAbsEta* clone() const { return new InfoHelpereLOGmOeAbsEta(*this); }

        virtual bool isValid(const xAOD::Jet& jet) const
        {
            return m_validHist.getValue(jet.e()*m_energyScale,log(getMassOverE(jet)),fabs(jet.eta()));
        }
};


double InfoHelper::getAbsMass(const xAOD::Jet& jet) const
{
    // Check for the simple case (where we want the default four-vector itself)
    if (m_massDef == CompMassDef::UNKNOWN || m_massDef == CompMassDef::FourVecMass)
        return jet.m();
    
    // Not the simple case, check for the specified four-vector instead and return it if it is available
    JetFourMomAccessor scale(CompMassDef::getJetScaleString(m_massDef).Data());
    if (scale.isAvailable(jet))
        return scale(jet).M();

    // Fall-back on the TA moment as a float if applicable (legacy support)
    SG::AuxElement::ConstAccessor<float> scaleTAMoment("JetTrackAssistedMassCalibrated");
    if (m_massDef == CompMassDef::TAMass && scaleTAMoment.isAvailable(jet))
        return scaleTAMoment(jet);

    // Fall-back on the calo mass as the 4-vec if applicable (legacy support)
    if (m_massDef == CompMassDef::CaloMass)
        return jet.m();

    // Specified scale is not available, error
    return JESUNC_ERROR_CODE;
}

double InfoHelper::getMassOverPt(const xAOD::Jet& jet) const
{
    // Check for the simple case (where we want the default four-vector itself)
    if (m_massDef == CompMassDef::UNKNOWN || m_massDef == CompMassDef::FourVecMass)
        return jet.m()/jet.pt();
    
    // Not the simple case, check for the specified four-vector instead and return it if it is available
    JetFourMomAccessor scale(CompMassDef::getJetScaleString(m_massDef).Data());
    if (scale.isAvailable(jet))
        return scale(jet).M()/scale(jet).Pt();
    
    // Fall-back on the TA moment as a float if applicable (legacy support)
    SG::AuxElement::ConstAccessor<float> scaleTAMoment("JetTrackAssistedMassCalibrated");
    if (m_massDef == CompMassDef::TAMass && scaleTAMoment.isAvailable(jet))
        return scaleTAMoment(jet)/jet.pt();
    
    // Fall-back on the calo mass as the 4-vec if applicable (legacy support)
    if (m_massDef == CompMassDef::CaloMass)
        return jet.m()/jet.pt();

    // Specified scale is not available, error
    return JESUNC_ERROR_CODE;

}

double InfoHelper::getMassOverE(const xAOD::Jet& jet) const
{
    // Check for the simple case (where we want the default four-vector itself)
    if (m_massDef == CompMassDef::UNKNOWN || m_massDef == CompMassDef::FourVecMass)
        return jet.m()/jet.e();

    // Not the simple case, check for the specified four-vector instead and return it if it is available
    JetFourMomAccessor scale(CompMassDef::getJetScaleString(m_massDef).Data());
    if (scale.isAvailable(jet))
        return scale(jet).M()/scale(jet).E();
    
    // Fall-back on the TA moment as a float if applicable (legacy support)
    SG::AuxElement::ConstAccessor<float> scaleTAMoment("JetTrackAssistedMassCalibrated");
    if (m_massDef == CompMassDef::TAMass && scaleTAMoment.isAvailable(jet))
        return scaleTAMoment(jet)/jet.e();
    
    // Fall-back on the calo mass as the 4-vec if applicable (legacy support)
    if (m_massDef == CompMassDef::CaloMass)
        return jet.m()/jet.e();

    // Specified scale is not available, error
    return JESUNC_ERROR_CODE;

}




//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

ValidityHistogram::ValidityHistogram(const std::string& histName, const CompParametrization::TypeEnum parametrization, const float energyScale, const CompMassDef::TypeEnum massDef)
    : UncertaintyHistogram(histName,Interpolate::None)
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
        case CompParametrization::PtAbsMass:
        case CompParametrization::PtMass:
        case CompParametrization::eLOGmOe:
        case CompParametrization::PtLOGPtMassForTagSF:
            // 2D
            if (getNumDim() != 2)
            {
                ATH_MSG_ERROR("ValidityHistogram is " << getNumDim() << "D, but parametrization is 2D: " << CompParametrization::enumToString(m_param).Data());
                return StatusCode::FAILURE;
            }
            break;
        case CompParametrization::PtMassEta:
        case CompParametrization::PtMassAbsEta:
        case CompParametrization::PtAbsMassEta:
        case CompParametrization::PtAbsMassAbsEta:
        case CompParametrization::eLOGmOeEta:
        case CompParametrization::eLOGmOeAbsEta:
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
        case CompParametrization::PtAbsMass:
            m_helper = new InfoHelperPtAbsMass(*this,m_energyScale,m_massDef);
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
        case CompParametrization::PtAbsMassEta:
            m_helper = new InfoHelperPtAbsMassEta(*this,m_energyScale,m_massDef);
            break;
        case CompParametrization::PtAbsMassAbsEta:
            m_helper = new InfoHelperPtAbsMassAbsEta(*this,m_energyScale,m_massDef);
            break;
        case CompParametrization::eLOGmOe:
            m_helper = new InfoHelpereLOGmOe(*this,m_energyScale,m_massDef);
            break;
        case CompParametrization::eLOGmOeEta:
            m_helper = new InfoHelpereLOGmOeEta(*this,m_energyScale,m_massDef);
            break;
        case CompParametrization::eLOGmOeAbsEta:
            m_helper = new InfoHelpereLOGmOeAbsEta(*this,m_energyScale,m_massDef);
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


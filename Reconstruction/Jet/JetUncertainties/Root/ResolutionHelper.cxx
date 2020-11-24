/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#include "JetUncertainties/ResolutionHelper.h"
#include "JetUncertainties/Helpers.h"

#include "TEnv.h"
#include "TFile.h"

#include <stdexcept>

namespace jet
{

ResolutionHelper::ResolutionHelper(const std::string& name, const std::string& jetDef)
    : asg::AsgMessaging(name)
    , m_name(name)
    , m_jetDef(jetDef)
    , m_isInit(false)
    , m_smearOnlyMC(false)
    , m_ptNomHistData(NULL)
    , m_ptNomParamData(CompParametrization::UNKNOWN)
    , m_ptNomMassDefData(CompMassDef::UNKNOWN)
    , m_ptNomHistMC(NULL)
    , m_ptNomParamMC(CompParametrization::UNKNOWN)
    , m_ptNomMassDefMC(CompMassDef::UNKNOWN)
    , m_fvNomHistData(NULL)
    , m_fvNomParamData(CompParametrization::UNKNOWN)
    , m_fvNomMassDefData(CompMassDef::UNKNOWN)
    , m_fvNomHistMC(NULL)
    , m_fvNomParamMC(CompParametrization::UNKNOWN)
    , m_fvNomMassDefMC(CompMassDef::UNKNOWN)
    , m_mQCDNomHistData(NULL)
    , m_mQCDNomParamData(CompParametrization::UNKNOWN)
    , m_mQCDNomMassDefData(CompMassDef::UNKNOWN)
    , m_mQCDNomHistMC(NULL)
    , m_mQCDNomParamMC(CompParametrization::UNKNOWN)
    , m_mQCDNomMassDefMC(CompMassDef::UNKNOWN)
    , m_mWZNomHistData(NULL)
    , m_mWZNomParamData(CompParametrization::UNKNOWN)
    , m_mWZNomMassDefData(CompMassDef::UNKNOWN)
    , m_mWZNomHistMC(NULL)
    , m_mWZNomParamMC(CompParametrization::UNKNOWN)
    , m_mWZNomMassDefMC(CompMassDef::UNKNOWN)
    , m_mHbbNomHistData(NULL)
    , m_mHbbNomParamData(CompParametrization::UNKNOWN)
    , m_mHbbNomMassDefData(CompMassDef::UNKNOWN)
    , m_mHbbNomHistMC(NULL)
    , m_mHbbNomParamMC(CompParametrization::UNKNOWN)
    , m_mHbbNomMassDefMC(CompMassDef::UNKNOWN)
    , m_mTopNomHistData(NULL)
    , m_mTopNomParamData(CompParametrization::UNKNOWN)
    , m_mTopNomMassDefData(CompMassDef::UNKNOWN)
    , m_mTopNomHistMC(NULL)
    , m_mTopNomParamMC(CompParametrization::UNKNOWN)
    , m_mTopNomMassDefMC(CompMassDef::UNKNOWN)
{ }

ResolutionHelper::ResolutionHelper(const ResolutionHelper& toCopy)
    : asg::AsgMessaging(toCopy.m_name)
    , m_name(toCopy.m_name)
    , m_jetDef(toCopy.m_jetDef)
    , m_isInit(toCopy.m_isInit)
    , m_smearOnlyMC(toCopy.m_smearOnlyMC)
    , m_ptNomHistData(!toCopy.m_ptNomHistData ? NULL : new UncertaintyHistogram(*toCopy.m_ptNomHistData))
    , m_ptNomParamData(toCopy.m_ptNomParamData)
    , m_ptNomMassDefData(toCopy.m_ptNomMassDefData)
    , m_ptNomHistMC(!toCopy.m_ptNomHistMC ? NULL : new UncertaintyHistogram(*toCopy.m_ptNomHistMC))
    , m_ptNomParamMC(toCopy.m_ptNomParamMC)
    , m_ptNomMassDefMC(toCopy.m_ptNomMassDefMC)
    , m_fvNomHistData(!toCopy.m_fvNomHistData ? NULL : new UncertaintyHistogram(*toCopy.m_fvNomHistData))
    , m_fvNomParamData(toCopy.m_fvNomParamData)
    , m_fvNomMassDefData(toCopy.m_fvNomMassDefData)
    , m_fvNomHistMC(!toCopy.m_fvNomHistMC ? NULL : new UncertaintyHistogram(*toCopy.m_fvNomHistMC))
    , m_fvNomParamMC(toCopy.m_fvNomParamMC)
    , m_fvNomMassDefMC(toCopy.m_fvNomMassDefMC)
    , m_mQCDNomHistData(!toCopy.m_mQCDNomHistData ? NULL : new UncertaintyHistogram(*toCopy.m_mQCDNomHistData))
    , m_mQCDNomParamData(toCopy.m_mQCDNomParamData)
    , m_mQCDNomMassDefData(toCopy.m_mQCDNomMassDefData)
    , m_mQCDNomHistMC(!toCopy.m_mQCDNomHistMC ? NULL : new UncertaintyHistogram(*toCopy.m_mQCDNomHistMC))
    , m_mQCDNomParamMC(toCopy.m_mQCDNomParamMC)
    , m_mQCDNomMassDefMC(toCopy.m_mQCDNomMassDefMC)
    , m_mWZNomHistData(!toCopy.m_mWZNomHistData ? NULL : new UncertaintyHistogram(*toCopy.m_mWZNomHistData))
    , m_mWZNomParamData(toCopy.m_mWZNomParamData)
    , m_mWZNomMassDefData(toCopy.m_mWZNomMassDefData)
    , m_mWZNomHistMC(!toCopy.m_mWZNomHistMC ? NULL : new UncertaintyHistogram(*toCopy.m_mWZNomHistMC))
    , m_mWZNomParamMC(toCopy.m_mWZNomParamMC)
    , m_mWZNomMassDefMC(toCopy.m_mWZNomMassDefMC)
    , m_mHbbNomHistData(!toCopy.m_mHbbNomHistData ? NULL : new UncertaintyHistogram(*toCopy.m_mHbbNomHistData))
    , m_mHbbNomParamData(toCopy.m_mHbbNomParamData)
    , m_mHbbNomMassDefData(toCopy.m_mHbbNomMassDefData)
    , m_mHbbNomHistMC(!toCopy.m_mHbbNomHistMC ? NULL : new UncertaintyHistogram(*toCopy.m_mHbbNomHistMC))
    , m_mHbbNomParamMC(toCopy.m_mHbbNomParamMC)
    , m_mHbbNomMassDefMC(toCopy.m_mHbbNomMassDefMC)
    , m_mTopNomHistData(!toCopy.m_mTopNomHistData ? NULL : new UncertaintyHistogram(*toCopy.m_mTopNomHistData))
    , m_mTopNomParamData(toCopy.m_mTopNomParamData)
    , m_mTopNomMassDefData(toCopy.m_mTopNomMassDefData)
    , m_mTopNomHistMC(!toCopy.m_mTopNomHistMC ? NULL : new UncertaintyHistogram(*toCopy.m_mTopNomHistMC))
    , m_mTopNomParamMC(toCopy.m_mTopNomParamMC)
    , m_mTopNomMassDefMC(toCopy.m_mTopNomMassDefMC)
{ }

ResolutionHelper::~ResolutionHelper()
{
    JESUNC_SAFE_DELETE(m_ptNomHistData);
    JESUNC_SAFE_DELETE(m_ptNomHistMC);
    JESUNC_SAFE_DELETE(m_fvNomHistData);
    JESUNC_SAFE_DELETE(m_fvNomHistMC);
    JESUNC_SAFE_DELETE(m_mQCDNomHistData);
    JESUNC_SAFE_DELETE(m_mQCDNomHistMC);
    JESUNC_SAFE_DELETE(m_mWZNomHistData);
    JESUNC_SAFE_DELETE(m_mWZNomHistMC);
    JESUNC_SAFE_DELETE(m_mHbbNomHistData);
    JESUNC_SAFE_DELETE(m_mHbbNomHistMC);
    JESUNC_SAFE_DELETE(m_mTopNomHistData);
    JESUNC_SAFE_DELETE(m_mTopNomHistMC);
}

StatusCode ResolutionHelper::parseInput(TEnv& settings, TFile* histFile, const TString& key, const TString& defaultValue, UncertaintyHistogram*& hist, CompParametrization::TypeEnum& param, CompMassDef::TypeEnum& massDef, const TString& MCtype)
{
    // Get the string
    TString value = settings.GetValue(key,defaultValue);

    // Check the trivial case
    if (value == "")
        return StatusCode::SUCCESS;

    // Ensure it matches the expected format
    if (!value.Contains(","))
    {
        ATH_MSG_ERROR("Key of " << key.Data() << " has an unexpected value format (missing comma): " << value.Data());
        return StatusCode::FAILURE;
    }

    // Split the string
    std::vector<TString> splitValue = utils::vectorize<TString>(value,", ");
    if (splitValue.size() < 3)
    {
        ATH_MSG_ERROR("Key of " << key.Data() << " has unexpected value format (less than 3 strings): " << value.Data());
        return StatusCode::FAILURE;
    }
    else if (splitValue.size() > 4)
    {
        ATH_MSG_ERROR("Key of " << key.Data() << " has unexpected value format (more than 4 strings): " << value.Data());
        return StatusCode::FAILURE;
    }
    
    // Ensure that the parametrization is valid
    param = CompParametrization::stringToEnum(splitValue.at(1));
    if (param == CompParametrization::UNKNOWN)
    {
        ATH_MSG_ERROR("Key of " << key.Data() << " has unexpected parametrization value: " << splitValue.at(1));
        return StatusCode::FAILURE;
    }

    // Ensure that the interpolation type is valid
    Interpolate::TypeEnum interp = Interpolate::stringToEnum(splitValue.at(2));
    if (interp == Interpolate::UNKNOWN)
    {
        ATH_MSG_ERROR("Key of " << key.Data() << " has unexpected interpolation type: " << splitValue.at(2));
        return StatusCode::FAILURE;
    }

    // If this is a mass parametrization, ensure that a mass definition was specified
    if (CompParametrization::includesMass(param))
    {
        if (splitValue.size() != 4)
        {
            ATH_MSG_ERROR("Key of " << key.Data() << " has unexpected value format (missing mass definition): " << value.Data());
            return StatusCode::FAILURE;
        }
        massDef = CompMassDef::stringToEnum(splitValue.at(3));
        if (massDef == CompMassDef::UNKNOWN)
        {
            ATH_MSG_ERROR("Key of " << key.Data() << " has unexpected mass definition: " << splitValue.at(3));
            return StatusCode::FAILURE;
        }
    }

    // Replace generic MCTYPE string with the user-specified type if applicable
    TString histName = splitValue.at(0);
    histName.ReplaceAll("MCTYPE",MCtype);

    // Create the histogram
    hist = new UncertaintyHistogram(histName+"_"+m_jetDef.c_str(),interp);

    // Initialize the histogram
    if (hist->initialize(histFile).isFailure())
        return StatusCode::FAILURE;

    // Print out that we successfully read the input
    ATH_MSG_INFO(Form("  %s: \"%s\"",key.Data(),value.Data()));

    return StatusCode::SUCCESS;
}

StatusCode ResolutionHelper::initialize(TEnv& settings, TFile* histFile, const TString& MCtype)
{
    if (m_isInit)
    {
        ATH_MSG_ERROR("Blocking double-initialization: " << m_name);
        return StatusCode::FAILURE;
    }
    
    // Read nominal resolution information from the configuration file
    // In many cases (especially configs before summer 2018), there are no inputs to find
    // As such, it is perfectly normal behaviour to read zero inputs from the file
    // However, there are some expectations
    //  There should be no cases of data resolution without MC resolution
    //  There can be cases of MC resolution without data resolution
    //  Histograms and their parametrizations must be paired (potentially also with mass)

    // Expected format:
    //      KeyString: HistName,Parametrization
    //      KeyString: HistName,Parametrization,MassDef
    //  Example:
    //      NominalPtResData: PtResData,PtAbsEta
    //          Histogram name is PtResData_JETDEF
    //          Parametrization is pT vs |eta|
    //      NominalMassResMC: MassResMC,PtMass,Calo
    //          Histogram name is MassResMC_JETDEF
    //          Parametrization is pT vs m/pt
    //          Mass definition is calorimeter (not track-assisted or combined)


    // Start with nominal pT resolution
    if (parseInput(settings,histFile,"NominalPtResData","",m_ptNomHistData,m_ptNomParamData,m_ptNomMassDefData,MCtype).isFailure())
        return StatusCode::FAILURE;
    if (parseInput(settings,histFile,"NominalPtResMC","",m_ptNomHistMC,m_ptNomParamMC,m_ptNomMassDefMC,MCtype).isFailure())
        return StatusCode::FAILURE;
    if (m_ptNomHistData && !m_ptNomHistMC)
    {
        ATH_MSG_ERROR("There should never be a nominal data resolution without a nominal MC resolution (pT)");
        return StatusCode::FAILURE;
    }

    // Now the nominal four-vector resolution
    if (parseInput(settings,histFile,"NominalFourVecResData","",m_fvNomHistData,m_fvNomParamData,m_fvNomMassDefData,MCtype).isFailure())
        return StatusCode::FAILURE;
    if (parseInput(settings,histFile,"NominalFourVecResMC","",m_fvNomHistMC,m_fvNomParamMC,m_fvNomMassDefMC,MCtype).isFailure())
        return StatusCode::FAILURE;
    if (m_fvNomHistData && !m_fvNomHistMC)
    {
        ATH_MSG_ERROR("There should never be a nominal data resolution without a nominal MC resolution (four-vector)");
        return StatusCode::FAILURE;
    }

    // Now the nominal mass resolution (topology-dependent)
    // For simplicity, this is done with separate histograms for each one
    // Lots of code duplication, but time is of the essence, and it works

    // QCD topology
    if (parseInput(settings,histFile,"NominalMassResDataQCD","",m_mQCDNomHistData,m_mQCDNomParamData,m_mQCDNomMassDefData,MCtype).isFailure())
        return StatusCode::FAILURE;
    if (parseInput(settings,histFile,"NominalMassResMCQCD","",m_mQCDNomHistMC,m_mQCDNomParamMC,m_mQCDNomMassDefMC,MCtype).isFailure())
        return StatusCode::FAILURE;
    if (m_mQCDNomHistData && !m_mQCDNomHistMC)
    {
        ATH_MSG_ERROR("There should never be a nominal data resolution without a nominal MC resolution (four-vector)");
        return StatusCode::FAILURE;
    }

    // WZ topology
    if (parseInput(settings,histFile,"NominalMassResDataWZ","",m_mWZNomHistData,m_mWZNomParamData,m_mWZNomMassDefData,MCtype).isFailure())
        return StatusCode::FAILURE;
    if (parseInput(settings,histFile,"NominalMassResMCWZ","",m_mWZNomHistMC,m_mWZNomParamMC,m_mWZNomMassDefMC,MCtype).isFailure())
        return StatusCode::FAILURE;
    if (m_mWZNomHistData && !m_mWZNomHistMC)
    {
        ATH_MSG_ERROR("There should never be a nominal data resolution without a nominal MC resolution (four-vector)");
        return StatusCode::FAILURE;
    }

    // Hbb topology
    if (parseInput(settings,histFile,"NominalMassResDataHbb","",m_mHbbNomHistData,m_mHbbNomParamData,m_mHbbNomMassDefData,MCtype).isFailure())
        return StatusCode::FAILURE;
    if (parseInput(settings,histFile,"NominalMassResMCHbb","",m_mHbbNomHistMC,m_mHbbNomParamMC,m_mHbbNomMassDefMC,MCtype).isFailure())
        return StatusCode::FAILURE;
    if (m_mHbbNomHistData && !m_mHbbNomHistMC)
    {
        ATH_MSG_ERROR("There should never be a nominal data resolution without a nominal MC resolution (four-vector)");
        return StatusCode::FAILURE;
    }

    // Top topology
    if (parseInput(settings,histFile,"NominalMassResDataTop","",m_mTopNomHistData,m_mTopNomParamData,m_mTopNomMassDefData,MCtype).isFailure())
        return StatusCode::FAILURE;
    if (parseInput(settings,histFile,"NominalMassResMCTop","",m_mTopNomHistMC,m_mTopNomParamMC,m_mTopNomMassDefMC,MCtype).isFailure())
        return StatusCode::FAILURE;
    if (m_mTopNomHistData && !m_mTopNomHistMC)
    {
        ATH_MSG_ERROR("There should never be a nominal data resolution without a nominal MC resolution (four-vector)");
        return StatusCode::FAILURE;
    }

    // Check if the user has specified if this is full correlations or MC-only
    TString smearType = settings.GetValue("ResolutionSmearOnlyMC","");
    if (smearType != "")
    {
        if (!smearType.CompareTo("true",TString::kIgnoreCase))
            m_smearOnlyMC = true;
        else if (!smearType.CompareTo("false",TString::kIgnoreCase))
            m_smearOnlyMC = false;
        else
        {
            ATH_MSG_ERROR("The value of ResolutionSmearOnlyMC doesn't look like the expected boolean: " << smearType.Data());
            return StatusCode::FAILURE;
        }
        ATH_MSG_INFO(Form("  ResolutionSmearOnlyMC: \"%s\"",m_smearOnlyMC ? "true" : "false"));
    }

    // If we are also smearing data, we need the data histograms for any MC histogram
    if (!m_smearOnlyMC)
    {
        if (m_ptNomHistMC && !m_ptNomHistData)
        {
            ATH_MSG_ERROR("Requested full smearing correlations (both data and MC), but only provided the MC nominal histogram for pT");
            return StatusCode::FAILURE;
        }
        if (m_fvNomHistMC && !m_fvNomHistMC)
        {
            ATH_MSG_ERROR("Requested full smearing correlations (both data and MC), but only provided the MC nominal histogram for fourvec");
            return StatusCode::FAILURE;
        }
        if (m_mQCDNomHistMC && !m_mQCDNomHistData)
        {
            ATH_MSG_ERROR("Requested full smearing correlations (both data and MC), but only provided the MC nominal histogram for mQCD");
            return StatusCode::FAILURE;
        }
        if (m_mWZNomHistMC && !m_mWZNomHistData)
        {
            ATH_MSG_ERROR("Requested full smearing correlations (both data and MC), but only provided the MC nominal histogram for mWZ");
            return StatusCode::FAILURE;
        }
        if (m_mHbbNomHistMC && !m_mHbbNomHistData)
        {
            ATH_MSG_ERROR("Requested full smearing correlations (both data and MC), but only provided the MC nominal histogram for mHbb");
            return StatusCode::FAILURE;
        }
        if (m_mTopNomHistMC && !m_mTopNomHistData)
        {
            ATH_MSG_ERROR("Requested full smearing correlations (both data and MC), but only provided the MC nominal histogram for mTop");
            return StatusCode::FAILURE;
        }
    }


    m_isInit = true;
    return StatusCode::SUCCESS;
}

std::tuple<const UncertaintyHistogram*,CompParametrization::TypeEnum,CompMassDef::TypeEnum> ResolutionHelper::getNominalResolution(const CompScaleVar::TypeEnum smearType, const JetTopology::TypeEnum topology, const bool readMC) const
{
    // First get the resolution histogram and parametrization
    const jet::UncertaintyHistogram* resolution = NULL;
    CompParametrization::TypeEnum param         = CompParametrization::UNKNOWN;
    CompMassDef::TypeEnum massDef               = CompMassDef::UNKNOWN;
    
    if (!m_isInit)
    {
        ATH_MSG_ERROR("Asking for the nominal resolution before initialization");
        return std::tuple<const UncertaintyHistogram*,CompParametrization::TypeEnum,CompMassDef::TypeEnum>(NULL,CompParametrization::UNKNOWN,CompMassDef::UNKNOWN);
    }


    switch (smearType)
    {
        case CompScaleVar::MassRes:
        case CompScaleVar::MassResAbs:
            switch (topology)
            {
                case JetTopology::QCD:
                    if (readMC)
                    {
                        resolution = m_mQCDNomHistMC;
                        param      = m_mQCDNomParamMC;
                        massDef    = m_mQCDNomMassDefMC;
                    }
                    else
                    {
                        resolution = m_mQCDNomHistData;
                        param      = m_mQCDNomParamData;
                        massDef    = m_mQCDNomMassDefData;
                    }
                    break;

                case JetTopology::WZ:
                    if (readMC)
                    {
                        resolution = m_mWZNomHistMC;
                        param      = m_mWZNomParamMC;
                        massDef    = m_mWZNomMassDefMC;
                    }
                    else
                    {
                        resolution = m_mWZNomHistData;
                        param      = m_mWZNomParamData;
                        massDef    = m_mWZNomMassDefData;
                    }
                    break;

                case JetTopology::Hbb:
                    if (readMC)
                    {
                        resolution = m_mHbbNomHistMC;
                        param      = m_mHbbNomParamMC;
                        massDef    = m_mHbbNomMassDefMC;
                    }
                    else
                    {
                        resolution = m_mHbbNomHistData;
                        param      = m_mHbbNomParamData;
                        massDef    = m_mHbbNomMassDefData;
                    }
                    break;

                case JetTopology::Top:
                    if (readMC)
                    {
                        resolution = m_mTopNomHistMC;
                        param      = m_mTopNomParamMC;
                        massDef    = m_mTopNomMassDefMC;
                    }
                    else
                    {
                        resolution = m_mTopNomHistData;
                        param      = m_mTopNomParamData;
                        massDef    = m_mTopNomMassDefData;
                    }
                    break;

                case JetTopology::MIXED:
                    // We shouldn't read this, as it was checked at a higher level
                    // Just to be safe, check it and return error code
                    ATH_MSG_ERROR("Mass resolution depends on a single jet topology, not a mixed topology");
                    return std::tuple<const UncertaintyHistogram*,CompParametrization::TypeEnum,CompMassDef::TypeEnum>(NULL,CompParametrization::UNKNOWN,CompMassDef::UNKNOWN);

                default:
                    // We shouldn't read this, as it was checked at a higher level
                    // Just to be safe, check it and return error code
                    ATH_MSG_ERROR("Mass resolution depends on the jet topology, which was not specified");
                    return std::tuple<const UncertaintyHistogram*,CompParametrization::TypeEnum,CompMassDef::TypeEnum>(NULL,CompParametrization::UNKNOWN,CompMassDef::UNKNOWN);
            }
            break;

        case CompScaleVar::PtRes:
        case CompScaleVar::PtResAbs:
            if (readMC)
            {
                resolution = m_ptNomHistMC;
                param      = m_ptNomParamMC;
                massDef    = m_ptNomMassDefMC;
            }
            else
            {
                resolution = m_ptNomHistData;
                param      = m_ptNomParamData;
                massDef    = m_ptNomMassDefData;
            }
            break;
        
        case CompScaleVar::FourVecRes:
        case CompScaleVar::FourVecResAbs:
            if (readMC)
            {
                resolution = m_fvNomHistMC;
                param      = m_fvNomParamMC;
                massDef    = m_fvNomMassDefMC;
            }
            else
            {
                resolution = m_fvNomHistData;
                param      = m_fvNomParamData;
                massDef    = m_fvNomMassDefData;
            }
            break;

        default:
            // This is not a resolution uncertainty component
            // We should not get here
            // Print an erorr and return error code
            ATH_MSG_ERROR("Asked for the smearing factor for a non-resolution component");
            return std::tuple<const UncertaintyHistogram*,CompParametrization::TypeEnum,CompMassDef::TypeEnum>(NULL,CompParametrization::UNKNOWN,CompMassDef::UNKNOWN);
    }
    return std::tuple<const UncertaintyHistogram*,CompParametrization::TypeEnum,CompMassDef::TypeEnum>(resolution,param,massDef);
}

bool ResolutionHelper::hasRelevantInfo(const CompScaleVar::TypeEnum type, const JetTopology::TypeEnum topology) const
{
    if (!m_isInit)
    {
        throw std::runtime_error("Asking for nominal resolution information before initialization");
        return false;
    }

    // Check that the nominal MC histograms for the specified type exist
    //      We have already checked that the parametrizations exist for each histogram
    //      We have already checked that the data histograms exist for each MC if relevant
    switch (type)
    {
        case CompScaleVar::PtRes:
        case CompScaleVar::PtResAbs:
            return m_ptNomHistMC != NULL;

        case CompScaleVar::FourVecRes:
        case CompScaleVar::FourVecResAbs:
            return m_fvNomHistMC != NULL;

        case CompScaleVar::MassRes:
        case CompScaleVar::MassResAbs:
            switch (topology)
            {
                case JetTopology::QCD:
                    return m_mQCDNomHistMC != NULL;

                case JetTopology::WZ:
                    return m_mWZNomHistMC  != NULL;

                case JetTopology::Hbb:
                    return m_mHbbNomHistMC != NULL;

                case JetTopology::Top:
                    return m_mTopNomHistMC != NULL;

                default:
                    throw std::runtime_error(Form("Unexpected topology type, cannot determine if relevant info exists: %s",JetTopology::enumToString(topology).Data()));
                    return false;
            }

        default:
            throw std::runtime_error(Form("Unexpected variable type, cannot determine if relevant info exists: %s",CompScaleVar::enumToString(type).Data()));
            return false;
    }
}

} // end jet namespace


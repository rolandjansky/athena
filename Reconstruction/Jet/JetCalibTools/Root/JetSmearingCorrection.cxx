/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "JetCalibTools/CalibrationMethods/JetSmearingCorrection.h"
#include "PathResolver/PathResolver.h"
#include "JetCalibTools/RootHelpers.h"

JetSmearingCorrection::JetSmearingCorrection(const std::string name)
    : JetCalibrationToolBase(name)
    , m_config(NULL)
    , m_jetAlgo("")
    , m_calibAreaTag("")
    , m_dev(false)
    , m_rand()
    , m_jetStartScale("")
    , m_jetOutScale("")
    , m_smearType(SmearType::UNKNOWN)
    , m_histType(HistType::UNKNOWN)
    , m_interpType(InterpType::UNKNOWN)
    , m_smearResolutionMC()
    , m_smearResolutionData()
{ }

JetSmearingCorrection::JetSmearingCorrection(const std::string& name, TEnv* config, TString jetAlgo, TString calibAreaTag, bool dev)
    : JetCalibrationToolBase(name)
    , m_config(config)
    , m_jetAlgo(jetAlgo)
    , m_calibAreaTag(calibAreaTag)
    , m_dev(dev)
    , m_rand()
    , m_jetStartScale("")
    , m_jetOutScale("")
    , m_smearType(SmearType::UNKNOWN)
    , m_histType(HistType::UNKNOWN)
    , m_interpType(InterpType::UNKNOWN)
    , m_smearResolutionMC()
    , m_smearResolutionData()
{ }
 
JetSmearingCorrection::~JetSmearingCorrection()
{ }

StatusCode JetSmearingCorrection::initializeTool(const std::string&)
{
    ATH_MSG_INFO("Initializing the jet smearing correction tool");

    if (!m_config)
    {
        ATH_MSG_FATAL("Config file not specified.  Aborting.");
        return StatusCode::FAILURE;
    }
    if (m_jetAlgo == "")
    {
        ATH_MSG_FATAL("No jet algorithm specified.  Aborting.");
        return StatusCode::FAILURE;
    }

    // Get the starting and ending jet scales
    m_jetStartScale = m_config->GetValue("JSCStartingScale","JetGSCScaleMomentum");
    m_jetOutScale   = m_config->GetValue("JSCOutScale","JetSmearedMomentum");
    ATH_MSG_INFO("Reading from " << m_jetStartScale.Data() << " and writing to " << m_jetOutScale.Data());

    // Get information about how to smear
    TString smearType = m_config->GetValue("SmearType","");
    if (smearType == "")
    {
        ATH_MSG_FATAL("No jet smearing type was specified.  Aborting.");
        return StatusCode::FAILURE;
    }
    else if (!smearType.CompareTo("pt",TString::kIgnoreCase))
        m_smearType = SmearType::Pt;
    else if (!smearType.CompareTo("mass",TString::kIgnoreCase))
        m_smearType = SmearType::Mass;
    else if (!smearType.CompareTo("FourVec",TString::kIgnoreCase))
        m_smearType = SmearType::FourVec;
    else
    {
        ATH_MSG_FATAL("Unrecognized jet smearing type: " << smearType.Data());
        return StatusCode::FAILURE;
    }

    // Determine the histogram parametrization
    TString histType = m_config->GetValue("SmearingCorrectionHistType","");
    if (histType == "")
    {
        ATH_MSG_FATAL("No jet smearing histogram parametrization was specified.  Aborting.");
        return StatusCode::FAILURE;
    }
    else if (!histType.CompareTo("pt",TString::kIgnoreCase))
        m_histType = HistType::Pt;
    else if (!histType.CompareTo("PtEta",TString::kIgnoreCase))
        m_histType = HistType::PtEta;
    else if (!histType.CompareTo("PtAbsEta",TString::kIgnoreCase))
        m_histType = HistType::PtAbsEta;
    else
    {
        ATH_MSG_FATAL("Unrecognized jet smearing histogram parametrization: " << histType.Data());
        return StatusCode::FAILURE;
    }

    // Determine the histogram interpolation strategy
    TString interpType = m_config->GetValue("SmearingCorrectionInterpType","");
    if (interpType == "")
    {
        ATH_MSG_FATAL("No jet smearing histogram interpolation type was specified.  Aborting.");
        return StatusCode::FAILURE;
    }
    else if (!interpType.CompareTo("full",TString::kIgnoreCase))
        m_interpType = InterpType::Full;
    else if (!interpType.CompareTo("none",TString::kIgnoreCase))
        m_interpType = InterpType::None;
    else if (!interpType.CompareTo("onlyx",TString::kIgnoreCase))
        m_interpType = InterpType::OnlyX;
    else if (!interpType.CompareTo("onlyy",TString::kIgnoreCase))
        m_interpType = InterpType::OnlyY;
    else
    {
        ATH_MSG_FATAL("Unrecognized jet smearing interpolation type: " << interpType.Data());
        return StatusCode::FAILURE;
    }

    // Find the ROOT file containing the smearing histogram, path comes from the config file
    TString smearingFile = m_config->GetValue("SmearingCorrectionFile","");
    if (smearingFile == "")
    {
        ATH_MSG_FATAL("No jet smearing correction file specified.  Aborting.");
        return StatusCode::FAILURE;
    }

    // Find the name of the MC nominal resolution histogram, from the config file
    TString smearingHistNameMC = m_config->GetValue("SmearingHistNameResolutionMC","");
    if (smearingHistNameMC == "")
    {
        ATH_MSG_FATAL("No MC jet smearing histogram name specified.  Aborting.");
        return StatusCode::FAILURE;
    }
    TString smearingHistNameData = m_config->GetValue("SmearingHistNameResolutionData","");
    if (smearingHistNameData == "")
    {
        ATH_MSG_FATAL("No data jet smearing histogram name specified.  Aborting.");
        return StatusCode::FAILURE;
    }

    // Open the histogram file
    if (m_dev)
    {
        smearingFile.Remove(0,33);
        smearingFile.Insert(0,"JetCalibTools/");
    }
    else
        smearingFile.Insert(14,m_calibAreaTag);
    
    TString fileName = PathResolverFindCalibFile(smearingFile.Data());
    std::unique_ptr<TFile> inputFile(TFile::Open(fileName));
    if (!inputFile || inputFile->IsZombie())
    {
        ATH_MSG_FATAL("Cannot open jet smearing correction file: " << fileName);
        return StatusCode::FAILURE;
    }

    //  Retrieve the histogram froms the file
    m_smearResolutionMC = std::unique_ptr<TH1>(dynamic_cast<TH1*>(inputFile->Get(smearingHistNameMC)));
    if (!m_smearResolutionMC)
    {
        ATH_MSG_FATAL("Failed to get specified histogram from the file: " << smearingHistNameMC.Data());
        return StatusCode::FAILURE;
    }
    m_smearResolutionMC->SetDirectory(0);

    m_smearResolutionData = std::unique_ptr<TH1>(dynamic_cast<TH1*>(inputFile->Get(smearingHistNameData)));
    if (!m_smearResolutionData)
    {
        ATH_MSG_FATAL("Failed to get specified histogram from the file: " << smearingHistNameData.Data());
        return StatusCode::FAILURE;
    }
    m_smearResolutionData->SetDirectory(0);

    // Done with the input file, close it
    inputFile->Close();

    // Ensure that the histogram we retrieved has the right number of dimensions
    // It must match the dimensionality of the parametrization
    switch (m_histType)
    {
        case HistType::Pt:
            if (m_smearResolutionMC->GetDimension() != 1)
            {
                ATH_MSG_FATAL("Specified MC histogram has " << m_smearResolutionMC->GetDimension() << " dimensions, but parametrization expects 1 dimension");
                return StatusCode::FAILURE;
            }
            if (m_smearResolutionData->GetDimension() != 1)
            {
                ATH_MSG_FATAL("Specified data histogram has " << m_smearResolutionData->GetDimension() << " dimensions, but parametrization expects 1 dimension");
                return StatusCode::FAILURE;
            }
            break;

        case HistType::PtEta:
        case HistType::PtAbsEta:
            if (m_smearResolutionMC->GetDimension() != 2)
            {
                ATH_MSG_FATAL("Specified MC histogram has " << m_smearResolutionMC->GetDimension() << " dimensions, but parametrization expects 2 dimensions");
                return StatusCode::FAILURE;
            }
            if (m_smearResolutionData->GetDimension() != 2)
            {
                ATH_MSG_FATAL("Specified data histogram has " << m_smearResolutionData->GetDimension() << " dimensions, but parametrization expects 2 dimensions");
                return StatusCode::FAILURE;
            }
            break;

        default:
            ATH_MSG_FATAL("Read the histogram, but the parametrization is UNKNOWN");
            return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode JetSmearingCorrection::readHisto(double& returnValue, TH1* histo, double x) const
{
    // Ensure that the histogram exists
    if (!histo)
    {
        ATH_MSG_ERROR("Unable to read histogram - address is NULL");
        return StatusCode::FAILURE;
    }

    // Check dimensionality just to be safe
    if (histo->GetDimension() != 1)
    {
        ATH_MSG_ERROR("Blocking reading of a " << histo->GetDimension() << "D histogram as a 1D histogram");
        return StatusCode::FAILURE;
    }

    // Ensure we are within boundaries
    const double minX = histo->GetXaxis()->GetBinLowEdge(1);
    const double maxX = histo->GetXaxis()->GetBinLowEdge(histo->GetNbinsX()+1);
    if ( x >= maxX )
        x = maxX - 1.e-6;
    else if ( x <= minX )
        x = minX + 1.e-6;

    // Get the result, interpolating as appropriate
    switch (m_interpType)
    {
        case InterpType::Full:
        case InterpType::OnlyX:
            returnValue = histo->Interpolate(x);
            break;
        
        case InterpType::None:
            returnValue = histo->GetBinContent(histo->GetXaxis()->FindBin(x));
            break;

        default:
            ATH_MSG_ERROR("Unsupported interpolation type for a 1D histogram");
            return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode JetSmearingCorrection::readHisto(double& returnValue, TH1* histo, double x, double y) const
{
    // Ensure that the histogram exists
    if (!histo)
    {
        ATH_MSG_ERROR("Unable to read histogram - address is NULL");
        return StatusCode::FAILURE;
    }

    // Check dimensionality just to be safe
    if (histo->GetDimension() != 2)
    {
        ATH_MSG_ERROR("Blocking reading of a " << histo->GetDimension() << "D histogram as a 2D histogram");
        return StatusCode::FAILURE;
    }

    // Ensure we are within boundaries
    const double minX = histo->GetXaxis()->GetBinLowEdge(1);
    const double maxX = histo->GetXaxis()->GetBinLowEdge(histo->GetNbinsX()+1);
    if ( x >= maxX )
        x = maxX - 1.e-6;
    else if ( x <= minX )
        x = minX + 1.e-6;
    const double minY = histo->GetYaxis()->GetBinLowEdge(1);
    const double maxY = histo->GetYaxis()->GetBinLowEdge(histo->GetNbinsY()+1);
    if ( y >= maxY )
        y = maxY - 1.e-6;
    else if ( y <= minY )
        y = minY + 1.e-6;

    // Get the result, interpolating as appropriate
    // We need to use this as a 2D histogram to have access to the projection methods
    // We also need a histogram to project into
    TH2* localHist  = NULL;
    TH1* projection = NULL;
    switch (m_interpType)
    {
        case InterpType::Full:
            returnValue = histo->Interpolate(x,y);
            break;
        
        case InterpType::None:
            returnValue = histo->GetBinContent(histo->GetXaxis()->FindBin(x),histo->GetYaxis()->FindBin(y));
            break;

        case InterpType::OnlyX:
            localHist = dynamic_cast<TH2*>(histo);
            if (!localHist)
            {
                ATH_MSG_ERROR("Could not convert the histogram to a TH2, please check inputs");
                return StatusCode::FAILURE;
            }
            projection = localHist->ProjectionX("projx_2D",histo->GetYaxis()->FindBin(y),histo->GetYaxis()->FindBin(y));
            returnValue = projection->Interpolate(x);
            delete projection;
            break;

        case InterpType::OnlyY:
            localHist = dynamic_cast<TH2*>(histo);
            if (!localHist)
            {
                ATH_MSG_ERROR("Could not convert the histogram to a TH2, please check inputs");
                return StatusCode::FAILURE;
            }
            projection = localHist->ProjectionY("projy_2D",histo->GetXaxis()->FindBin(x),histo->GetXaxis()->FindBin(x));
            returnValue = projection->Interpolate(y);
            delete projection;
            break;

        default:
            ATH_MSG_ERROR("Unsupported interpolation type for a 2D histogram");
            return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode JetSmearingCorrection::readHisto(double& returnValue, TH1* histo, double x, double y, double z) const
{
    // Ensure that the histogram exists
    if (!histo)
    {
        ATH_MSG_ERROR("Unable to read histogram - address is NULL");
        return StatusCode::FAILURE;
    }

    // Check dimensionality just to be safe
    if (histo->GetDimension() != 3)
    {
        ATH_MSG_ERROR("Blocking reading of a " << histo->GetDimension() << "D histogram as a 3D histogram");
        return StatusCode::FAILURE;
    }

    // Ensure we are within boundaries
    const double minX = histo->GetXaxis()->GetBinLowEdge(1);
    const double maxX = histo->GetXaxis()->GetBinLowEdge(histo->GetNbinsX()+1);
    if ( x >= maxX )
        x = maxX - 1.e-6;
    else if ( x <= minX )
        x = minX + 1.e-6;
    const double minY = histo->GetYaxis()->GetBinLowEdge(1);
    const double maxY = histo->GetYaxis()->GetBinLowEdge(histo->GetNbinsY()+1);
    if ( y >= maxY )
        y = maxY - 1.e-6;
    else if ( y <= minY )
        y = minY + 1.e-6;
    const double minZ = histo->GetZaxis()->GetBinLowEdge(1);
    const double maxZ = histo->GetZaxis()->GetBinLowEdge(histo->GetNbinsZ()+1);
    if ( z >= maxZ )
        z = maxZ - 1.e-6;
    else if ( z <= minZ )
        z = minZ + 1.e-6;

    // Get the result, interpolating as appropriate
    // We need to use this as a 3D histogram to have access to the projection methods
    // We also need a histogram to project into
    TH3* localHist  = NULL;
    TH1* projection = NULL;
    switch (m_interpType)
    {
        case InterpType::Full:
            returnValue = histo->Interpolate(x,y,z);
            break;
        
        case InterpType::None:
            returnValue = histo->GetBinContent(histo->GetXaxis()->FindBin(x),histo->GetYaxis()->FindBin(y),histo->GetZaxis()->FindBin(z));
            break;

        case InterpType::OnlyX:
            localHist = dynamic_cast<TH3*>(histo);
            if (!localHist)
            {
                ATH_MSG_ERROR("Could not convert the histogram to a TH3, please check inputs");
                return StatusCode::FAILURE;
            }
            projection = localHist->ProjectionX("projx_3D",histo->GetYaxis()->FindBin(y),histo->GetYaxis()->FindBin(y),histo->GetZaxis()->FindBin(z),histo->GetZaxis()->FindBin(z));
            returnValue = projection->Interpolate(x);
            delete projection;
            break;

        case InterpType::OnlyY:
            localHist = dynamic_cast<TH3*>(histo);
            if (!localHist)
            {
                ATH_MSG_ERROR("Could not convert the histogram to a TH3, please check inputs");
                return StatusCode::FAILURE;
            }
            projection = localHist->ProjectionY("projy_3D",histo->GetXaxis()->FindBin(x),histo->GetXaxis()->FindBin(x),histo->GetZaxis()->FindBin(z),histo->GetZaxis()->FindBin(z));
            returnValue = projection->Interpolate(y);
            delete projection;
            break;

        default:
            ATH_MSG_ERROR("Unsupported interpolation type for a 3D histogram");
            return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode JetSmearingCorrection::getSigmaSmear(xAOD::Jet& jet, double& sigmaSmear) const
{
    /*
        Nominal jet smearing
            If sigma_data > sigma_MC, then we want to smear MC to match data
            If sigma_data < sigma_MC, then we do not want to smear data to match MC
            The second case is instead the source of an uncertainty (see JetUncertainties)

        To make MC agree with data:
            if (sigma_data > sigma_MC) then sigma_smear^2 = sigma_data^2 - sigma_MC^2
            if (sigma_data < sigma_MC) then do nothing
            Smearing using a Gaussian centered at 1 and with a width of sigma_smear

        Note that data is never smeared, as blocked in JetCalibrationTool.cxx
    */

    double resolutionMC = 0;
    if (getNominalResolutionMC(jet,resolutionMC).isFailure())
        return StatusCode::FAILURE;

    double resolutionData = 0;
    if (getNominalResolutionData(jet,resolutionData).isFailure())
        return StatusCode::FAILURE;

    // Nominal smearing only if data resolution is larger than MC resolution
    // This is because we want to smear the MC to match the data
    // if MC is larger than data, don't make the nominal data worse, so smear is 0
    if (resolutionMC < resolutionData)
        sigmaSmear = sqrt(resolutionData - resolutionMC);
    else
        sigmaSmear = 0;

    return StatusCode::SUCCESS;
}

StatusCode JetSmearingCorrection::getNominalResolution(const xAOD::Jet& jet, TH1* histo, double& resolution) const
{
    double localRes = 0;
    switch (m_histType)
    {
        case HistType::Pt:
            if (readHisto(localRes,histo,jet.pt()).isFailure())
                return StatusCode::FAILURE;
            break;

        case HistType::PtEta:
            if (readHisto(localRes,histo,jet.pt(),jet.eta()).isFailure())
                return StatusCode::FAILURE;
            break;

        case HistType::PtAbsEta:
            if (readHisto(localRes,histo,jet.pt(),fabs(jet.eta())).isFailure())
                return StatusCode::FAILURE;
            break;

        default:
            // We should never reach this, it was checked during initialization
            ATH_MSG_ERROR("Cannot get the nominal resolution, the smearing histogram type was not set");
            return StatusCode::FAILURE;
    }

    // If we got here, everything went well
    // Set the resolution and return success
    resolution = localRes;
    return StatusCode::SUCCESS;
}

StatusCode JetSmearingCorrection::getNominalResolutionData(const xAOD::Jet& jet, double& resolution) const
{
    return getNominalResolution(jet,m_smearResolutionData.get(),resolution);
}

StatusCode JetSmearingCorrection::getNominalResolutionMC(const xAOD::Jet& jet, double& resolution) const
{
    return getNominalResolution(jet,m_smearResolutionMC.get(),resolution);
}



StatusCode JetSmearingCorrection::calibrateImpl(xAOD::Jet& jet, JetEventInfo&) const
{
    // Apply the jet smearing correction

    // Calculate the smearing width to use
    double sigmaSmear = 0;
    if (getSigmaSmear(jet,sigmaSmear).isFailure())
        return StatusCode::FAILURE;

    // Set the random seed deterministically using jet phi
    m_rand.SetSeed(1.e+5*fabs(jet.phi()));

    // Get the Gaussian-distributed random number
    const double smearingFactor = m_rand.Gaus(1.,sigmaSmear);

    // Apply the smearing factor to the jet as appropriate
    xAOD::JetFourMom_t calibP4 = jet.jetP4();
    switch (m_smearType)
    {
        case SmearType::Pt:
            calibP4 = xAOD::JetFourMom_t(jet.pt()*smearingFactor,jet.eta(),jet.phi(),jet.m());
            break;

        case SmearType::Mass:
            calibP4 = xAOD::JetFourMom_t(jet.pt(),jet.eta(),jet.phi(),smearingFactor*jet.m());
            break;

        case SmearType::FourVec:
            calibP4 = xAOD::JetFourMom_t(jet.pt()*smearingFactor,jet.eta(),jet.phi(),jet.m()*smearingFactor);
            break;

        default:
            // We should never reach this, it was checked during initialization
            ATH_MSG_ERROR("Cannot smear the jet, the smearing type was not set");
            return StatusCode::FAILURE;
    }

    // Set the output scale
    jet.setAttribute<xAOD::JetFourMom_t>(m_jetOutScale.Data(),calibP4);
    jet.setJetP4(calibP4);


    return StatusCode::SUCCESS;
}



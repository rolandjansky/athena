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
    , m_smearingHist(NULL)
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
    , m_smearingHist(NULL)
{ }
 
JetSmearingCorrection::~JetSmearingCorrection()
{
    // Nothing to do so far
}

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

    // Find the ROOT file containing the smearing histogram, path comes from the config file
    TString smearingFile = m_config->GetValue("SmearingCorrectionFile","");
    if (smearingFile == "")
    {
        ATH_MSG_FATAL("No jet smearing correction file specified.  Aborting.");
        return StatusCode::FAILURE;
    }

    // Find the name of the smearing histogram, from the config file
    TString smearingHistName = m_config->GetValue("SmearingCorrectionHistName","");
    if (smearingHistName == "")
    {
        ATH_MSG_FATAL("No jet smearing histogram name specified.  Aborting.");
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
    TFile* inputFile = TFile::Open(fileName);
    if (!inputFile || inputFile->IsZombie())
    {
        ATH_MSG_FATAL("Cannot open jet smearing correction file: " << fileName);
        return StatusCode::FAILURE;
    }

    //  Retrieve the histogram from the file
    m_smearingHist = dynamic_cast<TH1*>(inputFile->Get(smearingHistName));
    if (!m_smearingHist)
    {
        ATH_MSG_FATAL("Failed to get specified histogram from the file: " << smearingHistName.Data());
        return StatusCode::FAILURE;
    }

    // Ensure that the histogram we retrieved has the right number of dimensions
    // It must match the dimensionality of the parametrization
    switch (m_histType)
    {
        case HistType::Pt:
            if (m_smearingHist->GetDimension() != 1)
            {
                ATH_MSG_FATAL("Specified histogram has " << m_smearingHist->GetDimension() << " dimensions, but parametrization expects 1 dimension");
                return StatusCode::FAILURE;
            }
            break;

        case HistType::PtEta:
        case HistType::PtAbsEta:
            if (m_smearingHist->GetDimension() != 2)
            {
                ATH_MSG_FATAL("Specified histogram has " << m_smearingHist->GetDimension() << " dimensions, but parametrization expects 2 dimensions");
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

    // All good!
    returnValue = histo->Interpolate(x);
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

    // All good!
    returnValue = histo->Interpolate(x);
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

    // All good!
    returnValue = histo->Interpolate(x);
    return StatusCode::SUCCESS;
}

StatusCode JetSmearingCorrection::getSigmaSmear(xAOD::Jet& jet, double& sigmaSmear) const
{
    switch (m_histType)
    {
        case HistType::Pt:
            return readHisto(sigmaSmear,m_smearingHist,jet.pt());

        case HistType::PtEta:
            return readHisto(sigmaSmear,m_smearingHist,jet.pt(),jet.eta());

        case HistType::PtAbsEta:
            return readHisto(sigmaSmear,m_smearingHist,jet.pt(),fabs(jet.eta()));

        default:
            // We should never reach this, it was checked during initialization
            ATH_MSG_ERROR("Cannot get the smearing factor, the smearing histogram type was not set");
            return StatusCode::FAILURE;
    }
}


StatusCode JetSmearingCorrection::calibrateImpl(xAOD::Jet& jet, JetEventInfo&) const
{
    // Apply the jet smearing correction

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



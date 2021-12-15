/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JetCalibTools/CalibrationMethods/GenericHistScaleCorrection.h"
#include "PathResolver/PathResolver.h"
#include "JetCalibTools/RootHelpers.h"

GenericHistScaleCorrection::GenericHistScaleCorrection(const std::string name)
    : JetCalibrationToolBase{name}
    , m_config{nullptr}, m_jetAlgo{""}, m_calibAreaTag{""}, m_dev{false}
    , m_jetOutScale{""}, m_hist{nullptr}
{ }

GenericHistScaleCorrection::GenericHistScaleCorrection(const std::string& name, TEnv* config, TString jetAlgo, TString calibAreaTag, bool dev)
    : JetCalibrationToolBase{name}
    , m_config{config}, m_jetAlgo{jetAlgo}, m_calibAreaTag{calibAreaTag}, m_dev{dev}
    , m_jetOutScale{""}, m_hist{nullptr}
{ }

GenericHistScaleCorrection::~GenericHistScaleCorrection()
{ }

StatusCode GenericHistScaleCorrection::initializeTool(const std::string& name)
{

    ATH_MSG_INFO("Initializing the GenericHistScaleCorrection tool: " << name);

    // Constructor argument checks
    if (m_jetAlgo.EqualTo(""))
    {
        ATH_MSG_ERROR("No jet algorithm specified.  Aborting.");
        return StatusCode::FAILURE;
    }

    if (!m_config)
    {
        ATH_MSG_ERROR("Config file not specified.  Aborting.");
        return StatusCode::FAILURE;
    }

    // Read inputs from the config file
    m_jetStartScale   = m_config->GetValue("GenericHistCorrStartingScale","");
    m_jetOutScale     = m_config->GetValue("GenericHistCorrOutScale","");
    TString histFile  = m_config->GetValue("GenericHistCorrInputFile","");
    TString histName  = m_config->GetValue("GenericHistCorrHistogram","");

    if (m_jetStartScale=="")
    {
        ATH_MSG_ERROR("The input scale was not specified: " << name);
        return StatusCode::FAILURE;
    }
    if (m_jetOutScale=="")
    {
        ATH_MSG_ERROR("The output scale was not specified: " << name);
        return StatusCode::FAILURE;
    }
    if (histFile=="")
    {
        ATH_MSG_ERROR("The input file was not specified: " << name);
        return StatusCode::FAILURE;
    }
    if (histName=="")
    {
        ATH_MSG_ERROR("The histogram name was not specified: " << name);
        return StatusCode::FAILURE;
    }

    // Open the input file
    TString inFile = histFile;
    if (m_dev)
    {
        // Copied from other correction tools
        inFile.Remove(0,33);
        inFile.Insert(0,"JetCalibTools/");
    }
    else
    {
        if (m_calibAreaTag == "")
        {
            ATH_MSG_ERROR("The calibration area tag was not specified: " << name);
            return StatusCode::FAILURE;
        }

        // Copied from other correction tools
        inFile.Insert(14,m_calibAreaTag);
    }
    inFile = PathResolverFindCalibFile(inFile.Data());
    std::unique_ptr<TFile> inFileHandle = std::unique_ptr<TFile>(TFile::Open(inFile));
    if (!inFileHandle)
    {
        ATH_MSG_ERROR("Cannot open the input file: " << inFile.Data());
        return StatusCode::FAILURE;
    }

    // Get the histogram from the file
    m_hist = std::unique_ptr<TH1>(dynamic_cast<TH1*>(inFileHandle->Get(histName)));
    if (!m_hist)
    {
        ATH_MSG_ERROR("Cannot retrieve the histogram: " << histName.Data());
        return StatusCode::FAILURE;
    }
    m_hist->SetDirectory(0);
    inFileHandle->Close();

    // Currently we only support 1D histograms, so check that now
    if (m_hist->GetDimension() != 1)
    {
        ATH_MSG_ERROR("Currently this tool only supports 1D histograms, not " << m_hist->GetDimension() << "D");
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("GenericHistScaleCorrection prepared for " << m_jetStartScale << " --> " << m_jetOutScale);
    return StatusCode::SUCCESS;

}


StatusCode GenericHistScaleCorrection::calibrateImpl(xAOD::Jet& jet, JetEventInfo&) const
{
    // Following what is done in other tools
    xAOD::JetFourMom_t jetStartP4;
    ATH_CHECK( setStartP4(jet) );
    jetStartP4 = jet.jetP4();

    xAOD::JetFourMom_t calibP4 = jet.jetP4();

    double pT_corr = jetStartP4.pt();
    double correctionFactor = 1;
    if (getCorrectionFactor(jet,correctionFactor) == StatusCode::FAILURE)
        return StatusCode::FAILURE;
    pT_corr *= correctionFactor;

    // Starting implementation is pre-defined as a pT scale factor for a 1D histogram vs pT
    // This is due to the original use case
    // However, this would be quite easy to extend to other use cases by adding more options to the config file
    TLorentzVector TLVjet;
    TLVjet.SetPtEtaPhiM(pT_corr,jetStartP4.eta(),jetStartP4.phi(),jetStartP4.mass());
    calibP4.SetPxPyPzE(TLVjet.Px(), TLVjet.Py(), TLVjet.Pz(), TLVjet.E());

    // Transfer calibrated jet properties to the Jet object
    jet.setAttribute<xAOD::JetFourMom_t>(m_jetOutScale.Data(),calibP4);
    jet.setJetP4(calibP4);

    return StatusCode::SUCCESS;
}


StatusCode GenericHistScaleCorrection::getCorrectionFactor(const xAOD::Jet& jet, double& factor) const
{
    // Starting implementation is pre-defined as a pT scale factor for a 1D histogram vs pT
    // This is due to the original use case
    // However, this would be quite easy to extend to other use cases by adding more options to the config file
    
    factor = m_hist->Interpolate(enforceAxisRange(*(m_hist->GetXaxis()),jet.pt()/m_GeV));
    return StatusCode::SUCCESS;
}


double GenericHistScaleCorrection::enforceAxisRange(const TAxis& axis, const double inputValue) const
{
    // edgeOffset should be chosen to be above floating point precision, but negligible compared to the bin size
    // An offset of edgeOffset*binWidth is therefore irrelevant for physics as the values don't change fast (but avoids edge errors)
    static constexpr double edgeOffset {1.e-4};

    // Root histogram binning:
    //  bin 0 = underflow bin
    //  bin 1 = first actual bin
    //  bin N = last actual bin
    //  bin N+1 = overflow bin
    const int numBins {axis.GetNbins()};
    const int binIndex {axis.FindFixBin(inputValue)};
    
    if (binIndex < 1)
        // Return just inside the range of the first real bin
        return axis.GetBinLowEdge(1) + edgeOffset*axis.GetBinWidth(1);
    if (binIndex > numBins)
        // Return just inside the range of the last real bin
        // Don't use the upper edge as floating point can make it roll into the next bin (which is overflow)
        // Instead, use the last bin width to go slightly within the boundary
        return axis.GetBinLowEdge(numBins) + (1-edgeOffset)*axis.GetBinWidth(numBins);
    return inputValue;
}



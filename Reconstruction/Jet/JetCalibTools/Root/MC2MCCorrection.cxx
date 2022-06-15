/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "JetCalibTools/CalibrationMethods/MC2MCCorrection.h"
#include "PathResolver/PathResolver.h"
#include "JetCalibTools/RootHelpers.h"

MC2MCCorrection::MC2MCCorrection(const std::string name)
    : JetCalibrationToolBase(name)
    , m_config(NULL)
    , m_jetAlgo("")
    , m_calibAreaTag("")
    , m_shower("")
    , m_dev(false)
    , m_jetStartScale("")
    , m_jetOutScale("")
    , m_mapTag("")
    , m_doCjetCorrection()
    , m_doBjetCorrection()
    , m_scalingMC_g()
    , m_scalingMC_q()
    , m_scalingMC_c()
    , m_scalingMC_b()
{ }

MC2MCCorrection::MC2MCCorrection(const std::string& name, TEnv* config, TString jetAlgo, TString calibAreaTag, TString shower, bool dev)
    : JetCalibrationToolBase(name)
    , m_config(config)
    , m_jetAlgo(jetAlgo)
    , m_calibAreaTag(calibAreaTag)
    , m_shower(shower)
    , m_dev(dev)
    , m_jetStartScale("")
    , m_jetOutScale("")
    , m_mapTag("")
    , m_doCjetCorrection()
    , m_doBjetCorrection()
    , m_scalingMC_g()
    , m_scalingMC_q()
    , m_scalingMC_c()
    , m_scalingMC_b()
{ }
 
MC2MCCorrection::~MC2MCCorrection()
{ }

StatusCode MC2MCCorrection::initializeTool(const std::string&)
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
    if (m_shower == "")
    {
        ATH_MSG_FATAL("No shower model specified.  Aborting.");
        return StatusCode::FAILURE;
    }

    // Get the starting and ending jet scales
    m_jetStartScale = m_config->GetValue("JMC2MCCStartingScale","JetSmearedMomentum");
    m_jetOutScale   = m_config->GetValue("JMC2MCCOutScale","JetMC2MCMomentum");
    ATH_MSG_INFO("Reading from " << m_jetStartScale.Data() << " and writing to " << m_jetOutScale.Data());

    // Get the tag of the maps from the config
    m_mapTag = m_config->GetValue("MC2MCmapTag","Jun22");

    // Get the config for heavy flavour from the config
    m_doCjetCorrection = m_config->GetValue("MC2MC_doCjetCorrection",true);
    m_doBjetCorrection = m_config->GetValue("MC2MC_doBjetCorrection",true);

    // Extract the file name and path for the corrections
    TString fileName = m_config->GetValue("MC2MCCorrectionFile","");
    if (fileName == "")
    {
      fileName.Append("JetCalibTools/"+m_calibAreaTag);
      fileName.Append("CalibrationFactors/MC2MCcorrection_"+m_jetAlgo+"_"+m_mapTag+"_"+m_shower+".root");
    }
    TString filePath = PathResolverFindCalibFile(fileName.Data());
    std::unique_ptr<TFile> inputFile(TFile::Open(filePath));
    if (!inputFile || inputFile->IsZombie())
    {
        ATH_MSG_FATAL("Cannot open MC2MC correction file: " << fileName);
        return StatusCode::FAILURE;
    }

    //  Retrieve the histogram froms the file
    m_scalingMC_q = (TH2 *)inputFile->Get("h_respMap_vs_pT_q_hist");
    if (!m_scalingMC_q)
    {
        ATH_MSG_FATAL("Failed to get specified histogram from the file: h_respMap_vs_pT_q_hist");
        return StatusCode::FAILURE;
    }
    m_scalingMC_q->SetDirectory(0);

    m_scalingMC_c = (TH2 *)inputFile->Get("h_respMap_vs_pT_c_hist");
    if (!m_scalingMC_c)
    {
        ATH_MSG_FATAL("Failed to get specified histogram from the file: h_respMap_vs_pT_c_hist");
        return StatusCode::FAILURE;
    }
    m_scalingMC_c->SetDirectory(0);

    m_scalingMC_b = (TH2 *)inputFile->Get("h_respMap_vs_pT_b_hist");
    if (!m_scalingMC_b)
    {
        ATH_MSG_FATAL("Failed to get specified histogram from the file: h_respMap_vs_pT_b_hist");
        return StatusCode::FAILURE;
    }
    m_scalingMC_b->SetDirectory(0);

    m_scalingMC_g = (TH2 *)inputFile->Get("h_respMap_vs_pT_g_hist");
    if (!m_scalingMC_g)
    {
        ATH_MSG_FATAL("Failed to get specified histogram from the file: h_respMap_vs_pT_g_hist");
        return StatusCode::FAILURE;
    }
    m_scalingMC_g->SetDirectory(0);

    // Done with the input file, close it
    inputFile->Close();



    return StatusCode::SUCCESS;
}


StatusCode MC2MCCorrection::readHisto(double& returnValue, TH2* histo, double x, double y) const
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

    returnValue = histo->Interpolate(x,y);

    return StatusCode::SUCCESS;
}



StatusCode MC2MCCorrection::calibrateImpl(xAOD::Jet& jet, JetEventInfo& jetEventInfo) const
{

    // Apply the smearing factor to the jet as appropriate
    xAOD::JetFourMom_t calibP4 = jet.jetP4();

    const int channelNumber = jetEventInfo.channelNumber();

    // Protect against a few commonly used Pythia8 samples to avoid analysis mistakes
    bool isPythiaSample=false;
    if (channelNumber>=364700 && channelNumber<=364700) isPythiaSample=true;//di-jets
    if (channelNumber>=410470 && channelNumber<=410472) isPythiaSample=true;//top
    if (channelNumber>=410654 && channelNumber<=410659) isPythiaSample=true;//single top
    if (channelNumber>=410646 && channelNumber<=410649) isPythiaSample=true;//single top
    if (channelNumber>=410644 && channelNumber<=410645) isPythiaSample=true;//single top
    if (channelNumber>=363123 && channelNumber<=363170) isPythiaSample=true;//MG+Py8 Z+jets
    if (channelNumber>=363600 && channelNumber<=363671) isPythiaSample=true;//MG+Py8 W+jets
    if (isPythiaSample)
    {
        ATH_MSG_FATAL("You are running the MC2MC correction on a Pythia8 sample - this should not be done!");
        return StatusCode::FAILURE;
    }

    // Do the correction here
    double scaling=1.0;
    int partonLabel = abs(jet.auxdata<int>("PartonTruthLabelID"));
    if (partonLabel==1 || partonLabel==2 || partonLabel==3){
      if (readHisto(scaling,m_scalingMC_q,jet.pt()/1000.0,fabs(jet.eta())).isFailure()){
        return StatusCode::FAILURE;
      }
    }
    if (partonLabel==4 && m_doCjetCorrection){
      if (readHisto(scaling,m_scalingMC_c,jet.pt()/1000.0,fabs(jet.eta())).isFailure()){
        return StatusCode::FAILURE;
      }
    }
    if (partonLabel==5 && m_doBjetCorrection){
      if (readHisto(scaling,m_scalingMC_b,jet.pt()/1000.0,fabs(jet.eta())).isFailure()){
        return StatusCode::FAILURE;
      }
    }
    if (partonLabel==21){
      if (readHisto(scaling,m_scalingMC_g,jet.pt()/1000.0,fabs(jet.eta())).isFailure()){
        return StatusCode::FAILURE;
      }
    }
    calibP4 *= 1.0/scaling;

    // Set the output scale
    jet.setAttribute<xAOD::JetFourMom_t>(m_jetOutScale.Data(),calibP4);
    jet.setJetP4(calibP4);


    return StatusCode::SUCCESS;
}


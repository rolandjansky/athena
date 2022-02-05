/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : JetFitterNNTool.h
/// Package : BTagTools 
/// Author  : Giacinto Piacquadio, Christian Weiser (University of Freiburg)
/// Created : January 2008
///
/// DESCRIPTION:
///
/// This class computes the Neural Network probability for being b-jet, c-jet or uds-jet for 
/// a single event. JetNet 3.5 (TM) is used for training, interfaced from C++ code compiled with ROOT.
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "GaudiKernel/ITHistSvc.h"

#include <TMath.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH2F.h>

#include "TrkNeuralNetworkUtils/TTrainedNetwork.h"
#include "JetTagTools/JetFitterNNTool.h"
#include "TrkNeuralNetworkUtils/NeuralNetworkToHistoTool.h"

#include "JetTagTools/JetFitterVariablesNormalization.h"
#include "JetTagTools/JetFitterPtEtaCategories.h"
#include "JetTagTools/IJetFitterClassifierTool.h"

#include <TString.h>

namespace Analysis {


JetFitterNNTool::JetFitterNNTool(const std::string& name,
                                 const std::string& n, const IInterface* p):
        base_class(name, n,p),
        m_calibrationDirectory("JetFitter"),
        m_calibrationSubDirectory("NeuralNetwork"),
        m_networkToHistoTool("Trk::NeuralNetworkToHistoTool", this),
        m_useCombinedIPNN(true),
        m_maximumRegisteredLayers(4)
{
  declareProperty("CalibrationDirectory",m_calibrationDirectory);
  declareProperty("CalibrationSubDirectory",m_calibrationSubDirectory);

  declareProperty("NeuralNetworkToHistoTool",m_networkToHistoTool);
  declareProperty("useCombinedIPNN",m_useCombinedIPNN);

  declareProperty("maximumRegisteredLayers",m_maximumRegisteredLayers);

  declareProperty("usePtCorrectedMass",m_usePtCorrectedMass = false);
}

JetFitterNNTool::~JetFitterNNTool()
{
}

StatusCode JetFitterNNTool::initialize() {
  ATH_CHECK(m_readKey.initialize()); 

  StatusCode sc = m_networkToHistoTool.retrieve();
  if (sc.isFailure())
  {
    ATH_MSG_FATAL(" Could not retrieve " << m_networkToHistoTool  << ". Aborting...");
    return sc;
  } else ATH_MSG_DEBUG(" Retrieved: " << m_networkToHistoTool << ". ");

  ATH_MSG_DEBUG("Calibration setting: cannot use Neural Network with more than: " << m_maximumRegisteredLayers << ".");

  ATH_MSG_DEBUG(" Initialization of JetFitterNNTool succesfull");
  return StatusCode::SUCCESS;
}

StatusCode JetFitterNNTool::finalize() {

  //here you should probably delete something :-)
  
  ATH_MSG_DEBUG(" Finalization of JetFitterNNTool succesful");
  return StatusCode::SUCCESS;
}

  std::unique_ptr<TTrainedNetwork>
  JetFitterNNTool::loadCalibration(const std::string & jetauthor) const {
    
    std::vector<const TH1*> retrievedHistos;
    
    SG::ReadCondHandle<JetTagCalibCondData> readCdo(m_readKey);
    
    //it is assumed that in the NN 0 is signal (bottom), 1 is charm and 2 is light
    
    TString directory(m_calibrationSubDirectory);
    directory+="/";
    if (m_useCombinedIPNN)
    {
      directory+="comb";
    }
    else
    {
      directory+="standalone";
    }
    directory+="/";
    
    const TH1* histoLayers = readCdo->retrieveHistogram("JetFitterNN", 
                                                                             jetauthor, 
                                                                             std::string((const char*)(directory+TString("LayersInfo"))));

    const TH1F* myHistoLayers=dynamic_cast<const TH1F*>(histoLayers);

    if (myHistoLayers==0)
    {
      ATH_MSG_ERROR(" Cannot retrieve LayersInfo histogram ");
      return std::unique_ptr<TTrainedNetwork>();
    }

    retrievedHistos.push_back(myHistoLayers);

    Int_t nHidden=myHistoLayers->GetNbinsX()-2;

    ATH_MSG_DEBUG(" Retrieving calibration for NN with: " << nHidden << " hidden layers.");

    for (Int_t i=0;i<nHidden+1;++i)
    {

      TString weightName("Layer");
      weightName+=i;
      weightName+="_weights";
      
      TString thresholdName("Layer");
      thresholdName+=i;
      thresholdName+="_thresholds";
      
      const TH1* weightHisto = readCdo->retrieveHistogram("JetFitterNN", 
                                                                               jetauthor, 
                                                                               std::string((const char*)(directory+weightName)));
      
      const TH2F* myWeightHisto=dynamic_cast<const TH2F*>(weightHisto);
      
      if (myWeightHisto==0)
      {
        ATH_MSG_ERROR(" Cannot retrieve histogram: " << weightName);
      } 
      else 
      {
        ATH_MSG_VERBOSE(" Retrieved histo: " << weightName << " for channel : " << jetauthor << 
            " the first bin content of the weight 2d histo is: " << myWeightHisto->GetBinContent(1,1));
      }
      

      retrievedHistos.push_back(myWeightHisto);

      const TH1* thresholdHisto = readCdo->retrieveHistogram("JetFitterNN", 
                                                                                  jetauthor, 
                                                                                  std::string((const char*)(directory+thresholdName)));

      const TH1F* myThresholdHisto=dynamic_cast<const TH1F*>(thresholdHisto);
      
      if (myThresholdHisto==0)
      {
        ATH_MSG_ERROR(" Cannot retrieve histogram: " << thresholdName);
      }

      retrievedHistos.push_back(myThresholdHisto);
    
    }
    
    return std::unique_ptr<TTrainedNetwork>
      (m_networkToHistoTool->fromHistoToTrainedNetwork(retrievedHistos));
  }



  StatusCode JetFitterNNTool::fillLikelihoodValues(xAOD::BTagging* BTag,
					       const std::string & jetauthor,
					       const std::string& inputbasename,
					       const std::string& outputbasename,
					       float jetpT,
					       float jeteta,
					       float IP3dlike) const
  {
  if (jetauthor=="") {
    ATH_MSG_WARNING(" Hypothesis or jetauthor is empty. No likelihood value given back. ");
  }

  //at each fillLikelihood call check if the calibration is updated
  //(or load it for the first time)
  std::unique_ptr<TTrainedNetwork> NN = loadCalibration(jetauthor);
  
  
  if (!NN)
  {
    ATH_MSG_WARNING(" JetFitter NN instance not found: cannot do any calculation...");
    return StatusCode::SUCCESS;
  }

  
  if (m_useCombinedIPNN)
  {
    if (NN->getnInput()!=9)
    {
      ATH_MSG_ERROR(" Expect 9 input nodes in NN. Have: " << NN->getnInput());
    }
  }
  else
  {
    if (NN->getnInput()!=8)
    {
      ATH_MSG_ERROR(" Expect 8 input nodes in NN. Have: " << NN->getnInput());
    }
  }
  
  int nVTX, nTracksAtVtx, nSingleTracks;
  float energyFraction, mass, significance3d;
  bool status = true;
  
  if("JetFitter" == inputbasename){
    status &= BTag->taggerInfo(nVTX, xAOD::BTagInfo::JetFitter_nVTX);
    status &= BTag->taggerInfo(nTracksAtVtx, xAOD::BTagInfo::JetFitter_nTracksAtVtx);
    status &= BTag->taggerInfo(nSingleTracks, xAOD::BTagInfo::JetFitter_nSingleTracks);
    status &= BTag->taggerInfo(energyFraction, xAOD::BTagInfo::JetFitter_energyFraction);

    if(m_usePtCorrectedMass){
      status &= BTag->taggerInfo(mass, xAOD::BTagInfo::JetFitter_mass);
    }
    else{
      status &= BTag->variable<float>(inputbasename, "massUncorr",mass );
    }
    status &= BTag->taggerInfo(significance3d, xAOD::BTagInfo::JetFitter_significance3d);
  }
  else{
    if(m_usePtCorrectedMass){
      status &= BTag->variable<float>(inputbasename, "mass",mass );
    }
    else{
      status &= BTag->variable<float>(inputbasename, "massUncorr",mass );
    }
    status &= BTag->variable<float>(inputbasename, "significance3d", significance3d);
    status &= BTag->variable<float>(inputbasename, "energyFraction", energyFraction);
    status &= BTag->variable<int>(inputbasename, "nVTX", nVTX);
    status &= BTag->variable<int>(inputbasename, "nTracksAtVtx", nTracksAtVtx);
    status &= BTag->variable<int>(inputbasename, "nSingleTracks", nSingleTracks);
  }

  if (!status) {
     ATH_MSG_DEBUG(" Missing input data: cannot compute desired results. Assigning default light values.");

     float defaultB=1e-10;
     float defaultC=1e-10;
     float defaultL=1.-defaultB-defaultC;

     if("JetFitter" == outputbasename)
     {
       BTag->setJetFitter_pb(defaultB);
       BTag->setJetFitter_pc(defaultC);
       BTag->setJetFitter_pu(defaultL);
     }
     else
     {
       BTag->setVariable<float>(outputbasename, "pb", defaultB);
       BTag->setVariable<float>(outputbasename, "pc", defaultC);
       BTag->setVariable<float>(outputbasename, "pu", defaultL);
     }
     


     return StatusCode::SUCCESS;
  }

  std::vector<Double_t> inputData;
  inputData.push_back(norm_nVTX(nVTX));
  inputData.push_back(norm_nTracksAtVtx(nTracksAtVtx));
  inputData.push_back(norm_nSingleTracks(nSingleTracks));
  inputData.push_back(norm_energyFraction(energyFraction));
  inputData.push_back(norm_mass(mass));
  inputData.push_back(norm_significance3d(significance3d));

  if (m_useCombinedIPNN) 
  {
    if (IP3dlike<-1e6)
    {
      ATH_MSG_WARNING(" Expected a value for the IP3D Tagger... it is: " << IP3dlike << " ... (No Tagging done)");
      return StatusCode::SUCCESS;
    }
    else
    {
      inputData.push_back(norm_IP3D(IP3dlike));
    }
  }

  inputData.push_back(norm_cat_pT(getPtCategory(jetpT).first));
  inputData.push_back(norm_cat_eta(getEtaCategory(jeteta).first));

  ATH_MSG_DEBUG(" Pt is: "<< jetpT 
       << " cat " << norm_cat_pT(getPtCategory(jetpT).first)
       << ". (Check scale...!) ");

  //std::vector<float> outputValues=NN->calculateOutputValues(inputData);
  std::vector<double> outputValuesD=NN->calculateOutputValues(inputData);
  std::vector<float> outputValues(begin(outputValuesD), end(outputValuesD));

  ATH_MSG_DEBUG(" NN Discriminator b: " << outputValues[0] << 
      " c: " << outputValues[1] << " u: " <<  outputValues[2]);


  if("JetFitter" == outputbasename){
    BTag->setJetFitter_pb(outputValues[0]);
    BTag->setJetFitter_pc(outputValues[1]);
    BTag->setJetFitter_pu(outputValues[2]);
  }
  else{
    BTag->setVariable<float>(outputbasename, "pb", outputValues[0]);
    BTag->setVariable<float>(outputbasename, "pc", outputValues[1]);
    BTag->setVariable<float>(outputbasename, "pu", outputValues[2]);
  }
  return StatusCode::SUCCESS;
}


}//end Analysis namespace

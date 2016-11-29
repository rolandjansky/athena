/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "JetTagCalibration/CalibrationBroker.h"

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
        AthAlgTool(name, n,p),
        m_calibrationDirectory("JetFitter"),
        m_calibrationSubDirectory("NeuralNetwork"),
        m_networkToHistoTool("Trk::NeuralNetworkToHistoTool"),
        m_useCombinedIPNN(true),
        m_calibrationTool("BTagCalibrationBroker"),
        m_maximumRegisteredLayers(4)
{
  declareProperty("CalibrationDirectory",m_calibrationDirectory);
  declareProperty("CalibrationSubDirectory",m_calibrationSubDirectory);

  declareProperty("NeuralNetworkToHistoTool",m_networkToHistoTool);
  declareProperty("useCombinedIPNN",m_useCombinedIPNN);

  declareProperty("calibrationTool",	m_calibrationTool);

  declareProperty("maximumRegisteredLayers",m_maximumRegisteredLayers);

  declareProperty("usePtCorrectedMass",m_usePtCorrectedMass = false);

  declareInterface<IJetFitterClassifierTool>(this);

} 

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

JetFitterNNTool::~JetFitterNNTool() {

  std::map<std::string,TTrainedNetwork*>::iterator NNbegin=m_NN.begin();
  std::map<std::string,TTrainedNetwork*>::iterator NNend=m_NN.end();

  for (std::map<std::string,TTrainedNetwork*>::iterator NNiter=NNbegin;
       NNiter!=NNend;
       ++NNiter)
  {
    delete (*NNiter).second;
    (*NNiter).second=0;
  }
  
}

StatusCode JetFitterNNTool::initialize() {

  //here you have to initialize the histograms needed for the fit...
  StatusCode sc = m_calibrationTool.retrieve();
  if (sc.isFailure())
  {
    ATH_MSG_FATAL(" Could not retrieve " << m_calibrationTool  << ". Aborting...");
    return sc;
  } else ATH_MSG_INFO(" Retrieved: " << m_calibrationTool);

  sc = m_networkToHistoTool.retrieve();
  if (sc.isFailure())
  {
    ATH_MSG_FATAL(" Could not retrieve " << m_networkToHistoTool  << ". Aborting...");
    return sc;
  } else ATH_MSG_INFO(" Retrieved: " << m_networkToHistoTool << ". ");

  ATH_MSG_INFO("Calibration setting: cannot use Neural Network with more than: " << m_maximumRegisteredLayers << ".");

  //now you need to initialize the Calibration Broker with the needed histograms...
  initializeCalibrationFile();

  ATH_MSG_INFO(" Initialization of JetFitterNNTool succesfull");
  return StatusCode::SUCCESS;
}

  void JetFitterNNTool::initializeCalibrationFile()
  {
    
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
    
    m_calibrationTool->registerHistogram(m_calibrationDirectory,
                                         std::string((const char*)(directory+"LayersInfo")));
    
    Int_t nHidden=m_maximumRegisteredLayers-2;
    
    for (Int_t i=0;i<nHidden+1;++i)
    {
      
      TString weightName("Layer");
      weightName+=i;
      weightName+="_weights";
      
      TString thresholdName("Layer");
      thresholdName+=i;
      thresholdName+="_thresholds";
      
      m_calibrationTool->registerHistogram(m_calibrationDirectory,
                                           std::string((const char*)(directory+weightName)));
      
      m_calibrationTool->registerHistogram(m_calibrationDirectory,
                                           std::string((const char*)(directory+thresholdName)));
      
    }

    ATH_MSG_DEBUG(" Registered NN histograms with directory: " << m_calibrationDirectory << " and subdirectory " << directory);

    m_calibrationTool->printStatus();
    
  }
  

StatusCode JetFitterNNTool::finalize() {

  //here you should probably delete something :-)
  
  ATH_MSG_INFO(" Finalization of JetFitterNNTool succesfull");
  return StatusCode::SUCCESS;
}

  void JetFitterNNTool::loadCalibration(const std::string & jetauthor) {
    
    std::vector<TH1*> retrievedHistos;
    
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
    
    std::pair<TH1*, bool> histoLayers = m_calibrationTool->retrieveHistogram(m_calibrationDirectory, 
                                                                             jetauthor, 
                                                                             std::string((const char*)(directory+TString("LayersInfo"))));
    
    if (histoLayers.second==false && m_NN[jetauthor]!=0)
    {
      return;
    } 
    else if(histoLayers.second==true)
    {
      ATH_MSG_DEBUG(" HistoLayers in " << directory << " was updated. Switching updated now to false ... ");
      m_calibrationTool->updateHistogramStatus(m_calibrationDirectory, 
                                               jetauthor, 
                                               std::string((const char*)(directory+TString("LayersInfo"))),
                                               false);
    }
    
        

    TH1F* myHistoLayers=dynamic_cast<TH1F*>(histoLayers.first);

    if (myHistoLayers==0)
    {
      ATH_MSG_ERROR(" Cannot retrieve LayersInfo histogram ");
      return;
    }

    retrievedHistos.push_back(myHistoLayers);

    Int_t nHidden=myHistoLayers->GetNbinsX()-2;

    ATH_MSG_INFO(" Retrieving calibration for NN with: " << nHidden << " hidden layers.");

    for (Int_t i=0;i<nHidden+1;++i)
    {

      TString weightName("Layer");
      weightName+=i;
      weightName+="_weights";
      
      TString thresholdName("Layer");
      thresholdName+=i;
      thresholdName+="_thresholds";
      
      std::pair<TH1*, bool> weightHisto = m_calibrationTool->retrieveHistogram(m_calibrationDirectory, 
                                                                               jetauthor, 
                                                                               std::string((const char*)(directory+weightName)));

      if (weightHisto.second==true)
      {
        m_calibrationTool->updateHistogramStatus(m_calibrationDirectory, 
                                                 jetauthor, 
                                                 std::string((const char*)(directory+weightName)),
                                                 false);
      }
      
      
      TH2F* myWeightHisto=dynamic_cast<TH2F*>(weightHisto.first);
      
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

      std::pair<TH1*, bool> thresholdHisto = m_calibrationTool->retrieveHistogram(m_calibrationDirectory, 
                                                                                  jetauthor, 
                                                                                  std::string((const char*)(directory+thresholdName)));

      if (thresholdHisto.second==true)
      {
        m_calibrationTool->updateHistogramStatus(m_calibrationDirectory, 
                                                 jetauthor, 
                                                 std::string((const char*)(directory+thresholdName)),
                                                 false);
      }

      TH1F* myThresholdHisto=dynamic_cast<TH1F*>(thresholdHisto.first);
      
      if (myThresholdHisto==0)
      {
        ATH_MSG_ERROR(" Cannot retrieve histogram: " << thresholdName);
      }

      retrievedHistos.push_back(myThresholdHisto);
    
    }
    
    TTrainedNetwork* NN=m_NN[jetauthor];
    if (NN!=0)
    {
      delete NN;
      NN=0;
      ATH_MSG_DEBUG(" Istantiating TTrainedNetwork for jet author: " << jetauthor);
    }

    m_NN[jetauthor]=m_networkToHistoTool->fromHistoToTrainedNetwork(retrievedHistos);

    

  }



  StatusCode JetFitterNNTool::fillLikelihoodValues(xAOD::BTagging* BTag,
					       const std::string & jetauthor,
					       const std::string& inputbasename,
					       const std::string& outputbasename,
					       double jetpT,
					       double jeteta,
					       double IP3dlike) {
 

  if (jetauthor=="") {
    ATH_MSG_WARNING(" Hypothesis or jetauthor is empty. No likelihood value given back. ");
  }

  //at each fillLikelihood call check if the calibration is updated
  //(or load it for the first time)
  loadCalibration(jetauthor);
  
  
  TTrainedNetwork* NN=m_NN[jetauthor];
  if (NN==0)
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

     double defaultB=1e-10;
     double defaultC=1e-10;
     double defaultL=1.-defaultB-defaultC;

     if("JetFitter" == outputbasename)
     {
       BTag->setJetFitter_pb(defaultB);
       BTag->setJetFitter_pc(defaultC);
       BTag->setJetFitter_pu(defaultL);
     }
     else
     {
       BTag->setVariable<double>(outputbasename, "pb", defaultB);
       BTag->setVariable<double>(outputbasename, "pc", defaultC);
       BTag->setVariable<double>(outputbasename, "pu", defaultL);
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

  std::vector<double> outputValues=NN->calculateOutputValues(inputData);

  ATH_MSG_DEBUG(" NN Discriminator b: " << outputValues[0] << 
      " c: " << outputValues[1] << " u: " <<  outputValues[2]);


  if("JetFitter" == outputbasename){
    BTag->setJetFitter_pb(outputValues[0]);
    BTag->setJetFitter_pc(outputValues[1]);
    BTag->setJetFitter_pu(outputValues[2]);
  }
  else{
    BTag->setVariable<double>(outputbasename, "pb", outputValues[0]);
    BTag->setVariable<double>(outputbasename, "pc", outputValues[1]);
    BTag->setVariable<double>(outputbasename, "pu", outputValues[2]);
  }
  return StatusCode::SUCCESS;
}


}//end Analysis namespace

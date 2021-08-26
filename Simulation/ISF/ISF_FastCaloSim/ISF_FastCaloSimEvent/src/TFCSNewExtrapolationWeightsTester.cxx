/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSNewExtrapolationWeightsTester.h"
#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitBase.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"
#include "ISF_FastCaloSimEvent/TFCSCenterPositionCalculation.h"

#include "TFile.h"

#include "HepPDT/ParticleData.hh"
#include "HepPDT/ParticleDataTable.hh"

#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandFlat.h"

#if defined(__FastCaloSimStandAlone__)
#include "CLHEP/Random/TRandomEngine.h"
#else
#include <CLHEP/Random/RanluxEngine.h>
#endif

#include <iostream>
#include <fstream>

//LWTNN
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/LightweightNeuralNetwork.hh"
#include "lwtnn/parse_json.hh"

// Onnx
//#include "AthOnnxruntimeService/IONNXRuntimeSvc.h"
//#include <core/session/onnxruntime_cxx_api.h>

// XML reader
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

//=============================================
//======= TFCSNewExtrapolationWeightsTester =========
//=============================================

TFCSNewExtrapolationWeightsTester::TFCSNewExtrapolationWeightsTester(const char* name, const char* title):TFCSParametrizationBinnedChain(name,title)
{
  set_GANfreemem();
}

TFCSNewExtrapolationWeightsTester::~TFCSNewExtrapolationWeightsTester()
{
  if(m_input!=nullptr) {
    delete m_input;
  }
  if(m_graph!=nullptr) {
    delete m_graph;
  }
}

// initialize lwtnn network 
bool TFCSNewExtrapolationWeightsTester::initializeNetwork(int pid,int etaMin,std::string FastCaloGANInputFolderName)
{

  // initialize all necessary constants
  // FIXME eventually all these could be stored in the .json file

  ATH_MSG_INFO("Using FastCaloGANInputFolderName: " << FastCaloGANInputFolderName );
  // get neural net JSON file as an std::istream object    
  int etaMax = etaMin + 5;
  
  reset_match_all_pdgid();
  set_pdgid(pid);
  if(pid==11) add_pdgid(-pid);
  if(pid==211) add_pdgid(-pid);
  set_eta_min(etaMin/100.0);
  set_eta_max(etaMax/100.0);
  set_eta_nominal((etaMin+etaMax)/200.0);

  std::string inputFile = FastCaloGANInputFolderName + "/neural_net_" + std::to_string(pid) +"_eta_" + std::to_string(etaMin) +"_" + std::to_string(etaMax) +".json";
  if (inputFile.empty()){
    ATH_MSG_ERROR("Could not find json file " << inputFile );
    return false;
  } else {
    ATH_MSG_INFO("For pid: " << pid <<" and eta " << etaMin <<"-" << etaMax <<", loading json file " << inputFile );
    std::ifstream input(inputFile);
    std::stringstream sin;
    sin << input.rdbuf();
    input.close();
    // build the graph
    auto config=lwt::parse_json_graph(sin);
    m_graph=new lwt::LightweightGraph(config);
    if (m_graph==nullptr){
      ATH_MSG_ERROR("Could not create LightWeightGraph from  " << inputFile );
      return false;
    }
    if(m_input!=nullptr) {
      delete m_input;
    }
    m_input = new std::string(sin.str());
  }                
  m_GANLatentSize = 50; 
    
  //Get all Binning histograms to store in memory
  GetBinning(pid,(etaMin+etaMax)/2,FastCaloGANInputFolderName);    

  if (m_GANLatentSize==0){
    ATH_MSG_ERROR("m_GANLatentSize uninitialized!");
    return false;
  }

  return true;
}

void TFCSNewExtrapolationWeightsTester::GetBinning(int pid,int etaMid,std::string FastCaloGANInputFolderName){ 
   std::string xmlFullFileName = FastCaloGANInputFolderName + "/binning.xml";
   ATH_MSG_DEBUG("Opening XML file in "<< xmlFullFileName);
   
   std::vector<Binning> AllBinning;
   std::vector<int> EtaMaxList;
   
   xmlDocPtr doc = xmlParseFile( xmlFullFileName.c_str() );
   for( xmlNodePtr nodeRoot = doc->children; nodeRoot != NULL; nodeRoot = nodeRoot->next) {
      if (xmlStrEqual( nodeRoot->name, BAD_CAST "Bins" )) {
         for( xmlNodePtr nodeBin = nodeRoot->children; nodeBin != NULL; nodeBin = nodeBin->next ) {
            if (xmlStrEqual( nodeBin->name, BAD_CAST "Bin" )) {
               int nodePid = atof( (const char*) xmlGetProp( nodeBin, BAD_CAST "pid" ) );
               //int nodeEtaMin = atof( (const char*) xmlGetProp( nodeBin, BAD_CAST "etaMin" ) );
               int nodeEtaMax = atof( (const char*) xmlGetProp( nodeBin, BAD_CAST "etaMax" ) );
               
               Binning binsInLayer;
               bool correctentry=true;
	             if(nodePid!=pid) correctentry=false;
               
               for( xmlNodePtr nodeLayer = nodeBin->children; nodeLayer != NULL; nodeLayer = nodeLayer->next ) {
                  if( xmlStrEqual( nodeLayer->name, BAD_CAST "Layer" ) ) {
                     std::vector<double> edges; 
                     std::string s( (const char*)xmlGetProp( nodeLayer, BAD_CAST "r_edges" ) );
                    
                     std::istringstream ss(s);
                     std::string token;
 
                     while(std::getline(ss, token, ',')) {
                        edges.push_back(atof( token.c_str() ));
                     }
                     
                     int binsInAlpha = atof( (const char*) xmlGetProp( nodeLayer, BAD_CAST "n_bin_alpha" ) );
                     int layer = atof( (const char*) xmlGetProp( nodeLayer, BAD_CAST "id" ) );
                     
                     if(correctentry) ATH_MSG_DEBUG("nodepid="<<nodePid<<" nodeEtaMax="<<nodeEtaMax<<" Layer: "<<layer<<" binsInAlpha: "<<binsInAlpha<<" edges: "<< s);
                     
                     std::string name = "hist_pid_" + std::to_string(nodePid) + "_etaSliceNumber_" + std::to_string(EtaMaxList.size()) + "_layer_" + std::to_string(layer);
                     int xBins = edges.size()-1;
                     if (xBins == 0) {
                       xBins = 1; //remove warning
                       edges.push_back(0);
                       edges.push_back(1);
                     }  
                     binsInLayer[layer] = TH2D(name.c_str(), name.c_str(), xBins, &edges[0], binsInAlpha, -TMath::Pi(), TMath::Pi());
                     binsInLayer[layer].SetDirectory(0);
                  }
               }
               
	             if(!correctentry) continue;
               AllBinning.push_back(binsInLayer);
               EtaMaxList.push_back(nodeEtaMax);
            }         
         }
      }
   }
   
   int index = 0;
   for (int etaMax : EtaMaxList){
     if (etaMid < etaMax) {
       m_Binning=AllBinning[index];
       break;
     }
     index++;
   }
   
   ATH_MSG_DEBUG("Done XML file");
}   

void TFCSNewExtrapolationWeightsTester::Streamer(TBuffer &R__b)
{
   // Stream an object of class TFCSNewExtrapolationWeightsTester

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(TFCSNewExtrapolationWeightsTester::Class(),this);
      if(m_graph!=nullptr) {
        delete m_graph;
        m_graph=nullptr;
      }
      if(m_input) {
        std::stringstream sin;
        sin.str(*m_input);
        auto config=lwt::parse_json_graph(sin);
        m_graph=new lwt::LightweightGraph(config);
      }  
#ifndef __FastCaloSimStandAlone__ 
      //When running inside Athena, delete config to free the memory     
      if(GANfreemem()) {
        delete m_input;
        m_input=nullptr;
      }  
#endif      
   } else {
      R__b.WriteClassBuffer(TFCSNewExtrapolationWeightsTester::Class(),this);
   }
}

void TFCSNewExtrapolationWeightsTester::unit_test(TFCSSimulationState* simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol)
{
  if(!simulstate) {
    simulstate=new TFCSSimulationState();
#if defined(__FastCaloSimStandAlone__)
    simulstate->setRandomEngine(new CLHEP::TRandomEngine());
#else
    simulstate->setRandomEngine(new CLHEP::RanluxEngine());
#endif    
  }  
  if(!truth) {
    TFCSTruthState* t=new TFCSTruthState();
    //t->SetPtEtaPhiM(8192000,0,0,130);
    t->SetPtEtaPhiM(524288000,0,0,130);
    t->set_pdgid(22); // photon
    truth=t;
  }  
  if(!extrapol) {
    TFCSExtrapolationState* e=new TFCSExtrapolationState();
    e->set_CaloSurface_eta(truth->Eta());
    e->set_IDCaloBoundary_eta(truth->Eta());
    for(int i=0;i<24;++i) {
      e->set_eta(i, TFCSExtrapolationState::SUBPOS_ENT, truth->Eta());
      e->set_eta(i, TFCSExtrapolationState::SUBPOS_EXT, truth->Eta());
      e->set_eta(i, TFCSExtrapolationState::SUBPOS_MID, truth->Eta());
      e->set_phi(i, TFCSExtrapolationState::SUBPOS_ENT, 0);
      e->set_phi(i, TFCSExtrapolationState::SUBPOS_EXT, 0);
      e->set_phi(i, TFCSExtrapolationState::SUBPOS_MID, 0);
      e->set_r(i, TFCSExtrapolationState::SUBPOS_ENT, 1500+i*10);
      e->set_r(i, TFCSExtrapolationState::SUBPOS_EXT, 1510+i*10);
      e->set_r(i, TFCSExtrapolationState::SUBPOS_MID, 1505+i*10);
      e->set_z(i, TFCSExtrapolationState::SUBPOS_ENT, 3500+i*10);
      e->set_z(i, TFCSExtrapolationState::SUBPOS_EXT, 3510+i*10);
      e->set_z(i, TFCSExtrapolationState::SUBPOS_MID, 3505+i*10);
    }
    extrapol=e;
  }

  const int   pdgId = truth->pdgid();
  const float Ekin  = truth->Ekin();
  const float eta   = truth->Eta();

  std::cout << "True energy " << Ekin <<" pdgId " << pdgId << " eta " << eta << std::endl;

  // Find eta bin
  int Eta = eta*10;
  std::string etaBinStr = "";
  for(int i=0;i<=25;++i){
    int etaTmp = i*5;
    if(Eta>=etaTmp && Eta<(etaTmp+5)){
      etaBinStr = std::to_string(i*5)+"_"+std::to_string((i+1)*5);
    }
  }

  std::cout << "etaBinStr = " << etaBinStr << std::endl;  

  ////////////////////
  // Get the NN model
  ////////////////////

  std::string inputModelFileName = "/eos/user/j/jbossios/FastCaloSim/lwtnn_inputs/json/pid22/NN_photons_v08_"+etaBinStr+".json";
  std::cout << "Opening " << inputModelFileName << std::endl;
  std::ifstream inputModel(inputModelFileName);
  if(!inputModel){
    std::cout << inputModelFileName << " could not be open, exiting" << std::endl;
    return;
  }
  auto config = lwt::parse_json(inputModel);
  lwt::LightweightNeuralNetwork NN(config.inputs, config.layers, config.outputs);

  // Get list of input variable names
  std::vector<std::string> InputNames;
  const std::size_t total_inputs = config.inputs.size();
  for (std::size_t nnn = 0; nnn < total_inputs; nnn++) {
    const auto& input = config.inputs.at(nnn);
    InputNames.push_back(input.name);
  }

  //// Temporary
  //// Print input variable names
  //std::cout << "Print input variable names" << std::endl;
  //for(unsigned int i=0;i<InputNames.size();++i){
  //  std::cout << InputNames.at(i) << std::endl;
  //}

  ////////////////////
  // set NN inputs
  ////////////////////

  // Get TXT files to normalize inputs
  std::map< std::string, std::vector<double> > NormInputs;
  std::string inputTXTFileName = "/eos/user/j/jbossios/FastCaloSim/lwtnn_inputs/txt/pid22/v08/MeanStdDevEnergyFractions_eta_"+etaBinStr+".txt";
  std::cout << "Opening " << inputTXTFileName << std::endl;
  std::ifstream inputTXT(inputTXTFileName);
  if(inputTXT.is_open()){
    int counter = 0;
    std::string line;
    while(getline(inputTXT,line)){
      std::stringstream ss(line);
      std::vector<double> vec;
      while(ss.good()){
        std::string substr;
        getline(ss, substr, ' ');
        vec.push_back(std::stof(substr));
      }
      NormInputs[InputNames.at(counter)] = vec;
      counter++;
    }
    inputTXT.close();
  } else {
    std::cout << "Unable to open file" << std::endl;
  }

  // Temporary
  // Print map
  for (auto& t : NormInputs){
    std::cout << t.first << " " << t.second.at(0) << " " << t.second.at(1) << std::endl;
  }

  ///////////////////////////
  // Prepare input variables
  ///////////////////////////
  
  std::map<std::string, double> inputVariables;
  for(unsigned int i=0;i<InputNames.size();++i){
    if(InputNames.at(i) == "ef_0"){
      //inputVariables["ef_0"] = -0.47957863;
      inputVariables["ef_0"] = -0.4915421;
    }
    else if(InputNames.at(i) == "ef_1"){
      //inputVariables["ef_1"] = -0.22813188;
      inputVariables["ef_1"] = -0.74249745;
    }
    else if(InputNames.at(i) == "ef_2"){
      //inputVariables["ef_2"] = 0.55993577;
      inputVariables["ef_2"] = 0.94123897;
    }
    else if(InputNames.at(i) == "ef_3"){
      //inputVariables["ef_3"] = -0.23842032;
      inputVariables["ef_3"] = 0.08035759;
    }
    else if(InputNames.at(i) == "ef_12"){
      //inputVariables["ef_12"] = -0.14959943;
      inputVariables["ef_12"] = -0.1299677;
    }
    else if(InputNames.at(i) == "etrue"){
      //inputVariables["etrue"] = ( truth->E()*0.001 - NormInputs[InputNames.at(i)].at(0) ) / NormInputs[InputNames.at(i)].at(1); // why it doesn't agree with CSV file?
      //inputVariables["etrue"] = -0.28425015;
      inputVariables["etrue"] = 0.8456684;
    }
  }

  // Print input variables
  for (auto& t : inputVariables){
    std::cout << t.first << " " << t.second << std::endl;
  }

  ///////////////////////////////////////
  // Get predicted extrapolation weights
  ///////////////////////////////////////

  auto outputs = NN.compute(inputVariables);

  // Print predicted values
  for (const auto& out: outputs) {
    std::cout << out.first << " " << out.second << std::endl;
  }

  //// Onnx TODO
  //// Handle to @c AthONNX::IONNXRuntimeSvc
  //ServiceHandle<IONNXRuntimeSvc> m_svc{ this, "ONNXRuntimeSvc", "AthONNX::ONNXRuntimeSvc", "Name of the service to use" };
  //// Access the service
  //ATH_CHECK( m_svc.retrieve() );
  //// Set up the ONNX Runtime session
  //Ort::SessionOptions sessionOptions;
  //sessionOptions.SetIntraOpNumThreads( 1 );
  //sessionOptions.SetGraphOptimizationLevel( ORT_ENABLE_BASIC );
  //Ort::AllocatorWithDefaultOptions allocator;
  //m_session = std::make_unique< Ort::Session >( m_svc->env(), e.c_str(), sessionOptions );
  //ATH_MSG_INFO( "Created the ONNX Runtime session" );
  
  return;

}


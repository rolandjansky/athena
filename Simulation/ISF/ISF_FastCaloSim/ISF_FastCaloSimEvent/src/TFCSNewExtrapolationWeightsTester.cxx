/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSNewExtrapolationWeightsTester.h"
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

TFCSNewExtrapolationWeightsTester::TFCSNewExtrapolationWeightsTester(const char* name, const char* title):TFCSLateralShapeParametrizationHitBase(name,title),m_extrapWeight(0.5) {
  set_freemem();
}

// Destructor
TFCSNewExtrapolationWeightsTester::~TFCSNewExtrapolationWeightsTester()
{
  if(m_input!=nullptr) {
    delete m_input;
  }
  if(m_normInputs!=nullptr) {
    delete m_normInputs;
  }
  if(m_nn!=nullptr) {
    delete m_nn;
  }
}

// simulate_hit()
FCSReturnCode TFCSNewExtrapolationWeightsTester::simulate_hit(Hit& hit, TFCSSimulationState& simulstate, const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol)
{
   const int cs=calosample();

   ////////////////////////////////
   // set new extrapolation weight
   ////////////////////////////////
   
   // Prepare input variables
   std::map<std::string, double> inputVariables;
   for(int ilayer=0;ilayer<CaloCell_ID_FCS::MaxSample;++ilayer) {
     if(ilayer == 0 || ilayer == 1 || ilayer == 2 || ilayer == 3 || ilayer == 12){
       std::string layer = std::to_string(ilayer);
       inputVariables["ef_"+layer] = (simulstate.Efrac(ilayer) - (*m_normInputs)["ef_"+layer].at(0)) / (*m_normInputs)["ef_"+layer].at(1);
     }
   }
   inputVariables["etrue"] = ( truth->E()*0.001 - (*m_normInputs)["etrue"].at(0) ) / (*m_normInputs)["etrue"].at(1);

   //// Print input variables
   //for (auto& t : inputVariables){
   //  std::cout << t.first << " " << t.second << std::endl;
   //}

   /////////////////////////////////////////
   //// Get predicted extrapolation weights
   /////////////////////////////////////////

   auto outputs = m_nn->compute(inputVariables);

   // Print predicted values
   for (const auto& out: outputs) {
     std::cout << out.first << " " << out.second << std::endl;
   }

   m_extrapWeight = outputs["extrapWeight_"+std::to_string(cs)];

   //std::cout << "m_extrapWeight = " << m_extrapWeight << std::endl;

   double r = (1.-m_extrapWeight)*extrapol->r(cs, SUBPOS_ENT) + m_extrapWeight*extrapol->r(cs, SUBPOS_EXT);
   double z = (1.-m_extrapWeight)*extrapol->z(cs, SUBPOS_ENT) + m_extrapWeight*extrapol->z(cs, SUBPOS_EXT);
   double eta = (1.-m_extrapWeight)*extrapol->eta(cs, SUBPOS_ENT) + m_extrapWeight*extrapol->eta(cs, SUBPOS_EXT);
   double phi = (1.-m_extrapWeight)*extrapol->phi(cs, SUBPOS_ENT) + m_extrapWeight*extrapol->phi(cs, SUBPOS_EXT);
   
   if(!std::isfinite(r) || !std::isfinite(z) || !std::isfinite(eta) || !std::isfinite(phi)) {
     ATH_MSG_WARNING("Extrapolator contains NaN or infinite number.\nSetting center position to calo boundary.");
     ATH_MSG_WARNING("Before fix: center_r: " << r << " center_z: " << z << " center_phi: " << phi << " center_eta: " << eta << " weight: " << m_extrapWeight << " cs: " << cs);
     // If extrapolator fails we can set position to calo boundary
     r =  extrapol->IDCaloBoundary_r(); 
     z =  extrapol->IDCaloBoundary_z(); 
     eta =  extrapol->IDCaloBoundary_eta(); 
     phi =  extrapol->IDCaloBoundary_phi();
     
     ATH_MSG_WARNING("After fix: center_r: " << r << " center_z: " << z << " center_phi: " << phi << " center_eta: " << eta << " weight: " << m_extrapWeight << " cs: " << cs);
   }

   hit.setCenter_r( r );
   hit.setCenter_z( z );
   hit.setCenter_eta( eta );
   hit.setCenter_phi( phi );
   
   ATH_MSG_DEBUG("TFCSCenterPositionCalculation: center_r: " << hit.center_r() << " center_z: " << hit.center_z() << " center_phi: " << hit.center_phi() << " center_eta: " << hit.center_eta() << " weight: " << m_extrapWeight << " cs: " << cs);
   
   return FCSSuccess;
}


// get values needed to normalize inputs
bool TFCSNewExtrapolationWeightsTester::getTXTs(int pid, std::string etaBin, std::string FastCaloTXTInputFolderName)
{

  ATH_MSG_INFO(" Getting normalization inputs... "); // Temporary

  // Open corresponding TXT file and extract mean/std dev for each variable
  if(m_normInputs != nullptr){
    m_normInputs->clear();
  } else {
    m_normInputs = new std::map< std::string, std::vector<double> >();
  }
  std::string inputTXTFileName = FastCaloTXTInputFolderName + "pid" + std::to_string(pid) + "/";
  if(pid == 22){
    inputTXTFileName += "v08/MeanStdDevEnergyFractions_eta_" + etaBin + ".txt";
  } else {
    std:: cout << "ERROR: pid != 22 not supported yet" << std::endl;
    return false;
  }
  std::cout << "Opening " << inputTXTFileName << std::endl;
  std::ifstream inputTXT(inputTXTFileName);
  if(inputTXT.is_open()){
    int counter = 0;
    std::string line;
    while(getline(inputTXT,line)){
      std::cout << "Line = " << line << std::endl;
      std::stringstream ss(line);
      std::string VarName = "NONE"; // Name for feature
      std::vector<double> vec;      // Collect Mean and Std Dev values
      while(ss.good()){
        std::string substr;
        getline(ss, substr, ' ');
        std::cout << "substr = " << substr << std::endl;
	if(VarName == "NONE"){
          VarName = substr;
	} else {
          vec.push_back(std::stof(substr));
	}
      }
      std::cout << "VarName = " << VarName << std::endl; 
      (*m_normInputs)[VarName] = vec;
      counter++;
    }
    inputTXT.close();
  } else {
    std::cout << "Unable to open file" << std::endl;
  }

  //// Temporary
  //// Print map
  //for (auto& t : (*m_normInputs)){
  //  std::cout << t.first << " " << t.second.at(0) << " " << t.second.at(1) << std::endl;
  //}

  return true;
}

// initialize lwtnn network 
bool TFCSNewExtrapolationWeightsTester::initializeNetwork(int pid, std::string etaBin, std::string FastCaloNNInputFolderName)
{

  ATH_MSG_INFO("Using FastCaloNNInputFolderName: " << FastCaloNNInputFolderName );

  std::string inputFileName = FastCaloNNInputFolderName + "pid" + std::to_string(pid) + "/NN_";
  if(pid == 22){         inputFileName += "photons_v08_"+etaBin+".json";
  } else if(pid == 211){ inputFileName += "pions_v09_"+etaBin+".json";}
  if(inputFileName.empty()){
    ATH_MSG_ERROR("Could not find json file " << inputFileName );
    return false;
  } else {
    ATH_MSG_INFO("For pid: " << pid <<" and etaBin" << etaBin <<", loading json file " << inputFileName );
    //std::string   inputModelFileName = "/eos/user/j/jbossios/FastCaloSim/lwtnn_inputs/json/pid22/NN_photons_v08_0_5.json"; // Temporary
    if(m_input!=nullptr) {
      delete m_input;
    }
    m_input = new std::string(inputFileName.c_str());
    std::ifstream inputModel(*m_input);
    std::cout << "Will read JSON file = " << inputFileName << std::endl;
    auto config = lwt::parse_json(inputModel);
    m_nn        = new lwt::LightweightNeuralNetwork(config.inputs, config.layers, config.outputs); // Temporary (I think I can avoid this since already done in Streamer()
    // m_nn technically not needed technically, but it is helpfull (usable always, when created/read)
  }                
    
  return true;
}

void TFCSNewExtrapolationWeightsTester::Streamer(TBuffer &R__b)
{
   // Stream an object of class TFCSNewExtrapolationWeightsTester

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(TFCSNewExtrapolationWeightsTester::Class(),this);
      if(m_nn!=nullptr) {
        delete m_nn;
        m_nn=nullptr;
      }
      if(m_input) {
        std::ifstream inputModel(*m_input);
	std::cout << "Will read JSON file = " << m_input->c_str() << std::endl;
        auto config = lwt::parse_json(inputModel);
        m_nn        = new lwt::LightweightNeuralNetwork(config.inputs, config.layers, config.outputs);
      }  
#ifndef __FastCaloSimStandAlone__ 
      //When running inside Athena, delete input/config/normInputs to free the memory     
      if(freemem()) {
        delete m_input;
        m_input = nullptr;
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
    //t->SetPtEtaPhiM(8192000,0,0,130); // 8192   GeV
    t->SetPtEtaPhiM(524288000,0,0,130); // 524288 GeV
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

  // Set energy in layers which then will be retrieved in simulate_hit()
  simulstate->set_E(0, 0.002009898);
  simulstate->set_E(1, 0.133239614);
  simulstate->set_E(2, 0.856243881);
  simulstate->set_E(3, 0.005908006);
  simulstate->set_E(12, 0.002598602);
  simulstate->set_E(0.002009898 + 0.133239614 + 0.856243881 + 0.005908006 + 0.002598602);
  simulstate->set_Efrac(0, simulstate->E(0) / simulstate->E());
  simulstate->set_Efrac(1, simulstate->E(1) / simulstate->E());
  simulstate->set_Efrac(2, simulstate->E(2) / simulstate->E());
  simulstate->set_Efrac(3, simulstate->E(3) / simulstate->E());
  simulstate->set_Efrac(12, simulstate->E(12) / simulstate->E());

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

  // New
  const int   pdgId = truth->pdgid();
  const float Ekin  = truth->Ekin();
  const float eta   = truth->Eta();

  std::cout << "True energy " << Ekin <<" pdgId " << pdgId << " eta " << eta << std::endl;

  // Find eta bin
  int Eta            = eta*10;
  std::string etaBin = "";
  for(int i=0;i<=25;++i){
    int etaTmp = i*5;
    if(Eta>=etaTmp && Eta<(etaTmp+5)){
      etaBin = std::to_string(i*5)+"_"+std::to_string((i+1)*5);
    }
  }

  std::cout << "etaBin = " << etaBin << std::endl;  

  TFCSNewExtrapolationWeightsTester NN("NN", "NN");
  NN.setLevel(MSG::VERBOSE);
  const int pid = truth->pdgid();
  NN.initializeNetwork(pid, etaBin,"/eos/user/j/jbossios/FastCaloSim/lwtnn_inputs/json/");
  NN.getTXTs(pid, etaBin, "/eos/user/j/jbossios/FastCaloSim/lwtnn_inputs/txt/");
  int layer = 0;
  NN.set_calosample(layer); // Try with other layers too (0,1,2,3,12)
  TFCSLateralShapeParametrizationHitBase::Hit hit;
  NN.simulate_hit(hit,*simulstate,truth,extrapol);
  
  // Write
  TFile* fNN = new TFile("FCSNNtest.root", "RECREATE");
  NN.Write();
  fNN->ls();
  fNN->Close();
  delete fNN;

  // Open
  fNN = TFile::Open("FCSNNtest.root");
  TFCSNewExtrapolationWeightsTester* NN2 = (TFCSNewExtrapolationWeightsTester*)(fNN->Get("NN"));
  
  NN2->setLevel(MSG::DEBUG);
  NN2->set_calosample(layer); // Try with other layers too (0,1,2,3,12)
  NN2->simulate_hit(hit,*simulstate,truth,extrapol);
  simulstate->Print();
  
  return;

}


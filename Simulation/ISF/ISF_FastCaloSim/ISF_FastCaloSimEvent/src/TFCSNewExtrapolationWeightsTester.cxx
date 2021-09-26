/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSNewExtrapolationWeightsTester.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"
#include "ISF_FastCaloSimEvent/TFCSCenterPositionCalculation.h"

#include "TFile.h"
#include "TClass.h"

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

TFCSNewExtrapolationWeightsTester::TFCSNewExtrapolationWeightsTester(const char* name, const char* title):TFCSLateralShapeParametrizationHitBase(name,title) {
  set_freemem();
}

// Destructor
TFCSNewExtrapolationWeightsTester::~TFCSNewExtrapolationWeightsTester()
{
  if(m_input!=nullptr) {
    delete m_input;
  }
  if(m_normLayers!=nullptr) {
    delete m_normLayers;
  }
  if(m_normMeans!=nullptr) {
    delete m_normMeans;
  }
  if(m_normStdDevs!=nullptr) {
    delete m_normStdDevs;
  }
  if(m_nn!=nullptr) {
    delete m_nn;
  }
}

bool TFCSNewExtrapolationWeightsTester::operator==(const TFCSParametrizationBase& ref) const
{
  if(IsA()!=ref.IsA()){
    ATH_MSG_DEBUG("operator==: different class types "<<IsA()->GetName()<<" != "<<ref.IsA()->GetName());
    return false;
  }
  const TFCSNewExtrapolationWeightsTester& ref_typed=static_cast<const TFCSNewExtrapolationWeightsTester&>(ref);
  return (m_input->compare(*ref_typed.m_input) == 0);
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
       //std::cout << "Energy in layer " << layer << " = " << simulstate.E(ilayer) << std::endl; // Temporary
       //std::cout << "Energy fraction in layer " << layer << " = " << simulstate.Efrac(ilayer) << std::endl; // Temporary
       // Find index
       auto itr = std::find(m_normLayers->begin(), m_normLayers->end(), ilayer);
       if(itr != m_normLayers->end()){
         int index = std::distance((*m_normLayers).begin(),itr);
         inputVariables["ef_"+layer] = (simulstate.Efrac(ilayer) - (*m_normMeans).at(index)) / (*m_normStdDevs).at(index);
       } else {
         std::cout << "Element not found";
       }
     }
   }
   // Find index for truth energy
   auto itr = std::find(m_normLayers->begin(), m_normLayers->end(), -1);
   int index = std::distance(m_normLayers->begin(), itr);
   inputVariables["etrue"] = ( truth->E()*0.001 - (*m_normMeans).at(index) ) / (*m_normStdDevs).at(index);

   //// Print input variables
   //std::cout << "Input variables" << std::endl;
   //for (auto& t : inputVariables){
   //  std::cout << t.first << " " << t.second << std::endl;
   //}

   /////////////////////////////////////////
   //// Get predicted extrapolation weights
   /////////////////////////////////////////

   auto outputs = m_nn->compute(inputVariables);

   //// Print predicted values
   //for (const auto& out: outputs) {
   //  std::cout << out.first << " " << out.second << std::endl;
   //}

   float extrapWeight = outputs["extrapWeight_"+std::to_string(cs)];

   //std::cout << "extrapWeight = " << extrapWeight << std::endl;
   
   double r = (1.-extrapWeight)*extrapol->r(cs, SUBPOS_ENT) + extrapWeight*extrapol->r(cs, SUBPOS_EXT);
   double z = (1.-extrapWeight)*extrapol->z(cs, SUBPOS_ENT) + extrapWeight*extrapol->z(cs, SUBPOS_EXT);
   double eta = (1.-extrapWeight)*extrapol->eta(cs, SUBPOS_ENT) + extrapWeight*extrapol->eta(cs, SUBPOS_EXT);
   double phi = (1.-extrapWeight)*extrapol->phi(cs, SUBPOS_ENT) + extrapWeight*extrapol->phi(cs, SUBPOS_EXT);
   
   if(!std::isfinite(r) || !std::isfinite(z) || !std::isfinite(eta) || !std::isfinite(phi)) {
     ATH_MSG_WARNING("Extrapolator contains NaN or infinite number.\nSetting center position to calo boundary.");
     ATH_MSG_WARNING("Before fix: center_r: " << r << " center_z: " << z << " center_phi: " << phi << " center_eta: " << eta << " weight: " << extrapWeight << " cs: " << cs);
     // If extrapolator fails we can set position to calo boundary
     r =  extrapol->IDCaloBoundary_r(); 
     z =  extrapol->IDCaloBoundary_z(); 
     eta =  extrapol->IDCaloBoundary_eta(); 
     phi =  extrapol->IDCaloBoundary_phi();
     
     ATH_MSG_WARNING("After fix: center_r: " << r << " center_z: " << z << " center_phi: " << phi << " center_eta: " << eta << " weight: " << extrapWeight << " cs: " << cs);
   }

   hit.setCenter_r( r );
   hit.setCenter_z( z );
   hit.setCenter_eta( eta );
   hit.setCenter_phi( phi );
   
   ATH_MSG_DEBUG("TFCSCenterPositionCalculation: center_r: " << hit.center_r() << " center_z: " << hit.center_z() << " center_phi: " << hit.center_phi() << " center_eta: " << hit.center_eta() << " weight: " << extrapWeight << " cs: " << cs);
   
   return FCSSuccess;
}


// get values needed to normalize inputs
bool TFCSNewExtrapolationWeightsTester::getNormInputs(int pid, std::string etaBin, std::string FastCaloTXTInputFolderName)
{

  ATH_MSG_DEBUG(" Getting normalization inputs... ");

  // Open corresponding TXT file and extract mean/std dev for each variable
  if(m_normLayers != nullptr){
    m_normLayers->clear();
  } else {
    m_normLayers = new std::vector<int>();
  }
  if(m_normMeans != nullptr){
    m_normMeans->clear();
  } else {
    m_normMeans = new std::vector<float>();
  }
  if(m_normStdDevs != nullptr){
    m_normStdDevs->clear();
  } else {
    m_normStdDevs = new std::vector<float>();
  }
  std::string inputFileName = FastCaloTXTInputFolderName + "pid" + std::to_string(pid) + "/";
  if(pid == 22){
    inputFileName += "v08/MeanStdDevEnergyFractions_eta_" + etaBin + ".txt";
  } else {
    std:: cout << "ERROR: pid != 22 not supported yet" << std::endl;
    return false;
  }
  std::cout << "Opening " << inputFileName << std::endl;
  std::ifstream inputTXT(inputFileName);
  if(inputTXT.is_open()){
    std::string line;
    while(getline(inputTXT,line)){
      std::stringstream ss(line);
      unsigned int counter = 0;
      while(ss.good()){
        std::string substr;
        getline(ss, substr, ' ');
	if(counter == 0){ // Get index (#layer or -1 if var == etrue)
          if(substr != "etrue"){
            int index = std::stoi(substr.substr(substr.find("_")+1));
	    m_normLayers->push_back(index);
	  } else { // etrue
	    m_normLayers->push_back(-1);
	  }
	} else if(counter == 1){
	  m_normMeans->push_back(std::stof(substr));
	} else if(counter == 2){
	  m_normStdDevs->push_back(std::stof(substr));
	}
	counter++;
      }
    }
    inputTXT.close();
  } else {
    ATH_MSG_ERROR(" Unable to open file ");
  }

  //// Temporary
  //std::cout << "Normalization layers" << std::endl;
  //for(auto& i : (*m_normLayers)){
  //  std::cout << i << std::endl;
  //}
  //std::cout << "Normalization means" << std::endl;
  //for(auto& i : (*m_normMeans)){
  //  std::cout << i << std::endl;
  //}
  //std::cout << "Normalization std devs" << std::endl;
  //for(auto& i : (*m_normStdDevs)){
  //  std::cout << i << std::endl;
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
  simulstate->set_E(0, 1028.77124023);
  simulstate->set_E(1, 68199.0625);
  simulstate->set_E(2, 438270.78125);
  simulstate->set_E(3, 3024.02929688);
  simulstate->set_E(12, 1330.10131836);
  simulstate->set_E(1028.77124023 + 68199.0625 + 438270.78125 + 3024.02929688 + 1330.10131836);
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
  NN.getNormInputs(pid, etaBin, "/eos/user/j/jbossios/FastCaloSim/lwtnn_inputs/txt/");
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
  NN2->simulate_hit(hit,*simulstate,truth,extrapol);
  simulstate->Print();
  
  return;

}


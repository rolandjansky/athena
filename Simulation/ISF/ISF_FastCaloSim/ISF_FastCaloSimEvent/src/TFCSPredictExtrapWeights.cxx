/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSPredictExtrapWeights.h"
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

// XML reader
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

//=============================================
//======= TFCSPredictExtrapWeights =========
//=============================================

TFCSPredictExtrapWeights::TFCSPredictExtrapWeights(const char* name, const char* title):TFCSLateralShapeParametrizationHitBase(name,title) {
  set_freemem();
}

// Destructor
TFCSPredictExtrapWeights::~TFCSPredictExtrapWeights()
{
  if(m_input!=nullptr) {
    delete m_input;
  }
  if(m_relevantLayers!=nullptr) {
    delete m_relevantLayers;
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

bool TFCSPredictExtrapWeights::operator==(const TFCSParametrizationBase& ref) const
{
  if(IsA()!=ref.IsA()){
    ATH_MSG_DEBUG("operator==: different class types "<<IsA()->GetName()<<" != "<<ref.IsA()->GetName());
    return false;
  }
  const TFCSPredictExtrapWeights& ref_typed=static_cast<const TFCSPredictExtrapWeights&>(ref);

  if(TFCSParametrizationBase::compare(ref))          return true;
  if(!TFCSParametrization::compare(ref))             return false;
  if(!TFCSLateralShapeParametrization::compare(ref)) return false;

  return (m_input->compare(*ref_typed.m_input) == 0);
}

// getNormInputs()
// Get values needed to normalize inputs
bool TFCSPredictExtrapWeights::getNormInputs(std::string etaBin, std::string FastCaloTXTInputFolderName)
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
  std::string inputFileName = FastCaloTXTInputFolderName + "MeanStdDevEnergyFractions_eta_" + etaBin + ".txt";
  ATH_MSG_DEBUG(" Opening " << inputFileName);
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

  return true;
}

// prepareInputs()
// Prepare input variables to the Neural Network
std::map<std::string,double> TFCSPredictExtrapWeights::prepareInputs(const int pid, TFCSSimulationState& simulstate, const float truthE) const
{
  std::map<std::string, double> inputVariables;
  /*std::vector<int>              inputLayers = {0,1,2,3,12};
  if(is_match_pdgid(211) || is_match_pdgid(-211)){
    inputLayers.push_back(13);
    inputLayers.push_back(14);
  }*/
  for(int ilayer=0;ilayer<CaloCell_ID_FCS::MaxSample;++ilayer) {
    //if(std::find(inputLayers.begin(), inputLayers.end(), ilayer) != inputLayers.end()){
    if(std::find(m_relevantLayers->begin(), m_relevantLayers->end(), ilayer) != m_relevantLayers->end()){
      std::string layer = std::to_string(ilayer);
      // Find index
      auto itr = std::find(m_normLayers->begin(), m_normLayers->end(), ilayer);
      if(itr != m_normLayers->end()){
        int index = std::distance((*m_normLayers).begin(),itr);
        inputVariables["ef_"+layer] = (simulstate.Efrac(ilayer) - (*m_normMeans).at(index)) / (*m_normStdDevs).at(index);
      } else {
        ATH_MSG_ERROR("Normalization information not found for layer " << ilayer);
      }
    }
  }
  // Find index for truth energy
  auto itr  = std::find(m_normLayers->begin(), m_normLayers->end(), -1);
  int index = std::distance(m_normLayers->begin(), itr);
  inputVariables["etrue"] = ( truthE - (*m_normMeans).at(index) ) / (*m_normStdDevs).at(index);
  if(pid == 211){
    inputVariables["pdgId"] = pid;
  }

  return inputVariables;
}

// simulate()
// get predicted extrapolation weights and save them as AuxInfo in simulstate
FCSReturnCode TFCSPredictExtrapWeights::simulate(TFCSSimulationState& simulstate, const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const
{
  (void)extrapol; // avoid unused variable warning

  const int pid = truth->pdgid();

  // Get inputs to Neural Network
  std::map<std::string,double> inputVariables = prepareInputs(pid, simulstate, truth->E()*0.001);

  // Get predicted extrapolation weights
  auto outputs            = m_nn->compute(inputVariables);
  std::vector<int> layers = {0,1,2,3,12};
  if(is_match_pdgid(211) || is_match_pdgid(-211)){ // charged pion
    layers.push_back(13);
    layers.push_back(14);
  }
  for(int ilayer : layers){ // loop over layers and decorate simulstate with corresponding predicted extrapolation weight
    simulstate.setAuxInfo<float>(ilayer,outputs["extrapWeight_"+std::to_string(ilayer)]);
  }
  return FCSSuccess;
}

// simulate_hit()
FCSReturnCode TFCSPredictExtrapWeights::simulate_hit(Hit& hit, TFCSSimulationState& simulstate, const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol)
{
   (void)truth; // avoid unused variable warning

   const int cs=calosample();

   // Get corresponding predicted extrapolation weight from simulstate
   float extrapWeight;
   if(simulstate.hasAuxInfo(cs)){
     extrapWeight = simulstate.getAuxInfo<float>(cs);
   } else{ // missing AuxInfo
     simulate(simulstate, truth, extrapol); // decorate simulstate with extrapolation weights
     extrapWeight = simulstate.getAuxInfo<float>(cs); // retrieve corresponding extrapolation weight
   }

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

// initializeNetwork()
// Initialize lwtnn network 
bool TFCSPredictExtrapWeights::initializeNetwork(int pid, std::string etaBin, std::string FastCaloNNInputFolderName)
{

  ATH_MSG_INFO("Using FastCaloNNInputFolderName: " << FastCaloNNInputFolderName );

  std::string inputFileName = FastCaloNNInputFolderName + "NN_"+etaBin+".json";
  ATH_MSG_DEBUG("Will read JSON file: " << inputFileName );
  if(inputFileName.empty()){
    ATH_MSG_ERROR("Could not find json file " << inputFileName );
    return false;
  } else {
    ATH_MSG_INFO("For pid: " << pid <<" and etaBin" << etaBin <<", loading json file " << inputFileName );
    std::ifstream input(inputFileName);
    std::stringstream sin;
    sin << input.rdbuf();
    input.close();
    auto config = lwt::parse_json(sin);
    m_nn        = new lwt::LightweightNeuralNetwork(config.inputs, config.layers, config.outputs);
    if(m_nn==nullptr){
      ATH_MSG_ERROR("Could not create LightWeightNeuralNetwork from " << inputFileName );
      return false;
    }
    if(m_input!=nullptr){
      delete m_input;
    }
    m_input = new std::string(sin.str());
    // Extract relevant layers from the outputs
    m_relevantLayers = new std::vector<int>();
    for(auto name : config.outputs){
      int layer = std::stoi( name.erase(0, 13) ); // remove "extrapWeight_" and convert to int
      m_relevantLayers->push_back(layer);
    }
  }                
  return true;
}

// Streamer()
void TFCSPredictExtrapWeights::Streamer(TBuffer &R__b)
{
   // Stream an object of class TFCSPredictExtrapWeights

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(TFCSPredictExtrapWeights::Class(),this);
      if(m_nn!=nullptr) {
        delete m_nn;
        m_nn=nullptr;
      }
      if(m_input && !m_input->empty()) {
        std::stringstream sin;
        sin.str(*m_input);
        auto config = lwt::parse_json(sin);
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
      R__b.WriteClassBuffer(TFCSPredictExtrapWeights::Class(),this);
   }
}

// unit_test()
// Function for testing
void TFCSPredictExtrapWeights::unit_test(TFCSSimulationState* simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol)
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
    t->SetPtEtaPhiM(524288000,0,0,130); // 524288 GeV
    t->set_pdgid(22);                   // photon
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

  TFCSPredictExtrapWeights NN("NN", "NN");
  NN.setLevel(MSG::VERBOSE);
  const int pid = truth->pdgid();
  NN.initializeNetwork(pid, etaBin,"/eos/atlas/atlascerngroupdisk/proj-simul/AF3_Run3/Jona/lwtnn_inputs/json/v23/");
  NN.getNormInputs(etaBin, "/eos/atlas/atlascerngroupdisk/proj-simul/AF3_Run3/Jona/lwtnn_inputs/txt/v23/");

  // Get extrapWeights and save them as AuxInfo in simulstate

  // Get inputs to Neural Network
  std::map<std::string,double> inputVariables = NN.prepareInputs(pid, *simulstate, truth->E()*0.001);

  // Get predicted extrapolation weights
  auto outputs = NN.m_nn->compute(inputVariables);
  std::vector<int> layers = {0,1,2,3,12};
  for(int ilayer : layers){
    simulstate->setAuxInfo<float>(ilayer,outputs["extrapWeight_"+std::to_string(ilayer)]);
  }

  // Simulate
  int layer = 0;
  NN.set_calosample(layer);
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
  TFCSPredictExtrapWeights* NN2 = (TFCSPredictExtrapWeights*)(fNN->Get("NN"));
  
  NN2->setLevel(MSG::DEBUG);
  NN2->simulate_hit(hit,*simulstate,truth,extrapol);
  simulstate->Print();
  
  return;
}

void TFCSPredictExtrapWeights::Print(Option_t* option) const
{
   TString opt(option);
   bool shortprint=opt.Index("short")>=0;
   bool longprint=msgLvl(MSG::DEBUG) || (msgLvl(MSG::INFO) && !shortprint);
   TString optprint=opt;optprint.ReplaceAll("short","");
   TFCSLateralShapeParametrizationHitBase::Print(option);

   if(longprint) ATH_MSG_INFO(optprint << "  m_input (TFCSPredictExtrapWeights): " << m_input);
   if(longprint) ATH_MSG_INFO(optprint << "  Address of m_nn: " << (void *)m_nn);
}

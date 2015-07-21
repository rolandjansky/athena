/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGEGAMMAANALYSISTOOLS_UTIL_TRIGCALONRINSSELECTORTOOL_H
#define TRIGEGAMMAANALYSISTOOLS_UTIL_TRIGCALORINGSSELECTORTOOL_H

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"

#define RESET_RNN_DECISION	999
#define INF                 9999


namespace ringer{

  const unsigned  NETBIN      = 10-1;
  const unsigned  NETABIN     = 10-1;
  const float     ETBINS[]    = {7, 10, 15, 20, 25, 30, 35, 40, 45, INF};
  const float     ETABINS[]   = {0.0, 0.6, 0.8, 1.15, 1.37, 1.52,1.81, 2.01, 2.37, 2.47};
 
  enum CaloRingsLayer{
    PS   = 0,
    EM1  = 1,
    EM2  = 2,
    EM3  = 3,
    HAD1 = 4,
    HAD2 = 5,
    HAD3 = 6
  };
  
  namespace TrigCaloRingsHelper{
    /*
     * rings map into std::vector
     * 0:7   PS; 8:71  EM1;  72:79 EM2;  80:87 EM3
     * 88:91 HAD1; 92:95 HAD2; 96:99 HAD3
     */
    void parseCaloFeatures( unsigned layer, unsigned &minRing, unsigned &maxRing, 
                            std::string &caloLayerName)
    {
      if(layer == PS   ){minRing = 0;   maxRing = 7 ; caloLayerName = "PreSampler";}
      if(layer == EM1  ){minRing = 8;   maxRing = 71; caloLayerName = "EM1";}
      if(layer == EM2  ){minRing = 72;  maxRing = 79; caloLayerName = "EM2";}
      if(layer == EM3  ){minRing = 80;  maxRing = 87; caloLayerName = "EM3";}
      if(layer == HAD1 ){minRing = 88;  maxRing = 91; caloLayerName = "HAD1";}
      if(layer == HAD2 ){minRing = 92;  maxRing = 95; caloLayerName = "HAD2";}
      if(layer == HAD3 ){minRing = 96;  maxRing = 99; caloLayerName = "HAD3";}
    }
    
  }///namespace
  
  namespace selector{
   
    class TrigCaloRingsSelectorTool{
      public:
      
        TrigCaloRingsSelectorTool( std::string fileName, std::string trig );
        ~TrigCaloRingsSelectorTool();
      
        bool initialize();
        bool executeTool(std::vector< float > input);
        bool executeTool(std::vector< float > input, float eta, float et);
        bool finalize();
  
        float getRnnDecision();
        bool passed();
        
      private:
        bool parse(std::string fileName,std::string location,
                                        std::vector<unsigned>&nodes, 
                                        std::vector<float>&weights, 
                                        std::vector<float>&bias,
                                        float &threshold);
        template <class T>
        void InitBranch(TTree* fChain, std::string branch_name, T* param, bool message);
   
        std::vector<float> normalize(std::vector<float> input);
        unsigned find_eta_bin( float eta );
        unsigned find_et_bin ( float et  );
  
      private:
  
        std::string m_appName;
        std::string m_trig;
        std::string m_fileName;
        float m_rnnDecision;
        bool m_passed;
        bool m_multiNets;
        hypo::NeuralNetwork *m_neuralNetwork;
        std::vector< std::vector<hypo::NeuralNetwork*> >m_vec_neuralNetwork;
  
    };
  }// namespace

  namespace hypo {
  
    class NeuralNetwork{
      
    public:
      NeuralNetwork(std::vector<unsigned> nodes, std::vector<float> weights, std::vector<float> bias);
      ~NeuralNetwork();
      float operator()(std::vector<float> input);
      
      float getThreshold(){return m_threshold;};  
      void setThreshold(float th){m_threshold = th;};
    
    private:
      bool allocateSpace();
      void releaseMatrix(float **b);
      void releaseMatrix(float ***w);
   
    private:
      float m_threshold;
      float ***m_w;
      float **m_b;
      float **m_layerOutputs;
      std::vector< unsigned > m_nNodes;
      std::string m_appName;
    };
  }
  

}/// namespace ringer

using namespace ringer;
using namespace hypo;
using namespace std;
using namespace selector;

TrigCaloRingsSelectorTool::TrigCaloRingsSelectorTool(std::string filename, std::string trig){

  m_appName = "TrigCaloRingsSelectorTool";
  m_trig = trig;
  m_multiNets = false;
  m_fileName = filename;
  std::cout << m_appName <<" INFO: TrigCaloRingsSelectorTool was created." << std::endl;
}

TrigCaloRingsSelectorTool::~TrigCaloRingsSelectorTool(){
  if(m_neuralNetwork){
    delete m_neuralNetwork;
  }// delete single network if available
  for(unsigned i = 0; i < NETBIN; ++i){
    for(unsigned j = 0; j < NETABIN; ++j){
      delete  (m_vec_neuralNetwork.at(i).at(j));
    }// loop over eta bins
  }//loop over et bins
}



bool TrigCaloRingsSelectorTool::initialize(){
  
  vector<unsigned> nodes;
  vector<float> weights, bias;
  float threshold = 0.0;
  if(m_multiNets){
    for(unsigned ietbin = 0; ietbin < NETBIN; ++ietbin){
      std::vector< NeuralNetwork* > vec_etabin;
      for(unsigned ietabin = 0; ietabin < NETABIN; ++ietabin){
        std::stringstream ss;
        ss << m_trig << "/" << m_trig << "_etbin_" << ietbin << "_etabin_" << ietabin; 
        if(!parse(m_fileName, ss.str(), nodes, weights, bias, threshold) ) return false;
        NeuralNetwork *nn = new NeuralNetwork(nodes, weights, bias);
        if(!nn){
          std::cout << m_appName << " FATAL:  Can not initalize network." << std::endl;
          return false;
        }
        nn->setThreshold(threshold);
        vec_etabin.push_back(nn);
      }
      m_vec_neuralNetwork.push_back( vec_etabin );
    }
  }else{
    if(!parse(m_fileName, m_trig, nodes, weights, bias, threshold))  return false;
    NeuralNetwork *m_neuralNetwork = new NeuralNetwork(nodes, weights, bias);
    if(!m_neuralNetwork){
      std::cout << m_appName << " FATAL:  Can not initalize network." << std::endl;
      return false;
    }
    m_neuralNetwork->setThreshold(threshold);
  }

  return true;
}

bool TrigCaloRingsSelectorTool::executeTool( std::vector< float > input){

  m_rnnDecision = RESET_RNN_DECISION;
  m_passed = false;
  auto input_normalized = normalize( input );
  m_rnnDecision = m_neuralNetwork->operator()(input_normalized);
  if(m_rnnDecision > m_neuralNetwork->getThreshold() ){
    m_passed = true;
  }   
  return m_passed;
}

bool TrigCaloRingsSelectorTool::executeTool( std::vector< float > input, float eta, float et ){

  unsigned etabin = find_eta_bin(eta);
  unsigned etbin  = find_et_bin(et);
  m_neuralNetwork = (m_vec_neuralNetwork.at(etbin)).at(etabin);
  m_rnnDecision = RESET_RNN_DECISION;
  m_passed = false;
  auto input_normalized = normalize( input );
  m_rnnDecision = m_neuralNetwork->operator()(input_normalized);
  if(m_rnnDecision > m_neuralNetwork->getThreshold() ){
    m_passed = true;
  }   
  return m_passed;
}


bool TrigCaloRingsSelectorTool::finalize(){
  return true;
}

float TrigCaloRingsSelectorTool::getRnnDecision(){
  return m_rnnDecision;
}

bool TrigCaloRingsSelectorTool::passed(){
  if(m_passed) return true;
  return false;
}

std::vector<float> TrigCaloRingsSelectorTool::normalize(std::vector<float> input){

  float sum = 0;
  std::vector<float> output;
  for(std::vector<float>::iterator it = input.begin(); it != input.end(); ++it){
    sum += (*it);
  }
  for(std::vector<float>::iterator it = input.begin(); it != input.end(); ++it){
    output.push_back((*it)/sum);
  }
  return output;
}

unsigned TrigCaloRingsSelectorTool::find_eta_bin( float eta ){

  for(unsigned bin = 0; bin < NETABIN; ++bin){
    if( (eta >= ETABINS[bin]) && ( eta < ETABINS[bin+1] ) ) return bin;
  }
  return 0;
}

unsigned TrigCaloRingsSelectorTool::find_et_bin( float et ){

  for(unsigned bin = 0; bin < NETBIN; ++bin){
    if( (et >= ETBINS[bin]) && ( et < ETBINS[bin+1] ) ) return bin;
  }
  return 0;
}



bool TrigCaloRingsSelectorTool::parse(std::string fileName,
                                      std::string location,
                                      std::vector<unsigned>&nodes, 
                                      std::vector<float>&weights, 
                                      std::vector<float>&bias,
                                      float &threshold)
{
  nodes.clear();
  weights.clear();
  bias.clear();  
  TFile *file = new TFile(fileName.c_str());
  if(!file){
    std::cout << m_appName << " ERROR:  Can not open the chain in file root. Please check the chains list with TBrowser..." << std::endl;
    return false;
  }
  TTree *nn = (TTree*)file->Get(location.c_str());
  if(!nn){
    std::cout << m_appName << " ERROR:  Can not found the location:" << location << ". abort!" << std::endl;
    return false;
  }

  std::vector<float> *nodesVector  = new std::vector<float>();
  std::vector<float> *weightVector = new std::vector<float>();
  std::vector<float>  *biasVector  = new std::vector<float>();
  threshold = 0;
  nn->SetBranchStatus("*",0);
 
  InitBranch(nn,"Threshold",    &threshold,     true);
  InitBranch(nn,"NodesVector",  &nodesVector,   true);
  InitBranch(nn,"WeightVector", &weightVector,  true);
  InitBranch(nn,"BiasVector",   &biasVector,    true);

  nn->GetEntry(0);
  for(unsigned int i = 0; i < nodesVector->size(); ++i)   nodes.push_back(nodesVector->at(i));
  for(unsigned int i = 0; i < weightVector->size(); ++i)  weights.push_back(weightVector->at(i));
  for(unsigned int i = 0; i < biasVector->size(); ++i)    bias.push_back(biasVector->at(i));
  
  // release memory
  file->Close();
  delete file;
  delete nodesVector;
  delete weightVector;
  delete biasVector; 
  return true;
}


template <class T>
void TrigCaloRingsSelectorTool::InitBranch(TTree* fChain, std::string branch_name, T* param, bool message){
   std::string bname = branch_name;
   if (fChain->GetAlias(bname.c_str()))
      bname = std::string(fChain->GetAlias(bname.c_str()));

   if (!fChain->FindBranch(bname.c_str()) && message) {
     std::cout << " WARNING: unknown branch " << bname << std::endl;
     return;  
   }
   fChain->SetBranchStatus(bname.c_str(), 1.);
   fChain->SetBranchAddress(bname.c_str(), param);
}

/* Neural netwotk discriminator */

NeuralNetwork::NeuralNetwork(std::vector<unsigned> nodes,
                             std::vector<float> weights,
                             std::vector<float> bias)
                            :m_nNodes(nodes), 
                             m_appName("NeuralNetwork")
{ 
  //Allocating the memory for the values.
  try {allocateSpace();}
  catch (bad_alloc xa) {throw;}
  
  // populating matrixs...
  const unsigned size =  m_nNodes.size() - 1;
  std::vector<float>::const_iterator itrB = bias.begin();
  std::vector<float>::const_iterator itrW = weights.begin();
  
  for (unsigned l = 0; l < size; ++l){
    for (unsigned i = 0; i < m_nNodes[l+1]; i++){
      //Populating bias matrix
      m_b[l][i]=(*itrB++);
      
      //Populating weight matrix
      for (unsigned j=0; j < m_nNodes[l]; j++) m_w[l][i][j]=(*itrW++);
    }
    
    for(unsigned i=0; i<m_nNodes[l]; i++){
      if (m_layerOutputs){
        if (m_layerOutputs[l]){
          m_layerOutputs[l][i] = 0;
        }
      }
    }
  }
}

NeuralNetwork::~NeuralNetwork(){
  releaseMatrix(m_b);
  releaseMatrix(m_w);
  releaseMatrix(m_layerOutputs);
}


float NeuralNetwork::operator()(std::vector<float> input){

  float *in = new float[input.size()];
  for(unsigned i = 0; i < input.size(); ++i)  in[i] = input.at(i);
  
  const unsigned size = (m_nNodes.size() - 1);
  m_layerOutputs[0] = in;
  
  //Propagating the input through the network.
  for (unsigned l=0; l<size; l++){
    for (unsigned i=0; i< m_nNodes[l+1]; i++){
      m_layerOutputs[l+1][i] = m_b[l][i];
      for (unsigned j=0; j < m_nNodes[l]; j++){
        m_layerOutputs[l+1][i] += m_layerOutputs[l][j]*m_w[l][i][j];
      }
      m_layerOutputs[l+1][i] = TMath::TanH(m_layerOutputs[l+1][i]);
    }
  }
  //Returning the network's output.
  return (m_layerOutputs[size][0]);
  
}

bool NeuralNetwork::allocateSpace(){

  try{
    m_layerOutputs = new float*[m_nNodes.size()];
    m_layerOutputs[0] = NULL;
    const unsigned size = m_nNodes.size() - 1;
    
    m_b = new float* [size];
    m_w = new float** [size];
    for (unsigned i=0; i<size; i++)
    {
      m_b[i] = new float [m_nNodes[i+1]];
      m_layerOutputs[i+1] = new float [m_nNodes[i+1]];
      m_w[i] = new float* [m_nNodes[i+1]];
      for (unsigned j=0; j< m_nNodes[i+1]; j++) m_w[i][j] = new float [m_nNodes[i]];
    }
  }
  catch (std::bad_alloc xa){
    std::cout << m_appName << " ERROR: bad_alloc" << std::endl;
    throw;
  }
  return true;
}

void NeuralNetwork::releaseMatrix(float **b){
  if (b){
    for (unsigned i=0; i<(m_nNodes.size()-1); i++){
      if (b[i]) delete [] b[i];
    }
    delete [] b;
    b = NULL;
  }
}

void NeuralNetwork::releaseMatrix(float ***w){
  if (w){
    for (unsigned i=0; i<(m_nNodes.size()-1); i++){
      if (w[i]){
        for (unsigned j=0; j<m_nNodes[i+1]; j++){
          if (w[i][j]) delete [] w[i][j];
        }
        delete [] w[i];
      }
    }
    delete [] w;
    w = NULL;
  }
}
#endif

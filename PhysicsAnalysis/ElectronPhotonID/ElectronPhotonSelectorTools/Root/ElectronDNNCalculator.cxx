/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
  @class ElectronDNNCalculator
  @brief Used by AsgElectronSelectorTool to calculate the score of a python
         trained DNN using lwtnn as interface to do electron ID. Also applies a
         transformation to the input variables based on a QuantileTransformer.

  @author Lukas Ehrke
*/

#include "ElectronDNNCalculator.h"
#include "PathResolver/PathResolver.h"
#include "TSystem.h"
#include "TFile.h"
#include <map>
#include <fstream>
#include <iostream>
#include "lwtnn/parse_json.hh"
#include "lwtnn/InputOrder.hh"
#include <Eigen/Dense>


ElectronDNNCalculator::ElectronDNNCalculator(AsgElectronSelectorTool* owner,
                                             const std::string& modelFileName,
                                             const std::string& quantileFileName,
                                             const std::vector<std::string>& variables,
                                             const bool multiClass) :
                                            asg::AsgMessagingForward(owner),
                                            m_multiClass(multiClass)
{
  ATH_MSG_INFO("Initializing ElectronDNNCalculator...");

  if (modelFileName.empty()){
    throw std::runtime_error("No file found at '" + modelFileName + "'");
  }

  // Make an input file object
  std::ifstream inputFile;

  // Open your trained model
  ATH_MSG_INFO("Loading model: " << modelFileName.c_str());

  // Create input order for the NN, the data needs to be passed in this exact order
  lwt::InputOrder order;
  std::vector<std::string> inputVariables = {"d0", "d0significance", "dPOverP",
                                             "deltaEta1", "deltaPhiRescaled2", "trans_TRTPID",
                                             "nPixHitsPlusDeadSensors", "nSCTHitsPlusDeadSensors",
                                             "EoverP", "eta", "et", "Rhad1", "Rhad", "f3", "f1",
                                             "weta2", "Rphi", "Reta", "Eratio", "wtots1"};
  order.scalar.push_back( std::make_pair("node_0", inputVariables) );

  // create the model
  inputFile.open(modelFileName);
  auto parsedGraph = lwt::parse_json_graph(inputFile);
  // Test whether the number of outputs of the given network corresponds to the expected number
  size_t nOutputs = parsedGraph.outputs.begin()->second.labels.size();
  if (nOutputs != 6 && nOutputs != 1){
    throw std::runtime_error("Given model does not have 1 or 6 outputs. Something seems to be wrong with the model file.");
  }
  else if (nOutputs == 1 && m_multiClass){
    throw std::runtime_error("Given model has 1 output but config file specifies mutliclass. Something is wrong");
  }
  else if (nOutputs == 6 && !m_multiClass){
    throw std::runtime_error("Given model has 6 output but config file does not specify mutliclass. Something is wrong");
  }

  m_graph = std::make_unique<lwt::generic::FastGraph<float>>(parsedGraph, order);

  if (quantileFileName.empty()){
    throw std::runtime_error("No file found at '" + quantileFileName + "'");
  }

  // Open quantiletransformer file
  ATH_MSG_INFO("Loading QuantileTransformer " << quantileFileName);
  TFile* qtfile = TFile::Open(quantileFileName.data());
  if (readQuantileTransformer((TTree*)qtfile->Get("tree"), variables) == 0){
    throw std::runtime_error("Could not load all variables for the QuantileTransformer");

  }
}


// takes the input variables, transforms them according to the given QuantileTransformer and predicts the DNN value(s)
Eigen::Matrix<float, -1, 1> ElectronDNNCalculator::calculate( const MVAEnum::MVACalcVars& varsStruct ) const
{
  // Create the input for the model
  Eigen::VectorXf inputVector(20);

  // This has to be in the same order as the InputOrder was defined
  inputVector(0) = transformInput( m_quantiles.d0, varsStruct.d0);
  inputVector(1) = transformInput( m_quantiles.d0significance, varsStruct.d0significance);
  inputVector(2) = transformInput( m_quantiles.dPOverP, varsStruct.dPOverP);
  inputVector(3) = transformInput( m_quantiles.deltaEta1, varsStruct.deltaEta1);
  inputVector(4) = transformInput( m_quantiles.deltaPhiRescaled2, varsStruct.deltaPhiRescaled2);
  inputVector(5) = transformInput( m_quantiles.trans_TRTPID, varsStruct.trans_TRTPID);
  inputVector(6) = transformInput( m_quantiles.nPixHitsPlusDeadSensors, varsStruct.nPixHitsPlusDeadSensors);
  inputVector(7) = transformInput( m_quantiles.nSCTHitsPlusDeadSensors, varsStruct.nSCTHitsPlusDeadSensors);
  inputVector(8) = transformInput( m_quantiles.EoverP, varsStruct.EoverP);
  inputVector(9) = transformInput( m_quantiles.eta, varsStruct.eta);
  inputVector(10) = transformInput( m_quantiles.et, varsStruct.et);
  inputVector(11) = transformInput( m_quantiles.Rhad1, varsStruct.Rhad1);
  inputVector(12) = transformInput( m_quantiles.Rhad, varsStruct.Rhad);
  inputVector(13) = transformInput( m_quantiles.f3, varsStruct.f3);
  inputVector(14) = transformInput( m_quantiles.f1, varsStruct.f1);
  inputVector(15) = transformInput( m_quantiles.weta2, varsStruct.weta2);
  inputVector(16) = transformInput( m_quantiles.Rphi, varsStruct.Rphi);
  inputVector(17) = transformInput( m_quantiles.Reta, varsStruct.Reta);
  inputVector(18) = transformInput( m_quantiles.Eratio, varsStruct.Eratio);
  inputVector(19) = transformInput( m_quantiles.wtots1, varsStruct.wtots1);

  std::vector<Eigen::VectorXf> inp;
  inp.emplace_back(std::move(inputVector));

  auto output = m_graph->compute(inp);
  return output;
}


// transform the input based on a QuantileTransformer. quantiles are bins in the variable, while references are bins from 0 to 1
// The interpolation is done averaging the interpolation going from small to large bins and going from large to small bins
// to deal with non strictly monotonic rising bins.
double ElectronDNNCalculator::transformInput( const std::vector<double>& quantiles, double value ) const
{
  int size = quantiles.size();

  // if given value is outside of range of the given quantiles return min (0) or max (1) of references
  if (value <= quantiles[0]) return m_references[0];
  if (value >= quantiles[size-1]) return m_references[size-1];

  // find the bin where the value is smaller than the next bin (going from low bins to large bins)
  auto lowBound = std::lower_bound(quantiles.begin(), quantiles.end(), value);
  int lowBin = lowBound - quantiles.begin() - 1;

  // get x and y values on left and right side from value while going up
  double xLup = quantiles[lowBin], yLup = m_references[lowBin], xRup = quantiles[lowBin+1], yRup = m_references[lowBin+1];

  // find the bin where the value is larger than the next bin (going from large bins to low bins)
  auto upperBound = std::upper_bound(quantiles.begin(), quantiles.end(), value);
  int upperBin = upperBound - quantiles.begin();

  // get x and y values on left and right side from value while going down
  double xRdown = quantiles[upperBin], yRdown = m_references[upperBin], xLdown = quantiles[upperBin-1], yLdown = m_references[upperBin-1];

  // calculate the gradients
  double dydxup = ( yRup - yLup ) / ( xRup - xLup );
  double dydxdown = ( yRdown - yLdown ) / ( xRdown - xLdown );

  // average linear interpolation of up and down case
  return 0.5 * ((yLup + dydxup * (value - xLup)) + (yLdown + dydxdown * (value - xLdown)));
}


// Read the information needed for the QuantileTransformer from a ROOT TTree
int ElectronDNNCalculator::readQuantileTransformer( TTree* tree, const std::vector<std::string>& variables )
{
  int sc(1);
  // the reference bins to which the variables will be transformed to
  double references;
  sc = tree->SetBranchAddress("references", &references) == -5 ? 0 : 1;

  std::map<std::string, double> readVars;
  for ( const auto& var : variables ){
    sc = tree->SetBranchAddress(TString(var), &readVars[var]) == -5 ? 0 : 1;
  }
  for (int i = 0; i < tree->GetEntries(); i++){
    tree->GetEntry(i);
    m_references.push_back(references);
    m_quantiles.d0.push_back(readVars["d0"]);
    m_quantiles.d0significance.push_back(readVars["d0significance"]);
    m_quantiles.dPOverP.push_back(readVars["dPOverP"]);
    m_quantiles.deltaEta1.push_back(readVars["deltaEta1"]);
    m_quantiles.deltaPhiRescaled2.push_back(readVars["deltaPhiRescaled2"]);
    m_quantiles.trans_TRTPID.push_back(readVars["trans_TRTPID"]);
    m_quantiles.nPixHitsPlusDeadSensors.push_back(readVars["nPixHitsPlusDeadSensors"]);
    m_quantiles.nSCTHitsPlusDeadSensors.push_back(readVars["nSCTHitsPlusDeadSensors"]);
    m_quantiles.EoverP.push_back(readVars["EoverP"]);
    m_quantiles.eta.push_back(readVars["eta"]);
    m_quantiles.et.push_back(readVars["et"]);
    m_quantiles.Rhad1.push_back(readVars["Rhad1"]);
    m_quantiles.Rhad.push_back(readVars["Rhad"]);
    m_quantiles.f3.push_back(readVars["f3"]);
    m_quantiles.f1.push_back(readVars["f1"]);
    m_quantiles.weta2.push_back(readVars["weta2"]);
    m_quantiles.Rphi.push_back(readVars["Rphi"]);
    m_quantiles.Reta.push_back(readVars["Reta"]);
    m_quantiles.Eratio.push_back(readVars["Eratio"]);
    m_quantiles.wtots1.push_back(readVars["wtots1"]);
  }
  return sc;
}

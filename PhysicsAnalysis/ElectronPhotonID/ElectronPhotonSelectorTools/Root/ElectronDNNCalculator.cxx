/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include <fstream>
#include <iostream>
#include "lwtnn/parse_json.hh"


ElectronDNNCalculator::ElectronDNNCalculator(AsgElectronSelectorTool* owner,
                                             const std::string& modelFileName,
                                             const std::string& quantileFileName,
                                             const std::vector<std::string>& variables) :
                                            asg::AsgMessagingForward(owner)
{
  ATH_MSG_INFO("Initializing ElectronDNNCalculator...");

  if (modelFileName.size() == 0){
    throw std::runtime_error("No file found at '" + modelFileName + "'");
  }

  // Make an input file object
  std::ifstream inputFile;

  // Open your trained model
  ATH_MSG_INFO("Loading model: " << modelFileName.c_str());

  // create the model
  inputFile.open(modelFileName);
  auto parsedGraph = lwt::parse_json_graph(inputFile);
  m_graph = std::make_unique<lwt::LightweightGraph>(parsedGraph);


  if (quantileFileName.size() == 0){
    throw std::runtime_error("No file found at '" + quantileFileName + "'");
  }

  // Open quantiletransformer file
  ATH_MSG_INFO("Loading QuantileTransformer " << quantileFileName);
  TFile* qtfile = TFile::Open(quantileFileName.data());
  if (readQuantileTransformer((TTree*)qtfile->Get("tree"), variables) == 0){
      throw std::runtime_error("Could not load all variables for the QuantileTransformer");

  }
}


// takes the input variables, transforms them according to the given QuantileTransformer and predicts the DNN value
double ElectronDNNCalculator::calculate( const MVAEnum::MVACalcVars& varsStruct ) const
{
  double score = 0.;
  std::map<std::string, std::map<std::string, double> > inputs;
  inputs["node_0"] = {
                      {"d0", transformInput( m_quantiles.d0, varsStruct.d0)},
                      {"d0significance", transformInput( m_quantiles.d0significance, varsStruct.d0significance)},
                      {"dPOverP", transformInput( m_quantiles.dPOverP, varsStruct.dPOverP)},
                      {"deltaEta1", transformInput( m_quantiles.deltaEta1, varsStruct.deltaEta1)},
                      {"deltaPhiRescaled2", transformInput( m_quantiles.deltaPhiRescaled2, varsStruct.deltaPhiRescaled2)},
                      {"trans_TRTPID", transformInput( m_quantiles.trans_TRTPID, varsStruct.trans_TRTPID)},
                      {"nPixHitsPlusDeadSensors", transformInput( m_quantiles.nPixHitsPlusDeadSensors, varsStruct.nPixHitsPlusDeadSensors)},
                      {"nSCTHitsPlusDeadSensors", transformInput( m_quantiles.nSCTHitsPlusDeadSensors, varsStruct.nSCTHitsPlusDeadSensors)},
                      {"EoverP", transformInput( m_quantiles.EoverP, varsStruct.EoverP)},
                      {"eta", transformInput( m_quantiles.eta, varsStruct.eta)},
                      {"et", transformInput( m_quantiles.et, varsStruct.et)},
                      {"Rhad1", transformInput( m_quantiles.Rhad1, varsStruct.Rhad1)},
                      {"Rhad", transformInput( m_quantiles.Rhad, varsStruct.Rhad)},
                      {"f3", transformInput( m_quantiles.f3, varsStruct.f3)},
                      {"f1", transformInput( m_quantiles.f1, varsStruct.f1)},
                      {"weta2", transformInput( m_quantiles.weta2, varsStruct.weta2)},
                      {"Rphi", transformInput( m_quantiles.Rphi, varsStruct.Rphi)},
                      {"Reta", transformInput( m_quantiles.Reta, varsStruct.Reta)},
                      {"Eratio", transformInput( m_quantiles.Eratio, varsStruct.Eratio)},
                      {"wtots1", transformInput( m_quantiles.wtots1, varsStruct.wtots1)}
	                 };

  std::map<std::string, double> outputs = m_graph->compute(inputs);

  score = outputs["out_0"];

  return score;
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
  int i = 0;
  while (value > quantiles[i+1]) i++;

  // get x and y values on left and right side from value while going up
  double xLup = quantiles[i], yLup = m_references[i], xRup = quantiles[i+1], yRup = m_references[i+1];

  // find the bin where the value is larger than the next bin (going from large bins to low bins)
  i = size - 1;
  while (value < quantiles[i-1]) i--;

  // get x and y values on left and right side from value while going down
  double xRdown = quantiles[i], yRdown = m_references[i], xLdown = quantiles[i-1], yLdown = m_references[i-1];

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
  for ( auto var : variables ){
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

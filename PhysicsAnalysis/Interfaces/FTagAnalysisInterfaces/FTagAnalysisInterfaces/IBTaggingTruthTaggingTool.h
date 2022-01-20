// Dear emacs, this is -*- c++ -*-
///////////////////////////////////////////////////////////////////
// IBTaggingTruthTaggingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef CPIBTAGGINGTRUTHTAGGINGTOOL_H
#define CPIBTAGGINGTRUTHTAGGINGTOOL_H

#include "AsgTools/IAsgTool.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicSet.h"

#include <string>
#include <vector>

#include "CalibrationDataInterface/CalibrationDataVariables.h"
#include "xAODBTaggingEfficiency/TruthTagResults.h"

// xAOD jet
#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicSet.h"

class IBTaggingTruthTaggingTool : virtual public CP::ISystematicsTool {

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( IBTagTruthTaggingTool )

  public:

  virtual StatusCode setEffMapIndex(const std::string& flavour, unsigned int index)=0 ;

  virtual StatusCode CalculateResults( std::vector<float>& pt, std::vector<float>& eta, std::vector<int>& flav, std::vector<float>& tagw, Analysis::TruthTagResults& results,int rand_seed=-1)=0 ;
    
  virtual StatusCode CalculateResults( const xAOD::JetContainer& jets, Analysis::TruthTagResults& results,int rand_seed=-1)=0;

  /* will use onnx tool
  node_feat: input to the network that'll calculate the efficiencies, where each vector corresponds to a set of variables associated with a jet
  node_feat = {
   {flav_jet1, pt_jet1, eta_jet1, phi_jet1, ...},
   {flav_jet2, pt_jet2, eta_jet2, phi_jet2, ...},
   ...
  }
  */
  virtual StatusCode CalculateResultsONNX( const std::vector<std::vector<float>>& node_feat, std::vector<float>& tagw,  Analysis::TruthTagResults& results, int rand_seed=-1)=0 ;
    
  virtual StatusCode CalculateResultsONNX( const xAOD::JetContainer& jets, const std::vector<std::vector<float>>& node_feat, Analysis::TruthTagResults& results, int rand_seed=-1)=0;

};
#endif // CPIBTAGGINGTRUTHTAGGINGTOOL_H

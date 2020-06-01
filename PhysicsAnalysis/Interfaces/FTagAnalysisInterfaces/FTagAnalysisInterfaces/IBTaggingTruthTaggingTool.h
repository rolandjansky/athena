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

  virtual StatusCode CalculateResults( std::vector<double>& pt, std::vector<double>& eta, std::vector<int>& flav, std::vector<double>& tagw, Analysis::TruthTagResults& results,int rand_seed=-1)=0 ;
  virtual StatusCode CalculateResults( const xAOD::JetContainer& jets, Analysis::TruthTagResults& results,int rand_seed=-1)=0;


};
#endif // CPIBTAGGINGTRUTHTAGGINGTOOL_H

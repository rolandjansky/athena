// IJSSMLTool.h

#ifndef IJSSMLTool_H
#define IJSSMLTool_H

#include "AsgTools/IAsgTool.h"

// System include(s).
#include <memory>
#include <string>
#include <iostream> 
#include <fstream>
#include <arpa/inet.h>
#include <numeric>

// root
#include "TH2.h"

// xAOD
#include "xAODJet/JetContainer.h"
#include "xAODPFlow/TrackCaloClusterContainer.h"

namespace AthONNX {

  class IJSSMLTool : virtual public asg::IAsgTool {
    ASG_TOOL_INTERFACE(IJSSMLTool)

      public:

    /// Function initialising the tool                                                                           
    virtual StatusCode initialize() = 0;
    /// Function executing the tool for a single event                                                           
    virtual StatusCode retriveConstituentsScore() = 0;
    virtual StatusCode retrieveHighLevelScore() = 0;

    virtual TH2D MakeJetImage(TString TagImage, const xAOD::Jet* jet, std::vector<xAOD::JetConstituent> constituents) = 0;
    virtual void PrintInfo() = 0;
    virtual void PrintInfoImages() = 0;
    virtual void ResetImages() = 0;
    virtual double GetCNNScore() = 0; 
    virtual void SetJSSInputs(const std::map<std::string, double> JSSVars) = 0;
    virtual double GetDNNScore() = 0;
    virtual StatusCode SetScaler(std::map<std::string, std::vector<double>> scaler) = 0;


  };

} // end name space AthONNX

#endif

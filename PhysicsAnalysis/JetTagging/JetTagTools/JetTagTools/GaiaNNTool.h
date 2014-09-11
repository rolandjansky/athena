//-*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGTOOL_GAIA_NN_TOOL_C
#define BTAGTOOL_GAIA_NN_TOOL_C

/******************************************************
    @class GaiaNNTool
    Package : JetTagTools 
    Created : September 2013
    DESCRIPTION:
    This class computes the Neural Network probability for being b-jet, 
    c-jet or uds-jet for a single event. 
    @authors Dan Guest, Luke de Oliveira
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "JetTagTools/IJetFitterClassifierTool.h"

#include <vector>
#include <string>
#include <map>

// class TList; 
// #include <TString.h>

// class TTrainedNetwork;
// class TH1;

// namespace Trk
// {
//   class NeuralNetworkToHistoTool;
// }

namespace JetTagger { 
  class NeuralNet; 
}

namespace Analysis {


class IJetFitterTagInfo;
class CalibrationBroker;
  

static const InterfaceID 
IID_GaiaNNTool("Analysis::GaiaNNTool", 1, 0);

  class GaiaNNTool : public IJetFitterClassifierTool, 
			       public AthAlgTool
  {
    
  public:

    static const std::string SPECS; 
    static const std::string TRAINING; 

    static const InterfaceID& interfaceID() { 
      return IID_GaiaNNTool; 
    };

    GaiaNNTool(const std::string& name,
                    const std::string& n, const IInterface* p);
    ~GaiaNNTool();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    
    void fillLikelihoodValues(IJetFitterTagInfo & myTagInfo,
			      const std::string & jetauthor,
                              double jetpT,
                              double jeteta,
                              double IP3dlike=-5000);

    
  private:

    // variable names used internally to pass jet pt / eta around 
    static const std::string JET_PT; 
    static const std::string JET_ETA; 

    // some input variables like IP3DNeg or SV1Flip need remapping
    std::map<std::string, std::string> m_variable_remapping; 

    void loadCalibration(const std::string & jetauthor);

    void initializeCalibrationFile();

    void registerHist(std::string name); 
    std::string getCalString(std::string jet_author, std::string name); 
    std::string getRemapping(const std::string&) const; 

    std::string m_calibrationDirectory;
    std::string m_calibrationSubDirectory;
    ToolHandle<CalibrationBroker> m_calibrationTool;

    std::map<std::string, JetTagger::NeuralNet*> m_networks; 

    double m_min_pt; 
    double m_max_abs_eta; 
    

  };
  
}//end Analysis namespace

#endif

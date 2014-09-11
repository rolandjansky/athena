//-*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGTOOL_JETFITTERCHARMNNTOOL_C
#define BTAGTOOL_JETFITTERCHARMNNTOOL_C

/******************************************************
    @class JetFitterNNTool
    Package : JetTagTools 
    Created : January 2008
    DESCRIPTION:
    This class computes the Neural Network probability for being b-jet, 
    c-jet or uds-jet for 
    a single event. Uses JETNET for training. 
    @authors Dan Guest, Giacinto Piacquadio, Christian Weiser 
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "JetTagTools/IJetFitterClassifierTool.h"

#include <vector>
#include <string>
#include <map>

#include <TString.h>

class TTrainedNetwork;
class TH1;

namespace Trk
{
  class NeuralNetworkToHistoTool;
}

namespace Analysis {


class IJetFitterTagInfo;
class CalibrationBroker;
  

static const InterfaceID 
IID_JetFitterCharmNNTool("Analysis::JetFitterCharmNNTool", 1, 0);

  class JetFitterCharmNNTool : public IJetFitterClassifierTool, 
			       public AthAlgTool
  {
    
  public:

    // strings used to look up hists
    static const std::string TOPOLOGY_HIST; 
    static const std::string WEIGHTS_HISTS; 
    static const std::string THRESHOLD_HISTS; 
    static const std::string INPUTS_HIST;
    static const std::string ETA_HIST;
    static const std::string PT_HIST;

    // names of the catigory information expected by the nn
    // (these must be defined in the nn calibration)
    static const std::string PT_CAT_NAME; 
    static const std::string ETA_CAT_NAME;

    static const InterfaceID& interfaceID() { 
      return IID_JetFitterCharmNNTool; 
    };

    JetFitterCharmNNTool(const std::string& name,
                    const std::string& n, const IInterface* p);
    ~JetFitterCharmNNTool();
    
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
    TH1* getHist(std::string jet_author, std::string name); 
    // std::string m_network_file_name; 
    // std::string m_nn_name; 
    
    std::string m_calibrationDirectory;
    std::string m_calibrationSubDirectory;
    ToolHandle<CalibrationBroker> m_calibrationTool;
    int m_maximumRegisteredLayers;

    typedef std::map<std::string,TH1*> HistMap; 

    class RobustNN { 
    public: 
      RobustNN(HistMap& input_hists, const JetFitterCharmNNTool* parent); 
      // RobustNN(const TFlavorNetwork*, 
      // 	       std::map<double,int>, 
      // 	       std::map<double,int>); 
      ~RobustNN(); 
      std::vector<double> calculate(const std::map<std::string,double>& ) const; 
    private: 
      RobustNN(const RobustNN&); 
      RobustNN& operator=(const RobustNN&); 
      TTrainedNetwork* m_flavnet; 
      std::map<double, int> m_pt_cat_map; 
      std::map<double, int> m_abseta_cat_map; 
      std::set<std::string> m_allowed_inputs; 
      int get_bin(const std::map<double, int>& bins, double value) const;  
    };
    std::map<std::string,RobustNN*> m_networks;
    friend class RobustNN; 

    ToolHandle<Trk::NeuralNetworkToHistoTool> m_networkToHistoTool;

  };
  
}//end Analysis namespace

#endif

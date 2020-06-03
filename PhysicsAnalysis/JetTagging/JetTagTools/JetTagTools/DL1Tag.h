/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGTOOLS_DL1TAG_C
#define BTAGTOOLS_DL1TAG_C

/******************************************************
    @class DL1Tag
    Package : JetTagTools
    Created : September 2015
    DESCRIPTION:
    This class computes the Neural Network probability for being b-jet,
    c-jet or uds-jet for a single event.
    @authors Dan Guest, Luke de Oliveira, Marie Lanfermann
********************************************************/
#include "AthenaBaseComps/AthAlgTool.h"
#include "lwtnn/lightweight_network_config.hh"
#include "JetTagTools/IMultivariateJetTagger.h"

#include <vector>
#include <istream>

namespace xAOD {
  class Vertex_v1;
  typedef Vertex_v1 Vertex;
}

namespace Analysis {
    class CalibrationBroker;
}

namespace lwt {
  class LightweightNeuralNetwork;
}

namespace Analysis {

  class DL1Tag : public AthAlgTool, public IMultivariateJetTagger
  {
  public:
    DL1Tag(const std::string& name,
	   const std::string& n,
	   const IInterface*);

    virtual ~DL1Tag();
    StatusCode initialize();
    StatusCode finalize();
    void finalizeHistos() {};

    void assignProbability(xAOD::BTagging* BTag, const std::map<std::string,double>& inputs, const std::string& jetauthor);

  private:
    typedef std::map<std::string, double> var_map;
    typedef std::map<std::string, std::map<std::string, double> > map_var_map;
    typedef std::map<std::string, std::string> str_map;
    typedef std::map<std::string, std::vector<lwt::Input> > map_var;
    typedef std::map<std::string, lwt::LightweightNeuralNetwork*> nn_map;

    // load the calibration file from the COOL db
    void load_calibration_file();
    void cache_calibration(const std::string& jetauthor);
    std::string get_calib_string(std::string jetauthor);

    void build_nn(const std::string& jetauthor, std::istream& nn_config_istream);
    void fill_outputs(xAOD::BTagging* BTag, var_map outputs);

    std::string m_calibrationDirectory;
    std::string m_calibrationSubDirectory;
    ToolHandle<CalibrationBroker> m_calibrationTool; // pointer to calibration in COOL
    std::string m_xAODBaseName;
    bool m_forceDL1CalibrationAlias;
    std::string m_DL1CalibAlias;

    nn_map m_NeuralNetworks;

    //error checking
    int m_n_compute_errors;

    std::string m_LocalNNConfigFile;
    map_var_map m_map_defaults;
    map_var m_map_variables;

    /** This switch is needed to indicate what to do. The algorithm can be run to produce
	reference histograms from the given MC files (m_runModus=0) or to work in analysis mode
	(m_runModus=1) where already made reference histograms are read.*/
    std::string    m_runModus;          //!< 0=Do not read histos, 1=Read referece histos (analysis mode)
  }; // end class

}// end Analysis namespace

#endif

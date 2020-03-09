/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "JetTagTools/IMultivariateJetTagger.h"
#include "JetTagCalibration/JetTagCalibCondData.h"

#include <vector>
#include <istream>
#include <atomic>

namespace xAOD {
  class Vertex_v1;
  typedef Vertex_v1 Vertex;
}

namespace lwt {
  class LightweightNeuralNetwork;
}

namespace Analysis {

  class DL1Tag : public extends<AthAlgTool, IMultivariateJetTagger>
  {
  public:
    DL1Tag(const std::string& name,
	   const std::string& n,
	   const IInterface*);

    virtual ~DL1Tag();
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    virtual
    void assignProbability(xAOD::BTagging* BTag,
                           const std::map<std::string,double>& inputs,
                           const std::string& jetauthor) const override;

  private:
    typedef std::map<std::string, double> var_map;

    /** Key of calibration data: */
    SG::ReadCondHandleKey<JetTagCalibCondData> m_readKey{this, "HistosKey", "JetTagCalibHistosKey", "Key of input (derived) JetTag calibration data"};
    std::unique_ptr<const lwt::LightweightNeuralNetwork>
    load_calibration(const std::string& jetauthor,
                     std::vector<lwt::Input>& variables,
                     var_map& defaults) const;

    std::unique_ptr<const lwt::LightweightNeuralNetwork>
    build_nn(std::istream& nn_config_istream,
             std::vector<lwt::Input>& variables,
             var_map& defaults) const;
    void fill_outputs(xAOD::BTagging* BTag, var_map outputs) const;

    std::string m_calibrationDirectory;
    std::string m_calibrationSubDirectory;
    std::string m_xAODBaseName;
    bool m_forceDL1CalibrationAlias;
    std::string m_DL1CalibAlias;

    std::unique_ptr<const lwt::LightweightNeuralNetwork> m_localNN;
    var_map m_local_defaults;
    std::vector<lwt::Input> m_local_variables;

    //error checking
    mutable std::atomic<int> m_n_compute_errors;

    std::string m_LocalNNConfigFile;

    /** This switch is needed to indicate what to do. The algorithm can be run to produce
	reference histograms from the given MC files (m_runModus=0) or to work in analysis mode
	(m_runModus=1) where already made reference histograms are read.*/
    std::string    m_runModus;          //!< 0=Do not read histos, 1=Read referece histos (analysis mode)
  }; // end class

}// end Analysis namespace

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGTOOL_MULTIVARIATETAGMANAGER_C
#define BTAGTOOL_MULTIVARIATETAGMANAGER_C

/******************************************************
    @class MultivariateTagManager
    Package : JetTagTools
    Created : Oct 2015
    DESCRIPTION:
    General interface for MV based classifiers.

    @authors Dan Guest, Marie Lanfermann

********************************************************/
// #include "xAODBTagging/BTagging.h"
// #include "GaudiKernel/ToolHandle.h"
// #include "AthenaBaseComps/AthAlgTool.h"
#include "JetTagTools/ITagTool.h"
#include "JetTagTools/IMultivariateJetTagger.h"

#include <vector>

namespace Analysis {
  class CalibrationBroker;
}

namespace Analysis {
  class IMultivariateJetTagger;

  class MultivariateTagManager: public AthAlgTool, virtual public ITagTool
  {
  public:

    MultivariateTagManager(const std::string&,
		     const std::string&,
		     const IInterface*);

    virtual ~MultivariateTagManager(){};

    StatusCode initialize();
    StatusCode finalize();
    void setOrigin(const xAOD::Vertex* priVtx);
    void finalizeHistos() {};

    StatusCode tagJet(xAOD::Jet& jetToTag, xAOD::BTagging* BTag);


  private:

    // internal typdefs
    typedef std::map<std::string, double> var_map;
    var_map inputs;

    // load input variables from xAOD
    void fill_ip2d(var_map& inputs, xAOD::BTagging* BTag);
    void fill_ip3d(var_map& inputs, xAOD::BTagging* BTag);
    void fill_sv0(var_map& inputs, xAOD::BTagging* BTag);
    void fill_sv1(var_map& inputs, xAOD::BTagging* BTag);
    void fill_jetfitter(var_map& inputs, xAOD::BTagging* BTag);
    void fill_mvb(var_map& inputs, xAOD::Jet& jet, xAOD::BTagging* BTag);
    void fill_mv2cl100(var_map& inputs, xAOD::BTagging* BTag);
    void fill_trkSum(var_map& inputs, xAOD::BTagging* BTag);
    void fill_softmuon(var_map& inputs, xAOD::BTagging* BTag);

    //constants
    float m_sv1_efrc;
    int m_sv1_ntkv;
    float m_jf_efrc;
    int m_jf_nvtx1t;
    int m_jf_ntrkv;

    // container information
    std::string m_ip2d_infosource;
    std::string m_ip3d_infosource;
    std::string m_sv0_infosource;
    std::string m_sv1_infosource;
    std::string m_jftNN_infosource;
    std::string m_softmuon_infosource;

    ToolHandleArray< IMultivariateJetTagger > m_MultivariateTaggerHandleArray;

    /** Storage for the primary vertex. Can be removed when JetTag provides origin(). */
    // this pointer does not need to be deleted in the destructor (because it
    // points to something in storegate)
    const xAOD::Vertex* m_priVtx;
  }; // end class

  inline void MultivariateTagManager::setOrigin(const xAOD::Vertex* priVtx) { m_priVtx = priVtx; }
} //end Analysis namespace

#endif // BTAGTOOL_MULTIVARIATETAGMANAGER_C

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// standard stuff
#include <vector>
#include <cmath>

// general athena stuff
#include "GaudiKernel/IToolSvc.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
///Local include(s)
#include "TrigMultiVarHypo/preproc/TrigRingerPreprocessor.h"
#include "TrigMultiVarHypo/tools/MultiLayerPerceptron.h"

///xAOD include(s)
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigRinger/TrigRNNOutput.h"
#include "xAODTrigRinger/TrigRNNOutputContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"

#include "TrigTimeAlgs/TrigTimer.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"


class TrigL2CaloRingerFexMT : public AthAlgorithm  {

 public:
    TrigL2CaloRingerFexMT(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2CaloRingerFexMT();

    StatusCode initialize();
    StatusCode finalize();
    StatusCode execute();

  protected:
    TrigTimer* m_storeTimer;
    TrigTimer* m_normTimer;
    TrigTimer* m_decisionTimer;



 private:

    //feature keys
    Gaudi::Property<std::string>  m_hlt_feature  {this, "HltFeature", "TrigRingerNeuralFex", "..."};
    Gaudi::Property<std::string>  m_feature  {this, "Feature", "TrigRingerNeuralFex", "..."};

    //Prepoc configuration
    Gaudi::Property<std::vector<unsigned int>> m_normRings {this, "NormalisationRings", std::vector<unsigned int>(),  "..." };
    Gaudi::Property<std::vector<unsigned int>> m_sectionRings {this, "SectionRings", std::vector<unsigned int>(), "..."};
    Gaudi::Property<std::vector<unsigned int>> m_nRings {this, "NRings", std::vector<unsigned int>(), "..." };

    //Disriminator configuration
    Gaudi::Property<std::vector<unsigned int>> m_nodes {this, "Nodes", std::vector<unsigned int>(), "..."};
    Gaudi::Property<std::vector<std::vector<double>>> m_weights {this, "Weights",  std::vector<std::vector<double>>(), "..."};
    Gaudi::Property<std::vector<std::vector<double>>> m_bias {this, "Bias", std::vector<std::vector<double>>(), "..." };
    Gaudi::Property<std::vector<double>> m_threshold {this, "Threshold", std::vector<double>(), "..."};
    Gaudi::Property<std::vector<std::vector<double>>> m_etaBins {this, "EtaBins", std::vector<std::vector<double>>(), "..." };
    Gaudi::Property<std::vector<std::vector<double>>> m_etBins {this, "EtBins", std::vector<std::vector<double>>(), "..."};

    bool configurationInvalid();

    //Discriminator holder
    std::vector<MultiLayerPerceptron*> m_discriminators;
    //Pre-processing holder
    std::vector<TrigRingerPreprocessor*> m_preproc;


    unsigned  m_nDiscr;
    unsigned  m_nPreproc;
    float     m_output;

   SG::ReadHandleKey<xAOD::TrigRingerRings> m_ringsKey { 
	this,
        "CaloRingsKey",
        "CaloRings",
        ""
    };
  
   SG::ReadHandleKey<xAOD::TrigEMClusterContainer> m_clustersKey { 
	this,
        "ClustersKey",
        "CaloClusters",
        ""
    };

   SG::WriteHandleKey<xAOD::TrigRNNOutput> m_outputKey {
	this,
        "RNNOutputKey",
	"CaloRNNOutput",
	""
    };

   ToolHandle<GenericMonitoringTool> m_monTool { this, "MonTool", "GenericMonitoringTool/RingerFexMon", "Monitoring" };

};


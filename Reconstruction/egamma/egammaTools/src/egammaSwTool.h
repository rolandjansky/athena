/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EGAMMASWTOOL_H
#define EGAMMATOOLS_EGAMMASWTOOL_H
/**
  @class egammaSwTool
          Tool which applies positions and energy corrections to a CaloCluster
@author D. Zerwas
@author B. Lenzi
@author C. Anastopoulos

*/

// INCLUDE HEADER FILES:

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloRec/CaloClusterProcessor.h"
#include "GaudiKernel/EventContext.h"
#include "egammaInterfaces/IegammaSwTool.h"
#include "xAODCaloEvent/CaloClusterFwd.h"

class egammaSwTool
  : public AthAlgTool
  , virtual public IegammaSwTool
{

public:
  /** @brief Default constructor*/
  egammaSwTool(const std::string& type,
               const std::string& name,
               const IInterface* parent);

  /** @brief Destructor*/
  virtual ~egammaSwTool() = default;

  /** @brief initialize method*/
  virtual StatusCode initialize() override;
  /** @brief execute method*/
  virtual StatusCode execute(const EventContext& ctx,
                             xAOD::CaloCluster* cluster) const override final;
  virtual StatusCode execute(const EventContext& ctx,
                             xAOD::CaloCluster* cluster,
                             xAOD::EgammaParameters::EgammaType egType,
                             bool isBarrel) const override final;

private:
  StatusCode processTools(const EventContext& ctx,
                          const ToolHandleArray<CaloClusterProcessor>& tools,
                          xAOD::CaloCluster* cluster) const;

  /**
   * @brief a list of names for tools to correct clusters
   *
   * the tools in this list are executed after all maker tools are done
   * and run one after each other on each cluster in the container.  */
  ToolHandleArray<CaloClusterProcessor> m_clusterCorrectionPointersEle55{
    this,
    "ClusterCorrectionToolsEle55",
    {}
  };
  ToolHandleArray<CaloClusterProcessor> m_clusterCorrectionPointersEle35{
    this,
    "ClusterCorrectionToolsEle35",
    {}
  };
  ToolHandleArray<CaloClusterProcessor> m_clusterCorrectionPointersEle37{
    this,
    "ClusterCorrectionToolsEle37",
    {}
  };
  ToolHandleArray<CaloClusterProcessor> m_clusterCorrectionPointersGam35{
    this,
    "ClusterCorrectionToolsGam35",
    {}
  };
  ToolHandleArray<CaloClusterProcessor> m_clusterCorrectionPointersGam55{
    this,
    "ClusterCorrectionToolsGam55",
    {}
  };
  ToolHandleArray<CaloClusterProcessor> m_clusterCorrectionPointersGam37{
    this,
    "ClusterCorrectionToolsGam37",
    {}
  };
  ToolHandleArray<CaloClusterProcessor> m_clusterCorrectionPointersEconv55{
    this,
    "ClusterCorrectionToolsEconv55",
    {}
  };
  ToolHandleArray<CaloClusterProcessor> m_clusterCorrectionPointersEconv35{
    this,
    "ClusterCorrectionToolsEconv35",
    {}
  };
  ToolHandleArray<CaloClusterProcessor> m_clusterCorrectionPointersEconv37{
    this,
    "ClusterCorrectionToolsEconv37",
    {}
  };
  // SuperCluster
  ToolHandleArray<CaloClusterProcessor>
    m_clusterCorrectionPointersSuperClusterEle35{
      this,
      "ClusterCorrectionToolsSuperClusterEle35",
      {}
    };
  ToolHandleArray<CaloClusterProcessor>
    m_clusterCorrectionPointersSuperClusterEconv35{
      this,
      "ClusterCorrectionToolsSuperClusterEconv35",
      {}
    };
  ToolHandleArray<CaloClusterProcessor>
    m_clusterCorrectionPointersSuperClusterGam35{
      this,
      "ClusterCorrectionToolsSuperClusterGam35",
      {}
    };
  //
  ToolHandleArray<CaloClusterProcessor>
    m_clusterCorrectionPointersSuperClusterEle55{
      this,
      "ClusterCorrectionToolsSuperClusterEle55",
      {}
    };
  ToolHandleArray<CaloClusterProcessor>
    m_clusterCorrectionPointersSuperClusterGam55{
      this,
      "ClusterCorrectionToolsSuperClusterGam55",
      {}
    };
  ToolHandleArray<CaloClusterProcessor>
    m_clusterCorrectionPointersSuperClusterEconv55{
      this,
      "ClusterCorrectionToolsSuperClusterEconv55",
      {}
    };
  //
  ToolHandleArray<CaloClusterProcessor>
    m_clusterCorrectionPointersSuperClusterEle37{
      this,
      "ClusterCorrectionToolsSuperClusterEle37",
      {}
    };
  ToolHandleArray<CaloClusterProcessor>
    m_clusterCorrectionPointersSuperClusterGam37{
      this,
      "ClusterCorrectionToolsSuperClusterGam37",
      {}
    };
  ToolHandleArray<CaloClusterProcessor>
    m_clusterCorrectionPointersSuperClusterEconv37{
      this,
      "ClusterCorrectionToolsSuperClusterEconv37",
      {}
    };
};

#endif


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "egammaInterfaces/IegammaSwTool.h" 
#include "xAODCaloEvent/CaloClusterFwd.h"

class CaloClusterProcessor;

class egammaSwTool : public AthAlgTool, virtual public IegammaSwTool
{

 public:

  /** @brief Default constructor*/
  egammaSwTool(const std::string& type,
	       const std::string& name,
	       const IInterface* parent);

  /** @brief Destructor*/
  ~egammaSwTool();
  
  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief finalize method*/
  StatusCode finalize();
  /** @brief execute method*/
  StatusCode execute(xAOD::CaloCluster* cluster);
  StatusCode execute(xAOD::CaloCluster* cluster, xAOD::EgammaParameters::EgammaType egType , bool isBarrel) ;

 private:
  
  StatusCode populateTools(const std::vector<std::string>& names , std::vector<CaloClusterProcessor*>& tools) ;
  StatusCode processTools(const std::vector<CaloClusterProcessor*>& tools, xAOD::CaloCluster* cluster) const;

  /**
   * @brief a list of names for tools to correct clusters
   * 
   * the tools in this list are executed after all maker tools are done
   * and run one after each other on each cluster in the container.  */
  std::vector<std::string> m_clusterCorrectionNamesEle55;
  std::vector<std::string> m_clusterCorrectionNamesEle35;
  std::vector<std::string> m_clusterCorrectionNamesEle37;
  std::vector<std::string> m_clusterCorrectionNamesGam35;
  std::vector<std::string> m_clusterCorrectionNamesGam55;
  std::vector<std::string> m_clusterCorrectionNamesGam37;
  std::vector<std::string> m_clusterCorrectionNamesEconv55;
  std::vector<std::string> m_clusterCorrectionNamesEconv35;
  std::vector<std::string> m_clusterCorrectionNamesEconv37;
  //SuperCluster
  std::vector<std::string> m_clusterCorrectionNamesSuperClusterEle37;
  std::vector<std::string> m_clusterCorrectionNamesSuperClusterGam37;
  std::vector<std::string> m_clusterCorrectionNamesSuperClusterEconv37;
  //
  std::vector<std::string> m_clusterCorrectionNamesSuperClusterEle55;
  std::vector<std::string> m_clusterCorrectionNamesSuperClusterGam55;
  std::vector<std::string> m_clusterCorrectionNamesSuperClusterEconv55;
  //
  std::vector<std::string> m_clusterCorrectionNamesSuperClusterEle35;
  std::vector<std::string> m_clusterCorrectionNamesSuperClusterGam35;
  std::vector<std::string> m_clusterCorrectionNamesSuperClusterEconv35;

  /** @brief the actual list of tools corresponding to above names */
  std::vector<CaloClusterProcessor*>  m_clusterCorrectionPointersEle55; 
  std::vector<CaloClusterProcessor*>  m_clusterCorrectionPointersEle35; 
  std::vector<CaloClusterProcessor*>  m_clusterCorrectionPointersEle37; 
  std::vector<CaloClusterProcessor*>  m_clusterCorrectionPointersGam35; 
  std::vector<CaloClusterProcessor*>  m_clusterCorrectionPointersGam55; 
  std::vector<CaloClusterProcessor*>  m_clusterCorrectionPointersGam37;
  std::vector<CaloClusterProcessor*>  m_clusterCorrectionPointersEconv55; 
  std::vector<CaloClusterProcessor*>  m_clusterCorrectionPointersEconv35; 
  std::vector<CaloClusterProcessor*>  m_clusterCorrectionPointersEconv37; 
  //SuperCluster
  std::vector<CaloClusterProcessor*> m_clusterCorrectionPointersSuperClusterEle35;
  std::vector<CaloClusterProcessor*> m_clusterCorrectionPointersSuperClusterEconv35;
  std::vector<CaloClusterProcessor*> m_clusterCorrectionPointersSuperClusterGam35;
  //
  std::vector<CaloClusterProcessor*> m_clusterCorrectionPointersSuperClusterEle55;
  std::vector<CaloClusterProcessor*> m_clusterCorrectionPointersSuperClusterGam55;
  std::vector<CaloClusterProcessor*> m_clusterCorrectionPointersSuperClusterEconv55;
  //
  std::vector<CaloClusterProcessor*> m_clusterCorrectionPointersSuperClusterEle37;
  std::vector<CaloClusterProcessor*> m_clusterCorrectionPointersSuperClusterGam37;
  std::vector<CaloClusterProcessor*> m_clusterCorrectionPointersSuperClusterEconv37;

};

#endif










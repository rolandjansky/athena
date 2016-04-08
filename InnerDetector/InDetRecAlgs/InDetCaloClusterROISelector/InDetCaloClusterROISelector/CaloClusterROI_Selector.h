/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAREC_CALOCLUSTERROI_SELECTOR_H
#define EGAMMAREC_CALOCLUSTERROI_SELECTOR_H
/**
  @class CaloClusterROI_Selector
          Algorithm which creates an CaloClusterROICollection. 
          It retrieves data objects from TDS 
*/

// INCLUDE HEADER FILES:
#include <vector>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"

class IEMShowerBuilder;
class IegammaCheckEnergyDepositTool;
class IegammaIso;
class IegammaMiddleShape;


#include "xAODCaloEvent/CaloClusterFwd.h"

class CaloCellContainer;

namespace InDet {

class ICaloClusterROI_Builder;


class CaloClusterROI_Selector : public AthAlgorithm
{
 public:

  /** @brief Default constructor*/
  CaloClusterROI_Selector(const std::string& name, ISvcLocator* pSvcLocator);

  /** @brief Destructor*/
  ~CaloClusterROI_Selector();
	
  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief finalize method*/
  StatusCode finalize();
  /** @brief execute method*/
  StatusCode execute();

 private:
  bool PassClusterSelection(const xAOD::CaloCluster* cluster ,  const CaloCellContainer* cellcoll) ;
  /** @brief Name of the cluster intput collection*/
  std::string  m_inputClusterContainerName;
  /** @brief Name of the cluster output collection*/
  std::string  m_outputClusterContainerName;
  /** @brief Name of the cells container*/
  std::string  m_cellsName;

  //
  // The tools
  //
  // subalgorithm pointers cached in initialize:
  /** @brief isolation tool for Ethad1 / Ethad*/
  ToolHandle<IegammaIso>                       m_emCaloIsolationTool;
  /** @brief Pointer to the egammaCheckEnergyDepositTool*/
  ToolHandle<IegammaCheckEnergyDepositTool>    m_egammaCheckEnergyDepositTool;
  /** @Middle shape Tool*/
  ToolHandle<IegammaMiddleShape>               m_egammaMiddleShape;
  /** @brief Tool to build ROI*/
  ToolHandle<InDet::ICaloClusterROI_Builder>   m_caloClusterROI_Builder;
  //
  // All booleans
  //
  bool                              m_CheckEMsamples;
  bool                              m_CheckHadronicEnergy;
  bool                              m_CheckReta;
  //
  // Other properties.
  //
  /** @brief Cut on hadronic leakage*/
  double              m_HadRatioCut;
  double              m_RetaCut;
  double              m_ClusterEtCut;
	unsigned int         m_AllClusters;
  unsigned int         m_SelectedClusters;

  // others:
  IChronoStatSvc* m_timingProfile;

};

} //End namespace 
#endif

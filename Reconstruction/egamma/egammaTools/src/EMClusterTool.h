/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EMCLUSTERTOOL_H
#define EGAMMATOOLs_EMCLUSTERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "egammaInterfaces/IEMClusterTool.h"
#include "egammaBaseTool.h"

#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/EgammaEnums.h"

class IegammaSwTool;
class IegammaMVATool;
class StoreGateSvc;
class CaloCellDetPos;

/**
   @class EMClusterTool
   This tool makes the EM CaloCluster used by egamma objects.
   It also defines the cluster size 
       - in barrel
         3*7 for electrons
         3*7 for converted photons with Rconv<800mm
         3*5 for converted photons with Rconv>800mm
         3*5 for unconverted photons 
       - in end-caps
         5*5 for electrons
         5*5 for converted photons with Rconv<800mm
         5*5 for converted photons with Rconv>800mm
         5*5 for unconverted photons 
    and applies the right cluster corrections depending on classification as electron, unconverted photon or converted photon
    
   @author Thomas Koffas
   @author F. Derue
   @author B. Lenzi
   @author C. Anastopoulos
*/

class EMClusterTool : public egammaBaseTool, virtual public IEMClusterTool {

 public:

  /** @bried constructor */
  EMClusterTool (const std::string& type,const std::string& name, const IInterface* parent);

  /** @brief destructor */
  virtual ~EMClusterTool();

  /** @brief initialize method */
  virtual StatusCode initialize();
  /** @brief execute on container */
  virtual StatusCode contExecute();
  /** @brief finalize method */
  virtual StatusCode finalize();
  
  /** @brief Set new cluster to the egamma object, decorate the new cluster
    * with a link to the old one **/
  void setNewCluster(xAOD::Egamma *eg,
                     xAOD::CaloClusterContainer *outputClusterContainer,
                     xAOD::EgammaParameters::EgammaType egType);
  
  /** @brief creation of new cluster based on existing one **/
  virtual xAOD::CaloCluster* makeNewCluster(const xAOD::CaloCluster&, xAOD::Egamma *eg, xAOD::EgammaParameters::EgammaType);

  void fillPositionsInCalo(xAOD::CaloCluster* cluster);
 private:
  /** @brief Position in Calo frame**/


  /** @brief Name of the output cluster container **/
  std::string m_outputClusterContainerName;

  /** Handle to the MVA calibration Tool **/
  ToolHandle<IegammaMVATool>  m_MVACalibTool;  

  /** @brief Name of the input electron container **/
  std::string m_electronContainerName;

  /** @brief Name of the input photon container **/
  std::string m_photonContainerName;  
 
  /** @brief Tool to handle cluster corrections */
  ToolHandle<IegammaSwTool>   m_clusterCorrectionTool;
  
  /** @brief Name of tool for cluster corrections */
  std::string            m_ClusterCorrectionToolName;
  
  /** @brief Decorate clusters with positions in calo frame? **/
  bool m_doPositionInCalo;

  /** @brief Call CaloClusterStoreHelper::finalizeClusters ? **/ 
  bool m_finalizeClusters;
  
  StoreGateSvc*   m_storeGate;
  
  CaloCellDetPos *m_caloCellDetPos;
};

#endif // EGAMMATOOLS_EMCLUSTERTOOL_H 

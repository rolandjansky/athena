/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EMCLUSTERTOOL_H
#define EGAMMATOOLS_EMCLUSTERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "egammaInterfaces/IEMClusterTool.h"
#include "egammaBaseTool.h"

#include "xAODCaloEvent/CaloCluster.h" // cannot use CaloClusterFwd b/c of ClusterSize
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
  
  void fillPositionsInCalo(xAOD::CaloCluster* cluster) const ;
 private:

  /** @brief Set new cluster to the egamma object, decorate the new cluster
    * with a link to the old one **/
  void setNewCluster(xAOD::Egamma *eg,
                     xAOD::CaloClusterContainer *outputClusterContainer,
                     xAOD::EgammaParameters::EgammaType egType);
  
  /** @brief creation of new cluster based on existing one 
    * Return a new cluster using the seed eta0, phi0 from the existing one, 
    * applying cluster corrections and MVA calibration (requires the egamma object).
    * The cluster size depends on the given EgammaType
    */
  virtual xAOD::CaloCluster* makeNewCluster(const xAOD::CaloCluster&, xAOD::Egamma *eg, 
					    xAOD::EgammaParameters::EgammaType);

  /** @brief creation of new cluster based on existing one 
    * Return a new cluster with the given size using the seed eta0, phi0 from the
    * existing cluster and applying cluster corrections. 
    * If doDecorate is true, copy the cal to the raw signal state
    * and set the raw one to the cal e,eta,phi from the existing cluster
    */
  virtual xAOD::CaloCluster* makeNewCluster(const xAOD::CaloCluster&, 
                                            const xAOD::CaloCluster::ClusterSize&);

  /** @brief creation of new super cluster based on existing one */
  xAOD::CaloCluster* makeNewSuperCluster(const xAOD::CaloCluster& cluster, xAOD::Egamma *eg);  

  /** @brief Name of the output cluster container **/
  std::string m_outputClusterContainerName;

  /** @brief Name of the output cluster container for topo-seeded clusters **/
  std::string m_outputTopoSeededClusterContainerName;

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
  
  /** @brief do super clusters **/ 
  bool m_doSuperClusters;

  /** @brief flag to protect against applying the MVA to super Clusters **/ 
  bool m_applySuperClusters;

  /** @brief Position in Calo frame**/  
  std::unique_ptr<CaloCellDetPos> m_caloCellDetPos;
};

#endif // EGAMMATOOLS_EMCLUSTERTOOL_H

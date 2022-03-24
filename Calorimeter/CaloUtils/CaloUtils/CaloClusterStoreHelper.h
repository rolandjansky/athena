/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOUTILS_CALOCLUSTERSTOREHELPER_H
#define CALOUTILS_CALOCLUSTERSTOREHELPER_H

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "StoreGate/WriteHandle.h"
#include <memory>

class CaloCellContainer;
class StoreGateSvc;

class CaloClusterStoreHelper {
public:

  //////////////////
  // Make Cluster //
  //////////////////

  /** 
   *  @brief Creates a valid \a CaloCluster with a private Aux-Store and CellLink container 
   *  @param cellCont pointer to the underlying CaloCellContainer
   *  @return unique_ptr to a \a CaloCluster object.
   */
  static std::unique_ptr<xAOD::CaloCluster> makeCluster(const CaloCellContainer* cellCont);
  
  /** 
   *  @brief Creates a valid \a CaloCluster with a private Aux-Store and CellLink container 
   *  @param cellCont pointer to the underlying CaloCellContainer
   *  @param eta0 seed \f$ \eta \f$ of cluster 
   *  @param phi0 seed \f$ \varphi \f$ of cluster 
   *  @param clusterSize
   *
   *  @return unique_ptr to a \a CaloCluster object
   */
  static std::unique_ptr<xAOD::CaloCluster> makeCluster(const CaloCellContainer* cellCont,
							const double eta0, const double phi0,
							const xAOD::CaloCluster_v1::ClusterSize clusterSize);


  /** 
   *  @brief Creates a valid \a CaloCluster and pushes it into the cluster container
   *  @param cont ptr to and xAOD::CaloClusterContainer (must be associated with an CaloClusterAuxContainer);
   *  @param cellCont pointer to the underlying CaloCellContainer
   *  @return  Pointer to a \a CaloCluster object
   */

  static xAOD::CaloCluster* makeCluster(xAOD::CaloClusterContainer* cont, 
					const CaloCellContainer* cellCont);


  /** 
   * @brief Creates a new \a xAOD::CaloClusterContainer in the given WriteHandle + \a CaloClusterAuxContainer and records them to SG
   * @param clusColl SG write handle key of \a ClusterContainer
   * @return status code indicating sucess or failure
   */ 
  static StatusCode AddContainerWriteHandle(SG::WriteHandle<xAOD::CaloClusterContainer> &clusColl);

  ////////////////////
  // finalize Clusters //
  ////////////////////
	 
 
  /*! \brief Finalize clusters (move CaloClusterCellLink to separate container*/
  static StatusCode finalizeClusters(SG::WriteHandle<CaloClusterCellLinkContainer>& h,
				     xAOD::CaloClusterContainer* pClusterColl);




  /**
   * @brief Deep copy of a cluster container.
   * @param old The container to copy.
   * @return Copy of the container and its clusters (an owning DV).
   */
  //Moved to here from CaloRunClusterCorrection
  static void copyContainer (const xAOD::CaloClusterContainer* oldColl, xAOD::CaloClusterContainer* newColl);
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef CALOREC_CALOCLUSTERMAKER_H
#define CALOREC_CALOCLUSTERMAKER_H

/**
 * @class CaloClusterMaker
 * @version \$Id: CaloClusterMaker.h,v 1.13 2009-04-18 02:56:15 ssnyder Exp $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 26-April-2004
 * @brief Top algorithm to reconstruct CaloCluster objects from
 * CaloCell objects
 *
 * This class is an Algorithm to reconstruct objects of type
 * CaloCluster from a collection of CaloCell objects. The actual
 * clustering is performed by tools which can be specified in the
 * jobOptions. Only one CaloClusterContainer is created and all tools
 * will modify that container.  The top algorithm is responsible for
 * recording and locking the cluster container in StoreGate. Two types
 * of tools are supported: maker tools and correction tools. The
 * former modify the entire cluster container, the latter just single
 * clusters.  */

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloRec/CaloClusterCollectionProcessor.h"

class CaloClusterCellLinkContainer;
class IChronoStatSvc;

class CaloClusterMaker : public AthReentrantAlgorithm
{

 public:

  CaloClusterMaker(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CaloClusterMaker() override;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

  const std::string& getOutputContainerName() const;

 private:

  /**
   * @brief Method to create a CaloClusterContainer together with it's AuxStore and Link container
   *
   */
  StatusCode makeContainer();

  
  /** @brief the name of the key in StoreGate for the output
      CaloClusterContainer */
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_clusterOutput;

  /** @brief the name of the key in StoreGate for the output
      CaloClusterCellLinkContainer */
  SG::WriteHandleKey<CaloClusterCellLinkContainer> m_clusterCellLinkOutput;

  /**
   * @brief a list of names for tools to make clusters
   * 
   * the tools in this list are executed one after the other on the
   * entire CaloClusterContainer.  */
  //std::vector<std::string> m_clusterMakerNames;     
  ToolHandleArray<CaloClusterCollectionProcessor>  m_clusterMakerTools;

  /** @brief the actual list of tools corresponding to above names */
  //std::vector<CaloClusterCollectionProcessor*>  m_clusterMakerPointers;

  /**
   * @brief a list of names for tools to correct clusters
   * 
   * the tools in this list are executed after all maker tools are done
   * and run one after each other on each cluster in the container.  */
  //std::vector<std::string> m_clusterCorrectionNames; 
  ToolHandleArray<CaloClusterCollectionProcessor> m_clusterCorrectionTools; 

  //Handle to the ChronoStatSvc
  ServiceHandle<IChronoStatSvc> m_chrono;

  /** 
   * @brief controls saving the uncalibrated signal state just before
   * the first @c CaloClusterCorrectionTool is invoked. Is a configurable 
   * property with default value @c true.
   */
  bool m_saveSignalState;

  ///Use ChronotStatSvc to monitor each tool
  bool m_chronoTools;

};
#endif // CALOREC_CALOCLUSTERMAKER_H




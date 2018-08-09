///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloClusterCellLinksUpdater.h 
// Header file for class CaloClusterCellLinksUpdater
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef CALOREC_CALOCLUSTERCELLLINKSUPDATER_H
#define CALOREC_CALOCLUSTERCELLLINKSUPDATER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"



class CaloClusterCellLinksUpdater
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  CaloClusterCellLinksUpdater( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~CaloClusterCellLinksUpdater(); 

  // Athena algorithm's Hooks
  /// Standard algorithm initialize
  virtual StatusCode  initialize();
  /// Standard algorithm execute
  virtual StatusCode  execute();
  /// Standard algorithm finalize
  virtual StatusCode  finalize();

 private: 
  /// Default constructor: 
  CaloClusterCellLinksUpdater();

  /// Name of the CaloCellContainer the links should point to(jobProperty) 
  std::string m_newCaloCellName;

  /// CaloClusters to be considered (jobProperty)
  std::vector<std::string> m_caloClusterNames;

  /// Set fo CaloClusters to be considered (no duplicates)
  std::set<std::string> m_caloClusterNameSet;

  bool m_firstEvent;
 
}; 

#endif //> !CALOREC_CALOCLUSTERCELLLINKSUPDATER_H

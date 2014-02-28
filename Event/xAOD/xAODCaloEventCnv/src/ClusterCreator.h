// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ClusterCreator.h 581807 2014-02-06 08:40:10Z krasznaa $
#ifndef XAODCREATORALGS_CLUSTERCREATOR_H
#define XAODCREATORALGS_CLUSTERCREATOR_H

// System include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

/**
 *  @short Algorithm creating xAOD::CaloClusters from CaloClusters
 *
 *         This algorithm can be used to translate the CaloClusters coming
 *         from an AOD, and create xAOD::CaloCluster object out of them
 *         for an output xAOD.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 581807 $
 * $Date: 2014-02-06 09:40:10 +0100 (Thu, 06 Feb 2014) $
 */
class ClusterCreator : public AthAlgorithm {

public:
  /// Regular algorithm constructor
  ClusterCreator( const std::string& name, ISvcLocator* svcLoc );
  
  /// Function initialising the algorithm
  virtual StatusCode initialize();
  /// Function executing the algorithm
  virtual StatusCode execute();

private:
  /// The keys of the input CaloClusterContainer
  std::vector<std::string> m_aodContainerNames;
  /// The keys for the output xAOD::CaloClusterContainer
  std::vector<std::string> m_xaodContainerNames;
  
  ///True if keys set by jobO
  bool m_keySet;

}; // class ClusterCreator

#endif // XAODCREATORALGS_CLUSTERCREATOR_H

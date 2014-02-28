// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ClusterDumper.h 581807 2014-02-06 08:40:10Z krasznaa $
#ifndef XAODCREATORALGS_CLUSTERDUMPER_H
#define XAODCREATORALGS_CLUSTERDUMPER_H

// System include(s):
#include <string>
#include <fstream>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"


class ClusterDumper : public AthAlgorithm {

public:
  /// Regular algorithm constructor
  ClusterDumper( const std::string& name, ISvcLocator* svcLoc );
  
  /// Function initialising the algorithm
  virtual StatusCode initialize();
  /// Function executing the algorithm
  virtual StatusCode execute();

  virtual StatusCode finalize();

  

private:
  /// The key for the output xAOD::CaloClusterContainer
  std::string m_containerName;
  std::string m_fileName;

  std::ostream* m_out;
  std::ofstream m_fileOut;
  
}; // class ClusterDumper

#endif // XAODCREATORALGS_CLUSTERCREATOR_H

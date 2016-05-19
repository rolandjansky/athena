/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOREC_CALOCLUSTERSNAPSHOT_H
#define CALOREC_CALOCLUSTERSNAPSHOT_H

#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "AthenaBaseComps/AthAlgTool.h"

class CaloClusterSnapshot : public AthAlgTool, virtual public CaloClusterCollectionProcessor {
 public:

  // constructor 
  CaloClusterSnapshot(const std::string& type, const std::string& name,
		      const IInterface* parent);
  // destructor 
  ~CaloClusterSnapshot();

  virtual StatusCode initialize();
  virtual StatusCode execute(xAOD::CaloClusterContainer* );

 private:
  // properties 
  std::string m_outputName;
  bool m_setCrossLinks;

  const std::string* m_finalContName;
  
};

#endif










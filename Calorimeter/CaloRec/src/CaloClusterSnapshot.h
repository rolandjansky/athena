/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOREC_CALOCLUSTERSNAPSHOT_H
#define CALOREC_CALOCLUSTERSNAPSHOT_H

#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "CaloEvent/CaloClusterCellLinkContainer.h"
#include "AthenaBaseComps/AthAlgTool.h"

class CaloClusterSnapshot : public AthAlgTool, virtual public CaloClusterCollectionProcessor {
 public:

  // constructor 
  CaloClusterSnapshot(const std::string& type, const std::string& name,
		      const IInterface* parent);
  // destructor 
  virtual ~CaloClusterSnapshot() override;

  using CaloClusterCollectionProcessor::execute;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx,
                             xAOD::CaloClusterContainer* ) const override;

 private:
  // properties 
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_outputKey;
  SG::WriteHandleKey<CaloClusterCellLinkContainer> m_cellLinkOutputKey;

  bool m_setCrossLinks;

  const std::string* m_finalContName;
  
};

#endif










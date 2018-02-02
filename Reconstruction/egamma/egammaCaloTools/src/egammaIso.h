/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOTOOLS_EGAMMAISO_H
#define EGAMMACALOTOOLS_EGAMMAISO_H

/// @class egammaIso
/// @brief {Tool to estimate the hadronic energy behind the electromagnetic cluster, the 

#include "AthenaBaseComps/AthAlgTool.h"


class CaloCellList; 
class ICalorimeterNoiseTool;
#include "xAODCaloEvent/CaloClusterFwd.h"
#include <vector>
#include "egammaInterfaces/IegammaIso.h"

class egammaIso : public AthAlgTool, virtual public IegammaIso {

 public:

  /** @brief Default constructor*/
  egammaIso(const std::string& type,
	    const std::string& name,
	    const IInterface* parent);

  /** @brief Destructor*/
  ~egammaIso();  

  /** @brief initialize method*/
  StatusCode initialize() override;
  /** @brief finalize method*/
  StatusCode finalize() override;
  /** @brief  Method to just calculate hadronic leakage*/
  virtual StatusCode execute(const xAOD::CaloCluster& cluster, CaloCellList& had,Info& info) const override final;
 private:
 
};

#endif

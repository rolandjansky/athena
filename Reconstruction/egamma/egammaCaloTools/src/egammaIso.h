/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOTOOLS_EGAMMAISO_H
#define EGAMMACALOTOOLS_EGAMMAISO_H

/// @class egammaIso
/// @brief {Tool to estimate the hadronic energy behind the electromagnetic cluster, the 

#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "egammaInterfaces/IegammaIso.h"
#include <vector>

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
  virtual StatusCode execute(const xAOD::CaloCluster& cluster,
                             const CaloDetDescrManager& cmgr,                                                           
                             const CaloCellContainer& cellcoll,
                             Info& info) const override final;
 
};

#endif

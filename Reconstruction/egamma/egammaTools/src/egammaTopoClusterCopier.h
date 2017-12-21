/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EGAMMATOPOCLUSTERCOPIER_H
#define EGAMMATOOLS_EGAMMATOPOCLUSTERCOPIER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "egammaInterfaces/IegammaTopoClusterCopier.h"
#include "egammaBaseTool.h"
#include "xAODCaloEvent/CaloClusterFwd.h" 

class egammaTopoClusterCopier : public egammaBaseTool, virtual public IegammaTopoClusterCopier {

 public:

  /** @bried constructor */
  egammaTopoClusterCopier (const std::string& type,const std::string& name, const IInterface* parent);

  /** @brief destructor */
  virtual ~egammaTopoClusterCopier(){}

  /** @brief initialize method */
  virtual StatusCode initialize();
  /** @brief execute on container */
  virtual StatusCode contExecute();
  /** @brief finalize method */
  virtual StatusCode finalize();
  /** @brief execute on direct objects */
  virtual StatusCode hltExecute(std::pair<xAOD::CaloClusterContainer*, xAOD::ShallowAuxContainer* > &inputShallowcopy, ConstDataVector<xAOD::CaloClusterContainer>* viewCopy) const;
  
  private:

  StatusCode  copyCaloTopo()const;
  StatusCode  checkEMFraction (const xAOD::CaloCluster *clus, float &emFrac) const;
  StatusCode  copyCaloTopo_impl(std::pair<xAOD::CaloClusterContainer*, xAOD::ShallowAuxContainer* > &inputShallowcopy, ConstDataVector<xAOD::CaloClusterContainer>* viewCopy) const;

  std::string m_inputTopoCollection;
  std::string m_outputTopoCollection;
  float m_etaCut;
  float m_ECut;
  float m_EMFracCut;
  float m_EMCrackEtCut;
  
};

#endif // EGAMMATOOLS_EMCLUSTERTOOL_H

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
  
  private:
  StatusCode  copyEMTopo() const;
  StatusCode  copyCaloTopo() const;
  float GetEMFraction (const xAOD::CaloCluster *clus) const;
  std::string m_inputTopoCollection;
  std::string m_outputTopoCollection;
  bool m_isHadronic;
  float m_etaCut;
  float m_ECut;
  float m_EMFracCut;

};

#endif // EGAMMATOOLS_EMCLUSTERTOOL_H

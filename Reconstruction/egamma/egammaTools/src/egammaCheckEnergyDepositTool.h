/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EGAMMACHECKENERGYDEPOSITTOOL_H
#define EGAMMATOOLS_EGAMMACHECKENERGYDEPOSITTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"

#include "egammaInterfaces/IegammaCheckEnergyDepositTool.h"

/**
   @class egammaCheckEnergyDepositTool
   Tool to check in each sampling that fraction of energy 
   reconstructed is not greater thana a certain threshold

   @author Frederic Derue
*/

class egammaCheckEnergyDepositTool : virtual public IegammaCheckEnergyDepositTool, public AthAlgTool {

 public:

  /** @bried constructor */
  egammaCheckEnergyDepositTool (const std::string& type,const std::string& name, const IInterface* parent);

  /** @brief destructor */
  virtual ~egammaCheckEnergyDepositTool();

  /** @brief initialize method */
  virtual StatusCode initialize();
  /** @brief finalize method */
  virtual StatusCode finalize();
  /** @brief execute method */
  virtual bool checkFractioninSampling(const xAOD::Egamma* eg);
  virtual bool checkFractioninSamplingCluster(const xAOD::CaloCluster* cluster) ;
  /** @brief Boolean to apply threshold on maximal fraction of energy reconstructed in each sampling to write egamma objects */
  bool m_useThrFmax;
  /** @brief Threshold on maximal fraction of energy reconstructed in presampler */
  double m_thrF0max;
  /** @brief Threshold on maximal fraction of energy reconstructed in 1st sampling*/
  double m_thrF1max;
  /** @brief Threshold on maximal fraction of energy reconstructed in 2nd sampling */
  double m_thrF2max;

  /** @brief Threshold on maximal fraction of energy reconstructed in 3rd sampling */
  double m_thrF3max;

 private:

};

#endif // EGAMMATOOLS_EGAMMACHECKENERGYDEPOSITTOOL_H

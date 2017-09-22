/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EGAMMACHECKENERGYDEPOSITTOOL_H
#define EGAMMATOOLS_EGAMMACHECKENERGYDEPOSITTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

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

  /** @brief constructor */
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

 private:

  /** @brief Boolean to apply threshold on maximal fraction of energy reconstructed in each sampling to write egamma objects */
  Gaudi::Property<bool> m_useThrFmax {this, "UseThrFmax", false,
      "Boolean to apply threshold on maximal fraction of energy reconstructed in each sampling to write egamma objects"};

  /** @brief Threshold on maximal fraction of energy reconstructed in presampler */
  Gaudi::Property<double> m_thrF0max {this, "ThrF0max", 0.9,
      "Threshold on maximal fraction of energy reconstructed in presampler"};

  /** @brief Threshold on maximal fraction of energy reconstructed in 1st sampling*/
  Gaudi::Property<double> m_thrF1max {this, "ThrF1max", 0.8,
      "Threshold on maximal fraction of energy reconstructed in 1st sampling"};

  /** @brief Threshold on maximal fraction of energy reconstructed in 2nd sampling */
  Gaudi::Property<double> m_thrF2max {this, "ThrF2max", 0.98,
      "Threshold on maximal fraction of energy reconstructed in 2nd sampling"};

  /** @brief Threshold on maximal fraction of energy reconstructed in 3rd sampling */
  Gaudi::Property<double> m_thrF3max {this, "ThrF3max", 0.65,
      "Threshold on maximal fraction of energy reconstructed in 3rd sampling"};


};

#endif // EGAMMATOOLS_EGAMMACHECKENERGYDEPOSITTOOL_H

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOTOOLS_EGAMMACHECKENERGYDEPOSITTOOL_H
#define EGAMMACALOTOOLS_EGAMMACHECKENERGYDEPOSITTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "egammaInterfaces/IegammaCheckEnergyDepositTool.h"

/**
   @class egammaCheckEnergyDepositTool
   Tool to check in each sampling that fraction of energy 
   reconstructed is not greater thana a certain threshold
   @authors Frederic Derue
*/

class egammaCheckEnergyDepositTool : virtual public IegammaCheckEnergyDepositTool, public AthAlgTool {

 public:

  /** @brief constructor */
  egammaCheckEnergyDepositTool (const std::string& type,const std::string& name, const IInterface* parent);

  /** @brief destructor */
  virtual ~egammaCheckEnergyDepositTool();

  /** @brief initialize method */
  virtual StatusCode initialize() override;
  /** @brief finalize method */
  virtual StatusCode finalize() override;
  /** @brief execute method */
  virtual bool checkFractioninSamplingCluster(const xAOD::CaloCluster* cluster) const override final ;

 private:
  /** @brief Threshold on minimum energy reconstructed in 2nd sampling */
  Gaudi::Property<double> m_thrE2min{this, "ThrE2min", 400.,
      "Threshold on minimum  energy reconstructed in 2nd sampling"};

  /** @brief Threshold on maximum fraction of energy reconstructed in presampler */
  Gaudi::Property<double> m_thrF0max {this, "ThrF0max", 0.9,
      "Threshold on maximum fraction of energy reconstructed in presampler"};

  /** @brief Threshold on maximum fraction of energy reconstructed in 1st sampling*/
  Gaudi::Property<double> m_thrF1max {this, "ThrF1max", 0.8,
      "Threshold on maximum fraction of energy reconstructed in 1st sampling"};

  /** @brief Threshold on maximum fraction of energy reconstructed in 2nd sampling */
  Gaudi::Property<double> m_thrF2max {this, "ThrF2max", 0.98,
      "Threshold on maximum fraction of energy reconstructed in 2nd sampling"};

  /** @brief Threshold on maximum fraction of energy reconstructed in 3rd sampling */
  Gaudi::Property<double> m_thrF3max {this, "ThrF3max", 0.8,
      "Threshold on maximum fraction of energy reconstructed in 3rd sampling"};

};

#endif // EGAMMATOOLS_EGAMMACHECKENERGYDEPOSITTOOL_H

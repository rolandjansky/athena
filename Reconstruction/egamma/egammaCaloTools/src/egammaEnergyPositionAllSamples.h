/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOTOOLS_EGAMMAENERGYPOSITIONALLSAMPLES_H
#define EGAMMACALOTOOLS_EGAMMAENERGYPOSITIONALLSAMPLES_H

/// @class egammaEnergyPositionAllSamples
/// @brief   Tool to define precisely the energy and position in each sampling
///  From the original (eta,phi) position of the cluster, it finds the location
///  (sampling, barrel/end-cap, granularity)
///  For this we use the CaloCluster method inBarrel() and inEndcap()
///  but also, in case close to the crack region where both 
///  boolean can be true, the energy reconstructed in the sampling
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos
///
/// $Revision:$
/// $Date: 2014-02-11 17:40:48 +0100 (Tue, 11 Feb 2014) $
///


class CaloDetDescrManager;

#include "xAODCaloEvent/CaloClusterFwd.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "egammaInterfaces/IegammaEnergyPositionAllSamples.h"

class egammaEnergyPositionAllSamples : public AthAlgTool, virtual public IegammaEnergyPositionAllSamples {

 public:
  
  /** @brief Default constructor*/
  egammaEnergyPositionAllSamples(const std::string& type,
				 const std::string& name,
				 const IInterface* parent);
  /** @brief Destructor*/
  ~egammaEnergyPositionAllSamples();  

  /** @brief AlgTool initialize method.*/
  StatusCode initialize() override;
  /** @brief AlgTool finalize method */
  StatusCode finalize() override;

 /** @brief return the uncorrected sum of energy in all samples*/
  double e(const xAOD::CaloCluster& cluster) const override final;
  /** @brief return the uncorrected cluster energy in pre-sampler*/
  double e0(const xAOD::CaloCluster& cluster) const override final;
  /** @brief return the uncorrected cluster energy in 1st sampling*/
  double e1(const xAOD::CaloCluster& cluster) const override final;
  /** @brief return the uncorrected cluster energy in 2nd sampling*/
  double e2(const xAOD::CaloCluster& cluster) const override final;
  /** @brief return the uncorrected cluster energy in 3rd sampling*/
  double e3(const xAOD::CaloCluster& cluster) const override final;
  /** @brief return boolean to know if we are in barrel/end-cap*/
  bool inBarrel(const xAOD::CaloCluster& cluster, int is) const override final;

 private:

};

//
// set values for the different variables in the egammaEnergyPositionAllSamples
//
#endif

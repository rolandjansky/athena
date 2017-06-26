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
  StatusCode initialize();
  /** @brief AlgTool finalize method */
  StatusCode finalize();

  /** @brief AlgTool main method */
  virtual StatusCode execute(const xAOD::CaloCluster *cluster);
  virtual StatusCode execute(const xAOD::CaloCluster *cluster, int is);

  /** @brief method to print results*/
  void print() const;
  /** @brief return the uncorrected sum of energy in all samples*/
  double e() const;
  /** @brief return the uncorrected cluster energy in pre-sampler*/
  double e0() const;
  /** @brief return the uncorrected cluster energy in 1st sampling*/
  double e1() const;
  /** @brief return the uncorrected cluster energy in 2nd sampling*/
  double e2() const;
  /** @brief return the uncorrected cluster energy in 3rd sampling*/
  double e3() const;
  /** @brief return boolean to know if we are in barrel/end-cap*/
  bool inBarrel() const;

 private:
 
  StatusCode energy(); 
  /** @brief method to check if cluster is in barrel/end-cap 
   using also information from energy deposit*/
  bool isClusterinBarrel(int is = 2);

  /** @brief pointer to a CaloCluster*/
  const xAOD::CaloCluster* m_cluster; 

  /** @brief sum of energy in all samplings*/
  double m_eallsamples;
  /** @brief cluster energy in presampler*/
  double m_e0;
  /** @brief cluster energy in 1st sampling*/
  double m_e1;
  /** @brief cluster energy in 2nd sampling*/
  double m_e2;
  /** @brief cluster energy in 3rd sampling*/
  double m_e3;
  /** @brief boolean to know if we are in barrel/end-cap */
  bool m_inbarrel;

};

//
// set values for the different variables in the egammaEnergyPositionAllSamples
//
inline double egammaEnergyPositionAllSamples::e()        const { return m_eallsamples; }
inline double egammaEnergyPositionAllSamples::e0()       const { return m_e0; }
inline double egammaEnergyPositionAllSamples::e1()       const { return m_e1; }
inline double egammaEnergyPositionAllSamples::e2()       const { return m_e2; }
inline double egammaEnergyPositionAllSamples::e3()       const { return m_e3; }
inline bool   egammaEnergyPositionAllSamples::inBarrel() const { return m_inbarrel; }

#endif

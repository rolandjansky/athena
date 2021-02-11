/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_TAUPI0CLUSTERSCALER_H
#define TAURECTOOLS_TAUPI0CLUSTERSCALER_H

#include <string>
#include "tauRecTools/TauRecToolBase.h"
#include "AsgTools/ToolHandle.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/PFOAuxContainer.h"

/**
 * @brief scale cluster energy to take care of charged pion energy
 *
 * @author Stephanie Yuen <stephanie.yuen@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch> 
 * @author Will Davey <will.davey@cern.ch> 
 */

class TauPi0ClusterScaler : virtual public TauRecToolBase {

public:
  
  ASG_TOOL_CLASS2(TauPi0ClusterScaler, TauRecToolBase, ITauToolBase)

  TauPi0ClusterScaler(const std::string& name);
  virtual ~TauPi0ClusterScaler() = default;

  virtual StatusCode executePi0ClusterScaler(xAOD::TauJet& pTau, xAOD::PFOContainer& pNeutralPFOContainer, xAOD::PFOContainer& pChargedPFOContainer) const override; 

private:
  
  /** @brief Clear accosicated partcle links for the pfo container */
  void clearAssociatedParticleLinks(xAOD::PFOContainer& pfoContainer, xAOD::PFODetails::PFOParticleType type) const;

  /** @brief Get extrapolated position to the CAL */
  float getExtrapolatedPosition(const xAOD::PFO& chargedPFO, xAOD::TauJetParameters::TrackDetail detail) const; 

  /** @brief Correct neutral PFO kinematics to point at the current tau vertex */
  void correctNeutralPFOs(xAOD::TauJet& pTau, xAOD::PFOContainer& pNeutralPFOContainer) const;

  /** @brief create charged PFOs */
  void createChargedPFOs(xAOD::TauJet& pTau, xAOD::PFOContainer& pChargedPFOContainer) const;

  /** @brief associate hadronic PFOs to charged PFOs */
  void associateHadronicToChargedPFOs(xAOD::TauJet& pTau, xAOD::PFOContainer& pChargedPFOContainer) const;
  
  /** @brief associate charged PFOs to neutral PFOs */
  void associateChargedToNeutralPFOs(xAOD::TauJet& pTau, xAOD::PFOContainer& pNeutralPFOContainer) const;
  
  /** @brief associate charged PFOs to neutral PFOs */
  void subtractChargedEnergyFromNeutralPFOs(xAOD::PFOContainer& pNeutralPFOContainer) const;
};

#endif  // TAURECTOOLS_TAUPI0CLUSTERSCALER_H

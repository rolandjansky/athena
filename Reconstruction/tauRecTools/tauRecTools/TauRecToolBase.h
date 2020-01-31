/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLBASE_H
#define TAURECTOOLBASE_H
/**
 * @brief The base class for all tau tools.
 * 
 * @author Lukasz Janyst
 * @author Justin Griffiths
 * Thanks to Lianyou Shan, Lorenz Hauswald
 */

#include <string>

#include "tauRecTools/TauEventData.h"
#include "tauRecTools/ITauToolBase.h"
#include "AsgTools/AsgTool.h"

extern TauEventData defaultTauEventData;

class TauRecToolBase : public asg::AsgTool, virtual public ITauToolBase {
 public:

  ASG_TOOL_INTERFACE(TauRecToolBase)
  ASG_TOOL_CLASS1( TauRecToolBase, ITauToolBase )

    TauRecToolBase(const std::string& name);
  virtual ~TauRecToolBase() {}

  //-----------------------------------------------------------------
  //! Tool initializer
  //-----------------------------------------------------------------
  virtual StatusCode initialize() override;

  //-----------------------------------------------------------------
  //! Event initializer - called at the beginning of each event
  //-----------------------------------------------------------------
  virtual StatusCode eventInitialize() override;

  //-----------------------------------------------------------------
  //! Execute - called for each tau candidate
  //-----------------------------------------------------------------
  virtual StatusCode execute(xAOD::TauJet& pTau) override;
  virtual StatusCode executeShotFinder(xAOD::TauJet& pTau, xAOD::CaloClusterContainer& shotClusterContainer, xAOD::PFOContainer& PFOContainer ) override;
  virtual StatusCode executePi0CreateROI(xAOD::TauJet& pTau, CaloCellContainer& caloCellContainer, std::vector<CaloCell*>& map ) override;
  virtual StatusCode executePi0ClusterCreator(xAOD::TauJet& pTau, xAOD::PFOContainer& neutralPFOContainer, 
					      xAOD::PFOContainer& hadronicPFOContainer, 
					      xAOD::CaloClusterContainer& caloClusterContainer, 
					      const xAOD::CaloClusterContainer& pCaloClusterContainer ) override;
  virtual StatusCode executeVertexVariables(xAOD::TauJet& pTau, xAOD::VertexContainer& vertexContainer ) override;  
  virtual StatusCode executePi0ClusterScaler(xAOD::TauJet& pTau, xAOD::PFOContainer& neutralPFOContainer, xAOD::PFOContainer& chargedPFOContainer ) override;  
  virtual StatusCode executePi0nPFO(xAOD::TauJet& pTau, xAOD::PFOContainer& neutralPFOContainer) override;
  virtual StatusCode executePanTau(xAOD::TauJet& pTau, xAOD::ParticleContainer& particleContainer) override;

  //-----------------------------------------------------------------
  //! Event finalizer - called at the end of each event
  //-----------------------------------------------------------------
  virtual StatusCode eventFinalize() override;

  //-----------------------------------------------------------------
  //! Finalizer
  //-----------------------------------------------------------------
  virtual StatusCode finalize() override;

  std::string find_file(const std::string& fname) const;
  virtual StatusCode readConfig() override;

  void setTauEventData(TauEventData* data) override;
  TauEventData* tauEventData();
  const TauEventData* tauEventData() const;



 protected:
  TauEventData* m_data = 0;
  bool m_in_trigger = false;
  std::string m_tauRecToolsTag;

  bool inTrigger() const;

};

inline bool TauRecToolBase::inTrigger() const { return m_in_trigger; }

inline TauEventData* TauRecToolBase::tauEventData() { 
  return m_data;
}

inline const TauEventData* TauRecToolBase::tauEventData() const { 
  return m_data;
}

#endif //TAURECTOOLBASE_H

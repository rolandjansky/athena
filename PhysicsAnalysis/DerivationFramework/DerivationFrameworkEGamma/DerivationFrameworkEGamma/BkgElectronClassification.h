/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_BKGELECTRONCLASSIFICATION_H
#define DERIVATIONFRAMEWORK_BKGELECTRONCLASSIFICATION_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
//
#include "GaudiKernel/ToolHandle.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
///
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

namespace DerivationFramework {

class BkgElectronClassification
  : public AthAlgTool
  , public IAugmentationTool
{
public:
  BkgElectronClassification(const std::string& t,
                            const std::string& n,
                            const IInterface* p);

  virtual StatusCode initialize() override final;
  virtual StatusCode addBranches() const override final;

private:
  /** @brief MCTruthClassifier **/
  ToolHandle<IMCTruthClassifier> m_mcTruthClassifier{
    this,
    "MCTruthClassifierTool",
    "",
    "Handle to the MCTruthClassifier"
  };
  /** @brief barcode cut for egamma helpers **/
  int m_barcodecut;

  /** @brief input electron container **/
  SG::ReadHandleKey<xAOD::ElectronContainer> m_electronContainer{
    this,
    "ElectronContainerName",
    "Electrons",
    "Input Electrons"
  };
  /** @brief Input truth particle container **/
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthContainer{
    this,
    "TruthParticleContainerName",
    "TruthParticles",
    "Input Truth Particles"
  };

  // Write decoration handle keys
  // these are not really configuarable
  SG::WriteDecorHandleKey<xAOD::ElectronContainer>
    m_truthPdgId{ this, "DoNotSet_truthPdgId", "Electrons.truthPdgId", "" };
  SG::WriteDecorHandleKey<xAOD::ElectronContainer>
    m_firstEgMotherTruthType{ this, "firstEgMotherTruthType", "", "" };
  SG::WriteDecorHandleKey<xAOD::ElectronContainer>
    m_firstEgMotherTruthOrigin{ this, "firstEgMotherTruthOrigin", "", "" };
  SG::WriteDecorHandleKey<xAOD::ElectronContainer>
    m_firstEgMotherTruthParticleLink{ this,
                                      "firstEgMotherTruthParticleLink",
                                      "",
                                      "" };
  SG::WriteDecorHandleKey<xAOD::ElectronContainer>
    m_firstEgMotherPdgId{ this, "firstEgMotherPdgId", "", "" };
  SG::WriteDecorHandleKey<xAOD::ElectronContainer>
    m_lastEgMotherTruthType{ this, "lastEgMotherTruthType", "", "" };
  SG::WriteDecorHandleKey<xAOD::ElectronContainer>
    m_lastEgMotherTruthOrigin{ this, "lastEgMotherTruthOrigin", "", "" };
  SG::WriteDecorHandleKey<xAOD::ElectronContainer>
    m_lastEgMotherTruthParticleLink{ this,
                                     "lastEgMotherTruthParticleLink",
                                     "",
                                     "" };
  SG::WriteDecorHandleKey<xAOD::ElectronContainer>
    m_lastEgMotherPdgId{ this, "lastEgMotherPdgId", "", "" };
};
}

#endif // DERIVATIONFRAMEWORK_BKGELECTRONCLASSIFICATION_H

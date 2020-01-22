/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYCOPYALGS_COPYJETTRUTHINFO_H
#define OVERLAYCOPYALGS_COPYJETTRUTHINFO_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODJet/JetContainer.h"

class CopyJetTruthInfo : public AthReentrantAlgorithm
{
public:

  CopyJetTruthInfo(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;

private:
  SG::ReadHandleKey<xAOD::JetContainer> m_bkgInputKey{ this, "BkgInputKey", "", "ReadHandleKey for Background Jet Containers" };
  SG::WriteHandleKey<xAOD::JetContainer> m_outputKey{ this, "OutputKey", "", "WriteHandleKey for Output Jet Containers" };

};

#endif // OVERLAYCOPYALGS_COPYJETTRUTHINFO_H

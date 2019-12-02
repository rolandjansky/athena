/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYCOPYALGS_COPYJETTRUTHINFO_H
#define OVERLAYCOPYALGS_COPYJETTRUTHINFO_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODJet/JetContainer.h"

class CopyJetTruthInfo : public AthAlgorithm
{
public:

  CopyJetTruthInfo(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize();
  virtual StatusCode execute();

private:
  SG::ReadHandleKey<xAOD::JetContainer> m_bkgInputKey{ this, "BkgInputKey", "", "ReadHandleKey for Background Jet Containers" };
  SG::WriteHandleKey<xAOD::JetContainer> m_outputKey{ this, "OutputKey", "", "WriteHandleKey for Output Jet Containers" };

};

#endif // OVERLAYCOPYALGS_COPYJETTRUTHINFO_H

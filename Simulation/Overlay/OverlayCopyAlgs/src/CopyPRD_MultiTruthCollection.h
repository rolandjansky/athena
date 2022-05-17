/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYCOPYALGS_COPYPRDMULTITRUTHCOLLECTION_H
#define OVERLAYCOPYALGS_COPYPRDMULTITRUTHCOLLECTION_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

class CopyPRD_MultiTruthCollection : public AthReentrantAlgorithm
{
public:

  CopyPRD_MultiTruthCollection(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;

private:
  SG::ReadHandleKey<PRD_MultiTruthCollection> m_inputKey{ this, "InputKey", "", "ReadHandleKey for Pileup PRD_MultiTruthCollection" };
  SG::WriteHandleKey<PRD_MultiTruthCollection> m_outputKey{ this, "OutputKey", "", "WriteHandleKey for Output PRD_MultiTruthCollection" };

};

#endif // OVERLAYCOPYALGS_COPYPRDMULTITRUTHCOLLECTION_H

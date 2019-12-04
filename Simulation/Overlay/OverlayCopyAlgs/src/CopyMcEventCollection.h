/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYCOPYALGS_COPYMCEVENTCOLLECTION_H
#define OVERLAYCOPYALGS_COPYMCEVENTCOLLECTION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GeneratorObjects/McEventCollection.h"

class CopyMcEventCollection : public AthAlgorithm
{
public:

  CopyMcEventCollection(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize();
  virtual StatusCode execute();

private:
  SG::ReadHandleKey<McEventCollection> m_bkgInputKey{ this, "BkgInputKey", "Bkg_TruthEvent", "ReadHandleKey for Background McEventCollection" };
  SG::ReadHandleKey<McEventCollection> m_signalInputKey{ this, "SignalInputKey", "Sig_TruthEvent", "ReadHandleKey for Signal McEventCollection" };
  SG::WriteHandleKey<McEventCollection> m_outputKey{ this, "OutputKey", "TruthEvent", "WriteHandleKey for Output McEventCollection" };

  bool m_removeBkgHardScatterTruth;

};

#endif // OVERLAYCOPYALGS_COPYMCEVENTCOLLECTION_H

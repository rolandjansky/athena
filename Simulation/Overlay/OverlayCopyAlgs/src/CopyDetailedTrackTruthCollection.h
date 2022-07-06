/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYCOPYALGS_COPYDETAILEDTRACKTRUTHCOLLECTION_H
#define OVERLAYCOPYALGS_COPYDETAILEDTRACKTRUTHCOLLECTION_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"

class CopyDetailedTrackTruthCollection : public AthReentrantAlgorithm
{
public:

  CopyDetailedTrackTruthCollection(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;

private:
  SG::ReadHandleKey<DetailedTrackTruthCollection> m_inputKey{ this, "InputKey", "", "ReadHandleKey for Pileup DetailedTrackTruthCollection" };
  SG::WriteHandleKey<DetailedTrackTruthCollection> m_outputKey{ this, "OutputKey", "", "WriteHandleKey for Output DetailedTrackTruthCollection" };

};

#endif // OVERLAYCOPYALGS_COPYDETAILEDTRACKTRUTHCOLLECTION_H

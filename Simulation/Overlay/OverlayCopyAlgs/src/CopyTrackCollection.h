/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYCOPYALGS_COPYTRACKCOLLECTION_H
#define OVERLAYCOPYALGS_COPYTRACKCOLLECTION_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrkTrack/TrackCollection.h"

class CopyTrackCollection : public AthReentrantAlgorithm
{
public:

  CopyTrackCollection(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;

private:
  SG::ReadHandleKey<TrackCollection> m_inputKey{ this, "InputKey", "", "ReadHandleKey for Pileup TrackCollection" };
  SG::WriteHandleKey<TrackCollection> m_outputKey{ this, "OutputKey", "", "WriteHandleKey for Output TrackCollection" };

};

#endif // OVERLAYCOPYALGS_COPYTRACKCOLLECTION_H

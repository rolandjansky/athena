/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYCOMMONALGS_COPYTRACKRECORDCOLLECTION_H
#define OVERLAYCOMMONALGS_COPYTRACKRECORDCOLLECTION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrackRecord/TrackRecordCollection.h"

class CopyTrackRecordCollection : public AthAlgorithm
{
public:

  CopyTrackRecordCollection(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize();
  virtual StatusCode execute();

private:
  SG::ReadHandleKey<TrackRecordCollection> m_signalInputKey{ this, "SignalInputKey", "", "ReadHandleKey for Signal TrackRecordCollection" };
  SG::WriteHandleKey<TrackRecordCollection> m_outputKey{ this, "OutputKey", "", "WriteHandleKey for Output TrackRecordCollection" };

  Gaudi::Property<std::string> m_collectionName{ this, "collectionName", "", "TrackRecordCollection collection name" };
};

#endif // OVERLAYCOMMONALGS_COPYTRACKRECORDCOLLECTION_H

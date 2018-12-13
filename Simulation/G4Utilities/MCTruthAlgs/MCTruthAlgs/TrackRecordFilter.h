/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHALGS_TRACKRECORDFILTER_H
#define MCTRUTHALGS_TRACKRECORDFILTER_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"


// the TrackRecordCollection
#include "TrackRecord/TrackRecordCollection.h"

#include <string>

namespace HepPDT{
   class ParticleDataTable;
}

class TrackRecordFilter : public AthReentrantAlgorithm {


public:
  TrackRecordFilter (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrackRecordFilter(){};

  StatusCode initialize() override;
  StatusCode finalize() override;
  StatusCode execute(const EventContext& ctx) const override;

private:
  SG::ReadHandleKey<TrackRecordCollection> m_inputKey;
  SG::WriteHandleKey<TrackRecordCollection> m_outputKey;
  double m_cutOff;
  const HepPDT::ParticleDataTable* m_pParticleTable;
};

#endif

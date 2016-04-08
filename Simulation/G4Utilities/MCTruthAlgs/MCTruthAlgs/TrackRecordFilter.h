/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHALGS_TRACKRECORDFILTER_H
#define MCTRUTHALGS_TRACKRECORDFILTER_H

#include "AthenaBaseComps/AthAlgorithm.h"

// particle table
#include "HepPDT/ParticleDataTable.hh"


#include <string>

class StoreGateSvc;
class TrackRecordFilter : public AthAlgorithm {


public:
  TrackRecordFilter (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrackRecordFilter(){};

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

private:
  std::string m_inputName;
  std::string m_outputName;
  double m_cutOff;
  const HepPDT::ParticleDataTable* m_pParticleTable;
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Algorithm.h"

class StoreGateSvc;
using std::string; using std::vector;

/** @class HICentralityData

    Algorithm for printing/monitoring HITRTInfo object
    with info heavy ion on event centrality
    @author  Andrzej Olszewski <Andrzej.Olszewski@ifj.edu.pl>
*/

class PrintHITRTInfo:public Algorithm {
public:
        PrintHITRTInfo(const std::string& name, ISvcLocator* pSvcLocator);
        StatusCode initialize();
        StatusCode execute();
        StatusCode finalize();

private:
  StoreGateSvc* m_sgSvc;

  // Setable Properties:-
  // name of HITRTInfo in SG
  string m_key; 
  // possible use of new percentage schema
  vector<int> m_OccupancyPerPart;
};

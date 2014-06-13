/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Algorithm.h"

class StoreGateSvc;
using std::string; using std::vector;

/** @class HICentralityData

    Algorithm for printing/monitoring HICentralityData object
    with info heavy ion on event centrality
    @author  Andrzej Olszewski <Andrzej.Olszewski@ifj.edu.pl>
*/

class PrintHICentralityData:public Algorithm {
public:
        PrintHICentralityData(const std::string& name, ISvcLocator* pSvcLocator);
        StatusCode initialize();
        StatusCode execute();
        StatusCode finalize();

private:
  StoreGateSvc* m_sgSvc;

  // Setable Properties:-
  // name of HICentralityData in SG
  string m_key; 
  // possible use of new percentage schema
  vector<unsigned short> m_CentralityPercSchema;
};

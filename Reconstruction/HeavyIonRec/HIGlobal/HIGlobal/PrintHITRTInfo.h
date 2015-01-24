// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"

/** @class HICentralityData

    Algorithm for printing/monitoring HITRTInfo object
    with info heavy ion on event centrality
    @author  Andrzej Olszewski <Andrzej.Olszewski@ifj.edu.pl>
*/

class PrintHITRTInfo:public AthAlgorithm {
public:
        PrintHITRTInfo(const std::string& name, ISvcLocator* pSvcLocator);
        StatusCode initialize();
        StatusCode execute();
        StatusCode finalize();

private:
  // Setable Properties:-
  // name of HITRTInfo in SG
  std::string m_key; 
  // possible use of new percentage schema
  std::vector<int> m_OccupancyPerPart;
};

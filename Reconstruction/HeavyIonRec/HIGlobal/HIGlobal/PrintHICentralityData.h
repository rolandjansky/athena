// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"

/** @class HICentralityData

    Algorithm for printing/monitoring HICentralityData object
    with info heavy ion on event centrality
    @author  Andrzej Olszewski <Andrzej.Olszewski@ifj.edu.pl>
*/

class PrintHICentralityData:public AthAlgorithm {
public:
        PrintHICentralityData(const std::string& name, ISvcLocator* pSvcLocator);
        StatusCode initialize();
        StatusCode execute();
        StatusCode finalize();

private:
  // Setable Properties:-
  // name of HICentralityData in SG
  std::string m_key; 
  // possible use of new percentage schema
  std::vector<unsigned short> m_CentralityPercSchema;
};

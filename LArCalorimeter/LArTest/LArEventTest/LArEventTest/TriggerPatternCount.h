//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/** This algrithm produces a column-wise NTuple
    out of raw data. Only the finalize
    method is used, initalize and execute are empty.
    The key of the container is given by the jobOption 
    'ContainerKey' and only a maximum of 'NSamples' 
    will be written to the ntuple.
   * @author R. Lafaye
   * 19. 1. 2005


*/

#ifndef TRIGGERPATTERNCOUNT_H
#define TRIGGERPATTERNCOUNT_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArIdentifier/LArOnlineID.h"
#include <string>
#include <map>

#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"

class TriggerPatternCount : public AthAlgorithm
{
 public:
  TriggerPatternCount(const std::string & name, ISvcLocator * pSvcLocator);
  ~TriggerPatternCount();

  //standard algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 private:
  std::string m_contKey;

  int m_event;
  int m_pattern_max;
  int m_pattern_count[32];
};

#endif

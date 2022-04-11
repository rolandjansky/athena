/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowEvent/eflowTauObject.h"

eflowTauObject::eflowTauObject(){

  //initialise the pointers
  m_PFOContainer =  nullptr;
  m_clusterContainer = nullptr;
  m_trackContainer = nullptr;
  m_finalClusterContainer = nullptr;

  //initialise number of tracks
  m_numInnerTracks = 0;

}

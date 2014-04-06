/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowEvent/eflowTauObject.h"

eflowTauObject::eflowTauObject(){

  //initialise the pointers
  m_PFOContainer =  NULL;
  m_clusterContainer = NULL;
  m_trackContainer = NULL;
  m_finalClusterContainer = NULL;

  //initialise number of tracks
  m_numInnerTracks = 0;

}

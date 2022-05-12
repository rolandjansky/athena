/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
//Tool to mask Noisy Tubes
//Nov. 2009
//Author Justin Griffiths <griffith@cern.ch>
///////////////////////////////////////////////////////////////////////////


#ifndef MDTNoisyTubes_H
#define MDTNoisyTubes_H

#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include <map>
#include <set>
#include <string>

class MDTNoisyTubes {

 public:
  MDTNoisyTubes(bool doMask=true);
  ~MDTNoisyTubes();

  bool isNoisy(const Muon::MdtPrepData*);
  std::set<Identifier> getNoiseList(IdentifierHash);
    
 private:
  std::map< IdentifierHash, std::set<Identifier> > m_noise_map;

};

#endif

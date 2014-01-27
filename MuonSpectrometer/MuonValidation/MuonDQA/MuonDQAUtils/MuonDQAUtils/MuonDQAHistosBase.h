/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonDQAHist_MuonDQAHistosBase_H
#define MuonDQAHist_MuonDQAHistosBase_H

namespace MuonDQAHist {

  struct MuonDQAHistosBase {
    MuonDQAHistosBase() : wasBooked(false) , wasRegistered(false) {} 
    virtual ~MuonDQAHistosBase() {}
    bool wasBooked;
    bool wasRegistered;
  };

}


#endif

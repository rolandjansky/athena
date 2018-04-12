/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISFTrajectory_H
#define ISFTrajectory_H

#include "G4Trajectory.hh"

namespace ISF {
  class ITruthSvc;
}

namespace iGeant4 {
  
  
  class ISFTrajectory: public G4Trajectory {
  public:

    ISFTrajectory();
    ISFTrajectory(const G4Track* aTrack, ISF::ITruthSvc* truthSvc);
    ~ISFTrajectory();
    void AppendStep(const G4Step* aStep);

  private:
    ISF::ITruthSvc        *m_truthRecordSvcQuick;
    
  };
  
}

#endif

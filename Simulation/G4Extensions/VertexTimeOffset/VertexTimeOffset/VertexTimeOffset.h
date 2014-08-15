/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VertexTimeOffset_H
#define VertexTimeOffset_H

#include "FadsKinematics/VertexManipulator.h"

class G4PrimaryVertex;

class VertexTimeOffset: public FADS::VertexManipulator {
  private:

    G4int verboseLevel;
    bool isFirstVertex;
  public:
    VertexTimeOffset(std::string s)
      : FADS::VertexManipulator(s), verboseLevel(0), isFirstVertex(false) {};

    ~VertexTimeOffset() {}
    inline void switchOn(){TurnOn();};
    inline void switchOff(){TurnOff();};
    void SetVerboseLevel(G4int i) { verboseLevel=i; }
    G4int GetVerboseLevel() const { return verboseLevel; }
    bool GetStatus() {return IsOn();}

    // for the C++ layer only
    bool EditVertex(G4PrimaryVertex *);
    void EventInitialization();
};

#endif

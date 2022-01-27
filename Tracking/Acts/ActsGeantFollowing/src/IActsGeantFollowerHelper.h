/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IACTSGeantFollowerHelper_H
#define IACTSGeantFollowerHelper_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "G4ThreeVector.hh"

/** Interface ID for IGeantFollowerHelperFollowerHelper */
static const InterfaceID IID_IActsGeantFollowerHelper("IActsGeantFollowerHelper", 1, 0);

class IActsGeantFollowerHelper : virtual public IAlgTool {
  public:

    /**Virtual destructor*/
    virtual ~IActsGeantFollowerHelper(){}

    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IActsGeantFollowerHelper; }

    // Follower interface
    // a) begin event - initialize follower process
    virtual void beginEvent() const = 0;
    // b) track the particle
    virtual void trackParticle(const G4ThreeVector& pos, const G4ThreeVector& mom, int pdg, double charge, float t, float X0, bool isSensitive) const = 0;
    // c) end event - ntuple writing
    virtual void endEvent() const = 0;

};


#endif // IActsGeantFollowerHelper_H

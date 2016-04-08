/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// IPositionMomentumWriter.h, (c) ATLAS DETECTOR Software
///////////////////////////////////////////////////////////////////


#ifndef TRK_IPOSITIONMOMENTUMWRITER_H
#define TRK_IPOSITIONMOMENTUMWRITER_H

#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
static const InterfaceID IID_IPositionMomentumWriter("IPositionMomentumWriter",1,0);

/** @class IPositionMomentumWriter
    
    Very simple helper Tool to record track states at certain positions
    this works only for single track events.

    @author Andreas.Salzburger@cern.ch
*/

  class IPositionMomentumWriter : virtual public IAlgTool {
   public:
    /**Interface ID, declared here, and defined below*/
    static const InterfaceID& interfaceID();

    /** Initialize State */
    virtual void initializeTrack(const Amg::Vector3D& pos,
                                   const Amg::Vector3D& mom,
                                   double m, int pdg) const = 0;

    /** Record a single TrackState */
    virtual void recordTrackState(const Amg::Vector3D& pos, 
                                  const Amg::Vector3D& mom) const = 0;
    /** Finalization State */
    virtual void finalizeTrack() const = 0;

};

  inline const InterfaceID& Trk::IPositionMomentumWriter::interfaceID() {
    return IID_IPositionMomentumWriter;
  }

} // end of namespace

#endif

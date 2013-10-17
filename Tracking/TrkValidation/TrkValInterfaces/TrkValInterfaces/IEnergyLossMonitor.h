/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// IEnergyLossMonitor.h, (c) ATLAS DETECTOR Software
///////////////////////////////////////////////////////////////////


#ifndef TRK_IENERGYLOSSMONITOR_H
#define TRK_IENERGYLOSSMONITOR_H

#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
static const InterfaceID IID_IEnergyLossMonitor("IEnergyLossMonitor",1,0);

/** @class IEnergyLossMonitor
    
    Very simple helper Tool to record the eneryloss steps track,
    this works only for single track events.

    @author Andreas.Salzburger@cern.ch
*/

  class IEnergyLossMonitor : virtual public IAlgTool {
   public:
    /**Interface ID, declared here, and defined below*/
    static const InterfaceID& interfaceID();

    /** Initialize State */
    virtual void initializeTrack(double p,
                                 double E,
                                 double eta,
                                 double phi) const = 0;

    /** Record a single TrackState */
    virtual void recordTrackState(const Amg::Vector3D& pos, 
                                  const Amg::Vector3D& mom,
                                  double mass) const = 0;
    /** Finalization State */
    virtual void finalizeTrack() const = 0;

};

  inline const InterfaceID& Trk::IEnergyLossMonitor::interfaceID() {
    return IID_IEnergyLossMonitor;
  }

} // end of namespace

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IParticleCaloExtensionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ITRKTRACKPARTICLECALOEXTENSIONTOOL_H
#define ITRKTRACKPARTICLECALOEXTENSIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/ParticleCaloExtension.h"


namespace Trk 
{
  static const InterfaceID IID_IParticleCaloExtensionTool("Trk::IParticleCaloExtensionTool", 1, 0);

  /** @brief Interface for extending Particles with calo intersections */
  class IParticleCaloExtensionTool : virtual public IAlgTool {
  public:

    /** Method to dress a TrackParticle with the calo layers crossed by its track
        @param trackParticle 
        @return ParticleCaloExtension
    */
    virtual const xAOD::ParticleCaloExtension* caloExtension( xAOD::TrackParticle& trackParticle ) const = 0;

    static const InterfaceID& interfaceID( ) ;
  };

  inline const InterfaceID& Trk::IParticleCaloExtensionTool::interfaceID() {
    return IID_IParticleCaloExtensionTool; 
  }

} // end of namespace

#endif 

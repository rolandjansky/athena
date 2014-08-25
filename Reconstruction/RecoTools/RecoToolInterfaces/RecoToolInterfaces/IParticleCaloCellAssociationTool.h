/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IParticleCaloCellAssociationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ITRKTRACKPARTICLECALOCELLASSOCIATIONTOOL_H
#define ITRKTRACKPARTICLECALOCELLASSOCIATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/ParticleCaloExtension.h"

namespace Trk 
{
  static const InterfaceID IID_IParticleCaloCellAssociationTool("Trk::IParticleCaloCellAssociationTool", 1, 0);

  /** @brief Interface for extending Particles with calo intersections */
  class IParticleCaloCellAssociationTool : virtual public IAlgTool {
  public:

    /** Method to dress a Particle with the calo cells crossed by its track
        @param trackParticle 
        @return ParticleCaloExtension
    */
    virtual const xAOD::ParticleCaloExtension* caloAssociation( xAOD::TrackParticle& trackParticle ) const = 0;


    static const InterfaceID& interfaceID( ) ;
  };

  inline const InterfaceID& Trk::IParticleCaloCellAssociationTool::interfaceID() {
    return IID_IParticleCaloCellAssociationTool; 
  }

} // end of namespace

#endif 

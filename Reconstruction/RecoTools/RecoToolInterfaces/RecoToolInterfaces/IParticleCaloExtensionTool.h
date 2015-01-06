/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IParticleCaloExtensionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ITRKTRACKPARTICLECALOEXTENSIONTOOL_H
#define ITRKTRACKPARTICLECALOEXTENSIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODBase/IParticle.h"
#include "xAODTracking/TrackParticle.h"
#include "TrkCaloExtension/CaloExtension.h"


namespace Trk 
{
  static const InterfaceID IID_IParticleCaloExtensionTool("Trk::IParticleCaloExtensionTool", 1, 0);

  /** @brief Interface for extending Particles with calo intersections */
  class IParticleCaloExtensionTool : virtual public IAlgTool {
  public:

    /** Method to dress a IParticle with the calo layers crossed by its track
        @param IParticle     reference to the particle
        @param extension     reference to a pointer to a CaloExtesion, will be updated if call is successfull
                             NEVER delete the pointer, you will cause a crash! 
        @param useCaching    configure whether the tool caches the result on the track particle
                             The default behavior is 'true' to ensure optimal performance
        @return true if the call was successful
    */
    virtual bool caloExtension( const xAOD::IParticle& particle, const Trk::CaloExtension*& extension, 
                                bool useCaching = true  ) const = 0;


    virtual Trk::CaloExtension* caloExtension( const TrackParameters& startPars, PropDirection propDir, ParticleHypothesis particleType ) const =0;

    static const InterfaceID& interfaceID( ) ;
  };

  inline const InterfaceID& Trk::IParticleCaloExtensionTool::interfaceID() {
    return IID_IParticleCaloExtensionTool; 
  }

} // end of namespace

#endif 

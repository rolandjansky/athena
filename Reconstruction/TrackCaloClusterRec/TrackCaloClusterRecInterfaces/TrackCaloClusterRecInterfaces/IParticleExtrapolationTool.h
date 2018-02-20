/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IParticleExtrapolationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECINTERFACES_IPARTICLEEXTRAPOLATIONTOOL_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECINTERFACES_IPARTICLEEXTRAPOLATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODBase/IParticle.h"
#include "TrkCaloExtension/CaloExtension.h"
#include "TrkEventPrimitives/ParticleHypothesis.h" 

static const InterfaceID IID_IParticleExtrapolationTool("IParticleExtrapolationTool", 1, 0);

/** @brief Interface for extending Particles with calo intersections */
class IParticleExtrapolationTool : virtual public IAlgTool {
  public:
    
    /** Method to dress a IParticle with the calo layers crossed by its track
     *  Tracks are decorated using the calo layer index and the intersection
     *  @param IParticle     reference to the particle
     *  @param extension     reference to a pointer to a CaloExtesion, will be updated if call is successfull
     *                       NEVER delete the pointer, you will cause a crash! 
     *  @return true if the call was successful
     * 
     */
    
    virtual bool particleToCaloExtrapolate( const xAOD::IParticle& particle, const Trk::CaloExtension*& extension) const = 0;
    
    virtual Trk::CaloExtension* caloExtension( const Trk::TrackParameters& startPars, Trk::PropDirection propDir, Trk::ParticleHypothesis particleType ) const =0;
  
    static const InterfaceID& interfaceID( ) ;
};

inline const InterfaceID& IParticleExtrapolationTool::interfaceID() {
  return IID_IParticleExtrapolationTool; 
}

#endif // TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECINTERFACES_IPARTICLEEXTRAPOLATIONTOOL_H
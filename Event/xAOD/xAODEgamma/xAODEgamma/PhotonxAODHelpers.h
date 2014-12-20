// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_PHOTONXAODHELPERS_H
#define XAOD_PHOTONXAODHELPERS_H
#include "xAODEgamma/PhotonFwd.h"
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODEgamma/EgammaEnums.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include <cstddef>
#include <set>

namespace xAOD {

  namespace EgammaHelpers{

    ///@brief is the object a converted photon
    bool isConvertedPhoton(const xAOD::Photon *ph);

    ///@brief return the photon conversion type (see EgammaEnums)
    xAOD::EgammaParameters::ConversionType conversionType(const xAOD::Photon *ph);

    ///@brief return the photon conversion type (see EgammaEnums)
    xAOD::EgammaParameters::ConversionType conversionType(const xAOD::Vertex *vx);
    
    ///@brief return the number of Si tracks in the conversion
    std::size_t numberOfSiTracks(const xAOD::Photon *eg);

    ///@brief return the number of Si tracks in the conversion
    std::size_t numberOfSiTracks(const xAOD::Vertex *vx);

    ///@brief return the number of Si tracks in the conversion
    std::size_t numberOfSiTracks(const xAOD::EgammaParameters::ConversionType convType);

    ///@brief return the conversion radius or 9999.
    float conversionRadius(const xAOD::Vertex* vx);

    ///@brief return the conversion radius or 9999.
    float conversionRadius(const xAOD::Photon* ph);
    
    ///@brief return the momentum at the vertex (which can be 0)
    Amg::Vector3D momentumAtVertex(const xAOD::Photon*, bool debug = false);

    ///@brief return the momentum at the vertex (which can be 0)
    Amg::Vector3D momentumAtVertex(const xAOD::Vertex&, bool debug = false);

    /** Return a list of all or only the best TrackParticle associated to the object. 
      * (used for track isolation)
      * If useBremAssoc is set, get the original TrackParticle **/
    const std::set<const xAOD::TrackParticle*> getTrackParticles(const xAOD::Photon* ph,
								 bool useBremAssoc = true);
    

  } //namaspace EgammaHelpers

} // namespace xAOD

#endif // XAOD_PHOTONXAODHELPERS_H

// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_PHOTONXAODHELPERS_H
#define XAOD_PHOTONXAODHELPERS_H

#include "xAODTracking/Vertex.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/EgammaEnums.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include <cstddef>

namespace xAOD {

  namespace EgammaHelpers{

    using std::size_t;

    ///@brief is the object a converted photon
    bool isConvertedPhoton(const xAOD::Photon *ph);

    ///@brief return the photon conversion type (see EgammaEnums)
    xAOD::EgammaParameters::ConversionType conversionType(const xAOD::Photon *ph);

    ///@brief return the photon conversion type (see EgammaEnums)
    xAOD::EgammaParameters::ConversionType conversionType(const xAOD::Vertex *vx);
    
    ///@brief return the number of Si tracks in the conversion
    size_t numberOfSiTracks(const xAOD::Photon *eg);

    ///@brief return the number of Si tracks in the conversion
    size_t numberOfSiTracks(const xAOD::Vertex *vx);

    ///@brief return the number of Si tracks in the conversion
    size_t numberOfSiTracks(const xAOD::EgammaParameters::ConversionType convType);

    ///@brief return the conversion radius or 9999.
    float conversionRadius(const xAOD::Vertex* vx);

    ///@brief return the conversion radius or 9999.
    float conversionRadius(const xAOD::Photon* ph);
    
    ///@brief return the momentum at the vertex (which can be 0)
    Amg::Vector3D momentumAtVertex(const xAOD::Photon*, bool debug = false);

    ///@brief return the momentum at the vertex (which can be 0)
    Amg::Vector3D momentumAtVertex(const xAOD::Vertex&, bool debug = false);

    // xAOD::Vertex does not have method isAvailable for the moment
    bool isAvailable(const xAOD::Vertex& vertex, std::string name);
    
  } //namaspace EgammaHelpers

} // namespace xAOD

#endif // XAOD_PHOTONXAODHELPERS_H

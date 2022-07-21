/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "xAODCore/AuxStoreAccessorMacros.h"
#include "xAODTracking/versions/TrackParameters_v1.h"


namespace xAOD {
    AUXSTORE_OBJECT_SETTER_AND_GETTER(TrackParameters_v1, std::vector<double>, parameters, setParameters)

    std::vector<double>& TrackParameters_v1::parameters() {
        static const SG::AuxElement::Accessor<std::vector<double>> acc("parameters");
        if (!acc.isAvailable(*this)) throw std::runtime_error("Missing 'parameters' in TrackParameters_v1");
        return acc(*this);
    }

    AUXSTORE_OBJECT_SETTER_AND_GETTER(TrackParameters_v1, std::vector<double>, covariance, setCovariance)

    std::vector<double>& TrackParameters_v1::covariance() {
        static const SG::AuxElement::Accessor<std::vector<double>> acc("covariance");
        if (!acc.isAvailable(*this)) throw std::runtime_error("Missing 'covariance' in TrackParameters_v1");;
        return acc(*this);
    }
    void TrackParameters_v1::resize(size_t sz) {
        parameters().resize(sz); // 1D vector storage
        covariance().resize(sz * sz); // 2D Matrix storage
    }
}
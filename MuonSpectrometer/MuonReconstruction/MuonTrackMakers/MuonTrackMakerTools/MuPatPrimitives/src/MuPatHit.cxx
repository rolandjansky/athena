/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuPatPrimitives/MuPatHit.h"

#include <iostream>
#include <utility>

#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkMeasurementBase/MeasurementBase.h"

namespace Muon {

    // Static members
    std::atomic<unsigned int> MuPatHit::s_maxNumberOfInstantiations;
    std::atomic<unsigned int> MuPatHit::s_numberOfInstantiations = 0;
    std::atomic<unsigned int> MuPatHit::s_numberOfCopies = 0;

    // Static functions

    // member functions
    MuPatHit::MuPatHit(std::shared_ptr<const Trk::TrackParameters> pars, const Trk::MeasurementBase* presMeas,
                       std::unique_ptr<const Trk::MeasurementBase> broadMeas, const Info& info) :
        m_pars(std::move(pars)), m_precisionMeas(presMeas), m_broadMeas(std::move(broadMeas)), m_info(info) {
#ifdef MCTB_OBJECT_COUNTERS
        addInstance();
#endif
    }

    MuPatHit::MuPatHit(const MuPatHit& hit) {
        copy(hit);
#ifdef MCTB_OBJECT_COUNTERS
        addInstance();
        ++s_numberOfCopies;
#endif
    }

    MuPatHit& MuPatHit::operator=(const MuPatHit& hit) {
        if (&hit != this) {
            copy(hit);
#ifdef MCTB_OBJECT_COUNTERS
            ++s_numberOfCopies;
#endif
        }
        return *this;
    }

    MuPatHit::~MuPatHit() {
//     std::cout << " delete MuPatHit  " << this << std::endl;
#ifdef MCTB_OBJECT_COUNTERS
        removeInstance();
#endif
    }

    void MuPatHit::copy(const MuPatHit& hit) {
        m_pars = hit.m_pars;
        m_precisionMeas = hit.m_precisionMeas;
        m_broadMeas.reset(hit.m_broadMeas->clone());
        m_info = hit.m_info;
    }

    void MuPatHit::updateParameters(std::shared_ptr<const Trk::TrackParameters> pars) { m_pars = std::move(pars); }

    const Trk::TrackParameters& MuPatHit::parameters() const { return *m_pars; }

    const Trk::MeasurementBase& MuPatHit::measurement() const {
        if (info().selection == Precise) return *m_precisionMeas;
        return *m_broadMeas;
    }
    const Trk::MeasurementBase& MuPatHit::preciseMeasurement() const { return *m_precisionMeas; }
    const Trk::MeasurementBase& MuPatHit::broadMeasurement() const { return *m_broadMeas; }
    const MuPatHit::Info& MuPatHit::info() const { return m_info; }
    MuPatHit::Info& MuPatHit::info() { return m_info; }

}  // namespace Muon

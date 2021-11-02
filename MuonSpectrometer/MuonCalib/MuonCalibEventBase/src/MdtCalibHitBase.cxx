/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// std
#include <float.h>
// other packages

// this package
#include "MuonCalibEventBase/MdtCalibHitBase.h"

namespace MuonCalib {

    MdtCalibHitBase::MdtCalibHitBase(int tdc, int adc, const Amg::Vector3D &globalPos, const Amg::Vector3D &localPos) {
        m_pars.tdc = tdc;
        m_pars.adc = adc;
        m_pars.globalPosition = globalPos;
        m_pars.localPosition = localPos;
    }
    MdtCalibHitBase::MdtCalibHitBase(const MdtCalibHitBase::defineParams &pars) : m_pars{pars} {}

    std::ostream &MdtCalibHitBase::dump(std::ostream &stream) const {
        stream << "MdtCalibHitBase: " << std::endl;
        stream << "   Drift time " << driftTime() << " radius " << driftRadius() << " DistRO " << distanceToReadout() << std::endl;
        stream << "   Radial residual " << radialResidual() << std::endl;
        stream << "   Global position " << globalPosition() << "   local position " << localPosition() << std::endl;
        return stream;
    }
    void MdtCalibHitBase::setIdentifier(const MuonFixedId &id) { m_pars.id = id; }
    void MdtCalibHitBase::setTdc(unsigned short tdc) { m_pars.tdc = tdc; }
    void MdtCalibHitBase::setAdc(unsigned short adc) { m_pars.adc = adc; }
    void MdtCalibHitBase::setGlobalPos(const Amg::Vector3D &globalPos) { m_pars.globalPosition = globalPos; }
    void MdtCalibHitBase::setLocalPos(const Amg::Vector3D &localPos) { m_pars.localPosition = localPos; }
    void MdtCalibHitBase::setGlobalPointOfClosestApproach(const Amg::Vector3D &point) { m_pars.globalPointOfClosestApproach = point; }
    void MdtCalibHitBase::setLocalPointOfClosestApproach(const Amg::Vector3D &point) { m_pars.localPointOfClosestApproach = point; }
    void MdtCalibHitBase::setDriftTime(float t) { m_pars.driftTime = t; }
    void MdtCalibHitBase::setDriftRadius(float r, float sigmaR) {
        m_pars.driftRadius = r;
        m_pars.sigma2DriftRadius = sigmaR * sigmaR;
    }
    void MdtCalibHitBase::setTubeRadius(float r) { m_pars.tubeRadius = r; }
    void MdtCalibHitBase::setDistanceToTrack(float dist, float sigmaDist) {
        m_pars.signedDistanceToTrack = dist;
        m_pars.sigma2DistanceToTrack = sigmaDist * sigmaDist;
    }
    void MdtCalibHitBase::setTimeFromTrackDistance(float t, float sigmaT) {
        m_pars.timeFromTrackDistance = t;
        m_pars.sigma2TimeFromTrackDistance = sigmaT * sigmaT;
    }
    void MdtCalibHitBase::setWiresagTime(float wsag) { m_pars.wiresagTime = wsag; }
    void MdtCalibHitBase::setSlewingTime(float slew) { m_pars.slewingTime = slew; }
    void MdtCalibHitBase::setBFieldTime(float lor) { m_pars.bFieldTime = lor; }
    void MdtCalibHitBase::setPropagationTime(float prop) { m_pars.propagationTime = prop; }
    void MdtCalibHitBase::setTimeOfFlight(float tof) { m_pars.timeOfFlight = tof; }
    void MdtCalibHitBase::setDistanceToReadout(float dist) { m_pars.distanceToReadout = dist; }
    void MdtCalibHitBase::setTemperatureTime(float temp) { m_pars.temperatureTime = temp; }
    void MdtCalibHitBase::setBackgroundTime(float bkgr) { m_pars.backgroundTime = bkgr; }
    void MdtCalibHitBase::setBFieldPerp(float val) { m_pars.bFieldPerp = val; }
    void MdtCalibHitBase::setBFieldPara(float val) { m_pars.bFieldPara = val; }
    void MdtCalibHitBase::setTemperature(float val) { m_pars.temperature = val; }
    void MdtCalibHitBase::setProjSag(float val) { m_pars.projSag = val; }
    void MdtCalibHitBase::setTubeT0(float t0) { m_pars.tube_t0 = t0; }
    void MdtCalibHitBase::setTubeAdcCal(float adccal) { m_pars.tube_adccal = adccal; }
    void MdtCalibHitBase::setLocXtwin(float xtwin) { m_pars.xtwin = xtwin; }
    void MdtCalibHitBase::setSigmaLocXtwin(float sigmaXTwin) { m_pars.sigmaXTwin = sigmaXTwin; }
    const MuonFixedId &MdtCalibHitBase::identify() const { return m_pars.id; }
    const Amg::Vector3D &MdtCalibHitBase::globalPosition() const { return m_pars.globalPosition; }
    const Amg::Vector3D &MdtCalibHitBase::localPosition() const { return m_pars.localPosition; }
    const Amg::Vector3D &MdtCalibHitBase::globalPointOfClosestApproach() const { return m_pars.globalPointOfClosestApproach; }
    const Amg::Vector3D &MdtCalibHitBase::localPointOfClosestApproach() const { return m_pars.localPointOfClosestApproach; }
    unsigned short MdtCalibHitBase::tdcCount() const { return m_pars.tdc; }
    unsigned short MdtCalibHitBase::adcCount() const { return m_pars.adc; }
    float MdtCalibHitBase::driftTime() const { return m_pars.driftTime; }
    float MdtCalibHitBase::driftRadius() const { return m_pars.driftRadius; }
    float MdtCalibHitBase::sigmaDriftRadius() const { return std::sqrt(m_pars.sigma2DriftRadius); }
    float MdtCalibHitBase::sigma2DriftRadius() const { return m_pars.sigma2DriftRadius; }
    float MdtCalibHitBase::signedDistanceToTrack() const { return m_pars.signedDistanceToTrack; }
    float MdtCalibHitBase::sigmaDistanceToTrack() const { return std::sqrt(m_pars.sigma2DistanceToTrack); }
    float MdtCalibHitBase::sigma2DistanceToTrack() const { return m_pars.sigma2DistanceToTrack; }
    float MdtCalibHitBase::distanceToReadout() const { return m_pars.distanceToReadout; }
    float MdtCalibHitBase::slewingTime() const { return m_pars.slewingTime; }
    float MdtCalibHitBase::bFieldTime() const { return m_pars.bFieldTime; }
    float MdtCalibHitBase::bFieldPerp() const { return m_pars.bFieldPerp; }
    float MdtCalibHitBase::bFieldPara() const { return m_pars.bFieldPara; }
    float MdtCalibHitBase::temperature() const { return m_pars.temperature; }
    float MdtCalibHitBase::TemperatureTime() const { return m_pars.temperatureTime; }
    float MdtCalibHitBase::WiresagTime() const { return m_pars.wiresagTime; }
    float MdtCalibHitBase::projSag() const { return m_pars.projSag; }
    float MdtCalibHitBase::propagationTime() const { return m_pars.propagationTime; }
    float MdtCalibHitBase::timeOfFlight() const { return m_pars.timeOfFlight; }
    float MdtCalibHitBase::timeFromTrackDistance() const { return m_pars.timeFromTrackDistance; }
    float MdtCalibHitBase::sigmaTimeFromTrackDistance() const { return std::sqrt(m_pars.sigma2TimeFromTrackDistance); }
    float MdtCalibHitBase::sigma2TimeFromTrackDistance() const { return m_pars.sigma2TimeFromTrackDistance; }
    float MdtCalibHitBase::radialResidual() const { return std::abs(m_pars.driftRadius) - std::abs(m_pars.signedDistanceToTrack); }
    float MdtCalibHitBase::trackResidual() const { return m_pars.signedDistanceToTrack - m_pars.driftRadius; }
    float MdtCalibHitBase::timeResidual() const { return m_pars.driftTime - m_pars.timeFromTrackDistance; }
    float MdtCalibHitBase::tubeT0() const { return m_pars.tube_t0; }
    float MdtCalibHitBase::tubeAdcCal() const { return m_pars.tube_adccal; }
    float MdtCalibHitBase::xtwin() const { return m_pars.xtwin; }
    float MdtCalibHitBase::sigmaXtwin() const { return m_pars.sigmaXTwin; }
    bool MdtCalibHitBase::segmentT0Applied() const { return m_pars.segmentT0Applied; }
    void MdtCalibHitBase::setSegmentT0Applied(bool flag) { m_pars.segmentT0Applied = flag; }
    float MdtCalibHitBase::tubeRadius() const { return m_pars.tubeRadius; }
}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MdtCalibHitBase &hit) { return hit.dump(stream); }

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 least squared fit to track hit data => PerigeeParameters with covariance
 and fit quality
 ------------------------------------------------------------------------
 ***************************************************************************/

#ifndef IPATREC_TRACKFITTER_H
#define IPATREC_TRACKFITTER_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "iPatInterfaces/IiPatFitter.h"
#include <vector>


namespace Trk {
class ITrackingVolumesSvc;
class FitMeasurement;
class FitParameters;
class FitProcedure;
class FitProcedureQuality;
class IIntersector;
class IPropagator;
class IMaterialAllocator;
class Volume;
}

class TrackFitter
  : public AthAlgTool
  , virtual public IiPatFitter
{

public:
  TrackFitter(const std::string& type,
              const std::string& name,
              const IInterface* parent);
  virtual ~TrackFitter(void); // destructor

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // updates the track PerigeeParameters and FitQuality
  virtual void fit(TrackStatus status,
                   FitQuality*& fitQuality,
                   PerigeeParameters& perigee,
                   hit_const_iterator begin,
                   hit_const_iterator end) const override;

  // updates the track PerigeeParameters, FitQuality and residuals. May modify
  // hits
  virtual void fitWithRejection(TrackStatus status,
                                FitQuality*& fitQuality,
                                int maxReject,
                                PerigeeParameters& perigee,
                                parameter_vector*& scattererParameters,
                                hit_iterator begin,
                                hit_iterator end,
                                hit_const_iterator original_begin,
                                hit_const_iterator original_end) const override;

  // as fit above - but with residual set in HitOnTrack
  virtual void fitWithResiduals(TrackStatus status,
                                FitQuality*& fitQuality,
                                PerigeeParameters& perigee,
                                parameter_vector*& scattererParameters,
                                hit_const_iterator begin,
                                hit_const_iterator end) const override;

  // uses transverse vertex as a measurement
  virtual void fitWithVertex(TrackStatus status,
                             FitQuality*& fitQuality,
                             PerigeeParameters& perigee,
                             const Trk::RecVertex& vertex,
                             hit_const_iterator begin,
                             hit_const_iterator end) const override;

protected:
  struct State;

  // clean up memory
  void clearMeasurements(State& state) const;

  // create internal measurements from HitOnTrack or RIO_OnTrack
  void createMeasurements(State& state,
                          hit_const_iterator begin,
                          hit_const_iterator end) const;

  // set and invoke fortran fit procedure
  void fitProcedure(State& state,
                    TrackStatus status,
                    const PerigeeParameters& perigee,
                    hit_const_iterator begin,
                    hit_const_iterator end) const;

  // debug print-out
  void print(const State& state,
             TrackStatus status,
             const PerigeeParameters& perigee,
             const FitQuality& fitQuality) const;

  // get methods to extract fit results
  void createPerigee(const State& state, PerigeeParameters&) const;

  void createQuality(const State& state, FitQuality*&) const;
  parameter_vector* createScatterers(const State& state) const;
  void setResiduals(State& state) const;

  // temporary 3-point treatment
  void threePoint(State& state,
                  const PerigeeParameters& perigee,
                  hit_const_iterator begin,
                  hit_const_iterator end) const;

  ToolHandle<Trk::IMaterialAllocator> m_materialAllocator;
  ToolHandle<Trk::IIntersector> m_rungeKuttaIntersector;
  double m_scatteringAngle;
  ToolHandle<Trk::IIntersector> m_solenoidalIntersector;
  ToolHandle<Trk::IIntersector> m_straightLineIntersector;
  std::unique_ptr<Trk::FitProcedure> m_fitProcedure;
  ToolHandle<Trk::IPropagator> m_stepPropagator;
  ServiceHandle<Trk::ITrackingVolumesSvc> m_trackingVolumesSvc;
  Trk::FitMeasurement* m_vertexMeasurement;
  Trk::Volume* m_indetVolume;

  struct State
  {
    Trk::FitParameters* m_parameters = nullptr;
    std::unique_ptr<Trk::FitProcedureQuality> m_fitProcedureQuality;
    int m_driftHits = 0;
    int m_planarHits = 0;
    int m_fitCode = 0;
    int m_hit_pt3 = 0;
    double m_second_sigma_pt1 = 0;
    double m_sigma_pt1 = 0;
    double m_sigma_pt3 = 0;
    Trk::FitMeasurement* m_vertexMeasurement = nullptr;
    std::vector<Trk::FitMeasurement*> m_measurements;

    void clearMeasurements();
    ~State();
  };
};

#endif // IPATREC_TRACKFITTER_H

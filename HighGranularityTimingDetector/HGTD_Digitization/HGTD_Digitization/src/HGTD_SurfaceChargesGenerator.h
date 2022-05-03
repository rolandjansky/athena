/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_SurfaceChargesGenerator.h
 *
 * @author Tao Wang <tao.wang@cern.ch>
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 *
 * @date September, 2021
 *
 * @brief Deposited energy stored in SiHits gets transformed into charges
 * created in the active material.
 *
 * FIXME:
 * - for the estimation of the deposited charge, the total energy loss is
 * dividied by the mean ionization energy in Silicon. This should be refined, in
 * ITk specific tools are used for this!
 */

#ifndef HGTD_SURFACECHARGESGENERATOR_H
#define HGTD_SURFACECHARGESGENERATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "IHGTD_SurfaceChargesGenerator.h"
#include "HGTD_TimingResolution.h"

#include <string>

class HGTD_SurfaceChargesGenerator
    : public AthAlgTool,
      public virtual IHGTD_SurfaceChargesGenerator {

public:
  /**  constructor */
  HGTD_SurfaceChargesGenerator(const std::string &type, const std::string &name,
                               const IInterface *parent);

  /** Destructor */
  virtual ~HGTD_SurfaceChargesGenerator() = default;

  /** AlgTool initialize */
  StatusCode initialize() override;

  virtual void createSurfaceChargesFromHit(
      const TimedHitPtr<SiHit> &timed_hit_ptr,
      SiChargedDiodeCollection *diode_coll,
      const InDetDD::SolidStateDetectorElementBase *element,
      CLHEP::HepRandomEngine *rndm_engine,
      const EventContext& ctx) const override final;

private:
  // TODO: where do we get this from?
  FloatProperty m_small_step_length{this, "SmallStepLength", 1, ""}; /*um*/
  FloatProperty m_diffusion_constant{this, "DiffusionConstant", .007, ""};
  FloatProperty m_active_time_window{this, "ActiveTimeWindow", 1.25, "Hits within this time window are used for digitization, the rest are discarded (Given in ns)."};
  BooleanProperty m_smear_meantime{this, "SmearMeanTime", true, "Smear mean time based on radius and luminosity"};
  ToolHandle<HGTD_TimingResolution> m_hgtd_timing_resolution_tool{this, "TimingResolutionTool", "HGTD_TimingResolution", "Tool for smearing LGAD timing based on integrated luminosity and radius"};
};

#endif // HGTD_DIGITZATION_HGTD_SURFACECHARGESGENERATOR_H

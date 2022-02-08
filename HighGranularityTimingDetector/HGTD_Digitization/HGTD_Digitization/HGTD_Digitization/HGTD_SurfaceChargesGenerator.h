/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_Digitization/HGTD_SurfaceChargesGenerator.h
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
#include "HGTD_Digitization/IHGTD_SurfaceChargesGenerator.h"
#include "HGTD_Digitization/HGTD_TimingResolution.h"

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

  inline void setIntegratedLuminosity(float integrated_luminosity) override final {
    m_integrated_luminosity = integrated_luminosity;
  }

  inline void setSmearingTime(bool smear_meantime=true) override final {
    m_smear_meantime = smear_meantime;
  }

private:
  // TODO: where do we get this from?
  FloatProperty m_small_step_length{this, "SmallStepLength", 1, ""}; /*um*/
  ToolHandle<HGTD_TimingResolution> m_hgtd_timing_resolution_tool{this, "TimingResolutionTool", "HGTD_TimingResolution", "Tool for smearing LGAD timing based on integrated luminosity and radius"};
  FloatProperty m_diffusion_constant{this, "DiffusionConstant", .007, ""};
  bool m_smear_meantime{true};
  float m_integrated_luminosity{0};
};

#endif // HGTD_DIGITZATION_HGTD_SURFACECHARGESGENERATOR_H

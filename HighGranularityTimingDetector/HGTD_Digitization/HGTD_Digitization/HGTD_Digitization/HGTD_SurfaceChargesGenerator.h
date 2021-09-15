/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
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

#include <string>

class HGTD_SurfaceChargesGenerator
    : public AthAlgTool,
      public virtual IHGTD_SurfaceChargesGenerator {

public:
  /**  constructor */
  HGTD_SurfaceChargesGenerator(const std::string& type, const std::string& name,
                               const IInterface* parent);

  /** Destructor */
  virtual ~HGTD_SurfaceChargesGenerator() = default;

  /** AlgTool initialize */
  StatusCode initialize() override;

  virtual void createSurfaceChargesFromHit(
      const TimedHitPtr<SiHit>& timed_hit_ptr,
      SiChargedDiodeCollection* diode_coll,
      const InDetDD::SolidStateDetectorElementBase* element,
      CLHEP::HepRandomEngine* rndm_engine) const override;

private:
  // TODO: where do we get this from?
  FloatProperty m_small_step_length; /*um*/
  float m_diffusion_constant;
  // TODO what does the BooleanProperty allow to do?
  BooleanProperty m_needs_mc_evt_coll_helper{false};
};

#endif // HGTD_DIGITZATION_HGTD_SURFACECHARGESGENERATOR_H

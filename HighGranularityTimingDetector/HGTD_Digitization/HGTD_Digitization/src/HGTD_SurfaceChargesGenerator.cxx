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
 * @brief
 *
 */

#include "HGTD_Digitization/HGTD_SurfaceChargesGenerator.h"

#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "GeneratorObjects/McEventCollectionHelper.h"
#include "ReadoutGeometryBase/DetectorDesign.h"
#include "ReadoutGeometryBase/SiLocalPosition.h"
#include "ReadoutGeometryBase/SolidStateDetectorElementBase.h"
#include "SiDigitization/SiChargedDiodeCollection.h"

HGTD_SurfaceChargesGenerator::HGTD_SurfaceChargesGenerator(
    const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent),
      m_small_step_length(1),
      m_diffusion_constant(.007),
      m_needs_mc_evt_coll_helper(false) {
  declareProperty("SmallStepLength", m_small_step_length = 1);
  declareProperty("DiffusionConstant", m_diffusion_constant);
  declareProperty("UseMcEventCollectionHelper",
                  m_needs_mc_evt_coll_helper = false);
}

StatusCode HGTD_SurfaceChargesGenerator::initialize() {
  ATH_MSG_DEBUG("HGTD_SurfaceChargesGenerator::initialize()");

  m_small_step_length.setValue(m_small_step_length.value() * CLHEP::micrometer);

  return StatusCode::SUCCESS;
}

void HGTD_SurfaceChargesGenerator::createSurfaceChargesFromHit(
    const TimedHitPtr<SiHit>& timed_hit_ptr,
    SiChargedDiodeCollection* diode_coll,
    const InDetDD::SolidStateDetectorElementBase* element,
    CLHEP::HepRandomEngine* rndm_engine) const {

  const SiHit& hit = *timed_hit_ptr;

  // check the status of truth information for this SiHit
  // some Truth information is cut for pile up events
  HepMcParticleLink trklink(hit.particleLink());
  if (m_needs_mc_evt_coll_helper) {
    trklink.setEventCollection(
        McEventCollectionHelper::getMcEventCollectionHMPLEnumFromPileUpType(
            timed_hit_ptr.pileupType(), &msg()));
  }

  SiCharge::Process hitproc(SiCharge::track);
  if (hit.trackNumber() != 0) {
    if (not trklink.isValid()) {
      hitproc = SiCharge::cut_track;
    }
  }

  double sensor_thickness = element->design().thickness();
  int readout_side = element->design().readoutSide();

  double pixel_size_x = element->design().phiPitch();
  double pixel_size_y = element->design().etaPitch();

  const CLHEP::Hep3Vector start_pos(hit.localStartPosition());
  const CLHEP::Hep3Vector end_pos(hit.localEndPosition());

  CLHEP::Hep3Vector direction = end_pos - start_pos;
  double deposit_length = direction.mag();
  int n_steps = deposit_length / m_small_step_length + 1;
  direction.setMag(deposit_length / static_cast<float>(n_steps));

  double tot_eloss = hit.energyLoss();
  // FIXME using the mean ionization energy in Silicon
  const float tot_charge = tot_eloss / (3.62 * CLHEP::eV);

  double charge_per_step = tot_charge / static_cast<float>(n_steps);

  // FIXME is this correct? does the eventTime include a "later" truth event and
  // the meanTime is just the TOF?
  float time_of_flight = timed_hit_ptr.eventTime() + hit.meanTime();

  // FIXME needed to check for deposits in guardrings. This should be taken over
  // by the module design class and not hardcoded here!
  float x_offset = 9.75;
  float y_offset = 19.5;

  for (int i_step = 0; i_step < n_steps; i_step++) {
    CLHEP::Hep3Vector surface_pos = start_pos + i_step * direction;

    // Distance between charge and readout side.  p_design->readoutSide() is
    // +1 if readout side is in +ve depth axis direction and visa-versa.
    // FIXME ask Noemi about what happens here
    double spess =
        0.5 * sensor_thickness - readout_side * surface_pos[SiHit::xDep];
    if (spess < 0) {
      spess = 0; // FIXME this means I am on the surface already?
    }
    // diffusion sigma
    // FIXME where is the 0.3 from?
    double rdif = m_diffusion_constant * std::sqrt(spess / 0.3);

    // position at the surface, adding smearing
    // FIXME currently no Lorentz angle considered, can be studied in the future
    double surf_pos_x = surface_pos[SiHit::xEta] +
                        rdif * CLHEP::RandGaussZiggurat::shoot(rndm_engine);
    double surf_pos_y = surface_pos[SiHit::xPhi] +
                        rdif * CLHEP::RandGaussZiggurat::shoot(rndm_engine);

    // if the deposit is outside the guard ring, don't consider it
    if (fabs(surf_pos_x) > x_offset or fabs(surf_pos_y) > y_offset) {
      continue;
    }

    // FIXME this should be handled by the module design class in the future
    float interpad = 50 * CLHEP::micrometer;

    int bin_x = floor(fabs(surf_pos_x + x_offset) / pixel_size_x);
    int bin_y = floor(fabs(surf_pos_y + y_offset) / pixel_size_y);

    float pos_x_inpixel =
        fabs(surf_pos_x + x_offset) - float(bin_x) * pixel_size_x;
    float pos_y_inpixel =
        fabs(surf_pos_y + y_offset) - float(bin_y) * pixel_size_y;

    bool is_interpad_x =
        (pos_x_inpixel < interpad or pos_x_inpixel > (pixel_size_x - interpad));
    bool is_interpad_y =
        (pos_y_inpixel < interpad or pos_y_inpixel > (pixel_size_y - interpad));

    // check if the charge is sitting in the interpad region
    if (is_interpad_x or is_interpad_y) {
      ATH_MSG_DEBUG("INTERPAD DETECTED!!!");
      ATH_MSG_DEBUG("surf_pos_x=" << surf_pos_x
                                  << ", surf_pos_y=" << surf_pos_y);
      continue;
    }
    // charges deposited within the active sensor get added
    const InDetDD::SiLocalPosition position(
        element->hitLocalToLocal(surf_pos_x, surf_pos_y));

    SiSurfaceCharge surface_charge(
        position, SiCharge(charge_per_step, time_of_flight, hitproc,
                           trklink)); // FIXME is this obj even needed?

    InDetDD::SiCellId cell_id =
        element->cellIdOfPosition(surface_charge.position());

    if (cell_id.isValid()) {
      // add this charge to the collection (or merge in existing charged
      // diode)
      diode_coll->add(cell_id, surface_charge.charge());
    }
  } // END LOOP over steps
}

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
 * @brief
 *
 */

#include "HGTD_SurfaceChargesGenerator.h"

#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "ReadoutGeometryBase/DetectorDesign.h"
#include "ReadoutGeometryBase/SiLocalPosition.h"
#include "ReadoutGeometryBase/SolidStateDetectorElementBase.h"
#include "SiDigitization/SiChargedDiodeCollection.h"

HGTD_SurfaceChargesGenerator::HGTD_SurfaceChargesGenerator(
    const std::string &type, const std::string &name, const IInterface *parent)
    : AthAlgTool(type, name, parent) {
}

StatusCode HGTD_SurfaceChargesGenerator::initialize() {
  ATH_MSG_DEBUG("HGTD_SurfaceChargesGenerator::initialize()");

  m_small_step_length.setValue(m_small_step_length.value() * CLHEP::micrometer);

  ATH_CHECK(m_hgtd_timing_resolution_tool.retrieve());
  ATH_CHECK(m_hgtd_timing_resolution_tool->setIntegratedLuminosity(m_integrated_luminosity));

  return StatusCode::SUCCESS;
}

void HGTD_SurfaceChargesGenerator::createSurfaceChargesFromHit(
    const TimedHitPtr<SiHit> &timed_hit_ptr,
    SiChargedDiodeCollection *diode_coll,
    const InDetDD::SolidStateDetectorElementBase *element,
    CLHEP::HepRandomEngine *rndm_engine,
    const EventContext& ctx) const {

  const SiHit &hit = *timed_hit_ptr;

  // check the status of truth information for this SiHit
  // some Truth information is cut for pile up events
  unsigned short eventId = timed_hit_ptr.eventId();
  const EBC_EVCOLL evColl = EBC_MAINEVCOLL;
  const HepMcParticleLink::PositionFlag idxFlag = (eventId==0) ? HepMcParticleLink::IS_POSITION: HepMcParticleLink::IS_INDEX;
  const HepMcParticleLink trklink{HepMcParticleLink(hit.trackNumber(), eventId, evColl, idxFlag, ctx)};
  SiCharge::Process hitproc{SiCharge::track};
  if (hit.trackNumber() != 0) {
    if (not trklink.isValid()) {
      hitproc = SiCharge::cut_track;
    }
  }

  float sensor_thickness = element->design().thickness();
  int readout_side = element->design().readoutSide();

  float pixel_size_xphi = element->design().phiPitch();
  float pixel_size_xeta = element->design().etaPitch();

  Amg::Vector3D element_center = element->center();
  ATH_MSG_DEBUG("x and y, z are: " << element_center.x() << ", "
                                   << element_center.y() << ", "
                                   << element_center.z());
  float element_r = sqrt(element_center.x() * element_center.x() +
                         element_center.y() * element_center.y());

  const CLHEP::Hep3Vector start_pos(hit.localStartPosition());
  const CLHEP::Hep3Vector end_pos(hit.localEndPosition());

  ATH_MSG_DEBUG("start_pos xEta=" << start_pos[SiHit::xEta]
                                  << ", xPhi=" << start_pos[SiHit::xPhi]
                                  << ", xDep=" << start_pos[SiHit::xDep]);

  CLHEP::Hep3Vector direction = end_pos - start_pos;
  float deposit_length = direction.mag();
  int n_steps = deposit_length / m_small_step_length + 1;
  //the start and end pos can sit at the same position. Resizing the zero-length
  //Hep3Vector would cause an error, so this we protect agains
  if (deposit_length > 1.e-10) {
    direction.setMag(deposit_length / static_cast<float>(n_steps));
  }

  float tot_eloss = hit.energyLoss();
  // FIXME using the mean ionization energy in Silicon
  const float tot_charge = tot_eloss / (3.62 * CLHEP::eV);

  float charge_per_step = tot_charge / static_cast<float>(n_steps);

  // FIXME is this correct? does the eventTime include a "later" truth event and
  // the meanTime is just the TOF?
  ATH_MSG_DEBUG(">>>>>>> before processing, event_t, t, E, r: "
                << timed_hit_ptr.eventTime() << ", " << hit.meanTime() << ", "
                << tot_eloss << ", " << element_r);

  float time_of_flight = timed_hit_ptr.eventTime() + hit.meanTime();
  if (m_smear_meantime) {
    // Smearing based on radius and luminosity, and substract the time shift due
    // to pulse leading edge (0.4 ns)
    time_of_flight = m_hgtd_timing_resolution_tool->calculateTime(
                         time_of_flight, tot_eloss, element_r, rndm_engine) -
                     0.4;
  }
  ATH_MSG_DEBUG(">>>>>>> after processing, t: " << time_of_flight);

  // FIXME needed to check for deposits in guardrings. This should be taken over
  // by the module design class and not hardcoded here!

  float xphi_offset = 9.75;
  float xeta_offset = 19.5;
  // FIXME this should be handled by the module design class in the future
  float interpad = 50 * CLHEP::micrometer;

  for (int i_step = 0; i_step < n_steps; i_step++) {
    CLHEP::Hep3Vector surface_pos = start_pos + i_step * direction;
    ATH_MSG_DEBUG("surface_pos x=" << surface_pos.x()
                                   << ", y=" << surface_pos.y()
                                   << ", z=" << surface_pos.z());
    ATH_MSG_DEBUG("surface_pos xEta=" << surface_pos[SiHit::xEta]
                                      << ", xPhi=" << surface_pos[SiHit::xPhi]
                                      << ", xDep=" << surface_pos[SiHit::xDep]);
    // NB! y aka xPhi is the long side of the module!
    // DEBUG surface_pos x=9.08365,  y=-1.17206, z=-0.025
    // DEBUG surface_pos xEta=-0.025, xPhi=-1.17206, xDep=9.08365
    // FIXME: eta, phi andd dep need to be revisited in Rel 22!

    // Distance between charge and readout side.  p_design->readoutSide() is
    // +1 if readout side is in +ve depth axis direction and visa-versa.
    // FIXME ask Noemi about what happens here
    float spess =
        0.5 * sensor_thickness - readout_side * surface_pos[SiHit::xDep];
    if (spess < 0) {
      spess = 0; // FIXME this means I am on the surface already?
    }
    // diffusion sigma
    // FIXME where is the 0.3 from?
    float rdif = m_diffusion_constant * std::sqrt(spess / 0.3);

    // position at the surface, adding smearing
    // FIXME currently no Lorentz angle considered, can be studied in the future
    float surf_pos_xphi = surface_pos[SiHit::xPhi] +
                          rdif * CLHEP::RandGaussZiggurat::shoot(rndm_engine);
    float surf_pos_xeta = surface_pos[SiHit::xEta] +
                          rdif * CLHEP::RandGaussZiggurat::shoot(rndm_engine);

    // if the deposit is outside the guard ring, don't consider it
    if (fabs(surf_pos_xphi) > xphi_offset or
        fabs(surf_pos_xeta) > xeta_offset) {
      ATH_MSG_DEBUG("Hit in guard ring");
      continue;
    }

    int bin_xphi = floor(fabs(surf_pos_xphi + xphi_offset) / pixel_size_xphi);
    int bin_xeta = floor(fabs(surf_pos_xeta + xeta_offset) / pixel_size_xeta);

    float pos_xphi_inpixel =
        fabs(surf_pos_xphi + xphi_offset) - float(bin_xphi) * pixel_size_xphi;
    float pos_xeta_inpixel =
        fabs(surf_pos_xeta + xeta_offset) - float(bin_xeta) * pixel_size_xeta;

    bool is_interpad_xphi = (pos_xphi_inpixel < interpad or
                             pos_xphi_inpixel > (pixel_size_xphi - interpad));
    bool is_interpad_xeta = (pos_xeta_inpixel < interpad or
                             pos_xeta_inpixel > (pixel_size_xeta - interpad));

    // check if the charge is sitting in the interpad region
    if (is_interpad_xphi or is_interpad_xeta) {
      ATH_MSG_DEBUG("Hit in interpad region");
      continue;
    }
    // charges deposited within the active sensor get added
    const InDetDD::SiLocalPosition position(
        element->hitLocalToLocal(surf_pos_xeta, surf_pos_xphi));

    SiSurfaceCharge surface_charge(
        position, SiCharge(charge_per_step, time_of_flight, hitproc,
                           trklink)); // FIXME is this obj even needed?

    InDetDD::SiCellId cell_id =
        element->cellIdOfPosition(surface_charge.position());
    ATH_MSG_DEBUG("cell_id x=" << cell_id);
    if (cell_id.isValid()) {
      // add this charge to the collection (or merge in existing charged diode)
      diode_coll->add(cell_id, surface_charge.charge());
    }
  } // END LOOP over steps
}

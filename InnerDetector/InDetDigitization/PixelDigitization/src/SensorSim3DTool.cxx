/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "SensorSim3DTool.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "InDetSimEvent/SiHit.h"
#include "SiDigitization/SiSurfaceCharge.h"
#include "InDetIdentifier/PixelID.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "SiPropertiesTool/SiliconProperties.h"

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussZiggurat.h"

#include "TLorentzVector.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "PathResolver/PathResolver.h"

#include <cmath>
#include <memory>

using namespace InDetDD;

//===============================================
//    C O N S T R U C T O R
//===============================================
SensorSim3DTool::SensorSim3DTool(const std::string& type, const std::string& name, const IInterface* parent) :
  SensorSimTool(type, name, parent) {
}

SensorSim3DTool::~SensorSim3DTool() { }

//===============================================
//    I N I T I A L I Z E
//===============================================
StatusCode SensorSim3DTool::initialize() {
  ATH_MSG_DEBUG("SensorSim3DTool::initialize()");

  ATH_CHECK(SensorSimTool::initialize());
  ATH_CHECK(m_radDamageUtil.retrieve());

  ATH_CHECK(readProbMap(m_cc_prob_file_fei3));
  ATH_CHECK(readProbMap(m_cc_prob_file_fei4));

  return StatusCode::SUCCESS;
}

//===============================================
//    F I N A L I Z E
//===============================================
StatusCode SensorSim3DTool::finalize() {
  ATH_MSG_DEBUG("SensorSim3DTool::finalize()");
  return StatusCode::SUCCESS;
}

//===============================================
//    I N D U C E    C H A R G E
//===============================================
StatusCode SensorSim3DTool::induceCharge(const TimedHitPtr<SiHit>& phit,
                                         SiChargedDiodeCollection& chargedDiodes,
                                         const InDetDD::SiDetectorElement& Module,
                                         const InDetDD::PixelModuleDesign& p_design,
                                         std::vector< std::pair<double, double> >& trfHitRecord,
                                         std::vector<double>& initialConditions,
                                         CLHEP::HepRandomEngine* rndmEngine,
                                         const EventContext &ctx) {
  // TODO: check that detectors other than ITk have this properly set
  if (p_design.getReadoutTechnology() == InDetDD::PixelReadoutTechnology::RD53) {
    // disable for now!
    return StatusCode::SUCCESS;
  } else {
    if (!Module.isBarrel()) {
      return StatusCode::SUCCESS;
    }
    if (p_design.getReadoutTechnology() != InDetDD::PixelReadoutTechnology::FEI4) {
      return StatusCode::SUCCESS;
    }
    if (p_design.numberOfCircuits() > 1) {
      return StatusCode::SUCCESS;
    }
  }

  ATH_MSG_DEBUG("Applying SensorSim3D charge processor");
  if (initialConditions.size() != 8) {
    ATH_MSG_ERROR("Starting coordinates were not filled correctly in EnergyDepositionSvc.");
    return StatusCode::FAILURE;
  }

  double eta_0 = initialConditions[0];
  double phi_0 = initialConditions[1];
  double depth_0 = initialConditions[2];
  double dEta = initialConditions[3];
  double dPhi = initialConditions[4];
  double dDepth = initialConditions[5];
  double ncharges = initialConditions[6];
  double iTotalLength = initialConditions[7];
  ncharges = 50;

  ATH_MSG_VERBOSE("Applying 3D sensor simulation.");
  double sensorThickness = Module.design().thickness();
  const InDet::SiliconProperties& siProperties = m_siPropertiesTool->getSiProperties(Module.identifyHash(), ctx);
  double eleholePairEnergy = siProperties.electronHolePairsPerEnergy();

  // Charge Collection Probability Map bin size
  const double x_bin_size = 0.001;
  const double y_bin_size = 0.001;

  // determine which readout is used
  // FEI4 : 50 X 250 microns
  double pixel_size_x = Module.width() / p_design.rows();
  double pixel_size_y = Module.length() / p_design.columns();
  double module_size_x = Module.width();
  double module_size_y = Module.length();

  const EBC_EVCOLL evColl = EBC_MAINEVCOLL;
  const HepMcParticleLink::PositionFlag idxFlag =
    (phit.eventId() == 0) ? HepMcParticleLink::IS_POSITION : HepMcParticleLink::IS_INDEX;

  // pre-make HepMcParticleLink
  auto particleLink = HepMcParticleLink(phit->trackNumber(), phit.eventId(), evColl, idxFlag, ctx);
  const double pHitTime = hitTime(phit);

  if (m_doRadDamage) {
    const bool doChunkCorrection = m_doChunkCorrection.value();
    //**************************************//
    //*** Now diffuse charges to surface *** //
    //**************************************//
    //Calculate trapping times based on fluence (already includes check for fluence=0)
    SG::ReadCondHandle<PixelRadiationDamageFluenceMapData> fluenceData(m_fluenceDataKey,ctx);
    if (m_doRadDamage) {
      std::pair < double, double > trappingTimes = m_radDamageUtil->getTrappingTimes(fluenceData->getFluenceLayer3D(0));   //0 = IBL
      m_trappingTimeElectrons = trappingTimes.first;
      m_trappingTimeHoles = trappingTimes.second;
    }
    const PixelHistoConverter& ramoPotentialMap = fluenceData->getRamoPotentialMap3D(0);
    const PixelHistoConverter& eFieldMap        = fluenceData->getEFieldMap3D(0);
    const PixelHistoConverter& xPositionMap_e   = fluenceData->getXPositionMap3D_e(0);
    const PixelHistoConverter& xPositionMap_h   = fluenceData->getXPositionMap3D_h(0);
    const PixelHistoConverter& yPositionMap_e   = fluenceData->getYPositionMap3D_e(0);
    const PixelHistoConverter& yPositionMap_h   = fluenceData->getYPositionMap3D_h(0);
    const PixelHistoConverter& timeMap_e        = fluenceData->getTimeMap3D_e(0);
    const PixelHistoConverter& timeMap_h        = fluenceData->getTimeMap3D_h(0);
    const PixelHistoConverter& avgChargeMap_e   = fluenceData->getAvgChargeMap3D_e();
    const PixelHistoConverter& avgChargeMap_h   = fluenceData->getAvgChargeMap3D_h();

    //Parameters which will be smeared by a random value for each charge propagated,
    //recomputed for every trfHitRecord but initialized only once 
    std::vector<double> DtElectron (ncharges, 0.);
    std::vector<double> DtHole (ncharges, 0.);
    std::vector<double> rdifElectron (ncharges, 0.);
    std::vector<double> rdifHole (ncharges, 0.);
    
    for (size_t istep = 0; istep < trfHitRecord.size(); istep++) {
      std::pair< double,double> const & iHitRecord = trfHitRecord[istep];

      double eta_i = eta_0;
      double phi_i = phi_0;
      double depth_i = depth_0;

      if (iTotalLength) {
        eta_i += 1.0 * iHitRecord.first / iTotalLength * dEta;
        phi_i += 1.0 * iHitRecord.first / iTotalLength * dPhi;
        depth_i += 1.0 * iHitRecord.first / iTotalLength * dDepth;
      }

      const double energy_per_step = 1.0 * iHitRecord.second / 1.E+6 / ncharges;    //in MeV
      ATH_MSG_DEBUG("es_current: " << energy_per_step << " split between " << ncharges << " charges");

      double dist_electrode = 0.5 * sensorThickness - Module.design().readoutSide() * depth_i;
      if (dist_electrode < 0) dist_electrode = 0;

      CLHEP::Hep3Vector chargepos;
      chargepos.setX(phi_i);
      chargepos.setY(eta_i);
      chargepos.setZ(dist_electrode);

      bool coord = Module.isModuleFrame();

      ATH_MSG_DEBUG(
        "ismoduleframe " << coord << " -- startPosition (x,y,z) = " << chargepos.x() << ", " << chargepos.y() << ", " <<
          chargepos.z());

      // -- change origin of coordinates to the left bottom of module
      double x_new = chargepos.x() + 0.5*module_size_x;
      double y_new = chargepos.y() + 0.5*module_size_y;

      // -- change from module frame to pixel frame
      int nPixX = int(x_new / pixel_size_x);
      int nPixY = int(y_new / pixel_size_y);
      ATH_MSG_DEBUG(" -- nPixX = " << nPixX << "  nPixY = " << nPixY);

      //position relative to the bottom left corner of the pixel
      double x_pix = x_new - pixel_size_x * (nPixX);
      double y_pix = y_new - pixel_size_y * (nPixY);
      // -- change origin of coordinates to the center of the pixel
      double x_pix_center = x_pix - pixel_size_x / 2;
      double y_pix_center = y_pix - pixel_size_y / 2;
      ATH_MSG_DEBUG(" -- current hit position w.r.t. pixel corner = " << x_pix << "  " << y_pix);
      ATH_MSG_DEBUG(" -- current hit position w.r.t. pixel center = " << x_pix_center << "  " << y_pix_center);

      //only process hits which are not on the electrodes (E-field zero)
      //all the maps have 250 as the x value, so need to invert x and y whenever reading maps
      double efield = eFieldMap.getContent(eFieldMap.getBinX(1e3*y_pix),eFieldMap.getBinY(1e3*x_pix))*1.0E-7;   //return efield in MV/mm (for mobility calculation)

      if (efield == 0) {
        ATH_MSG_DEBUG("Skipping since efield = 0 for x_pix = " << x_pix << " y_pix = " << y_pix);
        continue;
      }

      const double mobilityElectron = getMobility(efield, false);
      const double mobilityHole     = getMobility(efield, true);
      auto driftTimeElectron = getDriftTime(false, ncharges);
      auto driftTimeHole = getDriftTime(true, ncharges);
      //Need to determine how many elementary charges this charge chunk represents.
      double chunk_size = energy_per_step * eleholePairEnergy; //number of electrons/holes
      //set minimum limit to prevent dividing into smaller subcharges than one fundamental charge
      if (chunk_size < 1) chunk_size = 1;
      const double kappa = 1. / std::sqrt(chunk_size);
     
      const double timeToElectrodeElectron = timeMap_e.getContent(timeMap_e.getBinX(1e3*y_pix), timeMap_e.getBinY(1e3*x_pix));
      const double timeToElectrodeHole =     timeMap_h.getContent(timeMap_h.getBinX(1e3*y_pix), timeMap_h.getBinY(1e3*x_pix));
      
      /*  Diffusion via the Einstein relation
      D = mu * kB * T / q
      D = (mu / mm^2/MV*ns) * (T/273 K) * 0.024 microns^2 / ns  */ 
      const auto prefactor_e = mobilityElectron*0.024*m_temperature / 273.;
      const auto prefactor_h = mobilityHole*0.024*m_temperature / 273.;
      //Apply diffusion. rdif is the max. diffusion
      for(size_t i = 0; i < ncharges; ++i) {
        DtElectron[i] = prefactor_e * std::min(driftTimeElectron[i], timeToElectrodeElectron);
        DtHole[i] = prefactor_h * std::min(driftTimeHole[i], timeToElectrodeHole);
        rdifElectron[i] = 1e-3*std::sqrt(DtElectron[i]); //in mm
        rdifHole[i] = 1e-3*std::sqrt(DtHole[i]); //in mm
      }

      const float average_chargeElectron = avgChargeMap_e.getContent(avgChargeMap_e.getBinY(1e3*y_pix), avgChargeMap_e.getBinX(1e3*x_pix));
      const float average_chargeHole =     avgChargeMap_h.getContent(avgChargeMap_h.getBinY(1e3*y_pix), avgChargeMap_h.getBinX(1e3*x_pix));

      //We're sticking to the "old" convention here where there was a loop over -1/0/1 for both directions
      //(x/y) to the neighbouring pixels. We call them (p)lus, (z)ero, (m)inus in either i- or j- direction
      //hence giving us plus-in-i or pi or zero-in-j or zj etc.
      //i is in direction of x, whereas j is into the y direction, this code is ugly, but given that we don't have very few branching conditions
      //it's pretty fast
      
      //This was: ramoPotentialMap.getBinY(1000*(x_pix + pixel_size_x * 3 - i * pixel_size_x)
      const std::size_t ramo_init_bin_y_pi  = ramoPotentialMap.getBinY(1000*(x_pix + pixel_size_x * 2));
      const std::size_t ramo_init_bin_y_zi  = ramoPotentialMap.getBinY(1000*(x_pix + pixel_size_x * 3));
      const std::size_t ramo_init_bin_y_mi  = ramoPotentialMap.getBinY(1000*(x_pix + pixel_size_x * 4));

      //This was: ramoPotentialMap.getBinX(1000*(y_pix + 0.5*pixel_size_y - j * pixel_size_y)
      const std::size_t ramo_init_bin_x_pj  = ramoPotentialMap.getBinX(1000*(y_pix - 0.5*pixel_size_y));
      const std::size_t ramo_init_bin_x_zj  = ramoPotentialMap.getBinX(1000*(y_pix + 0.5*pixel_size_y));
      const std::size_t ramo_init_bin_x_mj  = ramoPotentialMap.getBinX(1000*(y_pix + 1.5*pixel_size_y));

      //For some reason the x- and y-indices for some values are swapped, this is extremely confusing
      float ramoInit_pipj  = ramoPotentialMap.getContent(ramo_init_bin_x_pj, ramo_init_bin_y_pi);
      float ramoInit_pizj  = ramoPotentialMap.getContent(ramo_init_bin_x_zj, ramo_init_bin_y_pi);
      float ramoInit_pimj  = ramoPotentialMap.getContent(ramo_init_bin_x_mj, ramo_init_bin_y_pi);
      float ramoInit_zipj  = ramoPotentialMap.getContent(ramo_init_bin_x_pj, ramo_init_bin_y_zi);
      float ramoInit_zizj  = ramoPotentialMap.getContent(ramo_init_bin_x_zj, ramo_init_bin_y_zi);
      float ramoInit_zimj  = ramoPotentialMap.getContent(ramo_init_bin_x_mj, ramo_init_bin_y_zi);
      float ramoInit_mipj  = ramoPotentialMap.getContent(ramo_init_bin_x_pj, ramo_init_bin_y_mi);
      float ramoInit_mizj  = ramoPotentialMap.getContent(ramo_init_bin_x_zj, ramo_init_bin_y_mi);
      float ramoInit_mimj  = ramoPotentialMap.getContent(ramo_init_bin_x_mj, ramo_init_bin_y_mi);

      auto mc_particle_link = HepMcParticleLink(phit->trackNumber(), phit.eventId(), evColl, idxFlag, ctx);
      const auto hit_time = hitTime(phit);

      //Loop over charge-carrier pairs, we're looping over electrons and holes at the same time
      for (int j = 0; j < ncharges; j++) {
          // In case the charge moves into a neighboring pixel
          int extraNPixXElectron = nPixX;
          int extraNPixYElectron = nPixY;
          int extraNPixXHole = nPixX;
          int extraNPixYHole = nPixY;
          
	  //Apply drift due to diffusion
          std::array<double, 4> randomNumbers{};
	  CLHEP::RandGaussZiggurat::shootArray(rndmEngine, 4, randomNumbers.data());
          
          double xposDiffElectron = x_pix + rdifElectron[j] * randomNumbers[0];
          double yposDiffElectron = y_pix + rdifElectron[j] * randomNumbers[1];
          double xposDiffHole = x_pix + rdifHole[j] * randomNumbers[2];
          double yposDiffHole = y_pix + rdifHole[j] * randomNumbers[3];

          // Account for drifting into another pixel
          while (xposDiffElectron > pixel_size_x) {
            extraNPixXElectron++;                            // increments or decrements pixel count in x
            xposDiffElectron -= pixel_size_x;        // moves xpos coordinate 1 pixel over in x
          }
          while (xposDiffElectron < 0) {
            extraNPixXElectron--;
            xposDiffElectron += pixel_size_x;
          }
          while (yposDiffElectron > pixel_size_y) {
            extraNPixYElectron++;               // increments or decrements pixel count in y
            yposDiffElectron -= pixel_size_y;        // moves xpos coordinate 1 pixel over in y
          }
          while (yposDiffElectron < 0) {
            extraNPixYElectron--;
            yposDiffElectron += pixel_size_y;
          }

	  //And drifting for for holes
          while (xposDiffHole > pixel_size_x) {
            extraNPixXHole++;
            xposDiffHole -= pixel_size_x;
          }
          while (xposDiffHole < 0) {
            extraNPixXHole--;
            xposDiffHole += pixel_size_x;
          }
          while (yposDiffHole > pixel_size_y) {
            extraNPixYHole++;
            yposDiffHole -= pixel_size_y;
          }
          while (yposDiffHole < 0) {
            extraNPixYHole--;
            yposDiffHole += pixel_size_y;
          }
          auto xposFinalElectron = yPositionMap_e.getContent(  yPositionMap_e.getBinX(1e3*yposDiffElectron),
			                                       yPositionMap_e.getBinY(1e3*xposDiffElectron),
							       yPositionMap_e.getBinZ(std::min(driftTimeElectron[j],timeToElectrodeElectron)) )*1e-3;  //[mm]
          auto yposFinalElectron = xPositionMap_e.getContent(  xPositionMap_e.getBinX(1e3*yposDiffElectron),
			                                       xPositionMap_e.getBinY(1e3*xposDiffElectron),
							       xPositionMap_e.getBinZ(std::min(driftTimeElectron[j],timeToElectrodeElectron)) )*1e-3;  //[mm]

          auto xposFinalHole = yPositionMap_h.getContent(  yPositionMap_h.getBinX(1e3*yposDiffHole),
			                                   yPositionMap_h.getBinY(1e3*xposDiffHole),
							   yPositionMap_h.getBinZ(std::min(driftTimeHole[j],timeToElectrodeHole))  )*1e-3;  //[mm]
          auto yposFinalHole = xPositionMap_h.getContent(  xPositionMap_h.getBinX(1e3*yposDiffHole),
			                                   xPositionMap_h.getBinY(1e3*xposDiffHole),
							   xPositionMap_h.getBinZ(std::min(driftTimeHole[j],timeToElectrodeHole))  )*1e-3;  //[mm]

          // -- Calculate signal in current pixel and in the neighboring ones
          //This was: ramoPotentialMap.getBinY(1000*(xposFinal + pixel_size_x * 3 - i * pixel_size_x)
          const std::size_t ramo_final_bin_y_pi_electrons = ramoPotentialMap.getBinY(1000*(xposFinalElectron + pixel_size_x * 2));
          const std::size_t ramo_final_bin_y_pi_holes = ramoPotentialMap.getBinY(1000*(xposFinalHole + pixel_size_x * 2));
          const std::size_t ramo_final_bin_y_zi_electrons = ramoPotentialMap.getBinY(1000*(xposFinalElectron + pixel_size_x * 3));
          const std::size_t ramo_final_bin_y_zi_holes = ramoPotentialMap.getBinY(1000*(xposFinalHole + pixel_size_x * 3));
          const std::size_t ramo_final_bin_y_mi_electrons = ramoPotentialMap.getBinY(1000*(xposFinalElectron + pixel_size_x * 4));
          const std::size_t ramo_final_bin_y_mi_holes = ramoPotentialMap.getBinY(1000*(xposFinalHole + pixel_size_x * 4));
          //This was: ramoPotentialMap.getBinX(1000*(yposFinal + 0.5*pixel_size_y - j * pixel_size_y)
          const std::size_t ramo_final_bin_x_pj_electrons = ramoPotentialMap.getBinX(1000*(yposFinalElectron - 0.5*pixel_size_y));
          const std::size_t ramo_final_bin_x_pj_holes = ramoPotentialMap.getBinX(1000*(yposFinalHole - 0.5*pixel_size_y));
          const std::size_t ramo_final_bin_x_zj_electrons = ramoPotentialMap.getBinX(1000*(yposFinalElectron + 0.5*pixel_size_y));
          const std::size_t ramo_final_bin_x_zj_holes = ramoPotentialMap.getBinX(1000*(yposFinalHole + 0.5*pixel_size_y));
          const std::size_t ramo_final_bin_x_mj_electrons = ramoPotentialMap.getBinX(1000*(yposFinalElectron + 1.5*pixel_size_y));
          const std::size_t ramo_final_bin_x_mj_holes = ramoPotentialMap.getBinX(1000*(yposFinalHole + 1.5*pixel_size_y));

          float ramoFinal_pipj_electrons = ramoPotentialMap.getContent(ramo_final_bin_x_pj_electrons, ramo_final_bin_y_pi_electrons);
          float ramoFinal_pizj_electrons = ramoPotentialMap.getContent(ramo_final_bin_x_zj_electrons, ramo_final_bin_y_pi_electrons);
          float ramoFinal_pimj_electrons = ramoPotentialMap.getContent(ramo_final_bin_x_mj_electrons, ramo_final_bin_y_pi_electrons);
          float ramoFinal_zipj_electrons = ramoPotentialMap.getContent(ramo_final_bin_x_pj_electrons, ramo_final_bin_y_zi_electrons);
          float ramoFinal_zizj_electrons = ramoPotentialMap.getContent(ramo_final_bin_x_zj_electrons, ramo_final_bin_y_zi_electrons);
          float ramoFinal_zimj_electrons = ramoPotentialMap.getContent(ramo_final_bin_x_mj_electrons, ramo_final_bin_y_zi_electrons);
          float ramoFinal_mipj_electrons = ramoPotentialMap.getContent(ramo_final_bin_x_pj_electrons, ramo_final_bin_y_mi_electrons);
          float ramoFinal_mizj_electrons = ramoPotentialMap.getContent(ramo_final_bin_x_zj_electrons, ramo_final_bin_y_mi_electrons);
          float ramoFinal_mimj_electrons = ramoPotentialMap.getContent(ramo_final_bin_x_mj_electrons, ramo_final_bin_y_mi_electrons);

          float ramoFinal_pipj_holes = ramoPotentialMap.getContent(ramo_final_bin_x_pj_holes, ramo_final_bin_y_pi_holes);
          float ramoFinal_pizj_holes = ramoPotentialMap.getContent(ramo_final_bin_x_zj_holes, ramo_final_bin_y_pi_holes);
          float ramoFinal_pimj_holes = ramoPotentialMap.getContent(ramo_final_bin_x_mj_holes, ramo_final_bin_y_pi_holes);
          float ramoFinal_zipj_holes = ramoPotentialMap.getContent(ramo_final_bin_x_pj_holes, ramo_final_bin_y_zi_holes);
          float ramoFinal_zizj_holes = ramoPotentialMap.getContent(ramo_final_bin_x_zj_holes, ramo_final_bin_y_zi_holes);
          float ramoFinal_zimj_holes = ramoPotentialMap.getContent(ramo_final_bin_x_mj_holes, ramo_final_bin_y_zi_holes);
          float ramoFinal_mipj_holes = ramoPotentialMap.getContent(ramo_final_bin_x_pj_holes, ramo_final_bin_y_mi_holes);
          float ramoFinal_mizj_holes = ramoPotentialMap.getContent(ramo_final_bin_x_zj_holes, ramo_final_bin_y_mi_holes);
          float ramoFinal_mimj_holes = ramoPotentialMap.getContent(ramo_final_bin_x_mj_holes, ramo_final_bin_y_mi_holes);

          // Record deposit
          double eHitRamo_pipj_electrons =   energy_per_step * (ramoFinal_pipj_electrons - ramoInit_pipj);
          double eHitRamo_pipj_holes = -1. * energy_per_step * (ramoFinal_pipj_holes     - ramoInit_pipj);
          double eHitRamo_pizj_electrons =   energy_per_step * (ramoFinal_pizj_electrons - ramoInit_pizj);
          double eHitRamo_pizj_holes = -1. * energy_per_step * (ramoFinal_pizj_holes     - ramoInit_pizj);
          double eHitRamo_pimj_electrons =   energy_per_step * (ramoFinal_pimj_electrons - ramoInit_pimj);
          double eHitRamo_pimj_holes = -1. * energy_per_step * (ramoFinal_pimj_holes     - ramoInit_pimj);
          double eHitRamo_zipj_electrons =   energy_per_step * (ramoFinal_zipj_electrons - ramoInit_zipj);
          double eHitRamo_zipj_holes = -1. * energy_per_step * (ramoFinal_zipj_holes     - ramoInit_zipj);
          double eHitRamo_zizj_electrons =   energy_per_step * (ramoFinal_zizj_electrons - ramoInit_zizj);
          double eHitRamo_zizj_holes = -1. * energy_per_step * (ramoFinal_zizj_holes     - ramoInit_zizj);
          double eHitRamo_zimj_electrons =   energy_per_step * (ramoFinal_zimj_electrons - ramoInit_zimj);
          double eHitRamo_zimj_holes = -1. * energy_per_step * (ramoFinal_zimj_holes     - ramoInit_zimj);
          double eHitRamo_mipj_electrons =   energy_per_step * (ramoFinal_mipj_electrons - ramoInit_mipj);
          double eHitRamo_mipj_holes = -1. * energy_per_step * (ramoFinal_mipj_holes     - ramoInit_mipj);
          double eHitRamo_mizj_electrons =   energy_per_step * (ramoFinal_mizj_electrons - ramoInit_mizj);
          double eHitRamo_mizj_holes = -1. * energy_per_step * (ramoFinal_mizj_holes     - ramoInit_mizj);
          double eHitRamo_mimj_electrons =   energy_per_step * (ramoFinal_mimj_electrons - ramoInit_mimj);
          double eHitRamo_mimj_holes = -1. * energy_per_step * (ramoFinal_mimj_holes     - ramoInit_mimj);

          if(doChunkCorrection) {
            eHitRamo_pipj_electrons = energy_per_step * average_chargeElectron + kappa * (eHitRamo_pipj_electrons - energy_per_step * average_chargeElectron);
            eHitRamo_pipj_holes = energy_per_step * average_chargeHole + kappa *         (eHitRamo_pipj_holes - energy_per_step * average_chargeHole);
            eHitRamo_pizj_electrons = energy_per_step * average_chargeElectron + kappa * (eHitRamo_pizj_electrons - energy_per_step * average_chargeElectron);
            eHitRamo_pizj_holes = energy_per_step * average_chargeHole + kappa *         (eHitRamo_pizj_holes - energy_per_step * average_chargeHole);
            eHitRamo_pimj_electrons = energy_per_step * average_chargeElectron + kappa * (eHitRamo_pimj_electrons - energy_per_step * average_chargeElectron);
            eHitRamo_pimj_holes = energy_per_step * average_chargeHole + kappa *         (eHitRamo_pimj_holes - energy_per_step * average_chargeHole);
            eHitRamo_zipj_electrons = energy_per_step * average_chargeElectron + kappa * (eHitRamo_zipj_electrons - energy_per_step * average_chargeElectron);
            eHitRamo_zipj_holes = energy_per_step * average_chargeHole + kappa *         (eHitRamo_zipj_holes - energy_per_step * average_chargeHole);
            eHitRamo_zizj_electrons = energy_per_step * average_chargeElectron + kappa * (eHitRamo_zizj_electrons - energy_per_step * average_chargeElectron);
            eHitRamo_zizj_holes = energy_per_step * average_chargeHole + kappa *         (eHitRamo_zizj_holes - energy_per_step * average_chargeHole);
            eHitRamo_zimj_electrons = energy_per_step * average_chargeElectron + kappa * (eHitRamo_zimj_electrons - energy_per_step * average_chargeElectron);
            eHitRamo_pimj_holes = energy_per_step * average_chargeHole + kappa *         (eHitRamo_pimj_holes - energy_per_step * average_chargeHole);
            eHitRamo_mipj_electrons = energy_per_step * average_chargeElectron + kappa * (eHitRamo_mipj_electrons - energy_per_step * average_chargeElectron);
            eHitRamo_mipj_holes = energy_per_step * average_chargeHole + kappa *         (eHitRamo_mipj_holes - energy_per_step * average_chargeHole);
            eHitRamo_mizj_electrons = energy_per_step * average_chargeElectron + kappa * (eHitRamo_mizj_electrons - energy_per_step * average_chargeElectron);
            eHitRamo_mizj_holes = energy_per_step * average_chargeHole + kappa *         (eHitRamo_mizj_holes - energy_per_step * average_chargeHole);
            eHitRamo_mimj_electrons = energy_per_step * average_chargeElectron + kappa * (eHitRamo_mimj_electrons - energy_per_step * average_chargeElectron);
            eHitRamo_mimj_holes = energy_per_step * average_chargeHole + kappa *         (eHitRamo_mimj_holes - energy_per_step * average_chargeHole);
	  }

          // -- pixel coordinates --> module coordinates
          //This was: double x_mod = x_pix + pixel_size_x * extraNPixX - 0.5*module_size_x + i*pixel_size_x;
	  //This was: double y_mod = y_pix + pixel_size_y * extraNPixY - 0.5*module_size_y + j*pixel_size_y;
          double x_mod_pi_electrons = x_pix + pixel_size_x * extraNPixXElectron - 0.5*module_size_x + pixel_size_x;
          double y_mod_pj_electrons = y_pix + pixel_size_y * extraNPixYElectron - 0.5*module_size_y + pixel_size_y;
          double x_mod_pi_holes =     x_pix + pixel_size_x * extraNPixXHole     - 0.5*module_size_x + pixel_size_x;
          double y_mod_pj_holes =     y_pix + pixel_size_y * extraNPixYHole     - 0.5*module_size_y + pixel_size_y;
          double x_mod_zi_electrons = x_pix + pixel_size_x * extraNPixXElectron - 0.5*module_size_x;
          double y_mod_zj_electrons = y_pix + pixel_size_y * extraNPixYElectron - 0.5*module_size_y;
          double x_mod_zi_holes =     x_pix + pixel_size_x * extraNPixXHole     - 0.5*module_size_x;
          double y_mod_zj_holes =     y_pix + pixel_size_y * extraNPixYHole     - 0.5*module_size_y;
          double x_mod_mi_electrons = x_pix + pixel_size_x * extraNPixXElectron - 0.5*module_size_x - pixel_size_x;
          double y_mod_mj_electrons = y_pix + pixel_size_y * extraNPixYElectron - 0.5*module_size_y - pixel_size_y;
          double x_mod_mi_holes =     x_pix + pixel_size_x * extraNPixXHole     - 0.5*module_size_x - pixel_size_x;
          double y_mod_mj_holes =     y_pix + pixel_size_y * extraNPixYHole     - 0.5*module_size_y - pixel_size_y;

	  //This was: const SiLocalPosition& chargePos = Module.hitLocalToLocal(y_mod, x_mod) (for whatever reason half the maps x/y are inverted...)
          const SiLocalPosition& chargePos_pipj_electrons = Module.hitLocalToLocal(y_mod_pj_electrons, x_mod_pi_electrons);
          const SiLocalPosition& chargePos_pizj_electrons = Module.hitLocalToLocal(y_mod_zj_electrons, x_mod_pi_electrons);
          const SiLocalPosition& chargePos_pimj_electrons = Module.hitLocalToLocal(y_mod_mj_electrons, x_mod_pi_electrons);
          const SiLocalPosition& chargePos_zipj_electrons = Module.hitLocalToLocal(y_mod_pj_electrons, x_mod_zi_electrons);
          const SiLocalPosition& chargePos_zizj_electrons = Module.hitLocalToLocal(y_mod_zj_electrons, x_mod_zi_electrons);
          const SiLocalPosition& chargePos_zimj_electrons = Module.hitLocalToLocal(y_mod_mj_electrons, x_mod_zi_electrons);
          const SiLocalPosition& chargePos_mipj_electrons = Module.hitLocalToLocal(y_mod_pj_electrons, x_mod_mi_electrons);
          const SiLocalPosition& chargePos_mizj_electrons = Module.hitLocalToLocal(y_mod_zj_electrons, x_mod_mi_electrons);
          const SiLocalPosition& chargePos_mimj_electrons = Module.hitLocalToLocal(y_mod_mj_electrons, x_mod_mi_electrons);

          const SiLocalPosition& chargePos_pipj_holes = Module.hitLocalToLocal(y_mod_pj_holes, x_mod_pi_holes);
          const SiLocalPosition& chargePos_pizj_holes = Module.hitLocalToLocal(y_mod_zj_holes, x_mod_pi_holes);
          const SiLocalPosition& chargePos_pimj_holes = Module.hitLocalToLocal(y_mod_mj_holes, x_mod_pi_holes);
          const SiLocalPosition& chargePos_zipj_holes = Module.hitLocalToLocal(y_mod_pj_holes, x_mod_zi_holes);
          const SiLocalPosition& chargePos_zizj_holes = Module.hitLocalToLocal(y_mod_zj_holes, x_mod_zi_holes);
          const SiLocalPosition& chargePos_zimj_holes = Module.hitLocalToLocal(y_mod_mj_holes, x_mod_zi_holes);
          const SiLocalPosition& chargePos_mipj_holes = Module.hitLocalToLocal(y_mod_pj_holes, x_mod_mi_holes);
          const SiLocalPosition& chargePos_mizj_holes = Module.hitLocalToLocal(y_mod_zj_holes, x_mod_mi_holes);
          const SiLocalPosition& chargePos_mimj_holes = Module.hitLocalToLocal(y_mod_mj_holes, x_mod_mi_holes);

	  //This was: const SiSurfaceCharge scharge(chargePos, SiCharge(eHitRamo*eleholePairEnergy, hit_time, SiCharge::track, particleLink));
	  const SiSurfaceCharge scharge_pipj_electrons(chargePos_pipj_electrons, SiCharge(eHitRamo_pipj_electrons*eleholePairEnergy, hit_time, SiCharge::track, mc_particle_link));
	  const SiSurfaceCharge scharge_pizj_electrons(chargePos_pizj_electrons, SiCharge(eHitRamo_pizj_electrons*eleholePairEnergy, hit_time, SiCharge::track, mc_particle_link));
	  const SiSurfaceCharge scharge_pimj_electrons(chargePos_pimj_electrons, SiCharge(eHitRamo_pimj_electrons*eleholePairEnergy, hit_time, SiCharge::track, mc_particle_link));
	  const SiSurfaceCharge scharge_zipj_electrons(chargePos_zipj_electrons, SiCharge(eHitRamo_zipj_electrons*eleholePairEnergy, hit_time, SiCharge::track, mc_particle_link));
	  const SiSurfaceCharge scharge_zizj_electrons(chargePos_zizj_electrons, SiCharge(eHitRamo_zizj_electrons*eleholePairEnergy, hit_time, SiCharge::track, mc_particle_link));
	  const SiSurfaceCharge scharge_zimj_electrons(chargePos_zimj_electrons, SiCharge(eHitRamo_zimj_electrons*eleholePairEnergy, hit_time, SiCharge::track, mc_particle_link));
	  const SiSurfaceCharge scharge_mipj_electrons(chargePos_mipj_electrons, SiCharge(eHitRamo_mipj_electrons*eleholePairEnergy, hit_time, SiCharge::track, mc_particle_link));
	  const SiSurfaceCharge scharge_mizj_electrons(chargePos_mizj_electrons, SiCharge(eHitRamo_mizj_electrons*eleholePairEnergy, hit_time, SiCharge::track, mc_particle_link));
	  const SiSurfaceCharge scharge_mimj_electrons(chargePos_mimj_electrons, SiCharge(eHitRamo_mimj_electrons*eleholePairEnergy, hit_time, SiCharge::track, mc_particle_link));

	  const SiSurfaceCharge scharge_pipj_holes(chargePos_pipj_holes, SiCharge(eHitRamo_pipj_holes*eleholePairEnergy, hit_time, SiCharge::track, mc_particle_link));
	  const SiSurfaceCharge scharge_pizj_holes(chargePos_pizj_holes, SiCharge(eHitRamo_pizj_holes*eleholePairEnergy, hit_time, SiCharge::track, mc_particle_link));
	  const SiSurfaceCharge scharge_pimj_holes(chargePos_pimj_holes, SiCharge(eHitRamo_pimj_holes*eleholePairEnergy, hit_time, SiCharge::track, mc_particle_link));
	  const SiSurfaceCharge scharge_zipj_holes(chargePos_zipj_holes, SiCharge(eHitRamo_zipj_holes*eleholePairEnergy, hit_time, SiCharge::track, mc_particle_link));
	  const SiSurfaceCharge scharge_zizj_holes(chargePos_zizj_holes, SiCharge(eHitRamo_zizj_holes*eleholePairEnergy, hit_time, SiCharge::track, mc_particle_link));
	  const SiSurfaceCharge scharge_zimj_holes(chargePos_zimj_holes, SiCharge(eHitRamo_zimj_holes*eleholePairEnergy, hit_time, SiCharge::track, mc_particle_link));
	  const SiSurfaceCharge scharge_mipj_holes(chargePos_mipj_holes, SiCharge(eHitRamo_mipj_holes*eleholePairEnergy, hit_time, SiCharge::track, mc_particle_link));
	  const SiSurfaceCharge scharge_mizj_holes(chargePos_mizj_holes, SiCharge(eHitRamo_mizj_holes*eleholePairEnergy, hit_time, SiCharge::track, mc_particle_link));
	  const SiSurfaceCharge scharge_mimj_holes(chargePos_mimj_holes, SiCharge(eHitRamo_mimj_holes*eleholePairEnergy, hit_time, SiCharge::track, mc_particle_link));

	  auto addCharge = [&Module, &chargedDiodes](SiSurfaceCharge const & scharge) {
            const SiCellId& diode = Module.cellIdOfPosition(scharge.position());
	    if (diode.isValid()) {
              const SiCharge& charge = scharge.charge();
              chargedDiodes.add(diode, charge);
            }	
	  };
 
	  addCharge(scharge_pipj_electrons);
	  addCharge(scharge_pizj_electrons);
	  addCharge(scharge_pimj_electrons);
	  addCharge(scharge_zipj_electrons);
	  addCharge(scharge_zizj_electrons);
	  addCharge(scharge_zimj_electrons);
	  addCharge(scharge_mipj_electrons);
	  addCharge(scharge_mizj_electrons);
	  addCharge(scharge_mimj_electrons);
	  addCharge(scharge_pipj_holes);
	  addCharge(scharge_pizj_holes);
	  addCharge(scharge_pimj_holes);
	  addCharge(scharge_zipj_holes);
	  addCharge(scharge_zizj_holes);
	  addCharge(scharge_zimj_holes);
	  addCharge(scharge_mipj_holes);
	  addCharge(scharge_mizj_holes);
	  addCharge(scharge_mimj_holes);
      }
    }
  } else {
    //**************************************//
    //*** Now diffuse charges to surface *** //
    //**************************************//
    for (unsigned int istep = 0; istep < trfHitRecord.size(); istep++) {
      std::pair<double, double> iHitRecord = trfHitRecord[istep];

      double eta_i = eta_0;
      double phi_i = phi_0;
      double depth_i = depth_0;

      if (iTotalLength) {
        eta_i += 1.0 * iHitRecord.first / iTotalLength * dEta;
        phi_i += 1.0 * iHitRecord.first / iTotalLength * dPhi;
        depth_i += 1.0 * iHitRecord.first / iTotalLength * dDepth;
      }

      double es_current = 1.0 * iHitRecord.second / 1.E+6;

      double dist_electrode = 0.5 * sensorThickness - Module.design().readoutSide() * depth_i;
      if (dist_electrode < 0) dist_electrode = 0;

      CLHEP::Hep3Vector chargepos;
      chargepos.setX(phi_i);
      chargepos.setY(eta_i);
      chargepos.setZ(dist_electrode);

      bool coord = Module.isModuleFrame();

      ATH_MSG_DEBUG(
        "ismoduleframe " << coord << " -- startPosition (x,y,z) = " << chargepos.x() << ", " << chargepos.y() << ", " <<
          chargepos.z());

      // -- change origin of coordinates to the left bottom of module
      double x_new = chargepos.x() + 0.5*module_size_x;
      double y_new = chargepos.y() + 0.5*module_size_y;

      // -- change from module frame to pixel frame
      int nPixX = int(x_new / pixel_size_x);
      int nPixY = int(y_new / pixel_size_y);
      ATH_MSG_DEBUG(" -- nPixX = " << nPixX << "  nPixY = " << nPixY);
      double x_pix = x_new - pixel_size_x * (nPixX);
      double y_pix = y_new - pixel_size_y * (nPixY);
      // -- change origin of coordinates to the center of the pixel
      double x_pix_center = x_pix - pixel_size_x / 2;
      double y_pix_center = y_pix - pixel_size_y / 2;
      ATH_MSG_DEBUG(" -- current hit position w.r.t. pixel center = " << x_pix_center << "  " << y_pix_center);

      double x_neighbor;
      double y_neighbor;
      CLHEP::Hep3Vector pos_neighbor;
      // -- Calculate signal in current pixel and in the neighboring ones
      // -- loop in the x-coordinate
      for (int i = -1; i <= 1; i++) {
        x_neighbor = x_pix_center - i * pixel_size_x;
        // -- loop in the y-coordinate
        for (int j = -1; j <= 1; j++) {
          y_neighbor = y_pix_center - j * pixel_size_y;

          // -- check if the neighbor falls inside the charge collection prob map window
          if ((std::abs(x_neighbor) < pixel_size_x) && (std::abs(y_neighbor) < pixel_size_y)) {
            // -- change origin of coordinates to the bottom left of the charge
            //    collection prob map "window", i.e. shift of 1-pixel twd bottom left
            double x_neighbor_map = x_neighbor + pixel_size_x;
            double y_neighbor_map = y_neighbor + pixel_size_y;

            int x_bin_cc_map = x_neighbor_map / x_bin_size;
            int y_bin_cc_map = y_neighbor_map / y_bin_size;

            // -- retrieve the charge collection probability from Svc
            // -- swap x and y bins to match Map coord convention
            double ccprob_neighbor = getProbMapEntry(InDetDD::PixelReadoutTechnology::FEI4, y_bin_cc_map, x_bin_cc_map);
            if (ccprob_neighbor == -1.) return StatusCode::FAILURE;

            double ed = es_current * eleholePairEnergy * ccprob_neighbor;

            // -- pixel coordinates --> module coordinates
            double x_mod = x_neighbor + 0.5*pixel_size_x + pixel_size_x * nPixX - 0.5*module_size_x;
            double y_mod = y_neighbor + 0.5*pixel_size_y + pixel_size_y * nPixY - 0.5*module_size_y;
            const SiLocalPosition& chargePos = Module.hitLocalToLocal(y_mod, x_mod);

            const SiSurfaceCharge scharge(chargePos,
                                          SiCharge(ed, pHitTime, SiCharge::track, particleLink));
            const SiCellId& diode = Module.cellIdOfPosition(scharge.position());
            if (diode.isValid()) {
              const SiCharge& charge = scharge.charge();
              chargedDiodes.add(diode, charge);
            }
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

// read the Charge Collection Prob Map from text file
StatusCode SensorSim3DTool::readProbMap(const std::string& fileE) {
  std::string line;
  const std::string& fileName = fileE;
  std::string inputFile = PathResolverFindCalibFile(fileName);
  if (inputFile.empty()) {
    ATH_MSG_ERROR("Could not open input file!!!!!");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("opening file " << inputFile);
  std::ifstream myfile(inputFile.c_str());
  if (myfile.is_open()) {
    ATH_MSG_DEBUG(" opened file!");
    while (myfile.good()) {
      while (std::getline(myfile, line)) {
        std::istringstream sline(line);
        int xpos, ypos;
        double prob;
        sline >> xpos >> ypos >> prob;
        if (fileName.find("FEI4") != std::string::npos) {
          m_probMapFEI4.insert(std::make_pair(std::make_pair(xpos, ypos), prob));
          ATH_MSG_DEBUG("FEI4 inside xpos  " << xpos << "   ypos  " << ypos << "    prob  " << prob);
        } else if (fileName.find("FEI3") != std::string::npos) {
          m_probMapFEI3.insert(std::make_pair(std::make_pair(xpos, ypos), prob));
          ATH_MSG_DEBUG("FEI3 inside xpos  " << xpos << "   ypos  " << ypos << "    prob  " << prob);
        } else {
          ATH_MSG_ERROR("Please check name of Charge Coll Prob Maps! (should contain FEI3 or FEI4) ");
          return StatusCode::FAILURE;
        }
      }
    }
    myfile.close();
  }
  return StatusCode::SUCCESS;
}

// -- Print out the Charge Collection Probability map (full map)
StatusCode SensorSim3DTool::printProbMap(const std::string& readout) const {
  if (readout == "FEI4") {
    for (const auto& it : m_probMapFEI4) {
      ATH_MSG_DEBUG(
        "read full probMap  FEI4 --- bin x " << it.first.first << "   bin y  " << it.first.second << "    prob  " <<
          it.second);
    }
  } else if (readout == "FEI3") {
    for (const auto& it : m_probMapFEI3) {
      ATH_MSG_DEBUG(
        "read full probMap  FEI3 --- bin x " << it.first.first << "   bin y  " << it.first.second << "    prob  " <<
          it.second);
    }
  } else {
    ATH_MSG_ERROR("Error in printout Charge Coll Prob Maps! (readout should contain FEI3 or FEI4 strings) ");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// -- Returns the Charge Collection Probability at a given point (bin_x,bin_y)
double SensorSim3DTool::getProbMapEntry(const InDetDD::PixelReadoutTechnology &readout, int binx, int biny) const {
  std::pair<int, int> doublekey(binx, biny);
  double echarge;
  if (readout == InDetDD::PixelReadoutTechnology::FEI4) {
    std::multimap<std::pair<int, int>, double>::const_iterator iter = m_probMapFEI4.find(doublekey);
    echarge = iter->second;
  } else if (readout == InDetDD::PixelReadoutTechnology::FEI3) {
    std::multimap<std::pair<int, int>, double>::const_iterator iter = m_probMapFEI3.find(doublekey);
    echarge = iter->second;
  } else {
    ATH_MSG_ERROR("No Map Entry available for the requested readout");
    echarge = -1.;
  }
  return echarge;
}

double SensorSim3DTool::getMobility(double electricField, bool isHoleBit) {
  //Not exactly the same as the getMobility function in RadDamageUtil, since we don't have a Hall effect for 3D sensors
  // (B and E are parallel)
  //Maybe good to do something about this in the future

  double vsat = 0;
  double ecrit = 0;
  double beta = 0;

  //These parameterizations come from C. Jacoboni et al., Solid-State Electronics 20 (1977) 77-89. (see also
  // https://cds.cern.ch/record/684187/files/indet-2001-004.pdf).

  if (isHoleBit) {
    vsat = 1.62 * std::pow(m_temperature, -0.52); // mm/ns
    ecrit = 1.24E-7 * std::pow(m_temperature, 1.68); // MV/mm
    beta = 0.46 * std::pow(m_temperature, 0.17);
  } else {
    vsat = 15.3 * std::pow(m_temperature, -0.87); // mm/ns
    ecrit = 1.01E-7 * std::pow(m_temperature, 1.55); // MV/mm
    beta = 2.57E-2 * std::pow(m_temperature, 0.66);
  }

  double mobility = (vsat / ecrit) / std::pow(1 + std::pow((electricField / ecrit), beta), (1 / beta));
  return mobility; // mm^2/(MV*ns)
}

std::vector<double> SensorSim3DTool::getDriftTime(bool isHoleBit, size_t n) {
  std::vector<double> rand (n, 0.);
  std::vector<double> result (n, 0.);
  CLHEP::RandFlat::shootArray(n, rand.data(), 0., 1.);
  for(size_t i = 0; i < n; i++) {
    if (isHoleBit) {
      result[i]= (-1.) * m_trappingTimeHoles * logf(rand[i]); // ns
    } else {
      result[i] = (-1.) * m_trappingTimeElectrons * logf(rand[i]); // ns
    }
  }
  return result;
}



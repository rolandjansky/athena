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

#include "TFile.h"

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

  std::vector<std::string> mapsPath_list;

  if (!m_doRadDamage) {
    m_fluence = 0;
  }

  if (static_cast<int>(m_fluence) == 1) {
    mapsPath_list.push_back(PathResolverFindCalibFile("PixelDigitization/TCAD_IBL_3Dsensors_efields/phi_0_20V.root"));
    m_fluence_layers.push_back(1e-10);
  } else if (static_cast<int>(m_fluence) == 2) {
    mapsPath_list.push_back(PathResolverFindCalibFile("PixelDigitization/TCAD_IBL_3Dsensors_efields/phi_1e14_20V.root"));
    m_fluence_layers.push_back(1e14);
  } else if (static_cast<int>(m_fluence) == 3) {
    mapsPath_list.push_back(PathResolverFindCalibFile("PixelDigitization/TCAD_IBL_3Dsensors_efields/phi_2e14_30V.root"));
    m_fluence_layers.push_back(2e14);
  } else if (static_cast<int>(m_fluence) == 4) {
    mapsPath_list.push_back(PathResolverFindCalibFile("PixelDigitization/TCAD_IBL_3Dsensors_efields/phi_5e14_40V.root"));
    m_fluence_layers.push_back(5e14);
  } else if (static_cast<int>(m_fluence) == 5) {
    mapsPath_list.push_back(PathResolverFindCalibFile("PixelDigitization/TCAD_IBL_3Dsensors_efields/phi_1e15_50V.root"));
    m_fluence_layers.push_back(1e15);
  } else if (static_cast<int>(m_fluence) == 6) {
    mapsPath_list.push_back(PathResolverFindCalibFile("PixelDigitization/TCAD_IBL_3Dsensors_efields/phi_5e15_160V.root"));
    m_fluence_layers.push_back(5e15);
  } else if (static_cast<int>(m_fluence) == 7) {
    mapsPath_list.push_back(PathResolverFindCalibFile(
                              "PixelDigitization/TCAD_IBL_3Dsensors_efields/phi_6e15_190V_new.root"));
    m_fluence_layers.push_back(6e15);
  } else if (static_cast<int>(m_fluence) == 8) {
    mapsPath_list.push_back(PathResolverFindCalibFile(
                              "PixelDigitization/TCAD_IBL_3Dsensors_efields/phi_1e16_260V_new.root"));
    m_fluence_layers.push_back(1e16);
  }

  // *****************************
  // *** Setup Maps ****
  // *****************************
  //TODO This is only temporary until remotely stored maps and locally generated maps can be implemented

  for (unsigned int i = 0; i < mapsPath_list.size(); i++) {
    ATH_MSG_INFO("Using maps located in: " << mapsPath_list.at(i));
    std::unique_ptr<TFile> mapsFile(TFile::Open((mapsPath_list.at(i)).c_str(), "READ")); //this is the ramo potential.
    if (!mapsFile) {
        ATH_MSG_ERROR("Cannot open file: " << mapsPath_list.at(i));
        return StatusCode::FAILURE;
    }

    std::pair < int, int > Layer; // index for layer/end cap position
    Layer.first = 0; //Barrel (0) or End Cap (1)   -    Now only for Barrel. If we want to add End Caps, put them at iayer.first=1
    Layer.second = i; //Layer: 0 = IBL Planar, 1=B-Layer, 2=Layer-1, 3=Layer-2
    //For 3D sensor, only possible index should be 0-0
    //May want to be changed in the future, since there's no point having an index with only one possible value

    //Setup ramo weighting field map
    std::unique_ptr<TH2F> ramoPotentialMap_hold(mapsFile->Get<TH2F>("ramo"));
    if (!ramoPotentialMap_hold) {
      ATH_MSG_ERROR("Did not find a Ramo potential map.  Will use an approximate form.");
      return StatusCode::FAILURE; //Obviously, remove this when gen. code is set up
    }
    ramoPotentialMap_hold->SetDirectory(nullptr);
    m_ramoPotentialMap.emplace_back();
    ATH_CHECK(m_ramoPotentialMap.back().setHisto2D(ramoPotentialMap_hold.get()));

    ATH_MSG_INFO("Using fluence " << m_fluence_layers.at(i));

    //Now setup the E-field.
    std::unique_ptr<TH2F> eFieldMap_hold(mapsFile->Get<TH2F>("efield"));
    if (!eFieldMap_hold) {
      ATH_MSG_ERROR("Unable to load sensor e-field map, so generating one using approximations.");
      return StatusCode::FAILURE; //Obviously, remove this when gen. code is set up
    }
    eFieldMap_hold->SetDirectory(nullptr);
    m_eFieldMap.emplace_back();
    ATH_CHECK(m_eFieldMap.back().setHisto2D(eFieldMap_hold.get()));

    std::unique_ptr<TH3F> xPositionMap_e_hold(mapsFile->Get<TH3F>("xPosition_e"));
    std::unique_ptr<TH3F> xPositionMap_h_hold(mapsFile->Get<TH3F>("xPosition_h"));
    std::unique_ptr<TH3F> yPositionMap_e_hold(mapsFile->Get<TH3F>("yPosition_e"));
    std::unique_ptr<TH3F> yPositionMap_h_hold(mapsFile->Get<TH3F>("yPosition_h"));
    std::unique_ptr<TH2F> timeMap_e_hold(mapsFile->Get<TH2F>("etimes"));
    std::unique_ptr<TH2F> timeMap_h_hold(mapsFile->Get<TH2F>("htimes"));

    if (!xPositionMap_e_hold || !xPositionMap_h_hold || !yPositionMap_e_hold || !yPositionMap_h_hold ||
        !timeMap_e_hold || !timeMap_h_hold) {
      ATH_MSG_ERROR("Cannot find one of the maps.");
      return StatusCode::FAILURE; //Obviously, remove this when gen. code is set up
    }

    xPositionMap_e_hold->SetDirectory(nullptr);
    xPositionMap_h_hold->SetDirectory(nullptr);
    yPositionMap_e_hold->SetDirectory(nullptr);
    yPositionMap_h_hold->SetDirectory(nullptr);
    timeMap_e_hold->SetDirectory(nullptr);
    timeMap_h_hold->SetDirectory(nullptr);

    //Now, determine the time to reach the electrode and the trapping position.
    m_xPositionMap_e.emplace_back();
    m_xPositionMap_h.emplace_back();
    m_yPositionMap_e.emplace_back();
    m_yPositionMap_h.emplace_back();
    ATH_CHECK(m_xPositionMap_e.back().setHisto3D(xPositionMap_e_hold.get()));
    ATH_CHECK(m_xPositionMap_h.back().setHisto3D(xPositionMap_h_hold.get()));
    ATH_CHECK(m_yPositionMap_e.back().setHisto3D(yPositionMap_e_hold.get()));
    ATH_CHECK(m_yPositionMap_h.back().setHisto3D(yPositionMap_h_hold.get()));
    m_timeMap_e.emplace_back();
    m_timeMap_h.emplace_back();
    ATH_CHECK(m_timeMap_e.back().setHisto2D(timeMap_e_hold.get()));
    ATH_CHECK(m_timeMap_h.back().setHisto2D(timeMap_h_hold.get()));

    std::unique_ptr<TH2F> avgCharge_e(mapsFile->Get<TH2F>("avgCharge_e"));
    std::unique_ptr<TH2F> avgCharge_h(mapsFile->Get<TH2F>("avgCharge_h"));

    if (!avgCharge_e || !avgCharge_h) {
      ATH_MSG_ERROR("Cannot find one of the charge maps.");
      return StatusCode::FAILURE; //Obviously, remove this when gen. code is set up
    }

    avgCharge_e->SetDirectory(nullptr);
    avgCharge_h->SetDirectory(nullptr);

    // Get average charge data (for charge chunk effect correction)
    ATH_CHECK(m_avgChargeMap_e.setHisto2D(avgCharge_e.get()));
    ATH_CHECK(m_avgChargeMap_h.setHisto2D(avgCharge_h.get()));

    mapsFile->Close();
  }

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
                                         const PixelModuleData */*moduleData*/,
                                         std::vector< std::pair<double, double> >& trfHitRecord,
                                         std::vector<double>& initialConditions,
                                         CLHEP::HepRandomEngine* rndmEngine,
                                         const EventContext &ctx) {
  if (!Module.isBarrel()) {
    return StatusCode::SUCCESS;
  }
  if (p_design.getReadoutTechnology() != InDetDD::PixelModuleDesign::FEI4) {
    return StatusCode::SUCCESS;
  }
  if (p_design.numberOfCircuits() > 1) {
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG("Applying SensorSim3D charge processor");
  if (initialConditions.size() != 8) {
    ATH_MSG_INFO("ERROR! Starting coordinates were not filled correctly in EnergyDepositionSvc.");
    return StatusCode::FAILURE;
  }

  //Calculate trapping times based on fluence (already includes check for fluence=0)
  if (m_doRadDamage) {
    std::pair < double, double > trappingTimes = m_radDamageUtil->getTrappingTimes(m_fluence_layers.at(0));   //0 = IBL
    m_trappingTimeElectrons = trappingTimes.first;
    m_trappingTimeHoles = trappingTimes.second;
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
  if (m_doRadDamage && m_fluence > 0) {
    //**************************************//
    //*** Now diffuse charges to surface *** //
    //**************************************//
    for (unsigned int istep = 0; istep < trfHitRecord.size(); istep++) {
      std::pair < double, double > iHitRecord = trfHitRecord[istep];

      double eta_i = eta_0;
      double phi_i = phi_0;
      double depth_i = depth_0;

      if (iTotalLength) {
        eta_i += 1.0 * iHitRecord.first / iTotalLength * dEta;
        phi_i += 1.0 * iHitRecord.first / iTotalLength * dPhi;
        depth_i += 1.0 * iHitRecord.first / iTotalLength * dDepth;
      }

      double energy_per_step = 1.0 * iHitRecord.second / 1.E+6 / ncharges;    //in MeV
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
      // In case the charge moves into a neighboring pixel
      int extraNPixX = nPixX;
      int extraNPixY = nPixY;

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
      double efield = getElectricField(y_pix, x_pix);
      if (efield == 0) {
        ATH_MSG_DEBUG("Skipping since efield = 0 for x_pix = " << x_pix << " y_pix = " << y_pix);
        continue;
      }

      const double mobilityElectron = getMobility(efield, false);
      const double mobilityHole     = getMobility(efield, true);

      //Loop over charge-carrier pairs
      for (int j = 0; j < ncharges; j++) {
        double eHit = energy_per_step;
        //Need to determine how many elementary charges this charge chunk represents.
        double chunk_size = energy_per_step * eleholePairEnergy; //number of electrons/holes

        //set minimum limit to prevent dividing into smaller subcharges than one fundamental charge
        if (chunk_size < 1) chunk_size = 1;
        double kappa = 1. / std::sqrt(chunk_size);

        // Loop over everything twice: once for electrons and once for holes
        for (int eholes = 0; eholes < 2; eholes++) {
          const bool isHole = (eholes == 1); // Set a condition to keep track of electron/hole-specific functions

          // Reset extraPixel coordinates each time through loop
          extraNPixX = nPixX;
          extraNPixY = nPixY;

          const double timeToElectrode = getTimeToElectrode(y_pix, x_pix, isHole);
          const double driftTime = getDriftTime(isHole);

          //Apply drift due to diffusion
          const double phiRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);

          //Apply diffusion. rdif is teh max. diffusion
          const double Dt = (isHole ? mobilityHole : mobilityElectron) * (0.024) * std::min(driftTime, timeToElectrode) * m_temperature / 273.;
          const double rdif = 1e-3*std::sqrt(Dt); //in mm
          double xposDiff = x_pix + rdif * phiRand;
          const double etaRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
          double yposDiff = y_pix + rdif * etaRand;

          // Account for drifting into another pixel
          while (xposDiff > pixel_size_x) {
            extraNPixX = extraNPixX + 1;               // increments or decrements pixel count in x
            xposDiff = xposDiff - pixel_size_x;        // moves xpos coordinate 1 pixel over in x
          }
          while (xposDiff < 0) {
            extraNPixX = extraNPixX - 1;
            xposDiff = xposDiff + pixel_size_x;
          }
          while (yposDiff > pixel_size_y) {
            extraNPixY = extraNPixY + 1;               // increments or decrements pixel count in y
            yposDiff = yposDiff - pixel_size_y;        // moves xpos coordinate 1 pixel over in y
          }
          while (yposDiff < 0) {
            extraNPixY = extraNPixY - 1;
            yposDiff = yposDiff + pixel_size_y;
          }


          float average_charge = isHole ? m_avgChargeMap_h.getContent(m_avgChargeMap_h.getBinY(1e3*y_pix), m_avgChargeMap_h.getBinX(1e3*x_pix)) :
                                          m_avgChargeMap_e.getContent(m_avgChargeMap_e.getBinY(1e3*y_pix), m_avgChargeMap_e.getBinX(1e3*x_pix));

          double xposFinal = getTrappingPositionY(yposDiff, xposDiff, std::min(driftTime, timeToElectrode), isHole);
          double yposFinal = getTrappingPositionX(yposDiff, xposDiff, std::min(driftTime, timeToElectrode), isHole);

          // -- Calculate signal in current pixel and in the neighboring ones
          // -- loop in the x-coordinate
          for (int i = -1; i <= 1; i++) {
            double xNeighbor = i * pixel_size_x;
            // -- loop in the y-coordinate
            const std::size_t index = 0;
            const std::size_t ramo_init_bin_y  = m_ramoPotentialMap[index].getBinY(1000*(x_pix + pixel_size_x * 3 - xNeighbor));
            const std::size_t ramo_final_bin_y = m_ramoPotentialMap[index].getBinY(1000*(xposFinal + pixel_size_x * 3 - xNeighbor));
            for (int j = -1; j <= 1; j++) {
              double yNeighbor = j * pixel_size_y;

              //Ramo map over 500umx350um pixel area
              //Ramo init different if charge diffused into neighboring pixel -> change primary pixel!!
              float ramoInit  = m_ramoPotentialMap[index].getContent(m_ramoPotentialMap[index].getBinX(1000*(y_pix + 0.5*pixel_size_y - yNeighbor)), ramo_init_bin_y);
              float ramoFinal = m_ramoPotentialMap[index].getContent(m_ramoPotentialMap[index].getBinX(1000*(yposFinal + 0.5*pixel_size_y - yNeighbor)), ramo_final_bin_y);

              // Record deposit
              double eHitRamo = (isHole ? -1. : 1.) * eHit * (ramoFinal - ramoInit);

              if (m_doChunkCorrection) {
                eHitRamo = eHit * average_charge + kappa * (eHitRamo - eHit * average_charge);
              }

              double induced_charge = eHitRamo * eleholePairEnergy;

              // -- pixel coordinates --> module coordinates
              double x_mod = x_pix + xNeighbor + pixel_size_x * extraNPixX - 0.5*module_size_x;
              double y_mod = y_pix + yNeighbor + pixel_size_y * extraNPixY - 0.5*module_size_y;
              const SiLocalPosition& chargePos = Module.hitLocalToLocal(y_mod, x_mod);

              const SiSurfaceCharge scharge(chargePos, SiCharge(induced_charge, hitTime(
                                                            phit), SiCharge::track, HepMcParticleLink(
                                                            phit->trackNumber(), phit.eventId(), evColl, idxFlag, ctx)));
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
            double ccprob_neighbor = getProbMapEntry(SensorType::FEI4, y_bin_cc_map, x_bin_cc_map);
            if (ccprob_neighbor == -1.) return StatusCode::FAILURE;

            double ed = es_current * eleholePairEnergy * ccprob_neighbor;

            // -- pixel coordinates --> module coordinates
            double x_mod = x_neighbor + 0.5*pixel_size_x + pixel_size_x * nPixX - 0.5*module_size_x;
            double y_mod = y_neighbor + 0.5*pixel_size_y + pixel_size_y * nPixY - 0.5*module_size_y;
            const SiLocalPosition& chargePos = Module.hitLocalToLocal(y_mod, x_mod);

            const SiSurfaceCharge scharge(chargePos, SiCharge(ed, hitTime(phit), SiCharge::track, HepMcParticleLink(
                                                          phit->trackNumber(), phit.eventId(), evColl, idxFlag, ctx)));
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
  const std::string fileName = fileE;
  std::string inputFile = PathResolverFindCalibFile(fileName);
  if (inputFile == "") {
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
double SensorSim3DTool::getProbMapEntry(const SensorType& readout, int binx, int biny) const {
  std::pair<int, int> doublekey(binx, biny);
  double echarge;
  if (readout == SensorType::FEI4) {
    std::multimap<std::pair<int, int>, double>::const_iterator iter = m_probMapFEI4.find(doublekey);
    echarge = iter->second;
  } else if (readout == SensorType::FEI3) {
    std::multimap<std::pair<int, int>, double>::const_iterator iter = m_probMapFEI3.find(doublekey);
    echarge = iter->second;
  } else {
    ATH_MSG_ERROR("No Map Entry available for the requested readout");
    echarge = -1.;
  }
  return echarge;
}

double SensorSim3DTool::getElectricField(double x, double y) {
  std::size_t index = 0;
  double electricField = m_eFieldMap[index].getContent(m_eFieldMap[index].getBinX(1e3*x), m_eFieldMap[index].getBinY(1e3*y));
  return electricField * 1.0E-7; //return efield in MV/mm (for mobility calculation)
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

double SensorSim3DTool::getDriftTime(bool isHoleBit) {
  double u = CLHEP::RandFlat::shoot(0., 1.); //
  double driftTime = 0;

  if (isHoleBit) {
    driftTime = (-1.) * m_trappingTimeHoles * std::log(u); // ns
  } else {
    driftTime = (-1.) * m_trappingTimeElectrons * std::log(u); // ns
  }
  return driftTime;
}

double SensorSim3DTool::getTimeToElectrode(double x, double y, bool isHoleBit) {
  std::size_t index = 0;
  double timeToElectrode = 0;
  if (!isHoleBit) {
    timeToElectrode = m_timeMap_e[index].getContent(m_timeMap_e[index].getBinX(1e3*x), m_timeMap_e[index].getBinY(1e3*y));
  } else {
    timeToElectrode = m_timeMap_h[index].getContent(m_timeMap_h[index].getBinX(1e3*x), m_timeMap_h[index].getBinY(1e3*y));
  }
  return timeToElectrode; //[ns]
}

double SensorSim3DTool::getTrappingPositionX(double initX, double initY, double driftTime, bool isHoleBit) {
  std::size_t index = 0;
  double finalX(0);
  if (!isHoleBit) {
    finalX = m_xPositionMap_e[index].getContent(m_xPositionMap_e[index].getBinX(1e3*initX), m_xPositionMap_e[index].getBinY(1e3*initY), m_xPositionMap_e[index].getBinZ(driftTime));
  } else {
    finalX = m_xPositionMap_h[index].getContent(m_xPositionMap_h[index].getBinX(1e3*initX), m_xPositionMap_h[index].getBinY(1e3*initY), m_xPositionMap_h[index].getBinZ(driftTime));
  }

  return finalX * 1e-3; //[mm]
}

double SensorSim3DTool::getTrappingPositionY(double initX, double initY, double driftTime, bool isHoleBit) {
  std::size_t index = 0;
  double finalY(0);
  if (!isHoleBit) {
    finalY = m_yPositionMap_e[index].getContent(m_yPositionMap_e[index].getBinX(1e3*initX), m_yPositionMap_e[index].getBinY(1e3*initY), m_yPositionMap_e[index].getBinZ(driftTime));
  } else {
    finalY = m_yPositionMap_h[index].getContent(m_yPositionMap_h[index].getBinX(1e3*initX), m_yPositionMap_h[index].getBinY(1e3*initY), m_yPositionMap_h[index].getBinZ(driftTime));
  }

  return finalY * 1e-3; //[mm]
}

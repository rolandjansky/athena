/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "SensorSimPlanarTool.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "SiDigitization/SiSurfaceCharge.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetSimEvent/SiTrackDistance.h"
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

#include <cmath>
#include <memory>

using namespace InDetDD;

//===============================================
//    C O N S T R U C T O R
//===============================================
SensorSimPlanarTool::SensorSimPlanarTool(const std::string& type, const std::string& name, const IInterface* parent) :
  SensorSimTool(type, name, parent) {

  // This is a waste in some cases, but we need at most 3x3 elements
  // Reserving 9 removes the need to allocate new memory multipe time thus speeding up the code a bit
  m_centrePixelNNEtaPhi.resize(9);
}

SensorSimPlanarTool::~SensorSimPlanarTool() { }

//===============================================
//    I N I T I A L I Z E
//===============================================
StatusCode SensorSimPlanarTool::initialize() {
  ATH_MSG_DEBUG("SensorSimPlanarTool::initialize()");
  ATH_CHECK(SensorSimTool::initialize());

  ATH_CHECK(m_radDamageUtil.retrieve());
  ATH_MSG_DEBUG("RadDamageUtil tool retrieved successfully");

  ATH_CHECK(m_lorentzAngleTool.retrieve());

  //If any fluence or voltage initialized negative use benchmark maps and not interpolation
  std::vector<std::string> mapsPath_list;
  std::vector<std::string> TCADpath_list;

  // Use all TCAD E field files in this directory for creating E field via interpolation (pruned filed excluded)
  std::string iblFiles = PathResolverFindCalibDirectory("PixelDigitization/TCAD_IBL_efields/fei4-200um/");
  std::string sensorFiles = PathResolverFindCalibDirectory("PixelDigitization/TCAD_Blayer_efields/fei4-250um/");

  // For each layer one configuration
  TCADpath_list = {
    iblFiles, sensorFiles, sensorFiles, sensorFiles
  };           //IBL - 200um sensor depth, B layer - 20um, layer 1, layer 2

  if (m_doInterpolateEfield) {
    if (m_fluenceMap.size() == 0 || m_fluenceLayer.size() == 0 || m_voltageLayer.size() == 0 ||
        m_fluenceMap.size() != m_fluenceLayer.size() || m_fluenceMap.size() != m_voltageLayer.size()) {
      ATH_MSG_INFO("Use interpolation, but the input map/fluence/valtage are not set.");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("No benchmark value set for fluence. Use interpolation.");

    mapsPath_list.clear();
    for (size_t i = 0; i < m_fluenceMap.size(); i++) {
      mapsPath_list.push_back(PathResolverFindCalibFile(m_fluenceMap[i]));
    }

    // *****************************
    // *** Setup Maps ****
    // *****************************
    //TODO This is only temporary until remotely stored maps and locally generated maps can be implemented
    //E field already implemented: needs fluence and bias voltage given as Property m_fluence, m_fluenceB, ...,
    //  m_fluence1, ...
    for (unsigned int i = 0; i < mapsPath_list.size(); i++) {
      ATH_MSG_INFO("Using maps located in: " << mapsPath_list.at(i) << " for layer No." << i);
      ATH_MSG_INFO("Create E field via interpolation based on files from: " << TCADpath_list.at(i));
      std::unique_ptr<TFile> mapsFile(TFile::Open((mapsPath_list.at(i)).c_str(), "READ")); //this is the ramo potential.
      if (!mapsFile) {
        ATH_MSG_ERROR("Cannot open file: " << mapsPath_list.at(i));
        return StatusCode::FAILURE;
      }

      //Setup ramo weighting field map
      std::unique_ptr<TH3F> ramoPotentialMap_hold(mapsFile->Get<TH3F>("hramomap1"));
      if (!ramoPotentialMap_hold) {
        ramoPotentialMap_hold.reset(mapsFile->Get<TH3F>("ramo3d"));
        ATH_MSG_INFO("Did not find a Ramo potential map.  Will use an approximate form.");
      }
      if (!ramoPotentialMap_hold) {
        ATH_MSG_WARNING("Not implemented yet - exit");
        return StatusCode::FAILURE; //Obviously, remove this when gen. code is set up
      }
      ramoPotentialMap_hold->SetDirectory(nullptr);
      m_ramoPotentialMap.emplace_back();
      ATH_CHECK(m_ramoPotentialMap.back().setHisto3D(ramoPotentialMap_hold.get()));
      //Now setup the E-field.
      TH1F* eFieldMap_hold(nullptr);
      CHECK(m_radDamageUtil->generateEfieldMap(eFieldMap_hold, nullptr, m_fluenceLayer[i], m_voltageLayer[i], i,
                                               TCADpath_list.at(i), true));

      eFieldMap_hold->SetDirectory(nullptr);

      TH2F* lorentzMap_e_hold(nullptr);
      TH2F* lorentzMap_h_hold(nullptr);
      TH2F* distanceMap_h_hold(nullptr);
      TH2F* distanceMap_e_hold(nullptr);
      TH1F* timeMap_e_hold(nullptr);
      TH1F* timeMap_h_hold(nullptr);

      ATH_CHECK(m_radDamageUtil->generateDistanceTimeMap(distanceMap_e_hold, distanceMap_h_hold, timeMap_e_hold,
                                                         timeMap_h_hold, lorentzMap_e_hold, lorentzMap_h_hold,
                                                         eFieldMap_hold, nullptr));

      // For debugging and documentation: uncomment to save different maps which are based on the interpolated E field
      if (m_radDamageUtil->saveDebugMaps()) {
        TString prename = "map_layer_";
        prename += i;
        prename += "distance_e.root";
        distanceMap_e_hold->SaveAs(prename);
        prename.ReplaceAll("_e", "_h");
        distanceMap_h_hold->SaveAs(prename);
        prename.ReplaceAll("distance", "time");
        timeMap_h_hold->SaveAs(prename);
        prename.ReplaceAll("_h", "_e");
        timeMap_e_hold->SaveAs(prename);
        prename.ReplaceAll("time", "lorentz");
        lorentzMap_e_hold->SaveAs(prename);
        prename.ReplaceAll("_e", "_h");
        lorentzMap_h_hold->SaveAs(prename);
      }
      //Safetycheck
      if (!distanceMap_e_hold || !distanceMap_h_hold || !timeMap_e_hold || !timeMap_h_hold ||
          !lorentzMap_e_hold || !lorentzMap_h_hold) {
        ATH_MSG_ERROR("Unable to load at least one of the distance/time/Lorentz angle maps.");
        return StatusCode::FAILURE;//Obviously, remove this when gen. code is set up
      }

      lorentzMap_e_hold->SetDirectory(nullptr);
      lorentzMap_h_hold->SetDirectory(nullptr);
      distanceMap_e_hold->SetDirectory(nullptr);
      distanceMap_h_hold->SetDirectory(nullptr);
      timeMap_e_hold->SetDirectory(nullptr);
      timeMap_h_hold->SetDirectory(nullptr);

      m_distanceMap_e.emplace_back();
      m_distanceMap_h.emplace_back();
      ATH_CHECK(m_distanceMap_e.back().setHisto2D(distanceMap_e_hold));
      ATH_CHECK(m_distanceMap_h.back().setHisto2D(distanceMap_h_hold));
      m_lorentzMap_e.emplace_back();
      m_lorentzMap_h.emplace_back();
      ATH_CHECK(m_lorentzMap_e.back().setHisto2D(lorentzMap_e_hold));
      ATH_CHECK(m_lorentzMap_h.back().setHisto2D(lorentzMap_h_hold));

      delete eFieldMap_hold;
      delete lorentzMap_e_hold;
      delete lorentzMap_h_hold;
      delete distanceMap_e_hold;
      delete distanceMap_h_hold;
      delete timeMap_e_hold;
      delete timeMap_h_hold;

      mapsFile->Close();
    }
  }
  return StatusCode::SUCCESS;
}

//===============================================
//    F I N A L I Z E
//===============================================
StatusCode SensorSimPlanarTool::finalize() {
  ATH_MSG_DEBUG("SensorSimPlanarTool::finalize()");
  return StatusCode::SUCCESS;
}

//===============================================
//    I N D U C E    C H A R G E
//===============================================
StatusCode SensorSimPlanarTool::induceCharge(const TimedHitPtr<SiHit>& phit,
                                             SiChargedDiodeCollection& chargedDiodes,
                                             const InDetDD::SiDetectorElement& Module,
                                             const InDetDD::PixelModuleDesign& p_design,
                                             std::vector< std::pair<double, double> >& trfHitRecord,
                                             std::vector<double>& initialConditions,
                                             CLHEP::HepRandomEngine* rndmEngine,
                                             const EventContext &ctx) {
  if (p_design.getReadoutTechnology() == InDetDD::PixelReadoutTechnology::RD53) {
    // TODO: check that detectors other than ITk have this properly set
    // if (p_design.is3D()) {
    //   return StatusCode::SUCCESS;
    // }
    // pass
    // TODO: for now all RD53 sensors are digitized as planar
  } else {
    // So far, this is only discriminating variable from 3D sensor.
    if (p_design.numberOfCircuits() < 2) {
      if (!Module.isDBM()) {  //DBM modules also processed here
        return StatusCode::SUCCESS;
      }
    }
  }

  const PixelID* p_pixelId = static_cast<const PixelID*>(Module.getIdHelper());
  int layer = p_pixelId->layer_disk(Module.identify());

  //Load values from energyDeposition
  double eta_0 = initialConditions[0];
  double phi_0 = initialConditions[1];
  double depth_0 = initialConditions[2];
  double dEta = initialConditions[3];
  double dPhi = initialConditions[4];
  double dDepth = initialConditions[5];
  double ncharges = initialConditions[6];
  double iTotalLength = initialConditions[7];

  //Set up physical detector properties, switch on detector material
  ATH_MSG_DEBUG("Applying planar sensor simulation");
  double sensorThickness = Module.design().thickness();
  const InDet::SiliconProperties& siProperties = m_siPropertiesTool->getSiProperties(Module.identifyHash(), ctx);

  // Prepare values that are needed for radiation damage corrections later
  const HepGeom::Point3D<double>& startPosition = phit->localStartPosition(); 
  const HepGeom::Point3D<double>& endPosition   = phit->localEndPosition(); 
  const float zPosDiff = endPosition.Z - startPosition.Z;
  const float tanTheta = (zPosDiff != 0.) ? (endPosition.Y - startPosition.Y) / zPosDiff : (endPosition.Y - startPosition.Y) * 9999.;
  const float tanPhi   = (zPosDiff != 0.) ? (endPosition.X - startPosition.X) / zPosDiff : (endPosition.X - startPosition.X) * 9999.;

  int etaCells = p_design.columns();
  int phiCells = p_design.rows();

  double eleholePairEnergy = 0;
  double smearRand = 0;

  if (Module.isDBM()) {
    eleholePairEnergy = 1. / (13. * CLHEP::eV); // was 3.62 eV.
    m_diffusionConstant = .00265;
    smearRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
  } else {
    eleholePairEnergy = siProperties.electronHolePairsPerEnergy();
    m_diffusionConstant = .007;
  }

  double collectionDist = 0.2 * CLHEP::mm;
  double smearScale = 1. + 0.35 * smearRand;
  double tanLorentz = m_lorentzAngleTool->getTanLorentzAngle(Module.identifyHash());
  double coLorentz = std::sqrt(1.0 + (tanLorentz*tanLorentz));

  const EBC_EVCOLL evColl = EBC_MAINEVCOLL;
  const HepMcParticleLink::PositionFlag idxFlag =
    (phit.eventId() == 0) ? HepMcParticleLink::IS_POSITION : HepMcParticleLink::IS_INDEX;

  //**************************************//
  //*** Now diffuse charges to surface *** //
  //**************************************//
  // pre-make HepMcParticleLink
  const auto particleLink = HepMcParticleLink(phit->trackNumber(), phit.eventId(), evColl, idxFlag, ctx);
  const double pHitTime = hitTime(phit);

  const double halfEtaPitch = 0.5*Module.etaPitch();
  const double halfPhiPitch = 0.5*Module.phiPitch();

  if (m_doRadDamage && !(Module.isDBM()) && Module.isBarrel() && !m_doRadDamageTemplate) {
    SG::ReadCondHandle<PixelRadiationDamageFluenceMapData> fluenceData(m_fluenceDataKey,ctx);

    std::pair<double, double> trappingTimes;
    if (m_doInterpolateEfield) {
      trappingTimes = m_radDamageUtil->getTrappingTimes(m_fluenceLayer[layer]);
    } 
    else {
      trappingTimes = m_radDamageUtil->getTrappingTimes(fluenceData->getFluenceLayer(layer));
    }

    const PixelHistoConverter& distanceMap_e    = m_doInterpolateEfield ? m_distanceMap_e[layer] : fluenceData->getDistanceMap_e(layer);
    const PixelHistoConverter& distanceMap_h    = m_doInterpolateEfield ? m_distanceMap_h[layer] : fluenceData->getDistanceMap_h(layer);
    const PixelHistoConverter& lorentzMap_e     = m_doInterpolateEfield ? m_lorentzMap_e[layer] : fluenceData->getLorentzMap_e(layer);
    const PixelHistoConverter& lorentzMap_h     = m_doInterpolateEfield ? m_lorentzMap_h[layer] : fluenceData->getLorentzMap_h(layer);
    const PixelHistoConverter& ramoPotentialMap = m_doInterpolateEfield ? m_ramoPotentialMap[layer] : fluenceData->getRamoPotentialMap(layer);

    std::map<unsigned, std::pair<SiLocalPosition, double>> cachedChargeMap; 
    std::map<unsigned, SiCellId> diodeCellMap;
    for (const auto& iHitRecord : trfHitRecord) {

      double eta_i = eta_0;
      double phi_i = phi_0;
      double depth_i = depth_0;
      if (iTotalLength) {
        eta_i += 1.0 * iHitRecord.first / iTotalLength * dEta;
        phi_i += 1.0 * iHitRecord.first / iTotalLength * dPhi;
        depth_i += 1.0 * iHitRecord.first / iTotalLength * dDepth;
      }

      //Find the position of the centre of the pixel in which the charge carriers are created, wrt centre of module
      SiLocalPosition pos_i = Module.hitLocalToLocal(eta_i, phi_i);
      SiCellId pixel_i = Module.cellIdOfPosition(pos_i);
      if (!pixel_i.isValid()) continue;

      // Distance between charge and readout side.  p_design->readoutSide() is
      // +1 if readout side is in +ve depth axis direction and visa-versa.
      double dist_electrode = 0.5 * sensorThickness - Module.design().readoutSide() * depth_i;
      if (dist_electrode < 0) {
        dist_electrode = 0;
      }

      SiLocalPosition centreOfPixel_i = p_design.positionFromColumnRow(pixel_i.etaIndex(), pixel_i.phiIndex());

      //Make limits for NN loop
      int nnLoop_pixelEtaMax = std::min(1, pixel_i.etaIndex());
      int nnLoop_pixelEtaMin = std::max(-1, pixel_i.etaIndex() + 1 - etaCells);
      int nnLoop_pixelPhiMax = std::min(1, pixel_i.phiIndex());
      int nnLoop_pixelPhiMin = std::max(-1, pixel_i.phiIndex() + 1 - phiCells);

      std::array<double, 3> sensorScales{};

      const std::size_t distance_f_e_bin_x = distanceMap_e.getBinX(dist_electrode);
      const std::size_t distance_f_h_bin_x = distanceMap_h.getBinX(dist_electrode);
      const std::size_t tanLorentz_e_bin_x = lorentzMap_e.getBinX(dist_electrode);
      const std::size_t tanLorentz_h_bin_x = lorentzMap_h.getBinX(dist_electrode);
      
      const std::size_t sizePhi = std::abs(nnLoop_pixelPhiMax - nnLoop_pixelPhiMin) + 1;

      const auto pixel_eta = pixel_i.etaIndex();
      const auto pixel_phi = pixel_i.phiIndex();

      for (int p = nnLoop_pixelEtaMin; p <= nnLoop_pixelEtaMax; p++) {
        const std::size_t ieta = p - nnLoop_pixelEtaMin;
        // scale factors accounting for different pixel sizes
        double scale_f = 1.;
        double columnWidth = p_design.widthFromColumnRange(pixel_eta - p, pixel_eta - p);
        if (std::abs(columnWidth - 0.6) < 1e-9) {
          scale_f = 4. / 6.;
        } else if (std::abs(columnWidth - 0.45) < 1e-9) {
          scale_f = 25. / 45.;
        } else if (std::abs(columnWidth - 0.5) < 1e-9) {
          scale_f = 25. / 50.;
        }
        sensorScales[ieta] = scale_f;

        for (int q = nnLoop_pixelPhiMin; q <= nnLoop_pixelPhiMax; q++) {
          const SiLocalPosition& centreOfPixel_nn = p_design.positionFromColumnRow(pixel_eta - p,
                                                                                   pixel_phi - q);
          const std::size_t iphi = q - nnLoop_pixelPhiMin;
          const std::size_t index = iphi + ieta*sizePhi;
          m_centrePixelNNEtaPhi[index].first  = centreOfPixel_nn.xEta(); 
          m_centrePixelNNEtaPhi[index].second = centreOfPixel_nn.xPhi(); 
        }
      }


      for (int j = 0; j < ncharges; j++) {
        // amount of energy to be converted into charges at current step
        double energy_per_step = 1.0 * iHitRecord.second / 1.E+6 / ncharges;
        double u = CLHEP::RandFlat::shoot(0., 1.);
        // need to update to std::logf when we update gcc - this is a known bug in gcc libc
        const double drifttime_e = (-1.) * (trappingTimes.first) * logf(u); //ns
        u = CLHEP::RandFlat::shoot(0., 1.);
        const double drifttime_h = (-1.) * (trappingTimes.second) * logf(u); //ns

        //Now, need the z-position at the trap.
        //TODO: the holes map does not currently extend for a drift time long enough that, any hole will reach
        //the corresponding electrode. This needs to be rectified by either (a) extrapolating the current map or
        //(b) making a new map with a y-axis (drift time) that extends to at least 18 ns so all charge carriers reach
        // electrode.
        //However, if choose (b), will need to reduce granularity of map.
        const double depth_f_e = distanceMap_e.getContent(distance_f_e_bin_x, distanceMap_e.getBinY(drifttime_e));
        const double depth_f_h = distanceMap_h.getContent(distance_f_h_bin_x, distanceMap_h.getBinY(drifttime_h));
        const double tanLorentz_e = lorentzMap_e.getContent(tanLorentz_e_bin_x, lorentzMap_e.getBinY(depth_f_e));
        const double tanLorentz_h = lorentzMap_h.getContent(tanLorentz_h_bin_x, lorentzMap_h.getBinY(depth_f_h));
        const double dz_e = std::abs(dist_electrode - depth_f_e);
        const double dz_h = std::abs(depth_f_h - dist_electrode);
        const double coLorentz_e = std::sqrt(1.0 + (tanLorentz_e*tanLorentz_e));

        //Apply drift due to Lorentz force and diffusion
        double phiRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);

        //Apply diffusion. rdif is teh max. diffusion
        const double rdif_e = this->m_diffusionConstant * std::sqrt(dz_e * coLorentz_e / 0.3);
        const double phi_f_e = phi_i + dz_e * tanLorentz_e + rdif_e * phiRand;
        double etaRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
        double eta_f_e = eta_i + rdif_e * etaRand;

        phiRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
        const double coLorentz_h = std::sqrt(1.0 + (tanLorentz_h*tanLorentz_h));
        const double rdif_h = this->m_diffusionConstant * std::sqrt(dz_h * coLorentz_h / 0.3);
        const double phi_f_h = phi_i + dz_h * tanLorentz_h + rdif_h * phiRand;
        etaRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
        double eta_f_h = eta_i + rdif_h * etaRand;
        

        // Slim Edge for IBL planar sensors:
        if (p_design.getReadoutTechnology() == InDetDD::PixelReadoutTechnology::FEI4) {
          applyIBLSlimEdges(energy_per_step, eta_f_e);
          applyIBLSlimEdges(energy_per_step, eta_f_h);
        }

        const std::size_t ramo_f_e_bin_z = ramoPotentialMap.getBinZ(1e3*depth_f_e);
        const std::size_t ramo_f_h_bin_z = ramoPotentialMap.getBinZ(1e3*depth_f_h);

        const bool isFirstZ_e = ramoPotentialMap.isFirstZ(1e3*depth_f_e);
        const bool isOverflowZ_h = ramoPotentialMap.isOverflowZ(1e3*depth_f_h);

        const double pixelEta_f_e = eta_f_e - centreOfPixel_i.xEta();
        const double pixelPhi_f_e = phi_f_e - centreOfPixel_i.xPhi();

        const double pixelEta_f_h = eta_f_h - centreOfPixel_i.xEta();
        const double pixelPhi_f_h = phi_f_h - centreOfPixel_i.xPhi();

        //Loop over nearest neighbours in x and y
        //We assume that the lateral diffusion is minimal
        for (int p = nnLoop_pixelEtaMin; p <= nnLoop_pixelEtaMax; p++) {
          const std::size_t ieta = p - nnLoop_pixelEtaMin;

          for (int q = nnLoop_pixelPhiMin; q <= nnLoop_pixelPhiMax; q++) {
            //Since both e-h charge carriers start in the same place, they have the same initial ramo value
            //Centre of nearest neighbour (nn) pixel

            const std::size_t iphi = q - nnLoop_pixelPhiMin;
            const std::size_t index = iphi + ieta*sizePhi;
            //What is the displacement of the nn pixel from the primary pixel.
            //This is to index the correct entry in the Ramo weighting potential map
            const std::pair<double,double>& centrePixelNN = m_centrePixelNNEtaPhi[index];
            const double dPhi_nn_centre = centrePixelNN.second - centreOfPixel_i.xPhi(); //in mm
            const double dEta_nn_centre = centrePixelNN.first  - centreOfPixel_i.xEta(); //in mm

            //This all has to be done relative to the (0,0) position since the
            //Ramo weighting potential is only mapped out for 1/8th of a pixel. Much of this logic is reflecting the
            // charge carrier across the boundaries.
            //Find the displacment of the charge carriers from the centre of the pixel in +ve quadrant

            //Final position of charge carriers wrt nn centre
            const double dEta_f_e = std::abs(pixelEta_f_e - dEta_nn_centre)*sensorScales[ieta];
            const double dPhi_f_e = std::abs(pixelPhi_f_e - dPhi_nn_centre);
            const double dEta_f_h = 1e3*std::abs(pixelEta_f_h - dEta_nn_centre)*sensorScales[ieta];
            const double dPhi_f_h = 1e3*std::abs(pixelPhi_f_h - dPhi_nn_centre);

            //Boundary check on maps
            double ramo_f_e = 0.0;
            double ramo_f_h = 0.0;

            if (isFirstZ_e) {
              if (dEta_f_e >= halfEtaPitch || dPhi_f_e >= halfPhiPitch) {
                ramo_f_e = 0.0;
              } else {
                ramo_f_e = 1.0;
              }
            } else {
              ramo_f_e = ramoPotentialMap.getContent(ramoPotentialMap.getBinX(1e3*dPhi_f_e), ramoPotentialMap.getBinY(1e3*dEta_f_e), ramo_f_e_bin_z);
            }

            //Account for the imperfect binning that would cause charge to be double-counted
            if (isOverflowZ_h) {
              ramo_f_h = 0;
            } else {
              ramo_f_h = ramoPotentialMap.getContent(ramoPotentialMap.getBinX(dPhi_f_h), ramoPotentialMap.getBinY(dEta_f_h), ramo_f_h_bin_z);
            }

            //Given final position of charge carrier, find induced charge. The difference in Ramo weighting potential
            // gives the fraction of charge induced.
            //The energy_per_step is transformed into charge with the eleholePair per Energy
            const double potentialDiff = ramo_f_e - ramo_f_h;
            // this variable ^ can be used to apply some cut to skip the loop
            const double induced_charge = potentialDiff * energy_per_step * eleholePairEnergy;

            unsigned key = (static_cast<unsigned>(pixel_eta-p) << 16) | static_cast<unsigned>(pixel_phi-q);
            auto cacheIterator = cachedChargeMap.find(key);
            if(cacheIterator == cachedChargeMap.end()) {
              cachedChargeMap.insert(std::make_pair(key, std::make_pair(Module.hitLocalToLocal(centrePixelNN.first, centrePixelNN.second), induced_charge)));
            } else {
              cacheIterator->second.second += induced_charge;
            }
          } //For q
        } //for p
      }//end cycle for charge
    } //trfHitRecord.size()

    std::for_each(cachedChargeMap.begin(), cachedChargeMap.end(), [&diodeCellMap, &Module, &chargedDiodes, &pHitTime, &particleLink](auto& pos_charge_pair){
      auto& key = pos_charge_pair.first;
      auto& chargePos = pos_charge_pair.second.first;
      auto& charge_value = pos_charge_pair.second.second;

      const SiSurfaceCharge scharge(chargePos, SiCharge(charge_value, pHitTime, SiCharge::track, particleLink));
      auto diodeIterator = diodeCellMap.find(key);
      if(diodeIterator == diodeCellMap.end()) diodeIterator = diodeCellMap.insert(std::make_pair(key, Module.cellIdOfPosition(scharge.position()))).first;
      const SiCellId& thisDiode = diodeIterator->second;
      if (thisDiode.isValid()) {
        const SiCharge& charge = scharge.charge();
        chargedDiodes.add(thisDiode, charge);
      }
    });

  } 
  else if (m_doRadDamage && m_doRadDamageTemplate && !(Module.isDBM()) && Module.isBarrel()){ // will run radiation damage but with the template method
    for (size_t i = 0; i < trfHitRecord.size(); i++) {
      std::pair<double, double> const& iHitRecord = trfHitRecord[i];

      double eta_i = eta_0;
      double phi_i = phi_0;
      double depth_i = depth_0;
      if (iTotalLength) {
        eta_i += 1.0 * iHitRecord.first / iTotalLength * dEta;
        phi_i += 1.0 * iHitRecord.first / iTotalLength * dPhi;
        depth_i += 1.0 * iHitRecord.first / iTotalLength * dDepth;
      }

      // Distance between charge and readout side.  p_design->readoutSide() is
      // +1 if readout side is in +ve depth axis direction and visa-versa.
      double dist_electrode = 0.5 * sensorThickness - Module.design().readoutSide() * depth_i;
      if (dist_electrode < 0) {
        dist_electrode = 0;
      }

      // nonTrapping probability
      double nontrappingProbability = 1.0;
      if (Module.isDBM()) {
        nontrappingProbability = exp(-dist_electrode / collectionDist);
      }

      for (int j = 0; j < ncharges; j++) {
        // amount of energy to be converted into charges at current step
        double energy_per_step = 1.0 * iHitRecord.second / 1.E+6 / ncharges;
        // diffusion sigma
        double rdif = this->m_diffusionConstant * std::sqrt(dist_electrode * coLorentz / 0.3);

        // position at the surface
        double phiRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
        double phi_drifted = phi_i + dist_electrode * tanLorentz + rdif * phiRand;
        double etaRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
        double eta_drifted = eta_i + rdif * etaRand;

        // Slim Edge for IBL planar sensors:
        if (!(Module.isDBM()) && p_design.getReadoutTechnology() == InDetDD::PixelReadoutTechnology::FEI4) {
          applyIBLSlimEdges(energy_per_step, eta_drifted);
        }

        // Get the charge position in Reconstruction local coordinates.
        const SiLocalPosition& chargePos = Module.hitLocalToLocal(eta_drifted, phi_drifted);

        // The parametrization of the sensor efficiency (if needed)
        double ed = 0;
        if (Module.isDBM()) {
          ed = energy_per_step * eleholePairEnergy * nontrappingProbability * smearScale;
        } else {
          ed = energy_per_step * eleholePairEnergy;
        }

        // prepare SiTrackDistance object needed
        const SiTrackDistance trackDistance(tanTheta, tanPhi, startPosition.Z);
        //The following lines are adapted from SiDigitization's Inserter class
        const SiSurfaceCharge scharge(chargePos, SiCharge(ed, pHitTime, SiCharge::track, particleLink, trackDistance));

        const SiCellId& diode = Module.cellIdOfPosition(scharge.position());

        if (diode.isValid()) {
          const SiCharge& charge = scharge.charge();
          chargedDiodes.add(diode, charge);
        }
      }//end cycle for charge
    }//trfHitRecord.size()
  } else { // run without radiation damage
    for (size_t i = 0; i < trfHitRecord.size(); i++) {
      std::pair<double, double> const& iHitRecord = trfHitRecord[i];

      double eta_i = eta_0;
      double phi_i = phi_0;
      double depth_i = depth_0;
      if (iTotalLength) {
        eta_i += 1.0 * iHitRecord.first / iTotalLength * dEta;
        phi_i += 1.0 * iHitRecord.first / iTotalLength * dPhi;
        depth_i += 1.0 * iHitRecord.first / iTotalLength * dDepth;
      }

      // Distance between charge and readout side.  p_design->readoutSide() is
      // +1 if readout side is in +ve depth axis direction and visa-versa.
      double dist_electrode = 0.5 * sensorThickness - Module.design().readoutSide() * depth_i;
      if (dist_electrode < 0) {
        dist_electrode = 0;
      }

      // nonTrapping probability
      double nontrappingProbability = 1.0;
      if (Module.isDBM()) {
        nontrappingProbability = exp(-dist_electrode / collectionDist);
      }

      for (int j = 0; j < ncharges; j++) {
        // amount of energy to be converted into charges at current step
        double energy_per_step = 1.0 * iHitRecord.second / 1.E+6 / ncharges;
        // diffusion sigma
        double rdif = this->m_diffusionConstant * std::sqrt(dist_electrode * coLorentz / 0.3);

        // position at the surface
        double phiRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
        double phi_drifted = phi_i + dist_electrode * tanLorentz + rdif * phiRand;
        double etaRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
        double eta_drifted = eta_i + rdif * etaRand;

        // Slim Edge for IBL planar sensors:
        if (!(Module.isDBM()) && p_design.getReadoutTechnology() == InDetDD::PixelReadoutTechnology::FEI4) {
          applyIBLSlimEdges(energy_per_step, eta_drifted);
        }

        // Get the charge position in Reconstruction local coordinates.
        const SiLocalPosition& chargePos = Module.hitLocalToLocal(eta_drifted, phi_drifted);

        // The parametrization of the sensor efficiency (if needed)
        double ed = 0;
        if (Module.isDBM()) {
          ed = energy_per_step * eleholePairEnergy * nontrappingProbability * smearScale;
        } else {
          ed = energy_per_step * eleholePairEnergy;
        }

        //The following lines are adapted from SiDigitization's Inserter class
        const SiSurfaceCharge scharge(chargePos, SiCharge(ed, pHitTime, SiCharge::track, particleLink));

        const SiCellId& diode = Module.cellIdOfPosition(scharge.position());

        if (diode.isValid()) {
          const SiCharge& charge = scharge.charge();
          chargedDiodes.add(diode, charge);
        }
      }//end cycle for charge
    }
  }

  return StatusCode::SUCCESS;
}

void SensorSimPlanarTool::applyIBLSlimEdges(double& energy_per_step, double& eta_drifted) {
  if (std::abs(eta_drifted) > 20.440) {
    energy_per_step = 0.0;
  }
  if (std::abs(eta_drifted) < 20.440 && std::abs(eta_drifted) > 20.200) {
    if (eta_drifted > 0) {
      energy_per_step = energy_per_step * (68.13 - eta_drifted * 3.333);
      eta_drifted = eta_drifted - 0.250;
    } else {
      energy_per_step = energy_per_step * (68.13 + eta_drifted * 3.333);
      eta_drifted = eta_drifted + 0.250;
    }
  }
  if (std::abs(eta_drifted) < 20.200 && std::abs(eta_drifted) > 20.100) {
    if (eta_drifted > 0) {
      energy_per_step = energy_per_step * (41.2 - eta_drifted * 2.0);
      eta_drifted = eta_drifted - 0.250;
    } else {
      energy_per_step = energy_per_step * (41.2 + eta_drifted * 2.0);
      eta_drifted = eta_drifted + 0.250;
    }
  }
}

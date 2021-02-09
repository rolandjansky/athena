/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "SensorSimPlanarTool.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "SiDigitization/SiSurfaceCharge.h"
#include "InDetSimEvent/SiHit.h"
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

using namespace InDetDD;

//===============================================
//    C O N S T R U C T O R
//===============================================
SensorSimPlanarTool::SensorSimPlanarTool(const std::string& type, const std::string& name, const IInterface* parent) :
  SensorSimTool(type, name, parent) {
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
      //std::unique_ptr<TFile>  mapsFile=std::make_unique<TFile>( (mapsPath_list.at(i)).c_str() ); //this is the ramo
      // potential.
      TFile* mapsFile = new TFile((mapsPath_list.at(i)).c_str()); //this is the ramo potential.

      //Setup ramo weighting field map
      TH3F* ramoPotentialMap_hold;
      ramoPotentialMap_hold = 0;
      ramoPotentialMap_hold = (TH3F*) mapsFile->Get("hramomap1");
      if (ramoPotentialMap_hold == 0) ramoPotentialMap_hold = (TH3F*) mapsFile->Get("ramo3d");
      if (ramoPotentialMap_hold == 0) {
        ATH_MSG_INFO("Did not find a Ramo potential map.  Will use an approximate form.");
        ATH_MSG_WARNING("Not implemented yet - exit");
        return StatusCode::FAILURE; //Obviously, remove this when gen. code is set up
      }
      m_ramoPotentialMap.push_back(ramoPotentialMap_hold);
      //Now setup the E-field.
      TH1F* eFieldMap_hold;
      eFieldMap_hold = new TH1F();
      //ATH_MSG_INFO("Generating E field maps using interpolation.");
      CHECK(m_radDamageUtil->generateEfieldMap(eFieldMap_hold, NULL, m_fluenceLayer[i], m_voltageLayer[i], i,
                                               TCADpath_list.at(i), true));

      TH2F* lorentzMap_e_hold = new TH2F();
      TH2F* lorentzMap_h_hold = new TH2F();
      TH2F* distanceMap_h_hold = new TH2F();
      TH2F* distanceMap_e_hold = new TH2F();
      TH1F* timeMap_e_hold = new TH1F();
      TH1F* timeMap_h_hold = new TH1F();

      ATH_CHECK(m_radDamageUtil->generateDistanceTimeMap(distanceMap_e_hold, distanceMap_h_hold, timeMap_e_hold,
                                                         timeMap_h_hold, lorentzMap_e_hold, lorentzMap_h_hold,
                                                         eFieldMap_hold, NULL));
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
      if (distanceMap_e_hold == 0 || distanceMap_h_hold == 0 || timeMap_e_hold == 0 || timeMap_h_hold == 0 ||
          lorentzMap_e_hold == 0 || lorentzMap_h_hold == 0) {
        ATH_MSG_INFO("Unable to load at least one of the distance/time/Lorentz angle maps.");
        return StatusCode::FAILURE;//Obviously, remove this when gen. code is set up
      }
      m_distanceMap_e.push_back(distanceMap_e_hold);
      m_distanceMap_h.push_back(distanceMap_h_hold);
      m_lorentzMap_e.push_back(lorentzMap_e_hold);
      m_lorentzMap_h.push_back(lorentzMap_h_hold);
    }
    // Define necessary variables to describe hists stored in ramoPotentialMap
    // to skip calling FindBin during induceCharge for speed reasons
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
StatusCode SensorSimPlanarTool::induceCharge(const TimedHitPtr<SiHit>& phit, SiChargedDiodeCollection& chargedDiodes,
                                             const InDetDD::SiDetectorElement& Module,
                                             const InDetDD::PixelModuleDesign& p_design,
                                             std::vector< std::pair<double, double> >& trfHitRecord,
                                             std::vector<double>& initialConditions,
                                             CLHEP::HepRandomEngine* rndmEngine) {
  // So far, this is only discriminating variable from 3D sensor.
  if (p_design.numberOfCircuits() < 2) {
    if (!Module.isDBM()) {  //DBM modules also processed here
      return StatusCode::SUCCESS;
    }
  }

  const PixelID* p_pixelId = static_cast<const PixelID*>(Module.getIdHelper());
  int layer = p_pixelId->layer_disk(Module.identify());

  // retrieve conditions data
  SG::ReadCondHandle<PixelModuleData> moduleData(m_moduleDataKey);

  std::pair<double, double> trappingTimes;
  if (m_doRadDamage && Module.isBarrel()) {
    if (m_doInterpolateEfield) {
      trappingTimes = m_radDamageUtil->getTrappingTimes(m_fluenceLayer[layer]);
    } else {
      trappingTimes = m_radDamageUtil->getTrappingTimes(moduleData->getFluenceLayer(layer));
    }
  }

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
  const InDet::SiliconProperties& siProperties = m_siPropertiesTool->getSiProperties(Module.identifyHash());

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
  double coLorentz = std::sqrt(1.0 + pow(tanLorentz, 2));

  const EBC_EVCOLL evColl = EBC_MAINEVCOLL;
  const HepMcParticleLink::PositionFlag idxFlag =
    (phit.eventId() == 0) ? HepMcParticleLink::IS_POSITION : HepMcParticleLink::IS_INDEX;

  //**************************************//
  //*** Now diffuse charges to surface *** //
  //**************************************//
  for (unsigned int i = 0; i < trfHitRecord.size(); i++) {
    std::pair<double, double> iHitRecord = trfHitRecord[i];

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

    SiLocalPosition centreOfPixel_i;

    int nnLoop_pixelEtaMax = 0;
    int nnLoop_pixelEtaMin = 0;
    int nnLoop_pixelPhiMax = 0;
    int nnLoop_pixelPhiMin = 0;

    int numBins_driftTime_e = 0;
    int numBins_driftTime_h = 0;
    int numBins_weightingPotential_x = 0;
    int numBins_weightingPotential_y = 0;
    int numBins_weightingPotential_z = 0;

    if (m_doRadDamage && !(Module.isDBM()) && Module.isBarrel()) {
      centreOfPixel_i = p_design.positionFromColumnRow(pixel_i.etaIndex(), pixel_i.phiIndex());

      //Make limits for NN loop
      nnLoop_pixelEtaMax = std::min(1, pixel_i.etaIndex());
      nnLoop_pixelEtaMin = std::max(-1, pixel_i.etaIndex() + 1 - etaCells);

      nnLoop_pixelPhiMax = std::min(1, pixel_i.phiIndex());
      nnLoop_pixelPhiMin = std::max(-1, pixel_i.phiIndex() + 1 - phiCells);

      //Setup values to check for overflow when using maps
      if (m_doInterpolateEfield) {
        numBins_driftTime_e = m_distanceMap_e[layer]->GetNbinsY(); //Returns nBins = totalBins - underflow - overflow
        numBins_driftTime_h = m_distanceMap_h[layer]->GetNbinsY();
        numBins_weightingPotential_x = m_ramoPotentialMap[layer]->GetNbinsX();
        numBins_weightingPotential_y = m_ramoPotentialMap[layer]->GetNbinsY();
        numBins_weightingPotential_z = m_ramoPotentialMap[layer]->GetNbinsZ();
      } else { // use fluence value from conditions data
        numBins_driftTime_e = moduleData->getDistanceMap_e(layer)->GetNbinsY();
        numBins_driftTime_h = moduleData->getDistanceMap_h(layer)->GetNbinsY();
        numBins_weightingPotential_x = moduleData->getRamoPotentialMap(layer)->GetNbinsX();
        numBins_weightingPotential_y = moduleData->getRamoPotentialMap(layer)->GetNbinsY();
        numBins_weightingPotential_z = moduleData->getRamoPotentialMap(layer)->GetNbinsZ();
      }
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

    if (m_doInterpolateEfield) {
      m_ramo_x_binMap = 1000. *
                        (m_ramoPotentialMap[layer]->GetNbinsX() /
                         (m_ramoPotentialMap[layer]->GetXaxis()->GetXmax() -
                          m_ramoPotentialMap[layer]->GetXaxis()->GetXmin()));
      m_ramo_y_binMap = 1000. *
                        (m_ramoPotentialMap[layer]->GetNbinsY() /
                         (m_ramoPotentialMap[layer]->GetYaxis()->GetXmax() -
                          m_ramoPotentialMap[layer]->GetYaxis()->GetXmin()));
      m_ramo_z_binMap = 1000. *
                        (m_ramoPotentialMap[layer]->GetNbinsZ() /
                         (m_ramoPotentialMap[layer]->GetZaxis()->GetXmax() -
                          m_ramoPotentialMap[layer]->GetZaxis()->GetXmin()));
    } else {
      m_ramo_x_binMap = 1000. *
                        (moduleData->getRamoPotentialMap(layer)->GetNbinsX() /
                         (moduleData->getRamoPotentialMap(layer)->GetXaxis()->GetXmax() -
                          moduleData->getRamoPotentialMap(layer)->GetXaxis()->GetXmin()));
      m_ramo_y_binMap = 1000. *
                        (moduleData->getRamoPotentialMap(layer)->GetNbinsY() /
                         (moduleData->getRamoPotentialMap(layer)->GetYaxis()->GetXmax() -
                          moduleData->getRamoPotentialMap(layer)->GetYaxis()->GetXmin()));
      m_ramo_z_binMap = 1000. *
                        (moduleData->getRamoPotentialMap(layer)->GetNbinsZ() /
                         (moduleData->getRamoPotentialMap(layer)->GetZaxis()->GetXmax() -
                          moduleData->getRamoPotentialMap(layer)->GetZaxis()->GetXmin()));
    }

    for (int j = 0; j < ncharges; j++) {
      if (m_doRadDamage && !(Module.isDBM()) && Module.isBarrel()) {
        double u = CLHEP::RandFlat::shoot(0., 1.);
        double drifttime_e = (-1.) * (trappingTimes.first) * TMath::Log(u); //ns
        u = CLHEP::RandFlat::shoot(0., 1.);
        double drifttime_h = (-1.) * (trappingTimes.second) * TMath::Log(u); //ns

        //Now, need the z-position at the trap.
        double depth_f_e = 0.0;
        double depth_f_h = 0.0;
        double tanLorentz_e = 0.0;
        double tanLorentz_h = 0.0;
        //TODO: the holes map does not currently extend for a drift time long enough that, any hole will reach
        //the corresponding electrode. This needs to be rectified by either (a) extrapolating the current map or
        //(b) making a new map with a y-axis (drift time) that extends to at least 18 ns so all charge carriers reach
        // electrode.
        //However, if choose (b), will need to reduce granularity of map.
        if (m_doInterpolateEfield) {
          int nbin_z_e_xbin = m_distanceMap_e[layer]->GetXaxis()->FindBin(dist_electrode);
          int nbin_z_e_ybin = m_distanceMap_e[layer]->GetYaxis()->FindBin(drifttime_e);
          if (nbin_z_e_ybin > numBins_driftTime_e) {
            nbin_z_e_ybin = numBins_driftTime_e;
          }
          depth_f_e = m_distanceMap_e[layer]->GetBinContent(nbin_z_e_xbin, nbin_z_e_ybin);
          int nbin_z_h_xbin = m_distanceMap_h[layer]->GetXaxis()->FindBin(dist_electrode);
          int nbin_z_h_ybin = m_distanceMap_h[layer]->GetYaxis()->FindBin(drifttime_h);
          if (nbin_z_h_ybin > numBins_driftTime_h) {
            nbin_z_h_ybin = numBins_driftTime_h;
          }
          depth_f_h = m_distanceMap_h[layer]->GetBinContent(nbin_z_h_xbin, nbin_z_h_ybin);
          int nbin_Lorentz_e = m_lorentzMap_e[layer]->FindBin(dist_electrode, depth_f_e);
          tanLorentz_e = m_lorentzMap_e[layer]->GetBinContent(nbin_Lorentz_e);
          int nbin_Lorentz_h = m_lorentzMap_h[layer]->FindBin(dist_electrode, depth_f_h);
          tanLorentz_h = m_lorentzMap_h[layer]->GetBinContent(nbin_Lorentz_h);
        } else { // use fluence value from conditions data
          int nbin_z_e_xbin = moduleData->getDistanceMap_e(layer)->GetXaxis()->FindBin(dist_electrode);
          int nbin_z_e_ybin = moduleData->getDistanceMap_e(layer)->GetYaxis()->FindBin(drifttime_e);
          if (nbin_z_e_ybin > numBins_driftTime_e) {
            nbin_z_e_ybin = numBins_driftTime_e;
          }
          depth_f_e = moduleData->getDistanceMap_e(layer)->GetBinContent(nbin_z_e_xbin, nbin_z_e_ybin);
          int nbin_z_h_xbin = moduleData->getDistanceMap_h(layer)->GetXaxis()->FindBin(dist_electrode);
          int nbin_z_h_ybin = moduleData->getDistanceMap_h(layer)->GetYaxis()->FindBin(drifttime_h);
          if (nbin_z_h_ybin > numBins_driftTime_h) {
            nbin_z_h_ybin = numBins_driftTime_h;
          }
          depth_f_h = moduleData->getDistanceMap_h(layer)->GetBinContent(nbin_z_h_xbin, nbin_z_h_ybin);
          int nbin_Lorentz_e = moduleData->getLorentzMap_e(layer)->FindBin(dist_electrode, depth_f_e);
          tanLorentz_e = moduleData->getLorentzMap_e(layer)->GetBinContent(nbin_Lorentz_e);
          int nbin_Lorentz_h = moduleData->getLorentzMap_h(layer)->FindBin(dist_electrode, depth_f_h);
          tanLorentz_h = moduleData->getLorentzMap_h(layer)->GetBinContent(nbin_Lorentz_h);
        }
        double dz_e = fabs(dist_electrode - depth_f_e);
        double dz_h = fabs(depth_f_h - dist_electrode);
        double coLorentz_e = std::sqrt(1.0 + std::pow(tanLorentz_e, 2));

        //Apply drift due to Lorentz force and diffusion
        double phiRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);

        //Apply diffusion. rdif is teh max. diffusion
        double rdif_e = this->m_diffusionConstant * sqrt(fabs(dist_electrode - depth_f_e) * coLorentz_e / 0.3);
        double phi_f_e = phi_i + dz_e * tanLorentz_e + rdif_e * phiRand;
        double etaRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
        double eta_f_e = eta_i + rdif_e * etaRand;

        phiRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
        double coLorentz_h = std::sqrt(1.0 + std::pow(tanLorentz_h, 2));
        double rdif_h = this->m_diffusionConstant * sqrt(fabs(dist_electrode - depth_f_h) * coLorentz_h / 0.3);
        double phi_f_h = phi_i + dz_h * tanLorentz_h + rdif_h * phiRand;
        etaRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
        double eta_f_h = eta_i + rdif_h * etaRand;

        // amount of energy to be converted into charges at current step
        double energy_per_step = 1.0 * iHitRecord.second / 1.E+6 / ncharges;

        // Slim Edge for IBL planar sensors:
        if (p_design.getReadoutTechnology() == InDetDD::PixelModuleDesign::FEI4) {
          ATH_CHECK(applyIBLSlimEdges(energy_per_step, eta_f_e));
          ATH_CHECK(applyIBLSlimEdges(energy_per_step, eta_f_h));
        }

        int nbin_ramo_f_e_z;
        int nbin_ramo_f_h_z;
        // distinction necessary because of min(z) = -0.5
        float minz = 1;
        if (layer != 0) minz = 1.5;
        nbin_ramo_f_e_z = int( minz + depth_f_e * m_ramo_z_binMap );
        nbin_ramo_f_h_z = int( minz + depth_f_h * m_ramo_z_binMap );

        // Check for overflow in ramo hists in z-direction
        if (nbin_ramo_f_h_z > numBins_weightingPotential_z) {
          nbin_ramo_f_h_z = numBins_weightingPotential_z + 1;
        }
        if (nbin_ramo_f_e_z > numBins_weightingPotential_z) {
          nbin_ramo_f_e_z = numBins_weightingPotential_z + 1;
        }

        //Loop over nearest neighbours in x and y
        //We assume that the lateral diffusion is minimal
        for (int p = nnLoop_pixelEtaMin; p <= nnLoop_pixelEtaMax; p++) {
          // scale factors accounting for different pixel sizes
          double scale_f = 1.;
          double columnWidth = p_design.widthFromColumnRange(pixel_i.etaIndex() - p, pixel_i.etaIndex() - p);
          if (std::abs(columnWidth - 0.6) < 1e-9) {
            scale_f = 4. / 6.;
          } else if (std::abs(columnWidth - 0.45) < 1e-9) {
            scale_f = 25. / 45.;
          } else if (std::abs(columnWidth - 0.5) < 1e-9) {
            scale_f = 25. / 50.;
          }

          for (int q = nnLoop_pixelPhiMin; q <= nnLoop_pixelPhiMax; q++) {
            //Since both e-h charge carriers start in the same place, they have the same initial ramo value
            //Centre of nearest neighbour (nn) pixel
            SiLocalPosition centreOfPixel_nn = p_design.positionFromColumnRow(pixel_i.etaIndex() - p,
                                                                              pixel_i.phiIndex() - q);

            //What is the displacement of the nn pixel from the primary pixel.
            //This is to index the correct entry in the Ramo weighting potential map
            double dPhi_nn_centre = centreOfPixel_nn.xPhi() - centreOfPixel_i.xPhi(); //in mm
            double dEta_nn_centre = centreOfPixel_nn.xEta() - centreOfPixel_i.xEta(); //in mm

            //This all has to be done relative to the (0,0) position since the
            //Ramo weighting potential is only mapped out for 1/8th of a pixel. Much of this logic is reflecting the
            // charge
            //carrier across the boundaries.
            //Find the displacment of the charge carriers from the centre of the pixel in +ve quadrant

            double pixelEta_f_e = eta_f_e - centreOfPixel_i.xEta();
            double pixelPhi_f_e = phi_f_e - centreOfPixel_i.xPhi();

            double pixelEta_f_h = eta_f_h - centreOfPixel_i.xEta();
            double pixelPhi_f_h = phi_f_h - centreOfPixel_i.xPhi();

            //Final position of charge carriers wrt nn centre
            double dEta_f_e = pixelEta_f_e - dEta_nn_centre;
            double dPhi_f_e = pixelPhi_f_e - dPhi_nn_centre;
            dEta_f_e *= scale_f;
            double dEta_f_h = pixelEta_f_h - dEta_nn_centre;
            double dPhi_f_h = pixelPhi_f_h - dPhi_nn_centre;
            dEta_f_h *= scale_f;

            //Boundary check on maps
            double ramo_f_e = 0.0;
            double ramo_f_h = 0.0;

            int nbin_ramo_f_e_x = int( 1 + std::abs(dPhi_f_e) * m_ramo_x_binMap );
            int nbin_ramo_f_e_y = int( 1 + std::abs(dEta_f_e) * m_ramo_y_binMap );

            // Check for overflow in ramo hists in x- and y-direction
            if (nbin_ramo_f_e_x > numBins_weightingPotential_x) {
              nbin_ramo_f_e_x = numBins_weightingPotential_x + 1;
            }
            if (nbin_ramo_f_e_y > numBins_weightingPotential_y) {
              nbin_ramo_f_e_y = numBins_weightingPotential_y + 1;
            }

            if (nbin_ramo_f_e_x <= numBins_weightingPotential_x && nbin_ramo_f_e_y <= numBins_weightingPotential_y &&
                nbin_ramo_f_e_z <= numBins_weightingPotential_z) {
              if (m_doInterpolateEfield) {
                ramo_f_e = m_ramoPotentialMap[layer]->GetBinContent(nbin_ramo_f_e_x, nbin_ramo_f_e_y, nbin_ramo_f_e_z);
              } else {
                ramo_f_e = moduleData->getRamoPotentialMap(layer)->GetBinContent(nbin_ramo_f_e_x, nbin_ramo_f_e_y,
                                                                                 nbin_ramo_f_e_z);
              }
            }

            int nbin_ramo_f_h_x = int( 1 + std::abs(dPhi_f_h) * m_ramo_x_binMap );
            int nbin_ramo_f_h_y = int( 1 + std::abs(dEta_f_h) * m_ramo_y_binMap );

            // Check for overflow in ramo hists in x- and y-direction
            if (nbin_ramo_f_h_x > numBins_weightingPotential_x) {
              nbin_ramo_f_h_x = numBins_weightingPotential_x + 1;
            }
            if (nbin_ramo_f_h_y > numBins_weightingPotential_y) {
              nbin_ramo_f_h_y = numBins_weightingPotential_y + 1;
            }

            if (nbin_ramo_f_h_x <= numBins_weightingPotential_x && nbin_ramo_f_h_y <= numBins_weightingPotential_y &&
                nbin_ramo_f_h_z <= numBins_weightingPotential_z) {
              if (m_doInterpolateEfield) {
                ramo_f_h = m_ramoPotentialMap[layer]->GetBinContent(nbin_ramo_f_h_x, nbin_ramo_f_h_y, nbin_ramo_f_h_z);
              } else {
                ramo_f_h = moduleData->getRamoPotentialMap(layer)->GetBinContent(nbin_ramo_f_h_x, nbin_ramo_f_h_y,
                                                                                 nbin_ramo_f_h_z);
              }
            }
            //Account for the imperfect binning that would cause charge to be double-counted

            if (m_doInterpolateEfield) {
              if (m_ramoPotentialMap[layer]->GetZaxis()->FindBin(depth_f_h * 1000) ==
                  m_ramoPotentialMap[layer]->GetNbinsZ() + 1) {
                ramo_f_h = 0.0;
              } //this means the hole has reached the back end

              if (m_ramoPotentialMap[layer]->GetZaxis()->FindBin(depth_f_e * 1000) == 1) {
                if (fabs(dEta_f_e) >= Module.etaPitch() / 2.0 || fabs(dPhi_f_e) >= Module.phiPitch() / 2.0) {
                  ramo_f_e = 0.0;
                } else if (fabs(dEta_f_e) < Module.etaPitch() / 2.0 && fabs(dPhi_f_e) < Module.phiPitch() / 2.0) {
                  ramo_f_e = 1.0;
                }
              }
            } else {
              if (moduleData->getRamoPotentialMap(layer)->GetZaxis()->FindBin(depth_f_h * 1000)
                  == moduleData->getRamoPotentialMap(layer)->GetNbinsZ() + 1) {
                ramo_f_h = 0;
              } //this means the hole has reached the back end

              if (moduleData->getRamoPotentialMap(layer)->GetZaxis()->FindBin(depth_f_e * 1000) == 1) {
                if (fabs(dEta_f_e) >= Module.etaPitch() / 2.0 || fabs(dPhi_f_e) >= Module.phiPitch() / 2.0) {
                  ramo_f_e = 0.0;
                } else if (fabs(dEta_f_e) < Module.etaPitch() / 2.0 && fabs(dPhi_f_e) < Module.phiPitch() / 2.0) {
                  ramo_f_e = 1.0;
                }
              }
            }

            //Given final position of charge carrier, find induced charge. The difference in Ramo weighting potential
            // gives the fraction of charge induced.
            //The energy_per_step is transformed into charge with the eleholePair per Energy
            double induced_charge = (ramo_f_e - ramo_f_h) * energy_per_step * eleholePairEnergy;

            //Collect charge in centre of each pixel, since location within pixel doesn't matter for record
            SiLocalPosition chargePos = Module.hitLocalToLocal(centreOfPixel_nn.xEta(), centreOfPixel_nn.xPhi());

            //The following lines are adapted from SiDigitization's Inserter class
            SiSurfaceCharge scharge(
              chargePos,
              SiCharge(induced_charge, hitTime(phit), SiCharge::track,
                       HepMcParticleLink(phit->trackNumber(), phit.eventId(), evColl, idxFlag)
                       )
              );
            SiCellId diode = Module.cellIdOfPosition(scharge.position());
            SiCharge charge = scharge.charge();
            if (diode.isValid()) {
              chargedDiodes.add(diode, charge);
            } //IF
          } //For q
        } //for p
      } else { //If no radDamage, run original
        // diffusion sigma
        double rdif = this->m_diffusionConstant * sqrt(dist_electrode * coLorentz / 0.3);

        // position at the surface
        double phiRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
        double phi_drifted = phi_i + dist_electrode * tanLorentz + rdif * phiRand;
        double etaRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
        double eta_drifted = eta_i + rdif * etaRand;

        // amount of energy to be converted into charges at current step
        double energy_per_step = 1.0 * iHitRecord.second / 1.E+6 / ncharges;

        // Slim Edge for IBL planar sensors:
        if (!(Module.isDBM()) && p_design.getReadoutTechnology() == InDetDD::PixelModuleDesign::FEI4) {
          ATH_CHECK(applyIBLSlimEdges(energy_per_step, eta_drifted));
        }

        // Get the charge position in Reconstruction local coordinates.
        SiLocalPosition chargePos = Module.hitLocalToLocal(eta_drifted, phi_drifted);

        // The parametrization of the sensor efficiency (if needed)
        double ed = 0;
        if (Module.isDBM()) {
          ed = energy_per_step * eleholePairEnergy * nontrappingProbability * smearScale;
        } else {
          ed = energy_per_step * eleholePairEnergy;
        }

        //The following lines are adapted from SiDigitization's Inserter class
        SiSurfaceCharge scharge(chargePos, SiCharge(ed, hitTime(phit), SiCharge::track, HepMcParticleLink(
                                                      phit->trackNumber(), phit.eventId(), evColl, idxFlag)));

        SiCellId diode = Module.cellIdOfPosition(scharge.position());

        SiCharge charge = scharge.charge();

        if (diode.isValid()) {
          chargedDiodes.add(diode, charge);
        }
      } //else: no radDamage, run original
    }//end cycle for charge
  }//trfHitRecord.size()
  return StatusCode::SUCCESS;
}

StatusCode SensorSimPlanarTool::applyIBLSlimEdges(double& energy_per_step, double& eta_drifted) {
  if (fabs(eta_drifted) > 20.440) {
    energy_per_step = 0.0;
  }
  if (fabs(eta_drifted) < 20.440 && fabs(eta_drifted) > 20.200) {
    if (eta_drifted > 0) {
      energy_per_step = energy_per_step * (68.13 - eta_drifted * 3.333);
      eta_drifted = eta_drifted - 0.250;
    } else {
      energy_per_step = energy_per_step * (68.13 + eta_drifted * 3.333);
      eta_drifted = eta_drifted + 0.250;
    }
  }
  if (fabs(eta_drifted) < 20.200 && fabs(eta_drifted) > 20.100) {
    if (eta_drifted > 0) {
      energy_per_step = energy_per_step * (41.2 - eta_drifted * 2.0);
      eta_drifted = eta_drifted - 0.250;
    } else {
      energy_per_step = energy_per_step * (41.2 + eta_drifted * 2.0);
      eta_drifted = eta_drifted + 0.250;
    }
  }
  return StatusCode::SUCCESS;
}

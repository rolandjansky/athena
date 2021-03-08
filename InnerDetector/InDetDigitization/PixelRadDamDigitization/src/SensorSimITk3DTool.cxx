/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SensorSimITk3DTool.cxx
//   Implementation file for class SensorSimITk3DTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "SensorSimITk3DTool.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "SiDigitization/SiSurfaceCharge.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetIdentifier/PixelID.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "SiPropertiesSvc/SiliconProperties.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"

//TODO: these includes are just for settign up the ModuleDesign ID, shouldn't be needed after stuff is stored in detector store
#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"

#include "TLorentzVector.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "PathResolver/PathResolver.h"

#include "TFile.h"
#include <limits>

using namespace InDetDD;
using namespace RadDam;

//===============================================
//    C O N S T R U C T O R
//===============================================
  SensorSimITk3DTool::SensorSimITk3DTool(const std::string & type,
    const std::string & name,
    const IInterface * parent):
  SensorSimTool(type, name, parent),
  m_radDamageUtil(nullptr),
  m_numberOfSteps(50),
  m_numberOfCharges(50),
  m_diffusionConstant(.0), 
  m_doRadDamage(true), 
  m_doChunkCorrection(false),
  m_fluence(5),
  m_mapsPath(""),
  m_trappingTimeElectrons(0.),
  m_trappingTimeHoles(0.), 
  m_chargeCollSvc("ChargeCollProbSvc", name),
  m_geometry("50x50")
  {
    declareProperty("RadDamageUtil", m_radDamageUtil, "Rad Damage utility");
    declareProperty("numberOfSteps", m_numberOfSteps, "Geant4:number of steps for Pixel3D"); 
    declareProperty("numberOfCharges", m_numberOfCharges, "Geant4:number of charges for Pixel3D");
    declareProperty("diffusionConstant", m_diffusionConstant, "Geant4:Diffusion Constant for Pixel3D");
    declareProperty("doRadDamage", m_doRadDamage, "doRadDmaage bool: should be flag");
    declareProperty("doChunkCorrection", m_doChunkCorrection, "doChunkCorrection bool: should be flag");
    declareProperty("fluence", m_fluence, "this is the ITk fluence benchmark, 0-11.  0 is unirradiated (original digitizer code), 1 is unirradiated (using the unirradiated maps with the radiation damage implementation), 2-6 slowly increases the fluence with appropriate bias voltages, 7 is projected maximum ITk fluence before replacement, 8-11 is a bias voltage scan with this maximum fluence.");
    declareProperty("mapsPath", m_mapsPath, "Use a provided map string instead of one of the default benchmarks");
    declareProperty("trappingTimeElectrons", m_trappingTimeElectrons, "Characteristic time till electron is trapped [ns]");
    declareProperty("trappingTimeHoles", m_trappingTimeHoles, "Characteristic time till hole is trapped [ns]");
    declareProperty("ChargeCollProbSvc", m_chargeCollSvc);
    declareProperty("geometry", m_geometry, "String to specify geometry for the 3D sensors; must be either 50x50 (default), 25x100_2E, or 25x100_1E");
  }

  class DetCondCFloat;

  SensorSimITk3DTool::~SensorSimITk3DTool() {}

//===============================================
//    I N I T I A L I Z E
//===============================================
  StatusCode SensorSimITk3DTool::initialize() {
    CHECK(SensorSimTool::initialize());
    ATH_MSG_INFO("INITIALIZE 3D");

    ATH_MSG_DEBUG("SensorSimITk3DTool::initialize()");

    CHECK(m_radDamageUtil.retrieve());
    ATH_MSG_DEBUG("RadDamageUtil tool retrieved successfully");

    CHECK(m_chargeCollSvc.retrieve());
    ATH_MSG_DEBUG("THIS TOOL RETRIEVED SUCCESFFULLY");

    std::string mapsPath;

    if (m_fluence == 0) {

        //will run original code with no radiation damage
        return StatusCode::SUCCESS;

    } else if (m_mapsPath != "") {

        ATH_MSG_INFO("Provided custom 3D map path: will use this instead of one of the default benchmarks");

        mapsPath = m_mapsPath;

        // Read the fluence value from the given map path
        // Assumes something like /input/3D_50x50_phi_1.0e-14_20V_238K.root
        std::string first_delim = "_phi_";
        std::string second_delim = "_";
        unsigned first_flstring_pos = m_mapsPath.find(first_delim) + first_delim.length();
        unsigned second_flstring_pos = m_mapsPath.find(second_delim, first_flstring_pos);
        fluence = std::atof(mapsPath.substr(first_flstring_pos, second_flstring_pos - first_flstring_pos).c_str());
        ATH_MSG_INFO("Will assume fluence = " << fluence << " from the file name");

    } else if (m_fluence == 1) {

        mapsPath = "/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/3D_" + m_geometry + "_phi_1.0e-14_20V_238K.root";

        fluence = 1e-14;

    } else if (m_fluence == 2) {

        mapsPath = "/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/3D_" + m_geometry + "_phi_1.0e+15_50V_238K.root";

        fluence = 1e15;

    } else if (m_fluence == 3) {

        mapsPath = "/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/3D_" + m_geometry + "_phi_3.0e+15_100V_238K.root";

        fluence = 3e15;

    } else if (m_fluence == 4) {

        mapsPath = "/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/3D_" + m_geometry + "_phi_5.0e+15_160V_238K.root";

        fluence = 5e15;

    } else if (m_fluence == 5) {

        mapsPath = "/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/3D_" + m_geometry + "_phi_1.0e+16_250V_238K.root";

        fluence = 1e16;

    } else if (m_fluence == 6) {

        mapsPath = "/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/3D_" + m_geometry + "_phi_1.2e+16_250V_238K.root";

        fluence = 1.2e16;

    } else if (m_fluence == 7) {

        mapsPath = "/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/3D_" + m_geometry + "_phi_1.5e+16_250V_238K.root";

        fluence = 1.5e16;

    } else if (m_fluence == 8) {

        mapsPath = "/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/3D_" + m_geometry + "_phi_1.5e+16_200V_238K.root";

        fluence	= 1.5e16;

    } else if (m_fluence == 9) {

        mapsPath = "/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/3D_" + m_geometry + "_phi_1.5e+16_150V_238K.root";

        fluence = 1.5e16;

    } else if (m_fluence == 10) {

        mapsPath = "/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/3D_" + m_geometry + "_phi_1.5e+16_100V_238K.root";

        fluence = 1.5e16;

    } else if (m_fluence == 11) {

        mapsPath = "/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/3D_" + m_geometry + "_phi_1.5e+16_50V_238K.root";

        fluence = 1.5e16;
    }

    // *****************************
    // *** Setup Maps ****
    // *****************************
    //TODO This is only temporary until remotely stored maps and locally generated maps can be implemented 

    //NOTE: All maps generated from the efield using PixelRadDamageDigitization/share/createAllMaps.C

    ATH_MSG_INFO("Using maps located in: " << mapsPath);
//	auto mapsFile = std::make_unique<TFile>(mapsPath.c_str());
    TFile mapsFile(mapsPath.c_str());
    //Setup ramo weighting field map
    ramoPotentialMap = 0;
    ramoPotentialMap = (TH3F * ) mapsFile.Get("ramo");
    ramoPotentialMap->SetDirectory(0);
    if (ramoPotentialMap == 0) {
        ATH_MSG_INFO("Did not find a Ramo potential map.  Will use an approximate form.");
        return StatusCode::FAILURE; //Obviously, remove this when gen. code is set up
        //TODO
        //     CHECK(m_radDamageUtil->generateRamoMap( ramoPotentialMap, p_design_dummy ));
    }

    ATH_MSG_INFO("Using fluence " << fluence);

    //Now setup the E-field.
    eFieldMap = 0;
    eFieldMap = (TH2F * ) mapsFile.Get("efield");
    eFieldMap->SetDirectory(0);
    if (eFieldMap == 0) {
        ATH_MSG_INFO("Unable to load sensor e-field map, so generating one using approximations.");
        return StatusCode::FAILURE; //Obviously, remove this when gen. code is set up
        //TODO
        // CHECK(m_radDamageUtil->generateEfieldMap( eFieldMap, p_design_dummy ));
    }

    xPositionMap_e = 0;
    xPositionMap_h = 0;
    yPositionMap_e = 0;
    yPositionMap_h = 0;
    timeMap_e = 0;
    timeMap_h = 0;

    xPositionMap_e = (TH3F * ) mapsFile.Get("xPosition_e");
    xPositionMap_e->SetDirectory(0);
    xPositionMap_h = (TH3F * ) mapsFile.Get("xPosition_h");
    xPositionMap_h->SetDirectory(0);
    yPositionMap_e = (TH3F * ) mapsFile.Get("yPosition_e");
    yPositionMap_e->SetDirectory(0);
    yPositionMap_h = (TH3F * ) mapsFile.Get("yPosition_h");
    yPositionMap_h->SetDirectory(0);
    timeMap_e = (TH2F * ) mapsFile.Get("etimes");
    timeMap_e->SetDirectory(0);
    timeMap_h = (TH2F * ) mapsFile.Get("htimes");
    timeMap_h->SetDirectory(0);
    //Now, determine the time to reach the electrode and the trapping position.
    if (xPositionMap_e == 0 || xPositionMap_h == 0 || yPositionMap_e == 0 || yPositionMap_h == 0 || timeMap_e == 0 || timeMap_h == 0) {

        ATH_MSG_INFO("Unable to load at least one of teh distance/time maps, so generating all using approximations.");
        return StatusCode::FAILURE; //Obviously, remove this when gen. code is set up
        //TODO
        //CHECK(m_radDamageUtil->generateDistanceTimeMap( distanceMap_e, distanceMap_h, timeMap_e, timeMap_h, eFieldMap, p_design_dummy ));
    }

    // Get average charge data (for charge chunk effect correction)
    avgChargeMap_e=0;
    avgChargeMap_h=0;
    avgChargeMap_e=(TH2F*)mapsFile.Get("avgCharge_e");
    avgChargeMap_e->SetDirectory(0);
    avgChargeMap_h=(TH2F*)mapsFile.Get("avgCharge_h");
    avgChargeMap_h->SetDirectory(0);
    if (avgChargeMap_e == 0 || avgChargeMap_h == 0) {
        ATH_MSG_INFO("Unsuccessful picking up histogram: avgChargeMap");
    }


    return StatusCode::SUCCESS;
}

//===============================================
//    F I N A L I Z E
//===============================================
StatusCode SensorSimITk3DTool::finalize() {
    ATH_MSG_DEBUG("SensorSimITk3DTool::finalize()");
    return StatusCode::SUCCESS;
}

//===============================================
//    I N D U C E    C H A R G E
//===============================================
StatusCode SensorSimITk3DTool::induceCharge(const TimedHitPtr < SiHit > & phit, SiChargedDiodeCollection & chargedDiodes,
    const InDetDD::SiDetectorElement & Module,
    const InDetDD::PixelModuleDesign & p_design, std::vector < std::pair < double, double > > & trfHitRecord, std::vector < double > & initialConditions) {
//    return StatusCode::SUCCESS;
    if (!Module.isBarrel()) {
        return StatusCode::SUCCESS;
    }

    ATH_MSG_DEBUG(p_design.getReadoutTechnology() << " " << p_design.numberOfCircuits());

    //what should this be? for ITK 3D? : seems to be 2 for 50x50 default ... disable for now 
//    if (p_design.getReadoutTechnology() != InDetDD::PixelModuleDesign::FEI4) {
//        return StatusCode::SUCCESS;
//    }

//    if (p_design.numberOfCircuits() > 1) {	// seems to be 1 for default
//        return StatusCode::SUCCESS;
//    }

    if (Module.isInclined()) return StatusCode::SUCCESS;
    const PixelID* p_pixelId = static_cast<const PixelID *>(Module.getIdHelper());
    int layer=p_pixelId->layer_disk(Module.identify() );
    if (layer != 0) return StatusCode::SUCCESS; //150 x 50 x 50 3D

    ATH_MSG_DEBUG("readout technology: " << p_design.getReadoutTechnology() << " number of circuits: " << p_design.numberOfCircuits());
    if (p_design.getReadoutTechnology() != 2 || p_design.numberOfCircuits() != 1) {ATH_MSG_INFO("unexpected number of circuits");}

    ATH_MSG_DEBUG("Applying SensorSim3D charge processor");
    if (initialConditions.size() != 8) {
        ATH_MSG_INFO("ERROR! Starting coordinates were not filled correctly in EnergyDepositionSvc.");
        return StatusCode::FAILURE;
    }

    if (m_fluence == 0) m_doRadDamage = 0;

    //Calculate trapping times based on fluence (already includes check for fluence=0)
    if (m_doRadDamage) {
        std::pair < double, double > trappingTimes = m_radDamageUtil->getTrappingTimes(fluence);   //0 = IBL
        m_trappingTimeElectrons = trappingTimes.first;
        m_trappingTimeHoles = trappingTimes.second;
        ATH_MSG_DEBUG("For fluence = " << fluence << ", TrappingTimeElectrons = " << m_trappingTimeElectrons << ", TrappingTimeHoles = " << m_trappingTimeHoles);
    }
    


    double eta_0 = initialConditions[0];
    double phi_0 = initialConditions[1];
    double depth_0 = initialConditions[2];
    double dEta = initialConditions[3];
    double dPhi = initialConditions[4];
    double dDepth = initialConditions[5];
    double ncharges = initialConditions[6];
    double iTotalLength = initialConditions[7];
    ncharges = 1;		//temporary for now just to test ITk
    temperature = 300; // K 

    ATH_MSG_VERBOSE("Applying 3D sensor simulation.");
    double sensorThickness = Module.design().thickness();
    const InDet::SiliconProperties & siProperties = m_siPropertiesSvc->getSiProperties(Module.identifyHash());
    double eleholePairEnergy = siProperties.electronHolePairsPerEnergy();   //  = 1 / 3.6 eV -> expressed in MeV^-1 = 276243 MeV^-1

    // Charge Collection Probability Map bin size
    const double x_bin_size = 0.001;
    const double y_bin_size = 0.001;

    std::string readout;

    // determine which readout is used
    // FEI4 : 50 X 250 microns (variable is in units of mm)
    double pixel_size_x = Module.width() / p_design.rows();
    double pixel_size_y = Module.length() / p_design.columns();
    double module_size_x = Module.width();
    double module_size_y = Module.length();

    ATH_MSG_DEBUG("pixel pitch: " << pixel_size_x << " " << pixel_size_y);

    double pixel_size_x_half = pixel_size_x * 0.5;
    double pixel_size_y_half = pixel_size_y * 0.5;
    double module_size_x_half = module_size_x * 0.5;
    double module_size_y_half = module_size_y * 0.5;

    if (m_doRadDamage && m_fluence>0) {

        if (!(eFieldMap->GetXaxis()->FindBin(pixel_size_y * 1000) == eFieldMap->GetNbinsX() + 1) || !(eFieldMap->GetYaxis()->FindBin(pixel_size_x * 1000) == eFieldMap->GetNbinsY() + 1)) {
            ATH_MSG_ERROR("dimensions for precomputed maps and the pixel don't match!");
        }


    //**************************************//
    //*** Now diffuse charges to surface *** //
    //**************************************//
        for (const auto & iHitRecord : trfHitRecord) {
            double total_event_charge = 0;

            double eta_i = eta_0;
            double phi_i = phi_0;
            double depth_i = depth_0;

            if (iTotalLength) {
                double iHitRecord_temp = iHitRecord.first / iTotalLength;
                eta_i += iHitRecord_temp * dEta;
                phi_i += iHitRecord_temp * dPhi;
                depth_i += iHitRecord_temp * dDepth;
            }

            double energy_per_step = iHitRecord.second / 1.E+6 / ncharges;    //in MeV
            ATH_MSG_DEBUG("es_current: " << energy_per_step << " split between " << ncharges << " charges");

            double dist_electrode = 0.5 * sensorThickness - Module.design().readoutSide() * depth_i;
            if (dist_electrode < 0) dist_electrode = 0;

            CLHEP::Hep3Vector chargepos;
            chargepos.setX(phi_i);
            chargepos.setY(eta_i);
            chargepos.setZ(dist_electrode);

            bool coord = Module.isModuleFrame();

            ATH_MSG_DEBUG("ismoduleframe " << coord << " -- startPosition (x,y,z) = " << chargepos.x() << ", " << chargepos.y() << ", " << chargepos.z());

            // -- change origin of coordinates to the left bottom of module
            double x_new = chargepos.x() + module_size_x_half;
            double y_new = chargepos.y() + module_size_y_half;


            // -- change from module frame to pixel frame
            int nPixX = int(x_new / pixel_size_x);  
            int nPixY = int(y_new / pixel_size_y);
            ATH_MSG_DEBUG(" -- nPixX = " << nPixX << "  nPixY = " << nPixY);
            // In case the charge moves into a neighboring pixel
            int extraNPixX = nPixX;
            int extraNPixY = nPixY;
            double dExtraNPixX = 0;
            double dExtraNPixY = 0;

            //position relative to the bottom left corner of the pixel
            double x_pix = x_new - pixel_size_x * (nPixX);  
            double y_pix = y_new - pixel_size_y * (nPixY);
            // -- change origin of coordinates to the center of the pixel
            double x_pix_center = x_pix - pixel_size_x_half;
            double y_pix_center = y_pix - pixel_size_y_half;
            ATH_MSG_DEBUG(" -- current hit position w.r.t. pixel corner = " << x_pix << "  " << y_pix);
            ATH_MSG_DEBUG(" -- current hit position w.r.t. pixel center = " << x_pix_center << "  " << y_pix_center);

            //only process hits which are not on the electrodes (E-field zero)
            //all the maps have 250 as the x value, so need to invert x and y whenever reading maps
            double efield = getElectricField(y_pix, x_pix);                                                
            if (efield == 0) {
                ATH_MSG_DEBUG("Skipping since efield = 0 for x_pix = " << x_pix << " y_pix = " << y_pix);
                continue;
            }

            //Loop over charge-carrier pairs
            for (int j = 0; j < ncharges; j++) {

                if (m_doRadDamage && m_fluence > 0) {

                    double eHit = energy_per_step;
                    //Need to determine how many elementary charges this charge chunk represents.
                    double chunk_size = energy_per_step*eleholePairEnergy; //number of electrons/holes
                    ATH_MSG_DEBUG("Chunk size: " << energy_per_step << "*" << eleholePairEnergy << " = " << chunk_size);

                    //set minimum limit to prevent dividing into smaller subcharges than one fundamental charge
                    if (chunk_size < 1) chunk_size = 1;
                    double kappa = 1./std::sqrt(chunk_size);

                    double inducedECharge = 0;
                    double inducedHCharge = 0;
                    double inducedPairCharge = 0;
                    int doBinCorrection = 0;

                    // Sometimes the full charge is not collected, even when drift time > time to electrode
                    // probably because of the granularity issues in the Efield/ramo maps
                    // this is just a quick fix to that problem

                    if (getDriftTime(0) > getTimeToElectrode(y_pix, x_pix, 0) && getDriftTime(1) > getTimeToElectrode(y_pix, x_pix, 1)){
                                doBinCorrection = 1;
                    }

                    // Loop over everything twice: once for electrons and once for holes
                    for (int eholes = 0; eholes < 2; eholes++) {
                        bool isHole = false; // Set a condition to keep track of electron/hole-specific functions
                        if (eholes == 1) isHole = true;
                
                        // Reset extraPixel coordinates each time through loop
                        extraNPixX = nPixX;
                        extraNPixY = nPixY;
                        dExtraNPixX = 0;
                        dExtraNPixY = 0;

                        double timeToElectrode = getTimeToElectrode(y_pix, x_pix, isHole);
                        double driftTime = getDriftTime(isHole);

                        //Apply drift due to diffusion
                        double phiRand = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);

                        //Apply diffusion. rdif is teh max. diffusion
                        double Dt = getMobility(efield, isHole) * (0.024) * std::min(driftTime, timeToElectrode) * temperature / 273.;
                        double rdif = std::sqrt(Dt) / 1000; //in mm
                        double xposDiff = x_pix + rdif * phiRand;
                        xposDiff = x_pix;	//Diffusion: disabled for now
                        double etaRand = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);
                        double yposDiff = y_pix + rdif * etaRand;
                        yposDiff = y_pix;
                        // Account for drifting into another pixel 
                        dExtraNPixX = std::floor((xposDiff/pixel_size_x)-std::numeric_limits<double>::epsilon());
                        extraNPixX += dExtraNPixX;
                        xposDiff -= (dExtraNPixX * pixel_size_x);
                        x_pix -= (dExtraNPixX * pixel_size_x);
    
                        dExtraNPixY = std::floor((yposDiff/pixel_size_y)-std::numeric_limits<double>::epsilon());
                        extraNPixY += dExtraNPixY;
                        yposDiff -= (dExtraNPixY * pixel_size_y);
                        y_pix -= (dExtraNPixY * pixel_size_y);

                        ATH_MSG_DEBUG(" -- diffused position w.r.t. pixel edge = " << xposDiff << "  " << yposDiff);

                        double average_charge = avgChargeMap_e->GetBinContent(avgChargeMap_e->GetYaxis()->FindBin(y_pix*1000),avgChargeMap_e->GetXaxis()->FindBin(x_pix*1000)); 
                        if (isHole) average_charge = avgChargeMap_h->GetBinContent(avgChargeMap_h->GetYaxis()->FindBin(y_pix*1000),avgChargeMap_h->GetXaxis()->FindBin(x_pix*1000));

                        ATH_MSG_DEBUG(" -- driftTime, timeToElectrode = " << driftTime << "  " << timeToElectrode);    

                        double xposFinal = getTrappingPositionY(yposDiff, xposDiff, std::min(driftTime,timeToElectrode), isHole);
                        double yposFinal = getTrappingPositionX(yposDiff, xposDiff, std::min(driftTime,timeToElectrode), isHole);

                        ATH_MSG_DEBUG(" -- trapped position w.r.t. pixel edge = " << xposFinal << "  " << yposFinal);


                        // -- Calculate signal in current pixel and in the neighboring ones
                        // -- loop in the x-coordinate
                        for (int i = -1; i <= 1; i++) {
                            double xNeighbor = i * pixel_size_x;
                            // -- loop in the y-coordinate
                            for (int j = -1; j <= 1; j++) {
                                double yNeighbor = j * pixel_size_y;

                                double ramo_x_factor = 1.5;
                                double ramo_y_factor = 1.5;
                                if (m_geometry == "25x100_2E") {
                                    ramo_x_factor = 3;
                                    ramo_y_factor = 1;
                                } else if (m_geometry == "25x100_1E") {
                                    ramo_x_factor = 2;
                                    ramo_y_factor = 1;
                                }
                                ATH_MSG_DEBUG(" -- Ramo init position w.r.t. Ramo map edge = " << x_pix+pixel_size_x*ramo_x_factor-xNeighbor << "  " << y_pix+pixel_size_y*ramo_y_factor-yNeighbor);
                                ATH_MSG_DEBUG(" -- Ramo final position w.r.t. Ramo map edge = " << xposFinal+pixel_size_x*ramo_x_factor-xNeighbor << "  " << yposFinal+pixel_size_y*ramo_y_factor-yNeighbor);

                                //Ramo map over 500umx350um pixel area
                                //Ramo init different if charge diffused into neighboring pixel -> change primary pixel!!
                                double ramoInit = getRamoPotential(y_pix+pixel_size_y*ramo_y_factor-yNeighbor,x_pix+pixel_size_x*ramo_x_factor-xNeighbor);	//change from 0.5, 3 to 1.5, 1.5 for ITK ramo simulation
                                double ramoFinal = getRamoPotential(yposFinal+pixel_size_y*ramo_y_factor-yNeighbor,xposFinal+pixel_size_x*ramo_x_factor-xNeighbor);

                                // Record deposit
                                double eHitRamo = (1-2*isHole)*eHit*(ramoFinal - ramoInit);			
                                //doBinCorrection = 0;
                                if (doBinCorrection){
                                    ATH_MSG_DEBUG("Adjusting values to ensure the full charge is collected since driftTime = " << driftTime << ", timeToElectrode = " <<timeToElectrode);
                                    if (i == 0 && j == 0){				//only consider primary pixel
                                        eHitRamo = (1 - isHole) * eHit;		//give full charge to electron, nothing to holes
                                    } else {
                                        eHitRamo = 0;
                                    }					
                                }
                                ATH_MSG_DEBUG("At neighbor pixel " <<  i << " " << j << " Hit of " << eHitRamo << " including Ramo factor: " << ramoFinal - ramoInit);

                                if (m_doChunkCorrection) {
                                    ATH_MSG_DEBUG("Energy before chunk correction: " << eHitRamo);
                                    eHitRamo = eHit*average_charge + kappa*(eHitRamo - eHit*average_charge);
                                    ATH_MSG_DEBUG("Energy after chunk correction: " << eHitRamo);
                                }

                                double induced_charge = eHitRamo * eleholePairEnergy;

                                // -- pixel coordinates --> module coordinates
                                double x_mod = x_pix + xNeighbor + pixel_size_x * extraNPixX - module_size_x_half; 
                                double y_mod = y_pix + yNeighbor + pixel_size_y * extraNPixY - module_size_y_half;
                                SiLocalPosition chargePos = Module.hitLocalToLocal(y_mod, x_mod);

                                SiSurfaceCharge scharge(chargePos, SiCharge(induced_charge, hitTime(phit), SiCharge::track, HepMcParticleLink(phit->trackNumber(), phit.eventId())));
                                SiCellId diode = Module.cellIdOfPosition(scharge.position());
                                SiCharge charge = scharge.charge();
                                if (diode.isValid()) {
                                    chargedDiodes.add(diode, charge);
                                    ATH_MSG_DEBUG("induced charge: " << induced_charge << " x_mod: " << x_mod << " y_mod: " << y_mod);
                                    if (isHole) {inducedHCharge += induced_charge;}
                                    else {inducedECharge += induced_charge;}
                                    if (induced_charge > eHit * eleholePairEnergy){
                                        ATH_MSG_WARNING("induced_charge = " << induced_charge << " > initial charge = " << eHit * eleholePairEnergy << "; this should never happen!");
                                    }
                                    inducedPairCharge += induced_charge;
                                    total_event_charge += induced_charge;
                                }   

                            }
                        }
                    }
                ATH_MSG_DEBUG("induced pair charge: " << inducedPairCharge);

                }
            }
        ATH_MSG_DEBUG("Total event charge: " << total_event_charge);

        }        
        return StatusCode::SUCCESS;
    } else {


      //**************************************//
      //*** Now diffuse charges to surface *** //
      //**************************************//

        for(const auto & iHitRecord : trfHitRecord) {
            double total_event_charge = 0;

            double eta_i = eta_0;
            double phi_i = phi_0;
            double depth_i = depth_0;

            if (iTotalLength) {
                double iHitRecord_temp = iHitRecord.first / iTotalLength;
                eta_i += iHitRecord_temp*dEta;
                phi_i += iHitRecord_temp*dPhi;
                depth_i  += iHitRecord_temp*dDepth;
            }

            double es_current = iHitRecord.second*1.E-6;
            ATH_MSG_DEBUG("es_current: " << es_current);
            double dist_electrode = 0.5 * sensorThickness - Module.design().readoutSide() * depth_i;
            if (dist_electrode<0) dist_electrode=0;

            CLHEP::Hep3Vector chargepos;
            chargepos.setX(phi_i); chargepos.setY(eta_i);  chargepos.setZ(dist_electrode);

            bool coord = Module.isModuleFrame();

            ATH_MSG_DEBUG("ismoduleframe "<<coord << " -- startPosition (x,y,z) = " << chargepos.x() << ", " << chargepos.y() << ", " << chargepos.z());

            // -- change origin of coordinates to the left bottom of module
            double x_new = chargepos.x() + module_size_x_half;
            double y_new = chargepos.y() + module_size_y_half;

            // -- change from module frame to pixel frame
            int nPixX = int(x_new/pixel_size_x);
            int nPixY = int(y_new/pixel_size_y);
            ATH_MSG_DEBUG(" -- nPixX = "<<nPixX<<"  nPixY = "<<nPixY);    
            double x_pix = x_new - pixel_size_x*(nPixX);
            double y_pix = y_new - pixel_size_y*(nPixY);
            // -- change origin of coordinates to the center of the pixel
            double x_pix_center = x_pix - pixel_size_x_half;
            double y_pix_center = y_pix - pixel_size_y_half;
            ATH_MSG_DEBUG(" -- current hit position w.r.t. pixel center = "<<x_pix_center<<"  "<<y_pix_center);    
            double x_neighbor;    double y_neighbor;   CLHEP::Hep3Vector pos_neighbor;
            // -- Calculate signal in current pixel and in the neighboring ones
            // -- loop in the x-coordinate
            for (int i=-1; i<=1; i++){
                x_neighbor = x_pix_center - i*pixel_size_x;
                // -- loop in the y-coordinate
                for (int j=-1; j<=1; j++){
                    y_neighbor = y_pix_center - j*pixel_size_y;
     
                    // -- check if the neighbor falls inside the charge collection prob map window
                    if ( (std::abs(x_neighbor)<pixel_size_x) && (std::abs(y_neighbor)<pixel_size_y) ){
                        // -- change origin of coordinates to the bottom left of the charge
                        //    collection prob map "window", i.e. shift of 1-pixel twd bottom left
                        double x_neighbor_map = x_neighbor + pixel_size_x;
                        double y_neighbor_map = y_neighbor + pixel_size_y;

                        int x_bin_cc_map = static_cast<int>(x_neighbor_map / x_bin_size);
                        int y_bin_cc_map = static_cast<int>(y_neighbor_map / y_bin_size);

                        // -- retrieve the charge collection probability from Svc
                        // -- swap x and y bins to match Map coord convention
                        double ccprob_neighbor = m_chargeCollSvc->getProbMapEntry("FEI4",y_bin_cc_map,x_bin_cc_map);
                        if ( ccprob_neighbor == -1. ) return StatusCode::FAILURE;
                        if (i == 0 && j == 0) {ccprob_neighbor = 1;}
                        else {ccprob_neighbor = 0;}
            
                        //not sure what ccprob_neighbor is, but I think it doesn't work for ITK...
                        double ed=es_current*eleholePairEnergy*ccprob_neighbor;
                        ATH_MSG_DEBUG("es_current: " << es_current << " eleholePairEnergy: " << eleholePairEnergy << " ccprob_neighbor: " << ccprob_neighbor);

                        // -- pixel coordinates --> module coordinates
                        double x_mod = x_neighbor + pixel_size_x_half + pixel_size_x*nPixX -module_size_x_half;
                        double y_mod = y_neighbor + pixel_size_y_half + pixel_size_y*nPixY -module_size_y_half;
                        SiLocalPosition chargePos = Module.hitLocalToLocal(y_mod,x_mod);

                        SiSurfaceCharge scharge(chargePos,SiCharge(ed,hitTime(phit),SiCharge::track,HepMcParticleLink(phit->trackNumber(),phit.eventId())));
                        SiCellId diode = Module.cellIdOfPosition(scharge.position());
                        SiCharge charge = scharge.charge();
                        if (diode.isValid()) {
                            chargedDiodes.add(diode,charge);
                            ATH_MSG_DEBUG("induced charge: " << ed << " x_mod: " << x_mod << " y_mod: " << y_mod);
                            total_event_charge += ed;
                        }
                    }
                }
            }
        ATH_MSG_DEBUG("Total event charge: " << total_event_charge);

        }
        

        return StatusCode::SUCCESS;

    }

}

StatusCode SensorSimITk3DTool::applySlimEdges(double & energy_per_step, double & eta_drifted) {

    if (std::abs(eta_drifted) > 20.440) energy_per_step = 0.;
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
            energy_per_step = energy_per_step * (41.2 - eta_drifted * 2.);
            eta_drifted = eta_drifted - 0.250;
        } else {
            energy_per_step = energy_per_step * (41.2 + eta_drifted * 2.);
            eta_drifted = eta_drifted + 0.250;
        }
    }

    return StatusCode::SUCCESS;

}

double SensorSimITk3DTool::getElectricField(double x, double y) {

    int n_binx = eFieldMap->GetXaxis()->FindBin(x * 1000); //position coordinates in um to return electric field in V/cm
    int n_biny = eFieldMap->GetYaxis()->FindBin(y * 1000);
    double electricField = eFieldMap->GetBinContent(n_binx, n_biny);
    return electricField * 1.0E-7; //return efield in MV/mm (for mobility calculation)
}

double SensorSimITk3DTool::getMobility(double electricField, bool isHoleBit) {
    //Not exactly the same as the getMobility function in RadDamageUtil, since we don't have a Hall effect for 3D sensors (B and E are parallel)
    //Maybe good to do something about this in the future

    double vsat = 0;
    double ecrit = 0;
    double beta = 0;

    //These parameterizations come from C. Jacoboni et al., Solid‐State Electronics 20 (1977) 77‐89. (see also https://cds.cern.ch/record/684187/files/indet-2001-004.pdf).

    if (!isHoleBit) {
        vsat = 15.3 * std::pow(temperature, -0.87); // mm/ns
        ecrit = 1.01E-7 * std::pow(temperature, 1.55); // MV/mm
        beta = 2.57E-2 * std::pow(temperature, 0.66);
    }
    if (isHoleBit) {
        vsat = 1.62 * std::pow(temperature, -0.52); // mm/ns
        ecrit = 1.24E-7 * std::pow(temperature, 1.68); // MV/mm
        beta = 0.46 * std::pow(temperature, 0.17);
    }

    double mobility = (vsat / ecrit) / std::pow(1 + std::pow((electricField / ecrit), beta), (1 / beta));
    return mobility; // mm^2/(MV*ns)
}

double SensorSimITk3DTool::getDriftTime(bool isHoleBit) {
    double u = CLHEP::RandFlat::shoot(0., 1.); // 
    double driftTime = 0;

    if (!isHoleBit) driftTime = (-1.) * m_trappingTimeElectrons * std::log(u); // ns
    if (isHoleBit) driftTime = (-1.) * m_trappingTimeHoles * std::log(u); // ns
    return driftTime;
}

double SensorSimITk3DTool::getTimeToElectrode(double x, double y, bool isHoleBit) {

    // Uses (x,y) position in um to return time to electrode in ns
    double timeToElectrode = 0;
    if (!isHoleBit) {
        int n_binx = timeMap_e->GetXaxis()->FindBin(x * 1000); //convert from mm to um
        int n_biny = timeMap_e->GetYaxis()->FindBin(y * 1000);
        timeToElectrode = timeMap_e->GetBinContent(n_binx, n_biny);
    }
    if (isHoleBit) {
        int n_binx = timeMap_h->GetXaxis()->FindBin(x * 1000);
        int n_biny = timeMap_h->GetYaxis()->FindBin(y * 1000);
        timeToElectrode = timeMap_h->GetBinContent(n_binx, n_biny);
    }
    return timeToElectrode; //[ns]
}

double SensorSimITk3DTool::getTrappingPositionX(double initX, double initY, double driftTime, bool isHoleBit) {

    double finalX = initX;
    if (!isHoleBit) {
        int n_binx = xPositionMap_e->GetXaxis()->FindBin(initX * 1000);
        int n_biny = xPositionMap_e->GetYaxis()->FindBin(initY * 1000);
        int n_bint = xPositionMap_e->GetZaxis()->FindBin(driftTime);
        finalX = xPositionMap_e->GetBinContent(n_binx, n_biny, n_bint);

    } else {
        int n_binx = xPositionMap_h->GetXaxis()->FindBin(initX * 1000);
        int n_biny = xPositionMap_h->GetYaxis()->FindBin(initY * 1000);
        int n_bint = xPositionMap_h->GetZaxis()->FindBin(driftTime);
        finalX = xPositionMap_h->GetBinContent(n_binx, n_biny, n_bint);

    }

    return finalX * 1e-3; //[mm]
}

double SensorSimITk3DTool::getTrappingPositionY(double initX, double initY, double driftTime, bool isHoleBit) {

    double finalY = initY;
    if (!isHoleBit) {
        int n_binx = yPositionMap_e->GetXaxis()->FindBin(initX * 1000);
        int n_biny = yPositionMap_e->GetYaxis()->FindBin(initY * 1000);
        int n_bint = yPositionMap_e->GetZaxis()->FindBin(driftTime);
        finalY = yPositionMap_e->GetBinContent(n_binx, n_biny, n_bint);
    } else {
        int n_binx = yPositionMap_h->GetXaxis()->FindBin(initX * 1000);
        int n_biny = yPositionMap_h->GetYaxis()->FindBin(initY * 1000);
        int n_bint = yPositionMap_h->GetZaxis()->FindBin(driftTime);
        finalY = yPositionMap_h->GetBinContent(n_binx, n_biny, n_bint);
    }

    return finalY * 1e-3; //[mm]
}

double SensorSimITk3DTool::getRamoPotential(double x, double y){

    int n_binx = ramoPotentialMap->GetXaxis()->FindBin(x*1000);
    int n_biny = ramoPotentialMap->GetYaxis()->FindBin(y*1000);
    double ramoPotential = ramoPotentialMap->GetBinContent(n_binx,n_biny);
    return ramoPotential;
}



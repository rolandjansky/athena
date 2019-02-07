/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SensorSim3DTool.cxx
//   Implementation file for class SensorSim3DTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "SensorSim3DTool.h"
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

//Temp includes : check if these are needed TODO
#include "TMath.h"
#include "TFile.h"
#include "TH3F.h"
#include "TH2.h"
#include "TH1.h"

  using namespace InDetDD;
  using namespace RadDam;

//===============================================
//    C O N S T R U C T O R
//===============================================
  SensorSim3DTool::SensorSim3DTool(const std::string & type,
    const std::string & name,
    const IInterface * parent):
  SensorSimTool(type, name, parent),
  m_radDamageUtil(nullptr),
  m_numberOfSteps(50),
  m_numberOfCharges(50),
  m_diffusionConstant(.0), 
  m_doRadDamage(false), 
  m_doChunkCorrection(false),
  m_fluence(5),
  m_trappingTimeElectrons(0.),
  m_trappingTimeHoles(0.), 
  m_chargeCollSvc("ChargeCollProbSvc", name)
  {
    declareProperty("RadDamageUtil", m_radDamageUtil, "Rad Damage utility");
    declareProperty("numberOfSteps", m_numberOfSteps, "Geant4:number of steps for Pixel3D"); 
    declareProperty("numberOfCharges", m_numberOfCharges, "Geant4:number of charges for Pixel3D");
    declareProperty("diffusionConstant", m_diffusionConstant, "Geant4:Diffusion Constant for Pixel3D");
    declareProperty("doRadDamage", m_doRadDamage, "doRadDmaage bool: should be flag");
    declareProperty("doChunkCorrection", m_doChunkCorrection, "doChunkCorrection bool: should be flag");
    declareProperty("fluence", m_fluence, "this is the fluence benchmark, 0-6.  0 is unirradiated, 1 is start of Run 2, 5 is end of 2018 and 6 is projected end of 2018");
    declareProperty("trappingTimeElectrons", m_trappingTimeElectrons, "Characteristic time till electron is trapped [ns]");
    declareProperty("trappingTimeHoles", m_trappingTimeHoles, "Characteristic time till hole is trapped [ns]");
    declareProperty("ChargeCollProbSvc", m_chargeCollSvc);
  }

  class DetCondCFloat;

  SensorSim3DTool::~SensorSim3DTool() {}

//===============================================
//    I N I T I A L I Z E
//===============================================
  StatusCode SensorSim3DTool::initialize() {
    CHECK(SensorSimTool::initialize());
    ATH_MSG_DEBUG("SensorSim3DTool::initialize()");

    CHECK(m_radDamageUtil.retrieve());
    ATH_MSG_DEBUG("RadDamageUtil tool retrieved successfully");

    CHECK(m_chargeCollSvc.retrieve());
    ATH_MSG_DEBUG("THIS TOOL RETRIEVED SUCCESFFULLY");

    std::vector < std::string > mapsPath_list;

    if (m_fluence == 0) {

    //will run original code with no radiation damage

    } else if (m_fluence == 1) {

        mapsPath_list.push_back(PathResolverFindCalibFile("/afs/cern.ch/user/v/vewallan/public/TCADmaps/outputfiles/phi_0_20V.root")); 

        fluence_layers.push_back(1e-10);

    } else if (m_fluence == 2) {

        mapsPath_list.push_back(PathResolverFindCalibFile("/afs/cern.ch/user/v/vewallan/public/TCADmaps/outputfiles/phi_1e14_20V.root"));

        fluence_layers.push_back(1e14);

    } else if (m_fluence == 3) {

        mapsPath_list.push_back(PathResolverFindCalibFile("/afs/cern.ch/user/v/vewallan/public/TCADmaps/outputfiles/phi_2e14_30V.root"));

        fluence_layers.push_back(2e14);

    } else if (m_fluence == 4) {

        mapsPath_list.push_back(PathResolverFindCalibFile("/afs/cern.ch/user/v/vewallan/public/TCADmaps/outputfiles/phi_5e14_40V.root")); 

        fluence_layers.push_back(5e14);

    } else if (m_fluence == 5) {

        mapsPath_list.push_back(PathResolverFindCalibFile("/afs/cern.ch/user/v/vewallan/public/TCADmaps/outputfiles/phi_1e15_50V.root"));

        fluence_layers.push_back(1e15); 

    } else if (m_fluence == 6) {

        mapsPath_list.push_back(PathResolverFindCalibFile("/afs/cern.ch/user/v/vewallan/public/TCADmaps/outputfiles/phi_5e15_160V.root"));

        fluence_layers.push_back(5e15);

    } else if (m_fluence == 7) {

        mapsPath_list.push_back(PathResolverFindCalibFile("/afs/cern.ch/user/v/vewallan/public/TCADmaps/outputfiles/phi_6e15_190V_new.root"));       

        fluence_layers.push_back(6e15);

    } else if (m_fluence == 8) {

        mapsPath_list.push_back(PathResolverFindCalibFile("/afs/cern.ch/user/v/vewallan/public/TCADmaps/outputfiles/phi_1e16_260V_new.root"));

        fluence_layers.push_back(1e16);
    }

    // *****************************
    // *** Setup Maps ****
    // *****************************
    //TODO This is only temporary until remotely stored maps and locally generated maps can be implemented 

    for (unsigned int i = 0; i < mapsPath_list.size(); i++) {

        ATH_MSG_INFO("Using maps located in: " << mapsPath_list.at(i));
        //std::unique_ptr<TFile>  mapsFile=std::make_unique<TFile>( (mapsPath_list.at(i)).c_str() ); //this is the ramo potential.
        TFile * mapsFile = new TFile((mapsPath_list.at(i)).c_str()); //this is the ramo potential.

        std::pair < int, int > Layer; // index for layer/end cap position
        Layer.first = 0; //Barrel (0) or End Cap (1)   -    Now only for Barrel. If we want to add End Caps, put them at Layer.first=1
        Layer.second = i; //Layer: 0 = IBL Planar, 1=B-Layer, 2=Layer-1, 3=Layer-2
        //For 3D sensor, only possible index should be 0-0
        //May want to be changed in the future, since there's no point having an index with only one possible value

        //Setup ramo weighting field map
        TH3F * ramoPotentialMap_hold;
        ramoPotentialMap_hold = 0;
        ramoPotentialMap_hold = (TH3F * ) mapsFile->Get("ramo");
        if (ramoPotentialMap_hold == 0) {
            ATH_MSG_INFO("Did not find a Ramo potential map.  Will use an approximate form.");
            return StatusCode::FAILURE; //Obviously, remove this when gen. code is set up
            //TODO
            //     CHECK(m_radDamageUtil->generateRamoMap( ramoPotentialMap, p_design_dummy ));
        }
        //ramoPotentialMap.push_back(ramoPotentialMap_hold);
        ramoPotentialMap[Layer] = ramoPotentialMap_hold;
        fluence_layersMaps[Layer] = fluence_layers.at(i);

        ATH_MSG_INFO("Using fluence " << fluence_layers.at(i));

        //Now setup the E-field.
        TH2F * eFieldMap_hold;
        eFieldMap_hold = 0;
        eFieldMap_hold = (TH2F * ) mapsFile->Get("efield");
        if (eFieldMap_hold == 0) {
            ATH_MSG_INFO("Unable to load sensor e-field map, so generating one using approximations.");
            return StatusCode::FAILURE; //Obviously, remove this when gen. code is set up
            //TODO
            // CHECK(m_radDamageUtil->generateEfieldMap( eFieldMap, p_design_dummy ));
        }
        //eFieldMap.push_back(eFieldMap_hold);
        eFieldMap[Layer] = eFieldMap_hold;

        TH3F * xPositionMap_e_hold;
        TH3F * xPositionMap_h_hold;
        TH3F * yPositionMap_e_hold;
        TH3F * yPositionMap_h_hold;
        TH2F * timeMap_e_hold;
        TH2F * timeMap_h_hold;

        xPositionMap_e_hold = 0;
        xPositionMap_h_hold = 0;
        yPositionMap_e_hold = 0;
        yPositionMap_h_hold = 0;
        timeMap_e_hold = 0;
        timeMap_h_hold = 0;
        xPositionMap_e_hold = (TH3F * ) mapsFile->Get("xPosition_e");
        xPositionMap_h_hold = (TH3F * ) mapsFile->Get("xPosition_h");
        yPositionMap_e_hold = (TH3F * ) mapsFile->Get("yPosition_e");
        yPositionMap_h_hold = (TH3F * ) mapsFile->Get("yPosition_h");
        timeMap_e_hold = (TH2F * ) mapsFile->Get("etimes");
        timeMap_h_hold = (TH2F * ) mapsFile->Get("htimes");
        //Now, determine the time to reach the electrode and the trapping position.
        if (xPositionMap_e_hold == 0 || xPositionMap_h_hold == 0 || yPositionMap_e_hold == 0 || yPositionMap_h_hold == 0 || timeMap_e_hold == 0 || timeMap_h_hold == 0) {

            ATH_MSG_INFO("Unable to load at least one of teh distance/time maps, so generating all using approximations.");
            return StatusCode::FAILURE; //Obviously, remove this when gen. code is set up
            //TODO
            //CHECK(m_radDamageUtil->generateDistanceTimeMap( distanceMap_e, distanceMap_h, timeMap_e, timeMap_h, eFieldMap, p_design_dummy ));
        }
        xPositionMap_e[Layer] = xPositionMap_e_hold;
        xPositionMap_h[Layer] = xPositionMap_h_hold;
        yPositionMap_e[Layer] = yPositionMap_e_hold;
        yPositionMap_h[Layer] = yPositionMap_h_hold;
        timeMap_e[Layer] = timeMap_e_hold;
        timeMap_h[Layer] = timeMap_h_hold;

        // Get average charge data (for charge chunk effect correction)
        avgChargeMap_e=0;
        avgChargeMap_h=0;
        avgChargeMap_e=(TH2F*)mapsFile->Get("avgCharge_e");
        avgChargeMap_h=(TH2F*)mapsFile->Get("avgCharge_h");
        if (avgChargeMap_e == 0 || avgChargeMap_h == 0) {
            ATH_MSG_INFO("Unsuccessful picking up histogram: avgChargeMap");
        }
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
StatusCode SensorSim3DTool::induceCharge(const TimedHitPtr < SiHit > & phit, SiChargedDiodeCollection & chargedDiodes,
    const InDetDD::SiDetectorElement & Module,
    const InDetDD::PixelModuleDesign & p_design, std::vector < std::pair < double, double > > & trfHitRecord, std::vector < double > & initialConditions) {

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

    if (m_fluence == 0) m_doRadDamage = 0;

    //Calculate trapping times based on fluence (already includes check for fluence=0)
    if (m_doRadDamage) {
                std::pair < double, double > trappingTimes = m_radDamageUtil->getTrappingTimes(fluence_layers.at(0));   //0 = IBL
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

    if (m_doRadDamage && m_fluence>0) {


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

            ATH_MSG_DEBUG("ismoduleframe " << coord << " -- startPosition (x,y,z) = " << chargepos.x() << ", " << chargepos.y() << ", " << chargepos.z());

            // -- change origin of coordinates to the left bottom of module
            double x_new = chargepos.x() + module_size_x / 2.;
            double y_new = chargepos.y() + module_size_y / 2.;


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

            //Loop over charge-carrier pairs
            for (int j = 0; j < ncharges; j++) {

                if (m_doRadDamage && m_fluence > 0) {

                    double eHit = energy_per_step;
                    //Need to determine how many elementary charges this charge chunk represents.
                    double chunk_size = energy_per_step*eleholePairEnergy; //number of electrons/holes
                    ATH_MSG_DEBUG("Chunk size: " << energy_per_step << "*" << eleholePairEnergy << " = " << chunk_size);

                    //set minimum limit to prevent dividing into smaller subcharges than one fundamental charge
                    if (chunk_size < 1) chunk_size = 1;
                    double kappa = 1./sqrt(chunk_size);

                    // Loop over everything twice: once for electrons and once for holes
                    for (int eholes = 0; eholes < 2; eholes++) {
                        bool isHole = false; // Set a condition to keep track of electron/hole-specific functions
                        if (eholes == 1) isHole = true;

                        // Reset extraPixel coordinates each time through loop
                        extraNPixX = nPixX;
                        extraNPixY = nPixY;

                        double timeToElectrode = getTimeToElectrode(y_pix, x_pix, isHole);
                        double driftTime = getDriftTime(isHole);

                        //Apply drift due to diffusion
                        double phiRand = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);

                        //Apply diffusion. rdif is teh max. diffusion
                        double Dt = getMobility(efield, isHole) * (0.024) * std::min(driftTime, timeToElectrode) * temperature / 273.;
                        double rdif = sqrt(Dt) / 1000; //in mm
                        double xposDiff = x_pix + rdif * phiRand;
                        double etaRand = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);
                        double yposDiff = y_pix + rdif * etaRand;

                        // Account for drifting into another pixel 
                        while (xposDiff > pixel_size_x){
                            extraNPixX = extraNPixX + 1;               // increments or decrements pixel count in x
                            xposDiff = xposDiff - pixel_size_x;        // moves xpos coordinate 1 pixel over in x
                        }
                        while (xposDiff < 0){
                            extraNPixX = extraNPixX - 1;
                            xposDiff = xposDiff + pixel_size_x;
                        }
                        while (yposDiff > pixel_size_y){
                            extraNPixY = extraNPixY + 1;               // increments or decrements pixel count in y
                            yposDiff = yposDiff - pixel_size_y;        // moves xpos coordinate 1 pixel over in y
                        }
                        while (yposDiff < 0){
                            extraNPixY = extraNPixY - 1;
                            yposDiff = yposDiff + pixel_size_y;
                        }


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

                                ATH_MSG_DEBUG(" -- Ramo init position w.r.t. Ramo map edge = " << x_pix+pixel_size_x*3-xNeighbor << "  " << y_pix+pixel_size_y*1/2-yNeighbor);
                                ATH_MSG_DEBUG(" -- Ramo final position w.r.t. Ramo map edge = " << xposFinal+pixel_size_x*3-xNeighbor << "  " << yposFinal+pixel_size_y*1/2-yNeighbor);

                                //Ramo map over 500umx350um pixel area
                                //Ramo init different if charge diffused into neighboring pixel -> change primary pixel!!
                                double ramoInit = getRamoPotential(y_pix+pixel_size_y*1/2-yNeighbor,x_pix+pixel_size_x*3-xNeighbor);
                                double ramoFinal = getRamoPotential(yposFinal+pixel_size_y*1/2-yNeighbor,xposFinal+pixel_size_x*3-xNeighbor);

                                // Record deposit
                                double eHitRamo = (1-2*isHole)*eHit*(ramoFinal - ramoInit);

                                ATH_MSG_DEBUG("At neighbor pixel " <<  i << " " << j << " Hit of " << eHitRamo << " including Ramo factor: " << ramoFinal - ramoInit);

                                if (m_doChunkCorrection) {
                                    ATH_MSG_DEBUG("Energy before chunk correction: " << eHitRamo);
                                    eHitRamo = eHit*average_charge + kappa*(eHitRamo - eHit*average_charge);
                                    ATH_MSG_DEBUG("Energy after chunk correction: " << eHitRamo);
                                }

                                double induced_charge = eHitRamo * eleholePairEnergy;

                                // -- pixel coordinates --> module coordinates
                                double x_mod = x_pix + xNeighbor + pixel_size_x * extraNPixX - module_size_x / 2.; 
                                double y_mod = y_pix + yNeighbor + pixel_size_y * extraNPixY - module_size_y / 2.;
                                SiLocalPosition chargePos = Module.hitLocalToLocal(y_mod, x_mod);

                                SiSurfaceCharge scharge(chargePos, SiCharge(induced_charge, hitTime(phit), SiCharge::track, HepMcParticleLink(phit->trackNumber(), phit.eventId())));
                                SiCellId diode = Module.cellIdOfPosition(scharge.position());
                                SiCharge charge = scharge.charge();
                                if (diode.isValid()) {
                                    chargedDiodes.add(diode, charge);
                                    ATH_MSG_DEBUG("induced charge: " << induced_charge << " x_mod: " << x_mod << " y_mod: " << y_mod);

                                }   

                            }
                        }
                    }
                }
            }
        }        

        return StatusCode::SUCCESS;
    } else {


      //**************************************//
      //*** Now diffuse charges to surface *** //
      //**************************************//

        for(unsigned int istep = 0; istep < trfHitRecord.size(); istep++) {
            std::pair<double,double> iHitRecord = trfHitRecord[istep];

            double eta_i = eta_0;
            double phi_i = phi_0;
            double depth_i = depth_0;

            if (iTotalLength) {
                eta_i += 1.0*iHitRecord.first/iTotalLength*dEta;
                phi_i += 1.0*iHitRecord.first/iTotalLength*dPhi;
                depth_i  += 1.0*iHitRecord.first/iTotalLength*dDepth;
            }

            double es_current = 1.0*iHitRecord.second*1.E-6;
            ATH_MSG_DEBUG("es_current: " << es_current);
            double dist_electrode = 0.5 * sensorThickness - Module.design().readoutSide() * depth_i;
            if (dist_electrode<0) dist_electrode=0;

            CLHEP::Hep3Vector chargepos;
            chargepos.setX(phi_i); chargepos.setY(eta_i);  chargepos.setZ(dist_electrode);

            bool coord = Module.isModuleFrame();

            ATH_MSG_DEBUG("ismoduleframe "<<coord << " -- startPosition (x,y,z) = " << chargepos.x() << ", " << chargepos.y() << ", " << chargepos.z());

            // -- change origin of coordinates to the left bottom of module
            double x_new = chargepos.x() + module_size_x/2.;
            double y_new = chargepos.y() + module_size_y/2.;

            // -- change from module frame to pixel frame
            int nPixX = int(x_new/pixel_size_x);
            int nPixY = int(y_new/pixel_size_y);
            ATH_MSG_DEBUG(" -- nPixX = "<<nPixX<<"  nPixY = "<<nPixY);    
            double x_pix = x_new - pixel_size_x*(nPixX);
            double y_pix = y_new - pixel_size_y*(nPixY);
            // -- change origin of coordinates to the center of the pixel
            double x_pix_center = x_pix - pixel_size_x/2;
            double y_pix_center = y_pix - pixel_size_y/2;
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
                    if ( (fabs(x_neighbor)<pixel_size_x) && (fabs(y_neighbor)<pixel_size_y) ){

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

                        double ed=es_current*eleholePairEnergy*ccprob_neighbor;
                        

                        // -- pixel coordinates --> module coordinates
                        //double x_mod = x_neighbor - half_pixel_size_x + pixel_size_x*nPixX -module_size_x/2.;
                        //double y_mod = y_neighbor - half_pixel_size_y + pixel_size_y*nPixY -module_size_y/2.;
                        double x_mod = x_neighbor + pixel_size_x*0.5 + pixel_size_x*nPixX -module_size_x*0.5;
                        double y_mod = y_neighbor + pixel_size_y*0.5 + pixel_size_y*nPixY -module_size_y*0.5;
                        SiLocalPosition chargePos = Module.hitLocalToLocal(y_mod,x_mod);

                        SiSurfaceCharge scharge(chargePos,SiCharge(ed,hitTime(phit),SiCharge::track,HepMcParticleLink(phit->trackNumber(),phit.eventId())));
                        SiCellId diode = Module.cellIdOfPosition(scharge.position());
                        SiCharge charge = scharge.charge();

                        if (diode.isValid()) {
                            chargedDiodes.add(diode,charge);
                            ATH_MSG_DEBUG("induced charge: " << ed << " x_mod: " << x_mod << " y_mod: " << y_mod);
                        }
                    }
                }
            }
        }
        

        return StatusCode::SUCCESS;

    }

}

StatusCode SensorSim3DTool::applySlimEdges(double & energy_per_step, double & eta_drifted) {

    if (fabs(eta_drifted) > 20.440) energy_per_step = 0.;
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
            energy_per_step = energy_per_step * (41.2 - eta_drifted * 2.);
            eta_drifted = eta_drifted - 0.250;
        } else {
            energy_per_step = energy_per_step * (41.2 + eta_drifted * 2.);
            eta_drifted = eta_drifted + 0.250;
        }
    }

    return StatusCode::SUCCESS;

}

double SensorSim3DTool::getElectricField(double x, double y) {
    std::pair < int, int > Layer; // index for layer/end cap position
    Layer.first = 0; //Barrel (0) or End Cap (1)   -    Now only for Barrel. If we want to add End Caps, put them at Layer.first=1
    Layer.second = 0; //Layer: 0 = IBL Planar, 1=B-Layer, 2=Layer-1, 3=Layer-2

    int n_binx = eFieldMap[Layer]->GetXaxis()->FindBin(x * 1000); //position coordinates in um to return electric field in V/cm
    int n_biny = eFieldMap[Layer]->GetYaxis()->FindBin(y * 1000);
    double electricField = eFieldMap[Layer]->GetBinContent(n_binx, n_biny);
    return electricField * 1.0E-7; //return efield in MV/mm (for mobility calculation)
}

double SensorSim3DTool::getMobility(double electricField, bool isHoleBit) {
    //Not exactly the same as the getMobility function in RadDamageUtil, since we don't have a Hall effect for 3D sensors (B and E are parallel)
    //Maybe good to do something about this in the future

    double vsat = 0;
    double ecrit = 0;
    double beta = 0;

    //These parameterizations come from C. Jacoboni et al., Solid‐State Electronics 20 (1977) 77‐89. (see also https://cds.cern.ch/record/684187/files/indet-2001-004.pdf).

    if (!isHoleBit) {
        vsat = 15.3 * pow(temperature, -0.87); // mm/ns
        ecrit = 1.01E-7 * pow(temperature, 1.55); // MV/mm
        beta = 2.57E-2 * pow(temperature, 0.66);
    }
    if (isHoleBit) {
        vsat = 1.62 * pow(temperature, -0.52); // mm/ns
        ecrit = 1.24E-7 * pow(temperature, 1.68); // MV/mm
        beta = 0.46 * pow(temperature, 0.17);
    }

    double mobility = (vsat / ecrit) / pow(1 + pow((electricField / ecrit), beta), (1 / beta));
    return mobility; // mm^2/(MV*ns)
}

double SensorSim3DTool::getDriftTime(bool isHoleBit) {
    double u = CLHEP::RandFlat::shoot(0., 1.); // 
    double driftTime = 0;

    if (!isHoleBit) driftTime = (-1.) * m_trappingTimeElectrons * TMath::Log(u); // ns
    if (isHoleBit) driftTime = (-1.) * m_trappingTimeHoles * TMath::Log(u); // ns
    return driftTime;
}

double SensorSim3DTool::getTimeToElectrode(double x, double y, bool isHoleBit) {

    std::pair < int, int > Layer; // index for layer/end cap position
    Layer.first = 0; //Barrel (0) or End Cap (1)   -    Now only for Barrel. If we want to add End Caps, put them at Layer.first=1
    Layer.second = 0; //Layer: 0 = IBL Planar, 1=B-Layer, 2=Layer-1, 3=Layer-2

    // Uses (x,y) position in um to return time to electrode in ns
    double timeToElectrode = 0;
    if (!isHoleBit) {
        int n_binx = timeMap_e[Layer]->GetXaxis()->FindBin(x * 1000); //convert from mm to um
        int n_biny = timeMap_e[Layer]->GetYaxis()->FindBin(y * 1000);
        timeToElectrode = timeMap_e[Layer]->GetBinContent(n_binx, n_biny);
    }
    if (isHoleBit) {
        int n_binx = timeMap_h[Layer]->GetXaxis()->FindBin(x * 1000);
        int n_biny = timeMap_h[Layer]->GetYaxis()->FindBin(y * 1000);
        timeToElectrode = timeMap_h[Layer]->GetBinContent(n_binx, n_biny);
    }
    return timeToElectrode; //[ns]
}

double SensorSim3DTool::getTrappingPositionX(double initX, double initY, double driftTime, bool isHoleBit) {
    std::pair < int, int > Layer; // index for layer/end cap position
    Layer.first = 0; //Barrel (0) or End Cap (1)   -    Now only for Barrel. If we want to add End Caps, put them at Layer.first=1
    Layer.second = 0; //Layer: 0 = IBL Planar, 1=B-Layer, 2=Layer-1, 3=Layer-2

    double finalX = initX;
    if (!isHoleBit) {
        int n_binx = xPositionMap_e[Layer]->GetXaxis()->FindBin(initX * 1000);
        int n_biny = xPositionMap_e[Layer]->GetYaxis()->FindBin(initY * 1000);
        int n_bint = xPositionMap_e[Layer]->GetZaxis()->FindBin(driftTime);
        finalX = xPositionMap_e[Layer]->GetBinContent(n_binx, n_biny, n_bint);

    } else {
        int n_binx = xPositionMap_h[Layer]->GetXaxis()->FindBin(initX * 1000);
        int n_biny = xPositionMap_h[Layer]->GetYaxis()->FindBin(initY * 1000);
        int n_bint = xPositionMap_h[Layer]->GetZaxis()->FindBin(driftTime);
        finalX = xPositionMap_h[Layer]->GetBinContent(n_binx, n_biny, n_bint);

    }

    return finalX * 1e-3; //[mm]
}

double SensorSim3DTool::getTrappingPositionY(double initX, double initY, double driftTime, bool isHoleBit) {
    std::pair < int, int > Layer; // index for layer/end cap position
    Layer.first = 0; //Barrel (0) or End Cap (1)   -    Now only for Barrel. If we want to add End Caps, put them at Layer.first=1
    Layer.second = 0; //Layer: 0 = IBL Planar, 1=B-Layer, 2=Layer-1, 3=Layer-2

    double finalY = initY;
    if (!isHoleBit) {
        int n_binx = yPositionMap_e[Layer]->GetXaxis()->FindBin(initX * 1000);
        int n_biny = yPositionMap_e[Layer]->GetYaxis()->FindBin(initY * 1000);
        int n_bint = yPositionMap_e[Layer]->GetZaxis()->FindBin(driftTime);
        finalY = yPositionMap_e[Layer]->GetBinContent(n_binx, n_biny, n_bint);
    } else {
        int n_binx = yPositionMap_h[Layer]->GetXaxis()->FindBin(initX * 1000);
        int n_biny = yPositionMap_h[Layer]->GetYaxis()->FindBin(initY * 1000);
        int n_bint = yPositionMap_h[Layer]->GetZaxis()->FindBin(driftTime);
        finalY = yPositionMap_h[Layer]->GetBinContent(n_binx, n_biny, n_bint);
    }

    return finalY * 1e-3; //[mm]
}

double SensorSim3DTool::getRamoPotential(double x, double y){
    std::pair < int, int > Layer; // index for layer/end cap position
    Layer.first = 0; //Barrel (0) or End Cap (1)   -    Now only for Barrel. If we want to add End Caps, put them at Layer.first=1
    Layer.second = 0; //Layer: 0 = IBL Planar, 1=B-Layer, 2=Layer-1, 3=Layer-2


    int n_binx = ramoPotentialMap[Layer]->GetXaxis()->FindBin(x*1000);
    int n_biny = ramoPotentialMap[Layer]->GetYaxis()->FindBin(y*1000);
    double ramoPotential = ramoPotentialMap[Layer]->GetBinContent(n_binx,n_biny);
    return ramoPotential;
}



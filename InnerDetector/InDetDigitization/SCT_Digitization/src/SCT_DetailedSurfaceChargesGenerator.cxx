/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_DetailedSurfaceChargesGenerator.h"

// Athena
#include "GeneratorObjects/HepMcParticleLink.h"
#include "InDetSimEvent/SiHit.h" // for SiHit, SiHit::xDep, etc
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h"
#include "InDetReadoutGeometry/SCT_BarrelModuleSideDesign.h"

// random number service includes
#include "AthenaKernel/IAtRndmGenSvc.h"

// Gaudi
#include "GaudiKernel/ITHistSvc.h" // for ITHistSvc

// CLHEP
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussZiggurat.h" // for RandGaussZiggurat
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

// ROOT
#include "TMath.h" // for Log
#include "TProfile.h" // for TProfile
#include "TProfile2D.h" // for TProfile2D

using InDetDD::SiDetectorElement;
using InDetDD::SCT_ModuleSideDesign;
using InDetDD::SiLocalPosition;

//#define SCT_DIG_DEBUG

// constructor
SCT_DetailedSurfaceChargesGenerator::SCT_DetailedSurfaceChargesGenerator(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent),
  m_tHalfwayDrift{0.},
  m_distInterStrip{1.0},
  m_distHalfInterStrip{0.},
  m_SurfaceDriftFlag{false},
  m_thistSvc{"THistSvc", name},
  m_h_efieldz{nullptr},
  m_h_yzRamo{nullptr},
  m_h_yzEfield{nullptr},
  m_h_yEfield{nullptr},
  m_h_zEfield{nullptr},
  m_bulk_depth{0.0285}, //<!285 micron, expressed in cm units
  m_strip_pitch{0.0080}, //<! 80 micron, expressed in cm units
  m_depletion_depth{0.0285},
  m_y_origin_min{0.0}, //<! zero unless under-depleted
  m_kB{1.38E-23}, //<! Boltzmann const [m^2*kg/s^2/K]
  m_e{1.602E-19}, //<! electron charge [Coulomb]
  m_vs_e{11615084.7393}, //<! mobility at 273.15K
  m_Ec_e{6034.20429},
  m_vs_h{8761659.83530}, //<! hole mobility at 273.15K
  m_Ec_h{15366.52650},
  m_PotentialValue{{0.}},
  m_ExValue150{{0.}},
  m_EyValue150{{0.}},
  //m_stripCharge{{{{0.}}}},
  // sroe: the following were never initialised before, which begs the question:
  // Did this code *ever* work? Has it *ever* been used?
  m_stripCharge_iymax{285-1},
  m_stripCharge_dx{1.},
  m_stripCharge_dy{1.}
  {
    declareProperty("FixedTime", m_tfix=-999.); //!< fixed timing
    declareProperty("SubtractTime", m_tsubtract=-999.); //!< substract drift time
    declareProperty("SurfaceDriftTime", m_tSurfaceDrift=10); //!< max surface drift time
    declareProperty("NumberOfCharges", m_numberOfCharges=1);
    declareProperty("SmallStepLength", m_smallStepLength=5);
    declareProperty("ChargeDriftModel", m_chargeDriftModel=ehTransport);
    declareProperty("EFieldModel", m_eFieldModel=FEMsolution);
    declareProperty("DepletionVoltage", m_depletionVoltage=70.);
    declareProperty("BiasVoltage", m_biasVoltage=150.);
    declareProperty("MagneticField", m_magneticField=-2.0);
    declareProperty("SensorTemperature", m_sensorTemperature=0.+CLHEP::STP_Temperature);
    declareProperty("TransportTimeStep", m_transportTimeStep=0.25);
    declareProperty("TransportTimeMax", m_transportTimeMax=25.0);
    declareProperty("doDistortions", m_doDistortions=false, "Simulation of module distortions");
    declareProperty("doHistoTrap", m_doHistoTrap=false, "Allow filling of histos for charge trapping effect");
    declareProperty("doTrapping", m_doTrapping=false, "Simulation of charge trapping effect");
    declareProperty("Fluence", m_Fluence=0., "Fluence for charge trapping effect");
    declareProperty("isOverlay", m_isOverlay=false);
}

//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------
StatusCode SCT_DetailedSurfaceChargesGenerator::initialize() {
  ATH_MSG_DEBUG("SCT_DetailedSurfaceChargesGenerator::initialize()");

  //Get ISiPropertiesTool
  ATH_CHECK(m_siPropertiesTool.retrieve());

  //Get ISiliconConditionsTool
  ATH_CHECK(m_siConditionsTool.retrieve());

  ///////////////////////////////////////////////////
  if (m_doHistoTrap) {
    //-- Get Histogram Service
    StatusCode rc{m_thistSvc.retrieve()};
    if (rc.isFailure()) {
      ATH_MSG_FATAL("Unable to retrieve pointer to THistSvc");
      return StatusCode::FAILURE;
    }
    if (m_doHistoTrap) {
      m_h_efieldz = new TProfile("efieldz", "", 50, 0., 0.03);
      rc = m_thistSvc->regHist("/file1/efieldz", m_h_efieldz);
      if (rc.isFailure()) {
        ATH_MSG_FATAL("Unable to retrieve pointer to THistSvc");
        return StatusCode::FAILURE;
      }
      m_h_yzRamo = new TProfile2D("yzRamo", "", 60, -0.44, 0.44, 60, 0., m_bulk_depth*10.);
      rc = m_thistSvc->regHist("/file1/yzRamo", m_h_yzRamo);
      if (rc.isFailure()) {
        ATH_MSG_FATAL("Unable to retrieve pointer to THistSvc");
        return StatusCode::FAILURE;
      }
      m_h_yzEfield = new TProfile2D("yzEfield", "", 40, -0.004, 0.004, 60, 0., m_bulk_depth);
      rc = m_thistSvc->regHist("/file1/yzEfield", m_h_yzEfield);
      if (rc.isFailure()) {
        ATH_MSG_FATAL("Unable to retrieve pointer to THistSvc");
        return StatusCode::FAILURE;
      }
      m_h_yEfield = new TProfile2D("yEfield", "", 40, -0.004, 0.004, 60, 0., m_bulk_depth);
      rc = m_thistSvc->regHist("/file1/yEfield", m_h_yEfield);
      if (rc.isFailure()) {
        ATH_MSG_FATAL("Unable to retrieve pointer to THistSvc");
        return StatusCode::FAILURE;
      }
      m_h_zEfield = new TProfile2D("zEfield", "", 40, -0.004, 0.004, 60, 0., m_bulk_depth);
      rc = m_thistSvc->regHist("/file1/zEfield", m_h_zEfield);
      if (rc.isFailure()) {
        ATH_MSG_FATAL("Unable to retrieve pointer to THistSvc");
        return StatusCode::FAILURE;
      }
    }
  }
  //////////////////////////////////////////////////

  m_smallStepLength *= CLHEP::micrometer;
  m_tSurfaceDrift *= CLHEP::ns;

  // Surface drift time calculation Stuff
  m_tHalfwayDrift = m_tSurfaceDrift/2.0;
  m_distHalfInterStrip = m_distInterStrip/2.0;
  if ((m_tSurfaceDrift > m_tHalfwayDrift) and
      (m_tHalfwayDrift>=0.0) and
      (m_distHalfInterStrip>0.0) and
      (m_distHalfInterStrip<m_distInterStrip)) {
    m_SurfaceDriftFlag = true;
#ifdef SCT_DIG_DEBUG
    ATH_MSG_INFO("\tsurface drift ON: surface drift time at d=" << m_distInterStrip << "   (mid strip)    =" << this->SurfaceDriftTime(m_distInterStrip));
    ATH_MSG_INFO("\tsurface drift ON: surface drift time at d=" << m_distHalfInterStrip << " (half way)     =" << this->SurfaceDriftTime(m_distHalfInterStrip));
    ATH_MSG_INFO("\tsurface drift ON: surface drift time at d=0.0" << " (strip center) =" << this->SurfaceDriftTime(0.));
#endif
  } else {
    ATH_MSG_INFO("\tsurface drift still not on, wrong params");
  }

  // Make sure these two flags are not set simultaneously
  if (m_tfix>-998. and m_tsubtract>-998.) {
    ATH_MSG_FATAL("\tCannot set both FixedTime and SubtractTime options!");
    ATH_MSG_INFO("\tMake sure the two flags are not set simultaneously in jo");
    return StatusCode::FAILURE;
  }

  if (m_chargeDriftModel == ehTransport) {
    ATH_MSG_INFO("\tUsing Taka Kondo eh-transport code");
    initTransportModel();
  }
  if (m_chargeDriftModel == fixedChargeMap) {
    ATH_MSG_INFO("\tUsing Taka Kondo fixed charge map");
    //init_ChargeMapModel("..."); //PJ to be changed
  }

#ifdef SCT_DIG_DEBUG
  ATH_MSG_INFO("\tDetailedSurfaceChargesGenerator copied");
  ATH_MSG_INFO("\tn.charg " << m_numberOfCharges);
  ATH_MSG_INFO("\tdigi steps " << m_smallStepLength << " mm");
#endif

  if (m_doDistortions) {
    ATH_CHECK(m_distortionsTool.retrieve());
  } else {
    m_distortionsTool.disable();
  }

  ATH_CHECK(m_lorentzAngleTool.retrieve());

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// finalize
//----------------------------------------------------------------------
StatusCode SCT_DetailedSurfaceChargesGenerator::finalize() {
  ATH_MSG_DEBUG("SCT_DetailedSurfaceChargesGenerator::finalize()");
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// perpandicular Drift time calculation
//----------------------------------------------------------------------
float SCT_DetailedSurfaceChargesGenerator::DriftTime(float zhit, const SiDetectorElement* element) const {
  const double sensorThickness{element->thickness()};
  if ((zhit<0.0) or (zhit>sensorThickness)) {
    ATH_MSG_DEBUG("DriftTime: hit coordinate zhit=" << zhit/CLHEP::micrometer << " out of range");
    return -2.0;
  }
  const IdentifierHash hashId{element->identifyHash()};

  const double vdepl{m_siConditionsTool->depletionVoltage(hashId) * CLHEP::volt};
  const double vbias{m_siConditionsTool->biasVoltage(hashId) * CLHEP::volt};
  const double denominator{vdepl+vbias-(2.0*zhit*vdepl/sensorThickness)};
  if (denominator<=0.0) {
    if (vbias>=vdepl) { //Should not happen
      if (not m_isOverlay) {
        ATH_MSG_ERROR("DriftTime: negative argument X for log(X) " << zhit);
      }
      return -1.0;
    } else {
      // (vbias<vdepl) can happen with underdepleted sensors, lose charges in that volume
      // ATH_MSG_DEBUG("DriftTime: ->infinity since vdepl>vbias, zhit: " << zhit );
      return -10.0;
    }
  }

  float driftTime{static_cast<float>(log((vdepl+vbias)/denominator))};
  driftTime *= sensorThickness*sensorThickness/(2.0*m_siPropertiesTool->getSiProperties(hashId).holeDriftMobility()*vdepl);
  return driftTime;
}

//----------------------------------------------------------------------
// Sigma Diffusion calculation
//----------------------------------------------------------------------
float SCT_DetailedSurfaceChargesGenerator::DiffusionSigma(float zhit, const SiDetectorElement* element) const {
  const IdentifierHash hashId{element->identifyHash()};
  const float t{this->DriftTime(zhit, element)}; // in ns
  if (t>0.0) {
    const float diffusionSigma{static_cast<float>(sqrt(2.*m_siPropertiesTool->getSiProperties(hashId).holeDiffusionConstant()*t))}; // in mm
    return diffusionSigma;
  }
  return 0.0;
}

//----------------------------------------------------------------------
// Maximum drift time
//----------------------------------------------------------------------
float SCT_DetailedSurfaceChargesGenerator::MaxDriftTime(const SiDetectorElement* element) const {
  if (element) {
    const double sensorThickness{element->thickness()};
    return this->DriftTime(sensorThickness, element);
  } else {
    ATH_MSG_INFO("Error: SiDetectorElement not set!");
    return 0.;
  }
}

//----------------------------------------------------------------------
// Maximum Sigma difusion
//----------------------------------------------------------------------
float SCT_DetailedSurfaceChargesGenerator::MaxDiffusionSigma(const SiDetectorElement* element) const {
  if (element) {
    const double sensorThickness{element->thickness()};
    return this->DiffusionSigma(sensorThickness, element);
  } else {
    ATH_MSG_INFO("Error: SiDetectorElement not set!");
    return 0.;
  }
}

//----------------------------------------------------------------------
// Calculating the surface drift time but I should confess that 
// I haven't found out yet where the calculation come from 
//----------------------------------------------------------------------
float SCT_DetailedSurfaceChargesGenerator::SurfaceDriftTime(float ysurf) const {
  if (m_SurfaceDriftFlag) {
    if ((ysurf>=0.0) and (ysurf<=m_distInterStrip)) {
      float surfaceDriftTime{0.};
      if (ysurf<m_distHalfInterStrip) {
        const float y{ysurf/m_distHalfInterStrip};
        surfaceDriftTime =  m_tHalfwayDrift*y*y;
      } else {
        const float y{(m_distInterStrip-ysurf)/(m_distInterStrip-m_distHalfInterStrip)};
        surfaceDriftTime = m_tSurfaceDrift+(m_tHalfwayDrift-m_tSurfaceDrift)*y*y;
      }
      return surfaceDriftTime;
    } else {
      ATH_MSG_INFO(" ysurf out of range " << ysurf);
      return -1.0;
    }
  } else {
    return 0.0;
  }
}

//-------------------------------------------------------------------------------------------
// create a list of surface charges from a hit - called from SCT_Digitization AthAlgorithm
//-------------------------------------------------------------------------------------------
void SCT_DetailedSurfaceChargesGenerator::process(const SiDetectorElement* element, const TimedHitPtr<SiHit>& phit, const ISiSurfaceChargesInserter& inserter, CLHEP::HepRandomEngine * rndmEngine) const {
  ATH_MSG_VERBOSE("SCT_DetailedSurfaceChargesGenerator::process starts");
  const float p_eventTime{phit.eventTime()};
  const unsigned short p_eventId{phit.eventId()};
  processSiHit(element, *phit, inserter, p_eventTime, p_eventId, rndmEngine);
  return;
}

//-------------------------------------------------------------------------------------------
// create a list of surface charges from a hit - called from both AthAlgorithm and PileUpTool
//-------------------------------------------------------------------------------------------
void SCT_DetailedSurfaceChargesGenerator::processSiHit(const SiDetectorElement* element, const SiHit& phit, const ISiSurfaceChargesInserter& inserter, float p_eventTime, unsigned short p_eventId, CLHEP::HepRandomEngine * rndmEngine) const {
  const SCT_ModuleSideDesign* p_design{dynamic_cast<const SCT_ModuleSideDesign*>(&(element->design()))};
  if (p_design==nullptr) {
    ATH_MSG_ERROR("SCT_DetailedSurfaceChargesGenerator::process can not get " << p_design);
    return;
  }

  //---**************************************
  //  Time of Flight Calculation - separate method?
  //---**************************************
  //--- Original calculation of Time of Flight of the particle Time needed by the particle to reach the sensor
  float timeOfFlight{p_eventTime + hitTime(phit)}; //  hitTime(phit): Open functions of InDetSimEvent/SiHit.h  hitTime(phit) =  phit->meanTime();

  // Kondo 19/09/2007: Use the coordinate of the center of the module to calculate the time of flight
  timeOfFlight -= (element->center().mag())/CLHEP::c_light; //!< extract the distance to the origin of the module to Time of flight

  //!< timing set from jo to adjust (subtract) the timing
  if (m_tsubtract>-998.) timeOfFlight -= m_tsubtract;
  //---**************************************

  //Get HashId
  const IdentifierHash hashId{element->identifyHash()};

  //get sensor thickness and tg lorentz from SiDetectorDesign
  const double sensorThickness{p_design->thickness()};
  const double tanLorentz{m_lorentzAngleTool->getTanLorentzAngle(hashId)};

  const CLHEP::Hep3Vector pos{phit.localStartPosition()};
  const double xEta{pos[SiHit::xEta]};
  const double xPhi{pos[SiHit::xPhi]};
  const double xDep{pos[SiHit::xDep]};

  const CLHEP::Hep3Vector endPos{phit.localEndPosition()};
  const double cEta{endPos[SiHit::xEta]-xEta};
  const double cPhi{endPos[SiHit::xPhi]-xPhi};
  const double cDep{endPos[SiHit::xDep]-xDep};

  double LargeStep{sqrt(cEta*cEta+cPhi*cPhi+cDep*cDep)};
  int numberOfSteps{static_cast<int>(LargeStep/m_smallStepLength) + 1};
  double steps{static_cast<double>(m_numberOfCharges*numberOfSteps)};
  double e1{phit.energyLoss()/steps};
  double q1{e1*m_siPropertiesTool->getSiProperties(hashId).electronHolePairsPerEnergy()};

  //in the following, to test the code, we will use the original coordinate system of the SCTtest3SurfaceChargesGenerator x is eta y is phi z is depth 
  double xhit{xEta};
  double yhit{xPhi};
  double zhit{xDep};
  
  if (m_doDistortions) {
    if (element->isBarrel()==1) { //Only apply distortions to barrel modules
      Amg::Vector2D BOW;
      BOW[0] = (m_distortionsTool->correctSimulation(hashId, xhit, yhit, cEta, cPhi, cDep))[0];
      BOW[1] = (m_distortionsTool->correctSimulation(hashId, xhit, yhit, cEta, cPhi, cDep))[1];
      xhit = BOW.x();
      yhit = BOW.y();
    }
  }

  double StepX{cEta/numberOfSteps};
  double StepY{cPhi/numberOfSteps};
  double StepZ{cDep/numberOfSteps};

  //check the status of truth information for this SiHit
  //some Truth information is cut for pile up events
  HepMcParticleLink trklink{HepMcParticleLink(phit.trackNumber(), p_eventId)};
  SiCharge::Process hitproc{SiCharge::track};
  if (phit.trackNumber()!=0) {
    if (not trklink.isValid()) {
      hitproc = SiCharge::cut_track;
    }
  }

  double dstep{-0.5};
  for (int istep{0}; istep<numberOfSteps; ++istep) {
    dstep += 1.0;
    double z1{zhit+StepZ*dstep};//(static_cast<float>(istep)+0.5);

    //Distance between charge and readout side. p_design->readoutSide() is +1 if readout side is in +ve depth axis direction and visa-versa.
    double zReadout{0.5*sensorThickness - p_design->readoutSide() * z1};

    float tdrift{DriftTime(zReadout, element)}; //!< tdrift: perpandicular drift time
    if (tdrift>-2.0000002 and tdrift<-1.9999998) {
      ATH_MSG_DEBUG("Checking for rounding errors in compression");
      if ((fabs(z1)-0.5*sensorThickness)<0.000010) {
        ATH_MSG_DEBUG("Rounding error found attempting to correct it. z1 = " << std::fixed << std::setprecision(8) << z1);
        if (z1<0.0) {
          z1= 0.0000005-0.5*sensorThickness; //set new coordinate to be 0.5nm inside wafer volume.
        } else {
          z1 = 0.5*sensorThickness-0.0000005; //set new coordinate to be 0.5nm inside wafer volume.
        }
        zReadout = 0.5*sensorThickness - p_design->readoutSide() * z1;
        tdrift=DriftTime(zReadout, element);
        if (tdrift>-2.0000002 and tdrift<-1.9999998) {
          ATH_MSG_WARNING("Attempt failed. Making no correction.");
        } else {
          ATH_MSG_DEBUG("Correction Successful! z1 = " << std::fixed << std::setprecision(8) << z1 << ", zReadout = " << zReadout << ", tdrift = " << tdrift);
        }
      } else {
        ATH_MSG_DEBUG("No rounding error found. Making no correction.");
      }
    }
    if (tdrift > 0.0) {
      double x1{xhit+StepX*dstep};//(static_cast<float>(istep)+0.5);
      double y1{yhit+StepY*dstep};//(static_cast<float>(istep)+0.5);

      //PJ select driftmodel here
      if (m_chargeDriftModel == defaultSCTModel or element->isEndcap()) { //Standard SCT driftmodel
        y1 += tanLorentz*zReadout; //!< Taking into account the magnetic field
        float diffusionSigma{DiffusionSigma(zReadout, element)};
        for (int i{0}; i<m_numberOfCharges; ++i) {
          float rx{CLHEP::RandGaussZiggurat::shoot(rndmEngine)};
          double xd{x1+diffusionSigma*rx};
          float ry{CLHEP::RandGaussZiggurat::shoot(rndmEngine)};
          double yd{y1+diffusionSigma*ry};

          SiLocalPosition position{element->hitLocalToLocal(xd, yd)};
          if (p_design->inActiveArea(position)) {
            float sdist{static_cast<float>(p_design->scaledDistanceToNearestDiode(position))}; //!< dist on the surface from the hit point to the nearest strip (diode)
            float tsurf{this->SurfaceDriftTime(2.0*sdist)}; //!< Surface drift time
            float totaltime{(m_tfix>-998.) ? m_tfix : tdrift + timeOfFlight + tsurf}; //!< Total drift time
            inserter(SiSurfaceCharge(position, SiCharge(q1, totaltime, hitproc, trklink)));

#ifdef SCT_DIG_DEBUG
            ATH_MSG_INFO("Total Time: " << totaltime << " tdrift " << tdrift << " tsurf  " << tsurf << " sdist " << sdist << " charge: " << q1);
#endif

          } else {
#ifdef SCT_DIG_DEBUG
            ATH_MSG_INFO(std::fixed << std::setprecision(8) << "Local position: " << position << " of the element is out of active area, charge = " << q1);
#endif
          }
        } // end of loop on charges
      } else if (m_chargeDriftModel == ehTransport  and element->isBarrel()) { //TKs eh-transport model
        //PJ calculates induced charge from e's and h's for 5 strips with 0.5 ns steps
        // Set up local taka parameters, TK using cm...

        const InDetDD::SCT_BarrelModuleSideDesign* b_design{dynamic_cast<const InDetDD::SCT_BarrelModuleSideDesign*>(&(element->design()))};
        if (b_design==nullptr) {
          ATH_MSG_ERROR("SCT_DetailedSurfaceChargesGenerator::process can not get " << b_design);
          return;
        }

        double stripPitch{p_design->stripPitch()};
        double stripPatternCentre{b_design->phiStripPatternCentre()};
        double dstrip{(y1-stripPatternCentre)/stripPitch};

        // need the distance from the nearest strips edge not centre, xtaka = 1/2*stripPitch
        // centre of detector, y1=0, is in the middle of an interstrip gap
        if (dstrip>0.) {
          dstrip -= static_cast<double>(static_cast<int>(dstrip));
        } else {
          dstrip -= static_cast<double>(static_cast<int>(dstrip)-1);
        }
        double xtakadist{dstrip*stripPitch};
        double x0{xtakadist/10.}; // [mm] to [cm]
        double y0{(sensorThickness - zReadout)/10.}; // [mm] to [cm]

#ifdef SCT_DIG_DEBUG
        ATH_MSG_INFO("element->isBarrel(): " << element->isBarrel() << " stripPitch: " << stripPitch << " stripPatternCentre: " << stripPatternCentre);
        ATH_MSG_INFO("tanLorentz, y1, xtakadist = " << tanLorentz << ", " << y1 << ", " << xtakadist);
#endif

        double Q_m2[50], Q_m1[50], Q_00[50], Q_p1[50], Q_p2[50]; // Charge arrays for up to 25 ns
        // Clear charge arrays
        for (int it{0}; it<50; it++) {
          Q_m2[it]=0.;
          Q_m1[it]=0.;
          Q_00[it]=0.;
          Q_p1[it]=0.;
          Q_p2[it]=0.;
        }

        // Electron and hole transportation starting at x0 and y0
        holeTransport    (x0, y0, Q_m2, Q_m1, Q_00, Q_p1, Q_p2, rndmEngine);
        electronTransport(x0, y0, Q_m2, Q_m1, Q_00, Q_p1, Q_p2, rndmEngine);

        //Loop over the strips and add the surface charges from each step and strip
        for (int strip{-2}; strip<=2; strip++) {
          double ystrip{y1 + strip*stripPitch};
          SiLocalPosition position{element->hitLocalToLocal(x1, ystrip)};
          if (p_design->inActiveArea(position)) {
            for (int itq{0}; itq<50; itq++) {
              double charge{0.};
              if (strip == -2) charge = Q_m2[itq];
              if (strip == -1) charge = Q_m1[itq];
              if (strip ==  0) charge = Q_00[itq];
              if (strip ==  1) charge = Q_p1[itq];
              if (strip ==  2) charge = Q_p2[itq];
              double time{(itq+1)*0.5 + timeOfFlight};
              if (charge != 0.) inserter(SiSurfaceCharge(position, SiCharge(q1*charge,time,hitproc,trklink)));
#ifdef SCT_DIG_DEBUG
              ATH_MSG_DEBUG("strip: " << strip << " position, x,y:" << x1 << ", " << ystrip << " charge: " << q1*charge);
#endif
            }
          } else {
#ifdef SCT_DIG_DEBUG
            ATH_MSG_INFO(std::fixed << std::setprecision(8) << "Local position: " << position << " of the element is out of active area, charge = " << q1);
#endif
          }
        }
      } else if (m_chargeDriftModel == fixedChargeMap and element->isBarrel()) { //TKs fixed charge map model
        ATH_MSG_ERROR("Fixed charge map model, implementation not finished yet...");

        const InDetDD::SCT_BarrelModuleSideDesign* b_design{dynamic_cast<const InDetDD::SCT_BarrelModuleSideDesign*>(&(element->design()))};
        if (b_design==nullptr) {
          ATH_MSG_ERROR("SCT_DetailedSurfaceChargesGenerator::process can not get " << b_design);
          return;
        }

        // Set up local taka parameters, TK using um...I think
        double stripPitch{p_design->stripPitch()};
        double stripPatternCentre{b_design->phiStripPatternCentre()};
        double dstrip{(y1-stripPatternCentre)/stripPitch};

        // need the distance from the nearest strips edge not centre, xtaka = 1/2*stripPitch
        // centre of detector, y1=0, is in the middle of an interstrip gap
        if (dstrip>0.) {
          dstrip -= static_cast<double>(static_cast<int>(dstrip));
        } else {
          dstrip -= static_cast<double>(static_cast<int>(dstrip)-1);
        }
        double xtakadist{dstrip*stripPitch};
        double x0{xtakadist*1000.}; // [mm] to [um]
        double y0{(sensorThickness - zReadout)*1000.}; // [mm] to [um]

        //Loop over the strips and add the surface charges from each step and strip
        for (int strip{-2}; strip<=2; strip++) {
          double ystrip{y1 + strip*stripPitch};
          SiLocalPosition position{element->hitLocalToLocal(x1, ystrip)};
          if (p_design->inActiveArea(position)) {
            //PJ best way of doing this...
            double time{0.};
            double charge{0.};
            for (int it{0}; it<50; it++) {
              time = 0.25 + 0.5*it;
              charge = q1 * inducedCharge(strip, x0, y0, time);
              if (charge != 0.) {
                inserter(SiSurfaceCharge(position, SiCharge(charge, time+timeOfFlight, hitproc, trklink)));
              }
            }
          } else {
#ifdef SCT_DIG_DEBUG
            ATH_MSG_INFO(std::fixed << std::setprecision(8) << "Local position: " << position << " of the element is out of active area, charge = " << q1);
#endif
          }
        }
      } else {
        ATH_MSG_ERROR("Wrong chargeDriftModel " << m_chargeDriftModel << " 0, standard, 1 eh transport, 2 fixed charge map");
      }
    }
  }
  return;
}

//------------------------------------------------------------
//  initialization of e/h transport programme
//-----------------------------------------------------------
void SCT_DetailedSurfaceChargesGenerator::initTransportModel() {
  //------------------------ initialize subfunctions ------------
  init_mud_h(m_sensorTemperature);
  init_mud_e(m_sensorTemperature);
  initExEyArray();
  initPotentialValue();

  //------------ find delepletion deph for model=uniformEFieldModel and flatDiodeModel -------------
  ATH_MSG_INFO("\tmodel= " << m_eFieldModel << " VB= " << m_biasVoltage << "  B= " << m_magneticField);
  m_depletion_depth = m_bulk_depth;
  if (m_biasVoltage < m_depletionVoltage) {
    m_depletion_depth = sqrt(m_biasVoltage/m_depletionVoltage) * m_bulk_depth;
  }

  // ----------- find for the case of model = FEMsolution ---------------
  if (m_eFieldModel == FEMsolution) {
    double Ex{0.}, Ey{0.};
    for (double y{0.}; y < m_bulk_depth; y += 0.00025) {
      EField(0., y, Ex, Ey);
      if (Ey > 0.1) continue;
      m_depletion_depth = m_bulk_depth - y;
    }
  }
  m_y_origin_min = m_bulk_depth - m_depletion_depth;

  ATH_MSG_INFO("------ initialization of e-h transport ------");
  ATH_MSG_INFO("\tm_bulk_depth      = " << m_bulk_depth << " [cm]");
  ATH_MSG_INFO("\tm_depletion_depth = " << m_depletion_depth << " [cm]");
  ATH_MSG_INFO("\tm_y_origin _min   = " << m_y_origin_min << " [cm]");

  //----for the case of uniform field model (SCT digitization model)------
  double Emean{m_biasVoltage / m_depletion_depth};
  m_driftMobility = mud_h(Emean);

#ifdef SCT_DIG_DEBUG
  ATH_MSG_INFO("----parameters for old SCT digitization model ----");
  ATH_MSG_INFO("\tmean E field      = " << Emean << " [KV/cm] ");
  ATH_MSG_INFO("\tdriftMobility     = " << m_driftMobility<< " [ ]");

  double diffusion{m_kB * m_sensorTemperature * m_driftMobility / m_e};
  ATH_MSG_INFO("\tdiffusion D       = " << diffusion << " [ ]");

  double r_h{0.72 - 0.0005*(m_sensorTemperature-CLHEP::STP_Temperature)};
  double tanLA{r_h * m_driftMobility * m_magneticField * 1.E-4};
  ATH_MSG_INFO("\ttan(LA)           = " << tanLA);
  ATH_MSG_INFO("\tLA                = " << atan(tanLA)*180./M_PI << " [degree]");
  ATH_MSG_INFO("-----------------------------------------------");

  ATH_MSG_INFO("EfieldModel\t" << m_eFieldModel << "\t(default = FEMsolution)");
  ATH_MSG_INFO("DepletionVoltage_VD\t" << m_depletionVoltage  << "\t(default = 70.)");
  ATH_MSG_INFO("BiasVoltage_VB\t" << m_biasVoltage << "\t(150.)");
  ATH_MSG_INFO("MagneticField_B\t" << m_magneticField  << "\t(-2.0)");
  ATH_MSG_INFO("Temperature_T\t" << m_sensorTemperature  << "\t(273.15)");
  ATH_MSG_INFO("TransportTimeStep \t" << m_transportTimeStep << "\t(0.25)");
  ATH_MSG_INFO("TransportTimeMax\t" << m_transportTimeMax  << "\t(25.0)");
  ATH_MSG_INFO("BulkDepth\t" << m_bulk_depth  << "\t(0.0285)");
  ATH_MSG_INFO("StripPitch\t" << m_strip_pitch  << "\t(0.0080)");
  ATH_MSG_INFO("----------------------------------------------");
#endif

}

//--------------------------------------------------------------
//   initialize drift mobility for electrons
//--------------------------------------------------------------
void SCT_DetailedSurfaceChargesGenerator::init_mud_e(double T) {
  m_vs_e = 1.53E9 * pow(T, -0.87);
  m_Ec_e = 1.01 * pow(T, 1.55);
  m_beta_e = 2.57E-2 * pow(T, 0.66);

#ifdef SCT_DIG_DEBUG
  ATH_MSG_INFO("---- parameters for electron transport -----");
  ATH_MSG_INFO("     m_vs_e    = " << m_vs_e);
  ATH_MSG_INFO("     m_Ec_e    = " << m_Ec_e);
  ATH_MSG_INFO("     m_beta_e  = " << m_beta_e);
#endif

  return;
}

//--------------------------------------------------------------
//   initialize drift mobility for holes
//--------------------------------------------------------------
void SCT_DetailedSurfaceChargesGenerator::init_mud_h(double T) {
  m_vs_h = 1.62E8 * pow(T, -0.52);
  m_Ec_h = 1.24 * pow(T, 1.68);
  m_beta_h = 0.46 * pow(T, 0.17);

#ifdef SCT_DIG_DEBUG
  ATH_MSG_INFO("----parameters for hole transport -----");
  ATH_MSG_INFO("     m_vs_h    = " << m_vs_h);
  ATH_MSG_INFO("     m_Ec_h    = " << m_Ec_h);
  ATH_MSG_INFO("     m_beta_h  = " << m_beta_h);
#endif

  return;
}

//--------------------------------------------------------------
//   initialize ExEyArray
//--------------------------------------------------------------
void SCT_DetailedSurfaceChargesGenerator::initExEyArray() {
  for (int ix{0}; ix<17; ix++) {
    for (int iy{0}; iy<115; iy++) {
      m_ExValue150[ix][iy] = ExValue150(ix, iy);
      m_EyValue150[ix][iy] = EyValue150(ix, iy);
      if (m_doHistoTrap) {
        m_h_yzEfield->Fill( ix*0.00025, iy*0.00025, sqrt(ExValue150(ix, iy)*ExValue150(ix, iy)+EyValue150(ix, iy)*EyValue150(ix, iy)));
        m_h_yzEfield->Fill(-ix*0.00025, iy*0.00025, sqrt(ExValue150(ix, iy)*ExValue150(ix, iy)+EyValue150(ix, iy)*EyValue150(ix, iy)));
        m_h_yEfield->Fill(  ix*0.0005,  iy*0.00025, ExValue150(ix, iy));
        m_h_yEfield->Fill( -ix*0.0005,  iy*0.00025, ExValue150(ix, iy));
        m_h_zEfield->Fill(  ix*0.0005,  iy*0.00025, EyValue150(ix, iy));
        m_h_zEfield->Fill( -ix*0.0005,  iy*0.00025, EyValue150(ix, iy));
      }
    }
  }
}
 
//--------------------------------------------------------------
//   initialize PotentialValue
//--------------------------------------------------------------
void SCT_DetailedSurfaceChargesGenerator::initPotentialValue() {
  for (int ix{0}; ix<81; ix++) {
    for (int iy{0}; iy<115; iy++) {
      m_PotentialValue[ix][iy] = GetPotentialValue(ix, iy);
      if (m_doHistoTrap) {
        m_h_yzRamo->Fill( ix*0.005, iy*0.0025, GetPotentialValue(ix, iy));
        m_h_yzRamo->Fill(-ix*0.005, iy*0.0025, GetPotentialValue(ix, iy));
      }
    }
  }
}

//---------------------------------------------------------------
//      parameters for electron transport
//---------------------------------------------------------------
bool SCT_DetailedSurfaceChargesGenerator::electron(double x_e, double y_e, double& vx_e, double& vy_e, double& D_e) const {
  double Ex, Ey;
  EField(x_e, y_e, Ex, Ey); // [V/cm]
  if (Ey > 0.) {
    const double REx{-Ex}; // because electron has negative charge 
    const double REy{-Ey}; // because electron has negative charge
    const double E{sqrt(Ex*Ex+Ey*Ey)};
    const double mu_e{mud_e(E)};
    const double v_e{mu_e * E};
    const double r_e{1.13+0.0008*(m_sensorTemperature-CLHEP::STP_Temperature)};
    const double tanLA_e{r_e * mu_e * (-m_magneticField) * 1.E-4}; // because e has negative charge
    const double secLA{sqrt(1.+tanLA_e*tanLA_e)};
    const double cosLA{1./secLA};
    const double sinLA{tanLA_e / secLA};
    vy_e = v_e * (REy*cosLA - REx*sinLA)/E;
    vx_e = v_e * (REx*cosLA + REy*sinLA)/E;
    D_e = m_kB * m_sensorTemperature * mu_e / m_e;
    return true;
  } else {
    return false;
  }
}

//---------------------------------------------------------------
//      parameters for hole transport
//---------------------------------------------------------------
bool SCT_DetailedSurfaceChargesGenerator::hole(double x_h, double y_h, double& vx_h, double& vy_h, double& D_h) const {
  double Ex, Ey;
  EField(x_h, y_h, Ex, Ey); // [V/cm]

  if (m_doHistoTrap) {
    m_h_efieldz->Fill(y_h, Ey);
  }

  if (Ey > 0.) {
    const double E{sqrt(Ex*Ex+Ey*Ey)};
    const double mu_h{mud_h(E)};
    const double v_h{mu_h * E};
    const double r_h{0.72 - 0.0005*(m_sensorTemperature-CLHEP::STP_Temperature)};
    const double tanLA_h{r_h * mu_h * m_magneticField * 1.E-4};
    const double secLA{sqrt(1.+tanLA_h*tanLA_h)};
    const double cosLA{1./secLA};
    const double sinLA{tanLA_h / secLA};

    vy_h = v_h * (Ey*cosLA - Ex*sinLA)/E;
    vx_h = v_h * (Ex*cosLA + Ey*sinLA)/E;
    D_h = m_kB * m_sensorTemperature * mu_h / m_e;
    return true;
  } else {
    return false;
  }
}

//-------------------------------------------------------------------
//    calculation of induced charge using Weighting (Ramo) function
//-------------------------------------------------------------------
double SCT_DetailedSurfaceChargesGenerator::induced(int istrip, double x, double y) const {
  // x and y are the coorlocation of charge (e or hole)
  // induced chardege on the strip "istrip" situated at the height y = d
  // the center of the strip (istrip=0) is x = 0.004 [cm]
  static const double deltax{0.0005};
  static const double deltay{0.00025};

  if (y < 0. or y > m_bulk_depth) return 0.;
  const double xc{m_strip_pitch * (istrip + 0.5)};
  const double dx{fabs(x-xc)};
  const int ix{static_cast<int>(dx / deltax)};
  if (ix > 79) return 0.;
  const int iy{static_cast<int>(y  / deltay)};
  const double fx{(dx - ix*deltax) / deltax};
  const double fy{( y - iy*deltay) / deltay};
  double P{  m_PotentialValue[ix  ][iy  ] *(1.-fx)*(1.-fy)
           + m_PotentialValue[ix+1][iy  ] *    fx *(1.-fy)
           + m_PotentialValue[ix  ][iy+1] *(1.-fx)*    fy
           + m_PotentialValue[ix+1][iy+1] *    fx *    fy};
#ifdef SCT_DIG_DEBUG
  ATH_MSG_DEBUG("induced: x,y,iy=" << x << " " << y << " " << iy << " istrip,xc,dx,ix="
                << istrip << " " << xc << " " << dx << " " <<ix << " fx,fy=" << fx << " " << fy << ", P=" << P);
#endif

  return P;
}

//--------------------------------------------------------------
//   Electric Field Ex and Ey
//--------------------------------------------------------------
void SCT_DetailedSurfaceChargesGenerator::EField(double x, double y, double& Ex, double& Ey) const {
  // m_eFieldModel == uniformEFieldModel; uniform E-field model
  // m_eFieldModel == flatDiodeModel; flat diode model
  // m_eFieldModel == FEMsolution; FEM solutions
  // x == 0.0040 [cm] : at the center of strip
  // x must be within 0 and 0.008 [cm]

  static const double deltay{0.00025}, deltax{0.00025}; // [cm]

  Ex = 0.;
  Ey = 0.;
  if (y < 0. or y > m_bulk_depth) return;

  //---------- case for uniform electric field ------------------------
  if (m_eFieldModel==uniformEFieldModel) {
    Ey = m_biasVoltage / m_depletion_depth;
    return;
  }

  //---------- case for flat diode model ------------------------------
  if (m_eFieldModel==flatDiodeModel) {
    if (m_biasVoltage > m_depletionVoltage) {
      Ey = (m_biasVoltage+m_depletionVoltage)/m_depletion_depth - 2.*m_depletionVoltage*(m_bulk_depth-y)/(m_bulk_depth*m_bulk_depth);
    } else {
      double Emax{2.* m_depletion_depth * m_depletionVoltage / (m_bulk_depth*m_bulk_depth)};
      Ey = Emax*(1-(m_bulk_depth-y)/m_depletion_depth);
    }
    return;
  }

  //---------- case for FEM analysis solution  -------------------------
  if (m_eFieldModel==FEMsolution) {
    int iy{static_cast<int>(y/deltay)};
    double fy{(y-iy*deltay) / deltay};
    double xhalfpitch{m_strip_pitch/2.};
    double x999{999.*m_strip_pitch};
    double xx{fmod (x+x999, m_strip_pitch)};
    double xxx{xx};
    if (xx > xhalfpitch) xxx = m_strip_pitch - xx;
    int ix{static_cast<int>(xxx/deltax)};
    double fx{(xxx - ix*deltax) / deltax};
#ifdef SCT_DIG_DEBUG
    ATH_MSG_DEBUG("x,y,ix,iy,fx,fy,xx,xxx= " << x << " " << y << " " << ix << " " << iy << " " << fx << " " << fy << " " << xx << " " << xxx);
#endif
    int ix1{ix+1};
    int iy1{iy+1};
    double Ex00{0.}, Ex10{0.}, Ex01{0.}, Ex11{0.};
    double Ey00{0.}, Ey10{0.}, Ey01{0.}, Ey11{0.};
    //-------- pick up appropriate data file for m_biasVoltage-----------------------
    int iVB{static_cast<int>(m_biasVoltage)};
    switch (iVB) {
      case 150:
        Ex00 = m_ExValue150[ix][iy];  Ex10 = m_ExValue150[ix1][iy];
        Ex01 = m_ExValue150[ix][iy1]; Ex11 = m_ExValue150[ix1][iy1];
        Ey00 = m_EyValue150[ix][iy];  Ey10 = m_EyValue150[ix1][iy];
        Ey01 = m_EyValue150[ix][iy1]; Ey11 = m_EyValue150[ix1][iy1];
        break;
      default:
        ATH_MSG_WARNING("Only +150 V bias voltage case is available. However, " << iVB << " V bias voltage is used. Electoric field is not extracted!!!");
        break;
    }
    //---------------- end of data bank search---
    Ex = Ex00*(1.-fx)*(1.-fy) + Ex10*fx*(1.-fy) + Ex01*(1.-fx)*fy + Ex11*fx*fy;
#ifdef SCT_DIG_DEBUG
    ATH_MSG_DEBUG("xx, xhalfpitch = " << xx << " " << xhalfpitch);
#endif
    if (xx > xhalfpitch) Ex = -Ex;
    Ey = Ey00*(1.-fx)*(1.-fy) + Ey10*fx*(1.-fy) + Ey01*(1.-fx)*fy + Ey11*fx*fy;
    return;
  }
  return;
}

//--------------------------------------------------------------
//   drift mobility for electrons
//--------------------------------------------------------------
double SCT_DetailedSurfaceChargesGenerator::mud_e(double E) const {
  return m_vs_e/m_Ec_e/pow(1.+pow(E/m_Ec_e, m_beta_e), 1./m_beta_e);
}

//--------------------------------------------------------------
//   drift mobility for holes
//--------------------------------------------------------------
double SCT_DetailedSurfaceChargesGenerator::mud_h(double E) const {
  return m_vs_h/m_Ec_h/pow(1.+pow(E/m_Ec_h, m_beta_h), 1./m_beta_h);
}

//---------------------------------------------------------------------
//  holeTransport
//---------------------------------------------------------------------
void SCT_DetailedSurfaceChargesGenerator::holeTransport(double& x0, double& y0, double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2, CLHEP::HepRandomEngine * rndmEngine) const {
  // transport holes in the bulk 
  // T. Kondo, 2010.9.9
  // External parameters to be specified
  //     m_transportTimeMax  [nsec]
  //     m_transportTimeStep [nsec]     
  //     m_bulk_depth        [cm]
  // Induced currents are added to
  //     Q_m2[50],Q_m1[50],Q_00[50],Q_p1[50],Q_p2[50] 

  double x{x0}; // original hole position [cm]
  double y{y0}; // original hole position [cm]
  bool isInBulk{true}; //PJ ?!
  double t_current{0.};
  double qstrip[5];
  double vx, vy, D;

  // -- Charge Trapping -- //
  static const double betaHoles{5.1E-16};
  double trappingHoles{1./(m_Fluence*betaHoles)};
  double u{CLHEP::RandFlat::shoot(0., 1.)};
  double drift_time{-TMath::Log(u)*trappingHoles};

  for (int istrip{-2}; istrip<=2; istrip++) {
    qstrip[istrip+2] = induced(istrip, x, y);
  }
#ifdef SCT_DIG_DEBUG
  ATH_MSG_DEBUG("h:qstrip=" << qstrip[0] << " " << qstrip[1] << " " << qstrip[2] << " " << qstrip[3] << " " << qstrip[4]);
#endif
  while (t_current < m_transportTimeMax) {
    if (not isInBulk) break;
    if (not hole(x, y, vx, vy, D)) break;
    double delta_y{vy * m_transportTimeStep *1.E-9};
    y += delta_y;
    double dt{m_transportTimeStep};
    if (y > m_bulk_depth) {
      isInBulk = false;
      dt = (m_bulk_depth - (y-delta_y))/delta_y * m_transportTimeStep;
      y = m_bulk_depth;
    }
    t_current += dt;

    // -- Charge Trapping -- //
    if (m_doTrapping) {
      if (drift_time < t_current) break;
    }

    x += vx*dt*1.E-9;
    double diffusion{sqrt(2.*D*dt*1.E-9)};
    y += diffusion * CLHEP::RandGaussZiggurat::shoot(rndmEngine);
    x += diffusion * CLHEP::RandGaussZiggurat::shoot(rndmEngine);
    if (y > m_bulk_depth) {
      y = m_bulk_depth;
      isInBulk = false;
    }

    // Get induced current by subtracting induced charges
#ifdef SCT_DIG_DEBUG
    ATH_MSG_DEBUG("h:t,x,y=" << t_current << ", " << x*1e4 << ", " << y*1e4);
#endif
    for (int istrip{-2}; istrip<=2; istrip++) {
      double qnew{induced(istrip, x, y)};
      int jj{istrip + 2};
      double dq{qnew - qstrip[jj]};
      qstrip[jj] = qnew;
#ifdef SCT_DIG_DEBUG
      ATH_MSG_DEBUG("dq= " << dq);
#endif
      int jt{static_cast<int>((t_current+0.001) / 0.50)};
      if (jt<50) {
        switch (istrip) {
          case -2: Q_m2[jt] += dq; break;
          case -1: Q_m1[jt] += dq; break;
          case  0: Q_00[jt] += dq; break;
          case +1: Q_p1[jt] += dq; break;
          case +2: Q_p2[jt] += dq; break;
            //          default: break; // logically dead code
        } 
      }
    }
#ifdef SCT_DIG_DEBUG
    ATH_MSG_DEBUG("h:qstrip=" << qstrip[0] << " " << qstrip[1] << " " << qstrip[2] << " " << qstrip[3] << " " << qstrip[4]);
#endif
  } // end of hole tracing
#ifdef SCT_DIG_DEBUG
  ATH_MSG_DEBUG("holeTransport : x,y=(" << x0*1.e4<< "," <<y0*1.e4<< ")->(" << x*1.e4<< "," <<y*1.e4 << ") t=" << t_current);
#endif
  return;
}

//---------------------------------------------------------------------
//  electronTransport
//---------------------------------------------------------------------
void SCT_DetailedSurfaceChargesGenerator::electronTransport(double& x0, double& y0, double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2, CLHEP::HepRandomEngine * rndmEngine) const {
  // transport electrons in the bulk
  // T. Kondo, 2010.9.10
  // External parameters to be specified
  //     m_transportTimeMax  [nsec]
  //     m_transportTimeStep [nsec]
  //     m_y_origin_min      [cm]  0 except under-depleted cases
  // Induced currents are added to
  //     Q_m2[50],Q_m1[50],Q_00[50],Q_p1[50],Q_p2[50]

  double x{x0}; // original electron position [cm]
  double y{y0}; // original electron position [cm]
  bool isInBulk{true};
  double t_current{0.};
  double qstrip[5];
  double vx, vy, D;

  // -- Charge Trapping -- //
  static const double betaElectrons{3.1E-16};
  const double trappingElectrons{1./(m_Fluence*betaElectrons)};
  const double u{CLHEP::RandFlat::shoot(0., 1.)};
  const double drift_time{-TMath::Log(u)*trappingElectrons};

  for (int istrip{-2}; istrip<=2; istrip++) {
    qstrip[istrip+2] = -induced(istrip, x, y);
  }

  // note  0.004[cm] is to shift from Richard's to Taka's coordinates
  while (t_current < m_transportTimeMax) {
    if (not isInBulk) break;
    if (not electron(x, y, vx, vy, D)) break;
    double delta_y{vy * m_transportTimeStep *1.E-9};
    y += delta_y;
    double dt{m_transportTimeStep}; // [nsec]
    if (y < m_y_origin_min) {
      isInBulk = false;
      dt = (m_y_origin_min - (y-delta_y))/delta_y * m_transportTimeStep;
      y = m_y_origin_min;
    }
    t_current += dt;
 
    // -- Charge Trapping -- //
    if (m_doTrapping) {
      if (drift_time < t_current) break;
    }

    x += vx * dt *1.E-9;
    double diffusion{sqrt(2.* D * dt*1.E-9)};
    y += diffusion * CLHEP::RandGaussZiggurat::shoot(rndmEngine);
    x += diffusion * CLHEP::RandGaussZiggurat::shoot(rndmEngine);
    if (y < m_y_origin_min) {
      y = m_y_origin_min;
      isInBulk = false;
    }
    //   get induced current by subtracting induced charges
    for (int istrip{-2}; istrip<=2; istrip++) {
      double qnew{-induced(istrip, x, y)};
      int jj{istrip + 2};
      double dq{qnew - qstrip[jj]};
      qstrip[jj] = qnew;
#ifdef SCT_DIG_DEBUG
      if (istrip==0) ATH_MSG_DEBUG("e:t,x,y=" << t_current << " " << x*1e4 << " " << y*1e4 << " dq[0]=" << dq);
#endif
      int jt{static_cast<int>((t_current + 0.001) / 0.50)};
      if (jt<50) {
        switch (istrip) {
          case -2: Q_m2[jt] += dq; break;
          case -1: Q_m1[jt] += dq; break;
          case  0: Q_00[jt] += dq; break;
          case +1: Q_p1[jt] += dq; break;
          case +2: Q_p2[jt] += dq; break;
            //          default: break; // logically dead code
        }
      }
    }
  }  // end of hole tracing
#ifdef SCT_DIG_DEBUG
  ATH_MSG_DEBUG("elecTransport : x,y=(" << x0*1.e4 << "," << y0*1.e4 << ")->(" << x*1.e4 << "," << y*1.e4 << ") t=" << t_current);
#endif

  return;
}

//---------------------------------------------------------------------
//  Charge map calculations
//---------------------------------------------------------------------
double SCT_DetailedSurfaceChargesGenerator::inducedCharge (int& strip, double& x, double& y, double& t) const {
  // x and y are in um, t is in ns.
  double charge{0.};
  
  static const double dt{0.5};
  double fx, fy, ft;
  int ix, iy, it;//, iy1, it1;
  
  if (strip<-2 or strip>2) return charge;
  if (x<0. or x>=m_strip_pitch*10000.) return charge;
  if (y<0. or y>=m_bulk_depth*10000.) return charge;
  if (t<0. or t>=m_transportTimeMax) return charge;

  fx = x / m_stripCharge_dx;
  ix = static_cast<int>(fx);
  fx -= ix;

  fy = (y - 0.5*m_stripCharge_dy) / m_stripCharge_dy;
  iy = static_cast<int>(fy);
  fy -= iy;
  //iy1 = iy + 1;
  if (y <= 0.5*m_stripCharge_dy) {
    fy = 0.;
    iy = 0;
    /*iy1 = 0;*/
  }
  if (y >= m_bulk_depth*10000.-0.5*m_stripCharge_dy) {
    fy = 1.;
    iy = m_stripCharge_iymax;
    /*iy1 = iy;*/
  }

  ft = (t - m_transportTimeStep) / dt;
  it = static_cast<int>(ft);
  ft -= it;
  //it1 = it + 1;
  if (t <= m_transportTimeStep) {
    ft = 0.;
    it = 0;
    /*it1= 0;*/
  }
  if (t >= m_transportTimeMax-m_transportTimeStep) {
    ft = 1.;
    it = 49;
    /*it1=49;*/
  }

  double p000{0.};//m_stripCharge[strip+2][ix][iy][it];
  double p010{0.};//m_stripCharge[strip+2][ix][iy1][it];
  double p100{0.};//m_stripCharge[strip+2][ix+1][iy][it];
  double p110{0.};//m_stripCharge[strip+2][ix+1][iy1][it];
  double p001{0.};//m_stripCharge[strip+2][ix][iy][it1];
  double p011{0.};//m_stripCharge[strip+2][ix][iy1][it1];
  double p101{0.};//m_stripCharge[strip+2][ix+1][iy][it1];
  double p111{0.};//m_stripCharge[strip+2][ix+1][iy1][it1];

  const double charge0{p000*(1.-fx)*(1.-fy) + p010*(1.-fx)*fy + p100*fx*(1.-fy) + p110*fx*fy};
  const double charge1{p001*(1.-fx)*(1.-fy) + p011*(1.-fx)*fy + p101*fx*(1.-fy) + p111*fx*fy};

  charge = charge0*(1.-ft) + charge1*ft;

  return charge;
}

// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Header file for SCT_DetailedSurfaceChargesGenerator Class
 * @brief Class to drift the charged hits to the sensor surface for SCT
 * Based on SCT_SurfaceChargesGenerator
 * 23/08/2007 - Kondo.Gnanvo@cern.ch           
 *            - Conversion of the SCT_SurfaceChargesGenerator code AlgTool
 */

#ifndef SCT_DIGITIZATION_SCT_DETAILEDSURFACECHARGESGENERATOR_H
#define SCT_DIGITIZATION_SCT_DETAILEDSURFACECHARGESGENERATOR_H

//Inheritance
#include "AthenaBaseComps/AthAlgTool.h"
#include "SiDigitization/ISurfaceChargesGenerator.h"

// Athena
#include "InDetConditionsSummaryService/ISiliconConditionsTool.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "SCT_ModuleDistortions/ISCT_ModuleDistortionsTool.h"
#include "SiPropertiesTool/ISiPropertiesTool.h"

// Gaudi
#include "GaudiKernel/ITHistSvc.h" // for ITHistSvc
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// C++ Standard Library
#include <string>

// Charges and hits
class SiHit;

// -- do histos
class TProfile;
class TProfile2D;

namespace InDetDD {
  class SiDetectorElement;
}

namespace CLHEP {
  class HepRandomEngine;
}

/**
 * Class to take calculate Charge Transport in the SCT with a detailed charge transport model.
 * It's not clear (29 April 2016) that the code has ever been used successfully; prior to this edit
 * certain key parameters were uninitialised and yet used.
 **/

class SCT_DetailedSurfaceChargesGenerator : public extends<AthAlgTool, ISurfaceChargesGenerator> {
public:

  /**  constructor */
  SCT_DetailedSurfaceChargesGenerator(const std::string& type, const std::string& name, const IInterface* parent);

  /** Destructor */
  virtual ~SCT_DetailedSurfaceChargesGenerator() = default;

  /** AlgTool initialize */
  virtual StatusCode initialize() override;

  /** AlgTool finalize */
  virtual StatusCode finalize() override;

private:
  enum ChargeDriftMode {
    defaultSCTModel=0,
    ehTransport=1,
    fixedChargeMap=2
  };
  enum EFieldModel {
    uniformEFieldModel=0,
    flatDiodeModel=1,
    FEMsolution=2
  };

  // Non-const methods are used in initialization
  virtual void setFixedTime(float fixedTime) override { m_tfix.setValue(fixedTime); }

  /** create a list of surface charges from a hit */
  virtual void process(const InDetDD::SiDetectorElement* element, const TimedHitPtr<SiHit>& phit, ISiSurfaceChargesInserter& inserter, CLHEP::HepRandomEngine * rndmEngine, const EventContext& ctx) override;
  void processSiHit(const InDetDD::SiDetectorElement* element, const SiHit& phit, ISiSurfaceChargesInserter& inserter, const float eventTime, const unsigned short eventID, CLHEP::HepRandomEngine * rndmEngine, const EventContext& ctx);

  // some diagnostics methods are needed here too
  float DriftTime(float zhit, const InDetDD::SiDetectorElement* element, const EventContext& ctx) const; //!< calculate drift time perpandicular to the surface for a charge at distance zhit from mid gap
  float DiffusionSigma(float zhit, const InDetDD::SiDetectorElement* element, const EventContext& ctx) const; //!< calculate diffusion sigma from a gaussian dist scattered charge
  float SurfaceDriftTime(float ysurf) const; //!< Calculate of the surface drift time
  float MaxDriftTime(const InDetDD::SiDetectorElement* element, const EventContext& ctx) const; //!< max drift charge equivalent to the detector thickness
  float MaxDiffusionSigma(const InDetDD::SiDetectorElement* element, const EventContext& ctx) const; //!< max sigma diffusion

  // methods for Taka Kondos's new charge drift model
  // Non-const methods are used in initialization
  void initTransportModel();
  void init_mud_e(double T);
  void init_mud_h(double T);
  void initExEyArray();
  void initPotentialValue();

  void EField(double x, double y, double& Ex, double& Ey) const;
  double induced (int istrip, double x, double y) const;
  bool electron(double x_e, double y_e, double& vx_e, double& vy_e, double& D_e) const;
  bool hole( double x_h, double y_h, double& vx_h, double& vy_h, double& D_h);
  double mud_e(double E) const;
  double mud_h(double E) const;
  double ExValue150(int ix, int iy) const;
  double EyValue150(int ix, int iy) const;
  double GetPotentialValue(int ix, int iy) const;
  void holeTransport(double& x0, double& y0, double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2, CLHEP::HepRandomEngine * rndmEngine);
  void electronTransport(double& x0, double& y0, double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2, CLHEP::HepRandomEngine * rndmEngine) const;
  double inducedCharge(int& istrip, double& x, double& y, double& t) const;

  IntegerProperty m_numberOfCharges{this, "NumberOfCharges", 1}; //!< number of charges
  FloatProperty m_smallStepLength{this, "SmallStepLength", 5.}; //!< max internal step along the larger G4 step

  /** related to the surface drift */
  FloatProperty m_tSurfaceDrift{this, "SurfaceDriftTime", 10., "max surface drift time"};

  FloatProperty m_tfix{this, "FixedTime", -999., "ixed time"};
  FloatProperty m_tsubtract{this, "SubtractTime", -999., "subtract drift time from mid gap"};

  BooleanProperty m_doDistortions{this, "doDistortions", false, "Flag to set simulation of module distortions"};

  // -- Charge Trapping -- //
  BooleanProperty m_doHistoTrap{this, "doHistoTrap", false, "Allow filling of histos for charge trapping effect"};
  BooleanProperty m_doTrapping{this, "doTrapping", false, "Simulation of charge trapping effect"};
  DoubleProperty m_Fluence{this, "Fluence", 0., "Fluence for charge trapping effect"};

  //TK model settings
  IntegerProperty m_chargeDriftModel{this, "ChargeDriftModel", ehTransport, "use enum ChargeDriftMode"};
  IntegerProperty m_eFieldModel{this, "EFieldModel", FEMsolution, "use enum EFieldModel"};

  //------TK parameters given externally by jobOptions ------------------
  DoubleProperty m_depletionVoltage{this, "DepletionVoltage", 70.};
  DoubleProperty m_biasVoltage{this, "BiasVoltage", 150.};
  DoubleProperty m_magneticField{this, "MagneticField", -2.0};
  DoubleProperty m_sensorTemperature{this, "SensorTemperature", 0.+CLHEP::STP_Temperature};
  DoubleProperty m_transportTimeStep{this, "TransportTimeStep", 0.25};
  DoubleProperty m_transportTimeMax{this, "TransportTimeMax", 25.0};

  BooleanProperty m_isOverlay{this, "isOverlay", false, "flag for overlay"};

  //ToolHandles
  ToolHandle<ISCT_ModuleDistortionsTool> m_distortionsTool{this, "SCTDistortionsTool", "SCT_DistortionsTool", "Tool to retrieve SCT distortions"};
  ToolHandle<ISiPropertiesTool> m_siPropertiesTool{this, "SiPropertiesTool", "SCT_SiPropertiesTool", "Tool to retrieve SCT silicon properties"};
  ToolHandle<ISiliconConditionsTool> m_siConditionsTool{this, "SiConditionsTool", "SCT_SiliconConditionsTool", "Tool to retrieve SCT silicon information"};
  ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool{this, "LorentzAngleTool", "SiLorentzAngleTool/SCTLorentzAngleTool", "Tool to retreive Lorentz angle"};

  ServiceHandle<ITHistSvc> m_thistSvc{this, "THistSvc", "THistSvc"};

  float m_tHalfwayDrift{0.}; //!< Surface drift time
  float m_distInterStrip{1.0}; //!< Inter strip distance normalized to 1
  float m_distHalfInterStrip{0.}; //!< Half way distance inter strip

  bool m_SurfaceDriftFlag{false}; //!< surface drift ON/OFF

  //------TK parameters mostly fixed but can be changed externally  ------------
  double m_bulk_depth{0.0285}; //<!285 micron, expressed in cm units
  double m_strip_pitch{0.0080}; //<! 80 micron, expressed in cm units
  double m_depletion_depth{0.0285};
  double m_y_origin_min{0.0}; //<! zero unless under-depleted

  //------TK parameters for e, h transport --------------------------------
  double m_kB{1.38E-23}; //<! Boltzmann const [m^2*kg/s^2/K]
  double m_e{1.602E-19}; //<! electron charge [Coulomb]
  double m_vs_e{11615084.7393}; //<! mobility at 273.15K
  double m_Ec_e{6034.20429};
  double m_vs_h{8761659.83530}; //<! hole mobility at 273.15K
  double m_Ec_h{15366.52650};
  double m_beta_e{};
  double m_beta_h{};
  double m_driftMobility{};

  //------TK arrays of FEM analysis -----------------------------------
  double m_PotentialValue[81][115]{{0.}};
  double m_ExValue150[17][115]{{0.}};
  double m_EyValue150[17][115]{{0.}};

  //------TK parameters for charge map, uses file storage of map....
  // This member makes the class very large --- large enough that it fails
  // ubsan's sanity checks and produces a false positive.  However, it is not
  // actually used, so comment it out.  If it is ever actually needed,
  // then it should be allocated dynamically rather than being allocated
  // inline to the class.
  // double m_stripCharge[5][81][285][50]{{{{0.}}}};
  // sroe: the following were never initialised before, which begs the question:
  // Did this code *ever* work? Has it *ever* been used?
  int m_stripCharge_iymax{285-1};
  double m_stripCharge_dx{1.};
  double m_stripCharge_dy{1.};

  // -- Histograms
  TProfile* m_h_efieldz{nullptr};
  TProfile2D* m_h_yzRamo{nullptr};
  TProfile2D* m_h_yzEfield{nullptr};
  TProfile2D* m_h_yEfield{nullptr};
  TProfile2D* m_h_zEfield{nullptr};
};

#endif // SCT_DETAILEDSURFACECHARGESGENERATOR_H

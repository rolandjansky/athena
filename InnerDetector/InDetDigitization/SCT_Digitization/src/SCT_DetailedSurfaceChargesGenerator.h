/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "SCT_Digitization/ISCT_SurfaceChargesGenerator.h"

// Athena
#include "InDetConditionsSummaryService/ISiliconConditionsTool.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "SCT_ModuleDistortions/ISCT_ModuleDistortionsTool.h"
#include "SiPropertiesSvc/ISiPropertiesTool.h"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// C++ Standard Library
#include <string>

// Charges and hits
class SiHit;

// -- do histos
class ITHistSvc;
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

class SCT_DetailedSurfaceChargesGenerator : public extends<AthAlgTool, ISCT_SurfaceChargesGenerator> {
 public:

  /**  constructor */
  SCT_DetailedSurfaceChargesGenerator(const std::string& type, const std::string& name, const IInterface* parent);

  /** Destructor */
  virtual ~SCT_DetailedSurfaceChargesGenerator() = default;

  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize */
  virtual StatusCode finalize();

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
  void setFixedTime(float fixedTime) { m_tfix = fixedTime; }

  /** create a list of surface charges from a hit */
  virtual void process(const InDetDD::SiDetectorElement* element, const TimedHitPtr<SiHit>& phit, const ISiSurfaceChargesInserter& inserter, CLHEP::HepRandomEngine * rndmEngine) const;
  void processSiHit(const InDetDD::SiDetectorElement* element, const SiHit& phit, const ISiSurfaceChargesInserter& inserter, const float eventTime, const unsigned short eventID, CLHEP::HepRandomEngine * rndmEngine) const;

  // some diagnostics methods are needed here too
  float DriftTime(float zhit, const InDetDD::SiDetectorElement* element) const; //!< calculate drift time perpandicular to the surface for a charge at distance zhit from mid gap
  float DiffusionSigma(float zhit, const InDetDD::SiDetectorElement* element) const; //!< calculate diffusion sigma from a gaussian dist scattered charge
  float SurfaceDriftTime(float ysurf) const; //!< Calculate of the surface drift time
  float MaxDriftTime(const InDetDD::SiDetectorElement* element) const; //!< max drift charge equivalent to the detector thickness
  float MaxDiffusionSigma(const InDetDD::SiDetectorElement* element) const; //!< max sigma diffusion

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
  bool hole( double x_h, double y_h, double& vx_h, double& vy_h, double& D_h) const;
  double mud_e(double E) const;
  double mud_h(double E) const;
  double ExValue150(int ix, int iy) const;
  double EyValue150(int ix, int iy) const;
  double GetPotentialValue(int ix, int iy) const;
  void holeTransport(double& x0, double& y0, double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2, CLHEP::HepRandomEngine * rndmEngine) const;
  void electronTransport(double& x0, double& y0, double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2, CLHEP::HepRandomEngine * rndmEngine) const;
  double inducedCharge(int& istrip, double& x, double& y, double& t) const;

  int m_numberOfCharges; //!< number of charges
  float m_smallStepLength; //!< max internal step along the larger G4 step

  /** related to the surface drift */
  float m_tSurfaceDrift; //!< Surface drift time
  float m_tHalfwayDrift; //!< Surface drift time
  float m_distInterStrip; //!< Inter strip distance normalized to 1
  float m_distHalfInterStrip; //!< Half way distance inter strip

  bool m_SurfaceDriftFlag; //!< surface drift ON/OFF

  float m_tfix; //!< fixed time
  float m_tsubtract; //!< subtract drift time from mid gap

  bool m_doDistortions; //!< Flag to set Distortions

  // -- Charge Trapping -- //
  bool m_doHistoTrap; //!< Flag that allows to fill the histograms
  bool m_doTrapping;  //!< Flag to set Charge Trapping
  double m_Fluence; //!< Fluence used for Charge Trapping

  // -- Histograms
  ServiceHandle<ITHistSvc> m_thistSvc;
  TProfile* m_h_efieldz;
  TProfile2D* m_h_yzRamo;
  TProfile2D* m_h_yzEfield;
  TProfile2D* m_h_yEfield;
  TProfile2D* m_h_zEfield;

  //TK model settings
  int m_chargeDriftModel; // use enum ChargeDriftMode
  int m_eFieldModel; // use enum EFieldModel

  //------TK parameters given externally by jobOptions ------------------
  double m_depletionVoltage;
  double m_biasVoltage;
  double m_magneticField;
  double m_sensorTemperature;
  double m_transportTimeStep;
  double m_transportTimeMax;

  //------TK parameters mostly fixed but can be changed externally  ------------
  double m_bulk_depth;
  double m_strip_pitch;
  double m_depletion_depth;
  double m_y_origin_min;

  //------TK parameters for e, h transport --------------------------------
  double m_kB;
  double m_e;
  double m_vs_e;
  double m_Ec_e;
  double m_vs_h;
  double m_Ec_h;
  double m_beta_e;
  double m_beta_h;
  double m_driftMobility;

  //------TK arrays of FEM analysis -----------------------------------
  double m_PotentialValue[81][115];
  double m_ExValue150[17][115];
  double m_EyValue150[17][115];

  //------TK parameters for charge map, uses file storage of map....
  // This member makes the class very large --- large enough that it fails
  // ubsan's sanity checks and produces a false positive.  However, it is not
  // actually used, so comment it out.  If it is ever actually needed,
  // then it should be allocated dynamically rather than being allocated
  // inline to the class.
  //double m_stripCharge[5][81][285][50];
  int m_stripCharge_iymax;
  double m_stripCharge_dx;
  double m_stripCharge_dy;

  //ToolHandles
  ToolHandle<ISCT_ModuleDistortionsTool> m_distortionsTool{this, "SCTDistortionsTool", "SCT_DistortionsTool", "Tool to retrieve SCT distortions"};
  ToolHandle<ISiPropertiesTool> m_siPropertiesTool{this, "SiPropertiesTool", "SCT_SiPropertiesTool", "Tool to retrieve SCT silicon properties"};
  ToolHandle<ISiliconConditionsTool> m_siConditionsTool{this, "SiConditionsTool", "SCT_SiliconConditionsTool", "Tool to retrieve SCT silicon information"};
  ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool{this, "LorentzAngleTool", "SiLorentzAngleTool/SCTLorentzAngleTool", "Tool to retreive Lorentz angle"};

  bool m_isOverlay; // flag for overlay
};

#endif // SCT_DETAILEDSURFACECHARGESGENERATOR_H

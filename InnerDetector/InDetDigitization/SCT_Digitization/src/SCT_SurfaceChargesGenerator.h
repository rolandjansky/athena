// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Header file for SCT_SurfaceChargesGenerator Class
 * @brief Class to drift the charged hits to the sensor surface for SCT
 * version 1.0a Szymon Gadomski 31.05.2001
 * @author Szymon.Gadomski@cern.ch, Awatif.Belymam@cern.ch, Davide.Costanzo@cern.ch,    
 * tgcornel@nikhef.nl, Grant.Gorfine@cern.ch, Paul.Bell@cern.ch, Jorgen.Dalmau@cern.ch, 
 * This is based on the SCTtest3SurfaceChargesGenerator
 * compared to "test3", changes are all related to interfaces
 * this should work with the new SCT_BarrelModuleSideDesign
 * Compared to "test2" the following have been added in "test3":
 * - a possibility to work with long steps and to smear charge along
 *   the step in smaller steps
 * - Lorentz angle
 * 19/04/2001 -  change of hit class to SiTrackerHit
 * version 1.0 Szymon Gadomski 09/06/2001, compiles with framework
 * 26/07/2001 - changed to work with SiDigitization-00-02-00, tested
 * 20/10/2002 - Awatif Belymam
 * 29/10/2002 - the thickness of sensors is obtained from the objects
 *            - of SCT_ModuleSideDesign type instead from its jobOptions file.
 *            - Changes are done as well in jobOptions files.
 * 06/01/2004 - Everything is now in CLHEP units (mm, ns, MeV)
 * 23/08/2007 - Kondo.Gnanvo@cern.ch           
 *            - Conversion of the SCT_SurfaceChargesGenerator code Al gTool
 */

#ifndef SCT_DIGITIZATION_SCT_SURFACECHARGESGENERATOR_H
#define SCT_DIGITIZATION_SCT_SURFACECHARGESGENERATOR_H

//Inheritance
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_Digitization/ISCT_SurfaceChargesGenerator.h"

#include "Identifier/IdentifierHash.h"
#include "InDetConditionsSummaryService/ISiliconConditionsTool.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "SCT_ConditionsTools/ISCT_RadDamageSummaryTool.h"
#include "SCT_ModuleDistortions/ISCT_ModuleDistortionsTool.h"
#include "SiPropertiesTool/ISiPropertiesTool.h"

// Gaudi
#include "GaudiKernel/ITHistSvc.h" // for ITHistSvc
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

#include <iostream>

// Charges and hits
class SiHit;

// -- do charge trapping histos
class ITHistSvc;
class TH1F;
class TH2F;
class TProfile;

namespace InDetDD {
  class SiDetectorElement;
  class SCT_ModuleSideDesign;
}

namespace CLHEP {
  class HepRandomEngine;
}

template <class HIT> class TimedHitPtr;

class SCT_SurfaceChargesGenerator : public extends<AthAlgTool, ISCT_SurfaceChargesGenerator> {
 public:

  /**  constructor */
  SCT_SurfaceChargesGenerator(const std::string& type, const std::string& name, const IInterface* parent);

  /** Destructor */
  virtual ~SCT_SurfaceChargesGenerator() = default;

  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize */
  virtual StatusCode finalize();

 private:

  virtual void setFixedTime(float fixedTime)                             {m_tfix = fixedTime;}

  /** create a list of surface charges from a hit */
  virtual void process(const InDetDD::SiDetectorElement* element, const TimedHitPtr<SiHit>& phit, const ISiSurfaceChargesInserter& inserter, CLHEP::HepRandomEngine * rndmEngine) const;
  void processSiHit(const InDetDD::SiDetectorElement* element, const SiHit& phit, const ISiSurfaceChargesInserter& inserter, float eventTime, unsigned short eventID, CLHEP::HepRandomEngine * rndmEngine) const;
  
  // some diagnostics methods are needed here too
  float driftTime(float zhit, const InDetDD::SiDetectorElement* element) const; //!< calculate drift time perpandicular to the surface for a charge at distance zhit from mid gap
  float diffusionSigma(float zhit, const InDetDD::SiDetectorElement* element) const; //!< calculate diffusion sigma from a gaussian dist scattered charge
  float surfaceDriftTime(float ysurf) const; //!< Calculate of the surface drift time
  float maxDriftTime(const InDetDD::SiDetectorElement* element) const; //!< max drift charge equivalent to the detector thickness
  float maxDiffusionSigma(const InDetDD::SiDetectorElement* element) const; //!< max sigma diffusion

  // trap_pos and drift_time are updated based on spess.
  bool chargeIsTrapped(double spess, const InDetDD::SiDetectorElement* element, double& trap_pos, double& drift_time) const;

  IntegerProperty m_numberOfCharges{this, "NumberOfCharges", 1, "number of charges"};
  FloatProperty m_smallStepLength{this, "SmallStepLength", 5 * CLHEP::micrometer, "max internal step along the larger G4 step"};

  /** related to the surface drift */
  FloatProperty m_tSurfaceDrift{this, "SurfaceDriftTime", 10 * CLHEP::ns, "max surface drift time"};

  FloatProperty m_tfix{this, "FixedTime", -999., "fixed time"};
  FloatProperty m_tsubtract{this, "SubtractTime", -999., "subtract drift time from mid gap"};

  BooleanProperty m_doDistortions{this, "doDistortions", false, "Simulation of module distortions"};
  BooleanProperty m_useSiCondDB{this, "UseSiCondDB", false, "Usage of SiConditions DB values can be disabled to use setable ones"};
  FloatProperty m_vdepl{this, "DepletionVoltage", 70., "depletion voltage, default 70V"};
  FloatProperty m_vbias{this, "BiasVoltage", 150., "bias voltage, default 150V"};
  BooleanProperty m_doTrapping{this, "doTrapping", false, "Flag to set Charge Trapping"};
  BooleanProperty m_doHistoTrap{this, "doHistoTrap", false, "Histogram the charge trapping effect"};
  BooleanProperty m_doRamo{this, "doRamo", false, "Ramo Potential for charge trapping effect"};
  BooleanProperty m_isOverlay{this, "isOverlay", false, "flag for overlay"};

  //ToolHandles
  ToolHandle<ISCT_ModuleDistortionsTool> m_distortionsTool{this, "SCTDistortionsTool", "SCT_DistortionsTool", "Tool to retrieve SCT distortions"};
  ToolHandle<ISiPropertiesTool> m_siPropertiesTool{this, "SiPropertiesTool", "SCT_SiPropertiesTool", "Tool to retrieve SCT silicon properties"};
  ToolHandle<ISCT_RadDamageSummaryTool> m_radDamageTool{this, "RadDamageSummaryTool", "SCT_RadDamageSummaryTool", "Tool to retrieve SCT radiation damages"};
  ToolHandle<ISiliconConditionsTool> m_siConditionsTool{this, "SiConditionsTool", "SCT_SiliconConditionsTool", "Tool to retrieve SCT silicon information"};
  ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool{this, "LorentzAngleTool", "SiLorentzAngleTool/SCTLorentzAngleTool", "Tool to retreive Lorentz angle"};

  ServiceHandle<ITHistSvc> m_thistSvc{this, "THistSvc", "THistSvc"};

  float m_tHalfwayDrift{0.}; //!< Surface drift time
  float m_distInterStrip{1.0}; //!< Inter strip distance normalized to 1
  float m_distHalfInterStrip{0.}; //!< Half way distance inter strip

  bool m_SurfaceDriftFlag{false}; //!< surface drift ON/OFF

  // -- Histograms
  TProfile* m_h_efieldz{nullptr};
  TH1F* m_h_efield{nullptr};
  TH1F* m_h_spess{nullptr};
  TH1F* m_h_depD{nullptr};
  TH2F* m_h_drift_electrode{nullptr};
  TH1F* m_h_ztrap{nullptr};
  TH1F* m_h_drift_time{nullptr};
  TH1F* m_h_t_electrode{nullptr};
  TH1F* m_h_zhit{nullptr};
  TH1F* m_h_ztrap_tot{nullptr};
  TH1F* m_h_no_ztrap{nullptr};
  TH1F* m_h_trap_drift_t{nullptr};
  TH1F* m_h_notrap_drift_t{nullptr};
  TProfile* m_h_mob_Char{nullptr};
  TProfile* m_h_vel{nullptr};
  TProfile* m_h_drift1{nullptr};
  TProfile* m_h_gen{nullptr};
  TProfile* m_h_gen1{nullptr};
  TProfile* m_h_gen2{nullptr};
  TProfile* m_h_velocity_trap{nullptr};
  TProfile* m_h_mobility_trap{nullptr};
  TH1F* m_h_trap_pos{nullptr};
};

#endif // SCT_SURFACECHARGESGENERATOR_H

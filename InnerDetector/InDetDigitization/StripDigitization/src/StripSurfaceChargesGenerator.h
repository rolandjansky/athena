/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Header file for StripSurfaceChargesGenerator Class
 * @brief Class to drift the charged hits to the sensor surface for ITK strip
 */

#ifndef STRIP_DIGITIZATION_STRIP_SURFACECHARGESGENERATOR_H
#define STRIP_DIGITIZATION_STRIP_SURFACECHARGESGENERATOR_H

//Inheritance
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_Digitization/ISCT_SurfaceChargesGenerator.h"

#include "Identifier/IdentifierHash.h"
#include "InDetConditionsSummaryService/ISiliconConditionsSvc.h"
#include "SCT_ConditionsServices/ISCT_RadDamageSummarySvc.h"
#include "SCT_ModuleDistortions/ISCT_ModuleDistortionsTool.h"
#include "SiPropertiesSvc/ISiPropertiesSvc.h"
#include "SiPropertiesSvc/SiliconProperties.h"

// Gaudi
#include "GaudiKernel/ITHistSvc.h" // for ITHistSvc
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

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

//class StripSurfaceChargesGenerator : public AthAlgTool, virtual public ISCT_SurfaceChargesGenerator {
class StripSurfaceChargesGenerator : public extends<AthAlgTool, ISCT_SurfaceChargesGenerator> {
public:

  /**  constructor */
  StripSurfaceChargesGenerator(const std::string& type, const std::string& name, const IInterface* parent);

  /** Destructor */
  virtual ~StripSurfaceChargesGenerator() = default;

  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize */
  virtual StatusCode finalize();


private:

  virtual void setFixedTime(float fixedTime)                             {m_tfix = fixedTime;}

  // variable to bridge 22/21 interface; TO BE REMOVED
  float m_comTime{0};           //!< use cosmics time for timing
  bool   m_useComTime{false};   //!< Flag to decide the use of cosmics time for timing
  bool   m_cosmicsRun{false};   //!< Flag to set Cosmics Run
  const InDetDD::SiDetectorElement * m_element{nullptr};
  CLHEP::HepRandomEngine *           m_rndmEngine{nullptr};
  void setDetectorElement(const InDetDD::SiDetectorElement *ele) override {m_element = ele;};
  void setCosmicsRun(bool cosmicsRun) override {m_cosmicsRun = cosmicsRun;};
  void setComTimeFlag(bool useComTime) override {m_useComTime = useComTime;};
  void setComTime(float comTime) override {m_comTime = comTime;};
  void setRandomEngine(CLHEP::HepRandomEngine *rndmEngine) override {m_rndmEngine = rndmEngine;};
  void process(const TimedHitPtr<SiHit> & phit, const ISiSurfaceChargesInserter& inserter) const override {
    process(m_element, phit, inserter, m_rndmEngine);
  };
  void processFromTool(const SiHit*, const ISiSurfaceChargesInserter&,
                       const float, const unsigned short) const override {};

  double induced(double pitch, double thickness, int istrip, double x, double y) const;
  void transport(double pitch, double thickness,
                 double & x0, double & y0, double & xfin, double & yfin,
                 double & Q_m2, double & Q_m1, double& Q_00, double & Q_p1, double & Q_p2 ) const;


  BooleanProperty m_needsMcEventCollHelper{false};

  /** create a list of surface charges from a hit */
  void process(const InDetDD::SiDetectorElement* element, const TimedHitPtr<SiHit>& phit, const ISiSurfaceChargesInserter& inserter, CLHEP::HepRandomEngine * rndmEngine) const;
  void processSiHit(const InDetDD::SiDetectorElement* element, const SiHit& phit, const ISiSurfaceChargesInserter& inserter, float eventTime, unsigned short pileupType, CLHEP::HepRandomEngine * rndmEngine) const;

  // some diagnostics methods are needed here too
  float driftTime(float zhit, const InDetDD::SiDetectorElement* element) const; //!< calculate drift time perpandicular to the surface for a charge at distance zhit from mid gap
  float diffusionSigma(float zhit, const InDetDD::SiDetectorElement* element) const; //!< calculate diffusion sigma from a gaussian dist scattered charge
  float surfaceDriftTime(float ysurf) const; //!< Calculate of the surface drift time
  float maxDriftTime(const InDetDD::SiDetectorElement* element) const; //!< max drift charge equivalent to the detector thickness
  float maxDiffusionSigma(const InDetDD::SiDetectorElement* element) const; //!< max sigma diffusion

  // trap_pos and drift_time are updated based on spess.
  bool chargeIsTrapped(double spess, const InDetDD::SiDetectorElement* element, double& trap_pos, double& drift_time) const;

  unsigned loadPotenticalFile(const std::string& fileName,
                              double& deltax, double& deltay,
                              unsigned& nstepx, unsigned& nstepy,
                              std::vector<double>& potentialVec);
  double m_pot_deltax;
  double m_pot_deltay;
  unsigned m_pot_nstepx;
  unsigned m_pot_nstepy;
  std::vector<double> m_potentialVec;


  IntegerProperty m_numberOfCharges{1};
  FloatProperty m_smallStepLength{5};

  /** related to the surface drift */
  FloatProperty m_tSurfaceDrift{10};

  FloatProperty m_tfix{-999.};
  FloatProperty m_tsubtract{-999.};

  BooleanProperty m_doDistortions{false};
  BooleanProperty m_useSiCondDB{false};
  FloatProperty m_vdepl{70.};
  FloatProperty m_vbias{150.};
  BooleanProperty m_doTrapping{false};
  BooleanProperty m_doHistoTrap{false};
  BooleanProperty m_doRamo{false};
  BooleanProperty m_isOverlay{false};

  //ToolHandles
  ToolHandle<ISCT_ModuleDistortionsTool> m_distortionsTool;

  ServiceHandle<ISiliconConditionsSvc> m_siConditionsSvc;
  ServiceHandle<ISiPropertiesSvc> m_siPropertiesSvc;
  ServiceHandle<ISCT_RadDamageSummarySvc> m_radDamageSvc;

  ITHistSvc *m_thistSvc{nullptr};

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

#endif // STRIP_SURFACECHARGESGENERATOR_H

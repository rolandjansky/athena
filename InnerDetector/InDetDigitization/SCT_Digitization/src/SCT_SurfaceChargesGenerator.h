/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "SCT_ModuleDistortions/ISCT_ModuleDistortionsTool.h"

#include "GaudiKernel/ToolHandle.h"
#include <iostream>

// Charges and hits
class SiHit;
#include "Identifier/IdentifierHash.h"

// -- do charge trapping histos
class ITHistSvc;
class TH1F;
class TH2F;
class TProfile;

namespace InDetDD{
  class SiDetectorElement;
  class SCT_ModuleSideDesign;
}

namespace CLHEP {
  class HepRandomEngine;
}

class ISiliconConditionsSvc;
class ISiPropertiesSvc;
class ISCT_RadDamageSummarySvc;

template <class HIT> class TimedHitPtr;

class SCT_SurfaceChargesGenerator : public AthAlgTool, virtual public ISCT_SurfaceChargesGenerator {
 public:

  /**  constructor */
  SCT_SurfaceChargesGenerator( const std::string& type, const std::string& name, const IInterface* parent ) ;

  /** Destructor */
  virtual ~SCT_SurfaceChargesGenerator();

  /** AlgTool InterfaceID */
  //static const InterfaceID& interfaceID() ;

  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize */
  virtual StatusCode finalize();

private:

  void setComTime(float comTime)                                 {m_comTime = comTime;} 
  void setFixedTime(float fixedTime)                             {m_tfix = fixedTime;} 
  void setCosmicsRun(bool cosmicsRun)                            {m_cosmicsRun = cosmicsRun;}
  void setComTimeFlag(bool useComTime)                           {m_useComTime = useComTime;}      
  void setRandomEngine(CLHEP::HepRandomEngine *rndmEngine)       {m_rndmEngine = rndmEngine;}
  void setDetectorElement(const InDetDD::SiDetectorElement *ele) {m_element = ele; setVariables();} 

  /** create a list of surface charges from a hit */
  virtual void process(const TimedHitPtr<SiHit> & phit, const ISiSurfaceChargesInserter& inserter) const;
  virtual void processFromTool(const SiHit* phit, const ISiSurfaceChargesInserter& inserter, float p_eventTime, unsigned short p_eventId) const;
  void processSiHit(const SiHit& phit, const ISiSurfaceChargesInserter& inserter, const float eventTime, const unsigned short eventID) const;
  
  // some diagnostics methods are needed here too
  float DriftTime(float zhit) const;           //!< calculate drift time perpandicular to the surface for a charge at distance zhit from mid gap
  float DiffusionSigma(float zhit) const;      //!< calculate diffusion sigma from a gaussian dist scattered charge
  float SurfaceDriftTime(float ysurf) const;   //!< Calculate of the surface drift time 
  float MaxDriftTime() const;                   //!< max drift charge equivalent to the detector thickness
  float MaxDiffusionSigma() const;              //!< max sigma diffusion 
  bool  ChargeIsTrapped(double spess, double& trap_pos, double& drift_time) const;

  int m_numberOfCharges;           //!< number of charges
  float m_smallStepLength;         //!< max internal step along the larger G4 step

  /** related to the surface drift */
  float m_tSurfaceDrift;          //!< Surface drift time     
  float m_tHalfwayDrift;          //!< Surface drift time
  float m_distInterStrip ;        //!< Inter strip distance normalized to 1
  float m_distHalfInterStrip ;    //!< Half way distance inter strip

  char m_SurfaceDriftFlag;         //!< surface drift ON/OFF   

  float m_tfix;       //!< fixed time
  float m_tsubtract;  //!< subtract drift time from mid gap 

  float m_comTime ;       //!< use cosmics time for timing
  bool   m_useComTime ;   //!< Flag to decide the use of cosmics time for timing
  bool   m_cosmicsRun ;   //!< Flag to set Cosmics Run
  bool   m_doDistortions ;//!< Flag to set Distortions
  bool   m_useSiCondDB ;  //!< Flag to change from using DB values to below ones, default True
  float m_vdepl ;         //!< depletion voltage, default 70V
  float m_vbias ;         //!< bias voltage, default 150V
  bool   m_doTrapping ;   //!< Flag to set Charge Trapping
  bool   m_doHistoTrap;   //!< Flag that allows to fill the histograms
  bool   m_doRamo;        //!< Flag to use Ramo potential dor charge trapping 
  mutable bool   m_doCTrap;       //!< Flag that allows to get the quantities from ChargeTrappingSvc

  // -- Histograms
  ITHistSvc *m_thistSvc; 
  TProfile *m_h_efieldz;
  TH1F *m_h_efield;
  TH1F *m_h_spess;
  TH1F *m_h_depD;
  TH2F *m_h_drift_electrode;
  TH1F *m_h_ztrap;
  TH1F *m_h_drift_time;
  TH1F *m_h_t_electrode;
  TH1F *m_h_zhit;                               
  TH1F *m_h_ztrap_tot;
  TH1F *m_h_no_ztrap;
  TH1F *m_h_trap_drift_t;
  TH1F *m_h_notrap_drift_t;
  TProfile *m_h_mob_Char;
  TProfile *m_h_vel;
  TProfile *m_h_drift1;
  TProfile *m_h_gen;
  TProfile *m_h_gen1;
  TProfile *m_h_gen2;
  TProfile *m_h_velocity_trap;
  TProfile *m_h_mobility_trap;
  TH1F *m_h_trap_pos;

  mutable IdentifierHash m_hashId;

  //ToolHandles
  ToolHandle<ISCT_ModuleDistortionsTool> m_distortionsTool;
  //ServiceHandles
  ServiceHandle<ISiliconConditionsSvc> m_siConditionsSvc;
  ServiceHandle<ISiPropertiesSvc> m_siPropertiesSvc;
  ServiceHandle<ISCT_RadDamageSummarySvc> m_radDamageSvc;

  const InDetDD::SiDetectorElement * m_element;   
  CLHEP::HepRandomEngine *           m_rndmEngine;          //!< Random Engine
  std::string                        m_rndmEngineName;      //!< name of random engine, actual pointer in SiDigitization

  bool m_isOverlay; // flag for overlay

  void setVariables();
  const InDetDD::SCT_ModuleSideDesign* m_design;
  float m_depletionVoltage;
  float m_biasVoltage;
  double m_holeDriftMobility;
  double m_holeDiffusionConstant;
  double m_electronHolePairsPerEnergy;
  double m_thickness;
  double m_center;
  double m_tanLorentz;
  bool m_isBarrel;
};

#endif // SCT_SURFACECHARGESGENERATOR_H


 

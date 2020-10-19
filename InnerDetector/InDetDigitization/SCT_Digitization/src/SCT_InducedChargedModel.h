/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_DIGITIZATION_SCTINDUCEDCHARGEDMODEL_H
#define SCT_DIGITIZATION_SCTINDUCEDCHARGEDMODEL_H

//-----------------------------------------------
//   2020.8.12 Implementation in Athena by Manabu Togawa
//   2017.7.24 update for the case of negative VD (type-P)
//   2017.7.17  updated
//   2016.4.3  for ICM (induced charge model) by Taka Kondo (KEK)
//-----------------------------------------------

// Athena
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"
#include "Identifier/IdentifierHash.h"
#include "InDetConditionsSummaryService/ISiliconConditionsTool.h"
#include "PathResolver/PathResolver.h"
#include "SiPropertiesTool/ISiPropertiesTool.h"

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ToolHandle.h"

// Random number
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussZiggurat.h"  // for RandGaussZiggurat 
#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Units/SystemOfUnits.h"

// C++ Standard Library
#include <string>
#include <vector>

class SCT_InducedChargedModel {

 public:
  enum EFieldModel {UniformE=0, FlatDiodeModel=1, FEMsolutions=2};
  enum TransportStep {NTransportSteps=100};

  void init(const float vdepl, const float vbias, const IdentifierHash hashId,
            ToolHandle<ISiliconConditionsTool>& siConditionsTool);
  void setRandomEngine(CLHEP::HepRandomEngine* rndmEngine) { m_rndmEngine = rndmEngine; };
  void setMagneticField(const double B) { m_B = - B*1000.; }; // m_B in [Tesla], B in kTesla

  void transport(const bool isElectron,
                 const double x0, const double y0,
                 double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2,
                 const IdentifierHash hashId,
                 ToolHandle<ISiPropertiesTool>& siPropertiesTool) const;
  void holeTransport(const double x0, const double y0,
                     double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2,
                     const IdentifierHash hashId,
                     ToolHandle<ISiPropertiesTool>& siPropertiesTool) const;
  void electronTransport(const double x0, const double y0,
                         double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2,
                         const IdentifierHash hashId,
                         ToolHandle<ISiPropertiesTool>& siPropertiesTool) const;

 private:
  enum InducedStrips {StartStrip=-2, EndStrip=+2, Offset=-StartStrip, NStrips=EndStrip+Offset+1};
 
  void loadICMParameters();

  bool getVxVyD(const bool isElectron,
                const double x, const double y, double& vx, double& vy, double& D,
                const IdentifierHash hashId,
                ToolHandle<ISiPropertiesTool>& siPropertiesTool) const;
  double induced(const int istrip, const double x, const double y) const;
  void EField(const double x, const double y, double& Ex, double& Ey) const;


  /// Log a message using the Athena controlled logging system
  MsgStream& msg(MSG::Level lvl) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl(MSG::Level lvl) { return m_msg.get().level() <= lvl; }

  //-------- parameters for e, h transport --------------------------------
  static const double s_kB; // [m^2*kg/s^2/K]
  static const double s_e; // [Coulomb]

  // Private message stream member
  mutable Athena::MsgStreamMember m_msg ATLAS_THREAD_SAFE;
  
  CLHEP::HepRandomEngine* m_rndmEngine; //!< Random number generation engine 

  //------parameters given externally by jobOptions ------------------
  EFieldModel m_EFieldModel = FEMsolutions; // 0 (uniform E), 1 (flat diode model), 2 (FEM solusions)
  double m_VD = -30.; // full depletion voltage [Volt] negative for type-P
  double m_VB = 75.; // applied bias voltage [Volt]
  double m_T;
  double m_B;
  double m_transportTimeStep = 0.50; // one step side in time [nsec]
  double m_transportTimeMax = m_transportTimeStep*NTransportSteps; // maximun tracing time [nsec]

  //------parameters mostly fixed but can be changed externally  ------------
  double m_bulk_depth =  0.0285; // in [cm]
  double m_strip_pitch = 0.0080; // in [cm]
  double m_depletion_depth;
  double m_y_origin_min = 0.;

  //---------- arrays of FEM analysis -----------------------------------
  // Storages for Ramo potential and Electric field.
  // In strip pitch directions :
  //  Ramo potential : 80 divisions (81 points) with 5 um intervals from 40-440 um.
  //  Electric field : 16 divisions (17 points) with 2.5 um intervals from 0-40 um.
  // In sensor depth directions (for both potential and Electric field):
  //  114 divisions (115 points) with 2.5 nm intervals for 285 um.

  enum FEMNums {NRamoPoints=81, NEFieldPoints=17, NDepthPoints=115};

  double m_PotentialValue[NRamoPoints][NDepthPoints];
  double m_ExValue[NEFieldPoints][NDepthPoints];
  double m_EyValue[NEFieldPoints][NDepthPoints];

  static const std::vector<float> s_VFD0;
  static const std::vector<std::string> s_VFD0str;
};

#endif // SCT_DIGITIZATION_SCTINDUCEDCHARGEDMODEL_H

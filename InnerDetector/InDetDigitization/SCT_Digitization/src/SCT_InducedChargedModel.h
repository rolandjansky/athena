#ifndef SCT_DIGITIZATION_SCTINDUCEDCHARGEDMODEL_H
#define SCT_DIGITIZATION_SCTINDUCEDCHARGEDMODEL_H

//-----------------------------------------------
//   2020.8.12 Implementation in Athena by Manabu Togawa
//   2017.7.24 update for the case of negative VD (type-P)
//   2017.7.17  updated
//   2016.4.3  for ICM (induced charge model) by Taka Kondo (KEK)
//-----------------------------------------------

// C++ Standard Library
#include <iostream>

// ROOT
#include "TH2.h"
#include "TFile.h"

// Athena
#include "AthenaKernel/MsgStreamMember.h"
#include "SiPropertiesSvc/ISiPropertiesSvc.h"
#include "InDetConditionsSummaryService/ISiliconConditionsSvc.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "Identifier/IdentifierHash.h"
#include "PathResolver/PathResolver.h"

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Random number
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussZiggurat.h"  // for RandGaussZiggurat 
#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Units/SystemOfUnits.h"

namespace CLHEP {
  class HepRandomEngine;
}

class SCT_InducedChargedModel {

 public:

  void Init(float vdepl, float vnbais, IdentifierHash m_hashId, 
	    ServiceHandle<ISiliconConditionsSvc> m_siConditionsSvc);
  void setRandomEngine(CLHEP::HepRandomEngine *rndmEngine) {  m_rndmEngine = rndmEngine; };
  void setMagneticField(double B){ m_B = - B*1000; }; // m_B in [Tesla]

  void holeTransport(double x0, double y0, double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2, IdentifierHash m_hashId, ServiceHandle<ISiPropertiesSvc> m_siPropertiesSvc) ;
  void electronTransport(double x0, double y0, double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2, IdentifierHash m_hashId, ServiceHandle<ISiPropertiesSvc> m_siPropertiesSvc ) ;

 private:

  void loadICMParameters();

  bool electron(double x_e, double y_e, double &vx_e, double &vy_e, double &D_e,
		IdentifierHash m_hashId, 
		ServiceHandle<ISiPropertiesSvc> m_siPropertiesSvc) ;
  bool hole(double x_h, double y_h, double &vx_h, double &vy_h, double &D_h,
	    IdentifierHash m_hashId, 
	    ServiceHandle<ISiPropertiesSvc> m_siPropertiesSvc) ;
  double induced (int istrip, double x, double y) ;
  void EField( double x, double y, double &Ex, double &Ey ) ; 


  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) { return m_msg.get().level() <= lvl; }

  // Private message stream member
  mutable Athena::MsgStreamMember m_msg;
  
//------parameters given externally by jobOptions ------------------
   int    m_EfieldModel = 2 ; // 0(uniform E), 1(flat diode model), 2 (FEM solusions)
   double m_VD = -30. ;   // full depletion voltage [Volt] negative for type-P
   double m_VB = 75. ;   // applied bias voltage [Volt]
   double m_T;
   double m_B;
   double m_transportTimeStep = 0.50;    // one step side in time [nsec]
   double m_transportTimeMax = 50.0;     // maximun tracing time [nsec]
   double m_x0;
   double m_y0;

//------parameters mostly fixed but can be changed externally  ------------
   double m_bulk_depth =  0.0285 ; // in [cm]
   double m_strip_pitch = 0.0080 ;  // in [cm]
   double m_depletion_depth;
   double m_y_origin_min;

//-------- parameters for e, h transport --------------------------------
   double m_kB = Gaudi::Units::k_Boltzmann / Gaudi::Units::joule; // [m^2*kg/s^2/K]
   double m_e = Gaudi::Units::e_SI;  // [Coulomb]

//--------- parameters of induced charged model from root file ---------
   TH2F *h_Potential_FDM;
   TH2F *h_Potential_FEM;
   TH2F *h_Ex1;
   TH2F *h_Ex2;
   TH2F *h_Ey1;
   TH2F *h_Ey2;
   TH2F *h_ExHV;
   TH2F *h_EyHV;

//---------- arrays of FEM analysis -----------------------------------
// Storages for Ramo potential and Electric field.
// In strip pitch directions :                                                   
//  Ramo potential : 80 divisions (81 points) with 5 um intervals from 40-440 um. 
//  Electric field : 16 divisions (17 points) with 2.5 um intervals from 0-40 um.
// In sensor depth directions (for both potential and Electric field):
//  114 divisions (115 points) with 2.5 nm intervals for 285 um.
 
   double m_PotentialValue[81][115];
   double m_ExValue[17][115];
   double m_EyValue[17][115];

   CLHEP::HepRandomEngine   *m_rndmEngine; //!< Random number generation engine 

};

#endif // SCT_DIGITIZATION_SCTINDUCEDCHARGEDMODEL_H

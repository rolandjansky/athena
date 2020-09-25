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

#include "TH2.h"
#include "TFile.h"

#include "AthenaKernel/MsgStreamMember.h"

class SCT_InducedChargedModel{

 public:

  void holeTransport(double x0, double y0, double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2 ) ;
  void electronTransport(double x0, double y0, double* Q_m2, double* Q_m1, double* Q_00, double* Q_p1, double* Q_p2 ) ;

  void Init(float vdepl, float vnbais);

 private:

  bool electron( double x_e, double y_e, double &vx_e, double &vy_e, double &D_e);
  bool hole(double x_h, double y_h, double &vx_h, double &vy_h, double &D_h) ;
  double induced (int istrip, double x, double y) ;
  void EField( double x, double y, double &Ex, double &Ey ) ; 
  double mud_e(double E) ;
  double mud_h(double E) ;
  void init_mud_e(double T) ;
  void init_mud_h(double T) ;
  void loadICMParameters() ;

  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) { return m_msg.get().level() <= lvl; }

 private:

  // Private message stream member
  mutable Athena::MsgStreamMember m_msg;
  
//------parameters given externally by jobOptions ------------------
   int    m_EfieldModel = 2 ; // 0(uniform E), 1(flat diode model), 2 (FEM solusions)
   double m_VD = -30. ;   // full depletion voltage [Volt] negative for type-P
   double m_VB = 75. ;   // applied bias voltage [Volt]
   double m_T = 273.15;
   double m_B = -2.0 ;  // [Tesla]
   double m_transportTimeStep = 0.50;    // one step side in time [nsec]
   double m_transportTimeMax = 50.0;     // maximun tracing time [nsec]
   double m_x0;
   double m_y0;
   int    m_coutLevel = 1 ;

//------parameters mostly fixed but can be changed externally  ------------
   double m_bulk_depth =  0.0285 ; // in [cm]
   double m_strip_pitch = 0.0080 ;  // in [cm]
   double m_depletion_depth;
   double m_y_origin_min;

//-------- parameters for e, h transport --------------------------------
   double m_kB = 1.38E-23;  // [m^2*kg/s^2/K]
   double m_e = 1.602E-19;  // [Coulomb]
   double m_vs_e;
   double m_Ec_e ;
   double m_vs_h;
   double m_Ec_h ;
   double m_driftMobility; // [cm**2/V/s]
   double m_diffusion;     // [cm**2/s]
   double m_beta_e ;
   double m_beta_h ;
   double m_theta;
   double m_tanLA;

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
   double m_PotentialValue[81][115];
   double m_ExValue[17][115];
   double m_EyValue[17][115];

//-----------------char string for sprintf-------------------------------
   char m_cid[200];


};

#endif // SCT_DIGITIZATION_SCTINDUCEDCHARGEDMODEL_H

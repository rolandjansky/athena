/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_ElectronPidToolRun2_HTcalculation.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

/****************************************************************************************\

  This class is only instantiated once in the constuctor of the Pid Tool.
  It was created in order to encapsulate the code that is used for 
  the HT calulation that forms part of the Pid Tool.
  It is not true singleton as this was deemed unnecessary.

  Original creator: Simon Heisterkamp (simon.heisterkamp@cern.ch)
  Author: Troels Petersen (petersen@nbi.dk)

\****************************************************************************************/

class InDet::TRT_ElectronPidToolRun2::HTcalculator : public InDet::BaseTRTPIDCalculator {
 public:

  static const int my_CurrentVersion = 4;

  HTcalculator(AthAlgTool&);
  virtual ~HTcalculator();
  
  // set constants to hard coded defaults
  void setDefaultCalibrationConstants();

  //void PrintBlob();
  //bool FillBlob(const unsigned char*);

  float getProbHT( float pTrk, Trk::ParticleHypothesis hypothesis, int TrtPart, int StrawLayer, float ZRpos, float rTrkAnode, float Occupancy, bool UseOccupancy);
  float pHTvsP(int etaBin, float p, float mass) ;
  float Corr_el_SL(int part, float SL) ;
  float Corr_el_SP(int part, float SP) ;
  float Corr_el_RT(int part, float RT) ;
  float Corr_el_PU(int part, float PU) ;
  float Corr_el_OC(int part, float OC) ;
  float Corr_mu_SL(int part, float SL) ;
  float Corr_mu_SP(int part, float SP) ;
  float Corr_mu_RT(int part, float RT) ;
  float Corr_mu_PU(int part, float PU) ;
  float Corr_mu_OC(int part, float OC) ;

  //float Limit(float prob);

 private:
  // as long has reading from database does not work well yet, do this check:
  //bool HasDataBeenInitialized;
  //void checkIntialization();

  static const int N_PAR = 6;
  static const int N_DET = 3;
  
  float m_par_pHTvsP [N_DET][N_PAR];

  static const int N_SL_B  = 73;
  static const int N_SL_EA = 16;
  static const int N_SL_EB =  8;

	// SL corr:
  float m_CpHT_B_Zee_SL	[N_SL_B]	 ;
  float m_CpHT_EA_Zee_SL	[N_SL_EA]	 ;
  float m_CpHT_EB_Zee_SL	[N_SL_EB]	 ;

  	// ZR-position (ZR - Z in Barrel, R in Endcaps):

  static const int N_ZRpos_B  = 36;
  static const int N_ZRpos_EA = 50;
  static const int N_ZRpos_EB = 50;

  float m_CpHT_B_Zee_ZRpos	[N_ZRpos_B]  ;
  float m_CpHT_EA_Zee_ZRpos	[N_ZRpos_EA] ;
  float m_CpHT_EB_Zee_ZRpos	[N_ZRpos_EB] ;



  // Track-to-Wire distance (TWdist):
  static const int N_Rtrk_B  = 22;
  static const int N_Rtrk_EA = 22;
  static const int N_Rtrk_EB = 22;
  float m_CpHT_B_Zee_Rtrk	[N_Rtrk_B]  ;
  float m_CpHT_EA_Zee_Rtrk	[N_Rtrk_EA] ;
  float m_CpHT_EB_Zee_Rtrk	[N_Rtrk_EB] ;

  //   // Pile-UP (PU):
  static const int N_PU_B  = 40;
  static const int N_PU_EA = 40;
  static const int N_PU_EB = 40;
  float m_CpHT_B_Zee_PU	[N_PU_B]   ;
  float m_CpHT_EA_Zee_PU	[N_PU_EA]  ;
  float m_CpHT_EB_Zee_PU	[N_PU_EB]  ;

  // Occupancy (OC):
  static const int N_Occ_B  = 60;
  static const int N_Occ_EA = 60;
  static const int N_Occ_EB = 60;
  float m_CpHT_B_Zee_Occ	[N_Occ_B]  ;
  float m_CpHT_EA_Zee_Occ	[N_Occ_EA] ;
  float m_CpHT_EB_Zee_Occ	[N_Occ_EB] ;

	// SL corr:
  float m_CpHT_B_Zmm_SL	[N_SL_B]	 ;
  float m_CpHT_EA_Zmm_SL	[N_SL_EA]	 ;
  float m_CpHT_EB_Zmm_SL	[N_SL_EB]	 ;

  	// ZR-position (ZR - Z in Barrel, R in Endcaps):
  float m_CpHT_B_Zmm_ZRpos	[N_ZRpos_B]  ;
  float m_CpHT_EA_Zmm_ZRpos	[N_ZRpos_EA] ;
  float m_CpHT_EB_Zmm_ZRpos	[N_ZRpos_EB] ;

  // Track-to-Wire distance (TWdist):
  float m_CpHT_B_Zmm_Rtrk	[N_Rtrk_B] 	;
  float m_CpHT_EA_Zmm_Rtrk	[N_Rtrk_EA] ;
  float m_CpHT_EB_Zmm_Rtrk	[N_Rtrk_EB] ;

  //   // Pile-UP (PU):
  float m_CpHT_B_Zmm_PU	[N_PU_B]   ;
  float m_CpHT_EA_Zmm_PU	[N_PU_EA]  ;
  float m_CpHT_EB_Zmm_PU	[N_PU_EB]  ;

  // Occupancy (OC):
  float m_CpHT_B_Zmm_Occ	[N_Occ_B ] ;
  float m_CpHT_EA_Zmm_Occ	[N_Occ_EA] ;
  float m_CpHT_EB_Zmm_Occ	[N_Occ_EB] ;

  Trk::ParticleMasses        m_particlemasses;

  static const int SIZE_OF_HEADER = sizeof(float) * 4;

  static const int SIZE_OF_BLOB = sizeof(float) *( (N_PAR*N_DET) + 2*(N_SL_B+N_SL_EA+N_SL_EB) + 2*(N_ZRpos_B+N_ZRpos_EA+N_ZRpos_EB) + 2*( N_Rtrk_B+N_Rtrk_EA+N_Rtrk_EB ) + 2*(N_PU_B + N_PU_EA + N_PU_EB) + 2*(N_Occ_B + N_Occ_EA + N_Occ_EB) );

  bool m_datainplace;
};  

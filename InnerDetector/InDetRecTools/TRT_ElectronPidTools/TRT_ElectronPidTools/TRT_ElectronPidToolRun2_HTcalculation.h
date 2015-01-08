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


  double getProbHT( double pTrk, Trk::ParticleHypothesis hypothesis, int TrtPart, int StrawLayer, double ZRpos, double rTrkAnode, double Occupancy); 
  double pHTvsP(int etaBin, double p, double mass) ;
  double Corr_el_SL(int part, double SL) ;
  double Corr_el_SP(int part, double SP) ;
  double Corr_el_RT(int part, double RT) ;
  double Corr_el_PU(int part, double PU) ;
  double Corr_el_OC(int part, double OC) ;
  double Corr_mu_SL(int part, double SL) ;
  double Corr_mu_SP(int part, double SP) ;
  double Corr_mu_RT(int part, double RT) ;
  double Corr_mu_PU(int part, double PU) ;
  double Corr_mu_OC(int part, double OC) ;




  //float Limit(float prob);

 private:
  // as long has reading from database does not work well yet, do this check:
  //bool HasDataBeenInitialized;
  //void checkIntialization();

  Trk::ParticleMasses        m_particlemasses;

  // to linearize the indices of all calibration constants
  int resolveIndex( int BEC, int Layer, int Strawlayer);

  //The following describes the internal structure of the blob:
  //static const int SIZE_OF_HEADER = 12;

  static const int N_PAR = 5;

  static const int SIZE_PAR_SET= N_PAR * sizeof(float);

  static const int N_STRAWS_GLOBAL = 73 + 160;
  static const int N_CONSTS_PER_STRAW = 2;
  static const int N_STRAW_CONSTS = N_CONSTS_PER_STRAW * N_STRAWS_GLOBAL;
  static const int SIZE_STRAW_CONSTANTS = N_STRAW_CONSTS * sizeof(float);

  //internal offsets:
  //onset curve parameters
  static const int DEDX_C = 0;
  static const int DEDX_M = 1;
  static const int TR_SIZE = 2;
  static const int TR_MEAN = 3;
  static const int TR_WIDTH = 4;

  //strawwise correction
  static const int TR_CORR = 0;
  static const int DEDX_CORR = 1;

  static const int OFF_PAR_SET_BARREL  = SIZE_OF_HEADER;
  static const int OFF_PAR_SET_ENDCAP_A  = OFF_PAR_SET_BARREL + SIZE_PAR_SET;
  static const int OFF_PAR_SET_ENDCAP_B  = OFF_PAR_SET_ENDCAP_A + SIZE_PAR_SET;
  static const int OFF_STRAW_CONSTANTS = OFF_PAR_SET_ENDCAP_B + SIZE_PAR_SET;

  // unsigned char HTBlob[ BLOB_SIZE ];

  // the following are handles on the various sections of the HTBlob
  float* StrawConstants;

  float* ParsBarrel;
  float* ParsEndcapA;
  float* ParsEndcapB;


  //  float & UpperLimit;
  //  float & LowerLimit;
};  

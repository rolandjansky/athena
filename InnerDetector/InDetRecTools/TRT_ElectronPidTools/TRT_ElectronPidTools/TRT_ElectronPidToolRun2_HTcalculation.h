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
#include "AthenaPoolUtilities/CondAttrListVec.h"
#include <vector>

class InDet::TRT_ElectronPidToolRun2::StorePIDinfo{
  public:
   StorePIDinfo();
   StorePIDinfo(int nbins, float min, float max, std::vector<float> values);
   ~StorePIDinfo();
   void update (int nbins, float min, float max, std::vector<float> values );
   void push_back ( float value );
   StatusCode check ( int gas, int detpart); 
   float GetValue ( float input	); 
   float GetBinValue ( int bin ); 
   int   GetBin   ( float input	); 
  private:
   unsigned int m_nbins		;
   float m_min		;
   float m_max		;
   std::vector<float>  	m_values;
};


class InDet::TRT_ElectronPidToolRun2::HTcalculator : public InDet::BaseTRTPIDCalculator {
 public:

  static const int my_CurrentVersion = 4;

  HTcalculator(AthAlgTool&);
  virtual ~HTcalculator();
  
  // set constants to hard coded defaults
  void setDefaultCalibrationConstants();

  StatusCode ReadVectorDB( const DataHandle<CondAttrListVec> channel_values );

  //void PrintBlob();
  //bool FillBlob(const unsigned char*);

  float getProbHT( float pTrk, Trk::ParticleHypothesis hypothesis, int TrtPart, int GasType, int StrawLayer, float ZR, float rTrkAnode, float Occupancy, bool hasTrackPars);
//  float pHTvsP(int TrtPart, float p, float mass);
  float pHTvsPGOG(int TrtPart, int GasType, float p, float mass, float occ);

 private:
  // as long has reading from database does not work well yet, do this check:
  //bool HasDataBeenInitialized;
  //void checkIntialization();

  double inline sqr(double a) {return a*a;}

  static const int N_GAS = 3;
  static const int N_DET = 3;
  static const int N_PAR2 = 10;
  StorePIDinfo m_par_pHTvsPGOG_new [N_GAS][N_DET]; 	// New approach (useOccupancy = true)


// Store in a compact way all the corrections
  StorePIDinfo m_CpHT_B_Zee_SL_new  	[N_GAS]	[N_DET];
  StorePIDinfo m_CpHT_B_Zmm_SL_new  	[N_GAS]	[N_DET];

  StorePIDinfo m_CpHT_B_Zee_ZR_new  	[N_GAS]	[N_DET];
  StorePIDinfo m_CpHT_B_Zmm_ZR_new  	[N_GAS]	[N_DET];

  StorePIDinfo m_CpHT_B_Zee_TW_new  	[N_GAS]	[N_DET];
  StorePIDinfo m_CpHT_B_Zmm_TW_new  	[N_GAS]	[N_DET];

  StorePIDinfo m_CpHT_B_Zee_OR_new  	[N_GAS]	[N_DET];
  StorePIDinfo m_CpHT_B_Zmm_OR_new  	[N_GAS]	[N_DET];


  Trk::ParticleMasses        m_particlemasses;

  static const int SIZE_OF_HEADER = sizeof(float) * 4;
  static const int SIZE_OF_BLOB     = sizeof(float) *( (N_PAR2*N_DET));

  bool m_datainplace;
};  

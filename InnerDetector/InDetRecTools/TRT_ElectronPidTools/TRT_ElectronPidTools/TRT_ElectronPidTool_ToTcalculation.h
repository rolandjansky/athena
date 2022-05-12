/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_ElectronPidTool_ToTcalculation.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

/****************************************************************************************\

  This class is only instantiated once in the constuctor of the Pid Tool.
  It was created in order to encapsulate the code that is used for 
  the Time over Threshold calulation that forms part of the Pid Tool.
  It is not true singleton as this was deemed unnecessary.

  Original creator: Simon Heisterkamp (simon.heisterkamp@cern.ch)

\****************************************************************************************/
#include "AthenaBaseComps/AthMessaging.h"

class InDet::TRT_ElectronPidTool::ToTcalculator : public InDet::BaseTRTPIDCalculator,
                                                  public AthMessaging {
 public:

  static const int my_CurrentVersion = 4;

  ToTcalculator(AthAlgTool &);
  virtual ~ToTcalculator();
  
  // set constants to hard coded defaults
  void setDefaultCalibrationConstants();
  
  // get the ToT from the bitpattern and correct for local variations
  double GetToT(unsigned int bitpattern, double HitZ, double HitR, int BEC, int Layer, int Strawlayer);
  
  // get the distance used to normalize the ToT
  double GetD(double R_track);
  
  // get the likelyhood based on the sumToT/sumD we found for all hits on this track
  double GetElProb(double ToTbyD);
  //float  Limit(float prob);

  //void PrintBlob();
  // bool FillBlob(const unsigned char*);

 private:
  // as long has reading from database does not work well yet, do this check:
  // bool HasDataBeenInitialized;
  //void checkIntialization();

  // primary extraction of ToT from Bitpattern
  double ExtractToT(unsigned int bitpattern);

  // apply local corrections to the ToT
  double correctToT(double ToT, double HitZ, double HitR, int BEC, int Layer, int Strawlayer);

  // the following functions correct the ToT in various parts of the detector.
  // the right one is selected by correctToT
  double BarrelLongToTCorrection(double, double, const float*);
  double BarrelShortToTCorrection(double, double, const float*);
  double EndcapToTCorrection(double, double, const float*);

  // to linearize the indices of all calibration constants
  int resolveIndex(int BEC, int Layer, int Strawlayer);

  // /------------------------------------------------------------------------
  // | The following code describes the internal structure of the blob
  // | 
  // | Sizes:
  static const int NparsBarrel = 3;
  static const int NparsEndcap = 4;

  static const int NparametrizedSectionsBarrel = 73;
  static const int NparametrizedSectionsEndcap = 14;

  static const int SIZE_OF_HEADER = 16;
  static const int SIZE_GAUS_PARS = 9 * sizeof(float);
  static const int N_TOT_CONSTANTS = ( 2 * NparametrizedSectionsBarrel * NparsBarrel +
                                       2 * NparametrizedSectionsEndcap * NparsEndcap );
  static const int SIZE_TOT_CONSTANTS = N_TOT_CONSTANTS * sizeof(float);
  
  static const int SIZE_PAR_VAL = 4 * sizeof(float);

  static const int OFF_GAUS_PARS_ELE = SIZE_OF_HEADER;
  static const int OFF_GAUS_PARS_PIO = OFF_GAUS_PARS_ELE + SIZE_GAUS_PARS;
  static const int OFF_TOT_CONSTANTS = OFF_GAUS_PARS_PIO + SIZE_GAUS_PARS;
  static const int OFF_PAR_VALS      = OFF_TOT_CONSTANTS + SIZE_TOT_CONSTANTS;
  
  // parameter offsets
  static const int PAR0 = 0;
  static const int PAR1 = 1;
  static const int PAR2 = 2;
  static const int PAR3 = 3;
  //static const int PARVAL = 0;
  
  // |
  // | End of Blob structure
  // \------------------------------------------------------------------------

  //  unsigned char ToTBlob[ BLOB_SIZE ];

  // the following are handles on the various sections of the ToTBlob
  float* ToTCorrectionConstants;

  float* gausParametersElectron;
  float* gausParametersPion;

  //The following look like detector specific offsets,
  // but in reality the first one is a global offset and the others are not used
  float & parValBarrelLongNeg;
  float & parValBarrelLongPos;
  float & parValBarrelShortNeg;
  float & parValBarrelShortPos;

  //  float & UpperLimit;
  //  float & LowerLimit;
};

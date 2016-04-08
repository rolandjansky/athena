/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_ElectronPidTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRT_BASETRTPIDCALCULATOR_H
#define INDETTRT_BASETRTPIDCALCULATOR_H

// #include "GaudiKernel/AlgTool.h"           // Exchange OUT
// #include "GaudiKernel/MsgStream.h"         // Exchange OUT
#include "AthenaBaseComps/AthAlgTool.h"       // Exchange IN
//#include "TRT_ElectronPidTools/TRT_ElectronPidTool.h"

#include <vector>
#include <string>

namespace InDet
{
 class BaseTRTPIDCalculator {
 public:
  //Class to encode those aspects of the ToT and HT calculator that they have in common
  AthAlgTool & parent;
  const char * my_name;
  int BLOB_SIZE;
  unsigned char * Blob;

  float & UpperLimit;
  float & LowerLimit;

  bool HasBeenInitialized;

  static const int SIZE_OF_HEADER = 12;
  
  // | 
  // | The following are all the major offsets from the blob start:
  
  //versioning check constants
  int CurrentVersion;
  static const int _Version    = 0;
  static const int _Day   = 1;
  static const int _Month = 2;
  static const int _Year  = 3;
  
  static const int OFF_UpperLim = 4;
  static const int OFF_LowerLim = 8;

 BaseTRTPIDCalculator(AthAlgTool & p, int size, const char * name):parent(p),
    my_name(name),
    BLOB_SIZE(size),
    Blob(new unsigned char[size]),
    UpperLimit( * ((float*)( Blob + OFF_UpperLim) ) ),
    LowerLimit( * ((float*)( Blob + OFF_LowerLim) ) ),
    HasBeenInitialized(0)
      {
	CurrentVersion = -1;
      }
  
  ~BaseTRTPIDCalculator(){
    delete [] Blob;
  }

  // set constants to hard coded defaults
  virtual void setDefaultCalibrationConstants()=0;

 public:
  void checkInitialization();

  // Fill the data blob from a given pointer
  bool FillBlob(const unsigned char*);      
  
  //print the blob of data to the screen very verbosely.
  void PrintBlob();

  // Limit the allowed PID value to lie between a lower and an upper limt
   float Limit(float prob);      
 };
} 
#endif 

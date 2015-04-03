/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECALGS_TILEFILTERTESTER_H
#define TILERECALGS_TILEFILTERTESTER_H
//**************************************************************************************************
// Filename: TileFilterTester.h
// Author:   F. Merritt
// Created:  April 2004

// DESCRIPTION
// This class is to be called from the TileHitToRawChannel initialization section,
// after TileFilterManager has been instantiated.  A reference to TileFilterManager 
// is passed in the call, so it is possible to thoroughly test the program outside 
// of the event loop.
// *************************************************************************************************

class TileInfo;
class TileFilterManager;
class StoreGateSvc;

#include <vector>
/** Auxiliary class for TileRawChannelMakerManyAmps. 
 */
class TileFilterTester
{
 public:

  // Constructor
  TileFilterTester(TileFilterManager * tFilterManager, int Fmode, int Ftest, bool lDebug = false);
  //Destructor
  ~TileFilterTester();
  // Event Generator
  void GenEvents(int N);

 private:
  bool debug;
  int Fmode;
  int Ftest;
  int Cmode;
  int Ncross;
  int Npileup;
  int Namp;
  std::vector<int> Nconfig;
  std::vector<int> iAmpVec;
  std::vector<double> AmpVec;

  TileFilterManager * m_tileFilterManager;
};
#endif // TILERECALGS_TILEFILTERTESTER_H

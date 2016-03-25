/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILEFILTERTESTER_H
#define TILERECUTILS_TILEFILTERTESTER_H
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

//class TileInfo;
//class StoreGateSvc;
class TileFilterManager;

#include <vector>
/** Auxiliary class for TileRawChannelMakerManyAmps. 
 */
class TileFilterTester
{
 public:

  // Constructor
  TileFilterTester(TileFilterManager * filterManager, int filterMode, int filterTest, bool debug = false);
  //Destructor
  ~TileFilterTester();
  // Event Generator
  void genEvents(int nEvent);

 private:
  bool m_debug;
  int m_filterMode;
  int m_filterTest;
  int m_cMode;
  int m_nCross;
  int m_nPileup;
  int m_nAmp;
  std::vector<int> m_nConfig;
  std::vector<int> m_iAmpVec;
  std::vector<double> m_ampVec;

  TileFilterManager* m_tileFilterManager;
};
#endif // TILERECUTILS_TILEFILTERTESTER_H

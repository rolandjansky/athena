/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILETOPCALIBALG_H
#define TILETOPCALIBALG_H

// ********************************************************************
//
// NAME:     TileTopCalibAlg.h
// PACKAGE:  TileCalibAlgs
//
// AUTHOR:   Alexander Solodkov (Sanya.Solodkov@cern.ch)
// 
// May 2008
// ********************************************************************

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TileCalibAlgs/ITileCalibTool.h"

class TFile;

class TileTopCalibAlg : public AthAlgorithm
{
 public:

  TileTopCalibAlg(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~TileTopCalibAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  /**
   * Run number
   */
  int m_runNumber;

  /**
   * Run type
   */
  int m_runType;

  /**
   * Name of output file with ntuples
   */
  std::string m_fileName;

  /**
   * Vector of algtools to call
   */
  ToolHandleArray<ITileCalibTool> m_tileCalibToolList;
 
  /**
   * Pointer to the ROOT file
   */
  TFile *m_rootFile;

};

#endif // TILETOPCALIBALG_H

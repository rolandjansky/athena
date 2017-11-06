/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileFCSmStepToTileHitVec.h
// Author   : Sergey Karpov <Sergey.Karpov@cern.ch>
// Created  : Nov. 2013
//
// DESCRIPTION
//   TileFCSmStepToTileHitVec copies all TileHits from FCS_StepInfoCollection container
//   to TileHitVector_FCS with/without applying of new U-shape
//
// Properties (JobOption Parameters):
//   ISF_FCS_Parametrization::FCS_StepInfoCollection    string   Name of FCS_StepInfo container to read
//   TileHitVec_FCS                                     string   Name of TileHitVector to write
//   TileInfoName                                       string   Name of object in TDS with all parameters
//
// History:
//   01 Nov 2013 - Created from TileCellIDCToCell.h
//   28 Nov 2013 - Work with U-shape was added (Sasha Solodkov)
//
// BUGS:
//
//****************************************************************************

#ifndef TILEFASTCALOSIM_TILEFCSMSTEPTOTILEHITVEC_H
#define TILEFASTCALOSIM_TILEFCSMSTEPTOTILEHITVEC_H

#include "AthenaBaseComps/AthAlgorithm.h"

class TileID;
class TileHit;
class TileInfo;
class TileDetDescrManager;
class TileGeoG4SDCalc;
class ITileCalculator;
class FCS_StepInfoCollection;
class IGeoModelSvc;

// C++ STL includes
#include <string>
#include <vector>

class TileFCSmStepToTileHitVec: public AthAlgorithm {
public:
  // Constructor
  TileFCSmStepToTileHitVec(const std::string& name, ISvcLocator* pSvcLocator);

  // Destructor
  virtual ~TileFCSmStepToTileHitVec();

  // Gaudi Hooks
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  int getUshapeFromGM() const;

  ServiceHandle<IGeoModelSvc> m_geoModSvc;
  std::string m_FCS_StepInfo;
  std::string m_hitVec;
  std::string m_infoName;

  const TileID*               m_tileID;
  const TileInfo*             m_tileInfo;
  const TileDetDescrManager*  m_tileMgr;
  ServiceHandle<ITileCalculator> m_calc;

  float   m_deltaT;
  std::vector<TileHit*> m_allHits;

  int m_Ushape;
};

#endif // TILERECALGS_TILEFCSMSTEPTOTILEHITVEC_H

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***********************************************************************
//  Filename : CalibHitIDCheck.h
//
//  Author   : gia.khoriauli@cern.ch
//  Created  : April, 2005
//
//  Helper tool for CalibHits Identifiers checking
//**********************************************************************

#ifndef CalibHitIDCheck_H
#define CalibHitIDCheck_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "Identifier/Identifier.h"

#include <vector>
#include <string>

class AtlasDetectorID;
class CaloCalibrationHitContainer;

typedef std::vector<Identifier> ID ;

class CalibHitIDCheck : public AthAlgorithm {

 public:

  CalibHitIDCheck(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CalibHitIDCheck();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  void check(int);
  void show_all();
  void check_all_cnts();
  void merge(Identifier);
  void reset(){m_id_vec.clear();}

  bool m_Merge;

 private:

  std::string  m_larDMHitContainer;
  std::string  m_larActiveHitContainer;
  std::string  m_larInactiveHitContainer;
  std::string  m_tileActiveHitContainer;
  std::string  m_tileInactiveHitContainer;
  std::string  m_tiledmHitContainer;

  const CaloCalibrationHitContainer* m_LArDMHitCnt; 
  const CaloCalibrationHitContainer* m_ActiveHitCnt; 
  const CaloCalibrationHitContainer* m_InactiveHitCnt; 
  const CaloCalibrationHitContainer* m_TileActiveHitCnt;
  const CaloCalibrationHitContainer* m_TileInactiveHitCnt;
  const CaloCalibrationHitContainer* m_TileDMHitCnt;

  bool m_Check;
  bool m_ShowAll;
  bool m_CheckAll; 

  const AtlasDetectorID* m_id_helper;

  ID m_id_vec;
};

#endif

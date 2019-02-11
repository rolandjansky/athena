/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBALG_TILETOFTOOL_H
#define TILECALIBALG_TILETOFTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "CaloEvent/CaloCellContainer.h"
#include "StoreGate/ReadHandleKey.h"

#include "TileCalibAlgs/ITileCalibTool.h"


class StoreGateSvc;
class TileID;

class TileTOFTool : public AthAlgTool, virtual public ITileCalibTool 
{
 public:

  TileTOFTool(const std::string& type, const std::string& name, const IInterface* pParent);
  virtual ~TileTOFTool(); 

  virtual StatusCode initialize();
  virtual StatusCode initNtuple(int runNumber, int runType, TFile * rootfile);
  virtual StatusCode execute();
  virtual StatusCode finalizeCalculations();
  virtual StatusCode writeNtuple(int runNumber, int runType, TFile * rootfile);
  virtual StatusCode finalize();

 protected:
  const TileID* m_tileID;

 private:

  SG::ReadHandleKey<CaloCellContainer> m_caloCellContainerKey{this,
      "CaloCellContainer", "CaloCellContainer", "Calo cell container"};

  float timeCor(int mod_ref1, int mod_ref2, int part_ref1, int part_ref2);

  float (*m_timeCor)[64];

  float m_LBA_LBC[4];
  float m_LBA_EBA[4];
  float m_LBC_EBC[4];
  int m_Nlbc[4];
  int m_Neba[4]; 
  int m_Nebc[4];

  float m_LA_EA;
  float m_LA_LC;
  float m_LA_EC;

  float (*m_tcor)[32][32];
  int (*m_nPair)[32][32];
};

#endif

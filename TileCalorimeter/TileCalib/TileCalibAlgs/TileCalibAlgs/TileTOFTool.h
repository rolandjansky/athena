/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBALG_TILETOFTOOL_H
#define TILECALIBALG_TILETOFTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "TileCalibAlgs/ITileCalibTool.h"

class StoreGateSvc;
class TileID;

class TileTOFTool : public AlgTool, virtual public ITileCalibTool 
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

  StoreGateSvc* m_gatStore;

  StoreGateSvc* m_detStore;

  const TileID* m_tileID;

 private:

  int n,i,j;
  double x[20];
  double y[20];
  double z[20];
  double t[20];
  int part[20];
  int mod[20];

  float TimeCor[4][64];

  float LBA_LBC[4]; int Nlbc[4];
  float LBA_EBA[4]; int Neba[4]; 
  float LBC_EBC[4]; int Nebc[4];

  float LA_EA;
  float LA_LC;
  float LA_EC;

  float timeCor(int mod_ref1, int mod_ref2, int part_ref1, int part_ref2);
  int section;
  int side;
  float tcor[4][32][32];
  int Npair[4][32][32];
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_TILEHITSTESTTOOL
#define G4AT_TILEHITSTESTTOOL

#include "SimTestToolBase.h"

class TileID;
class TileTBID;
class TileDetDescrManager;


class TileHitsTestTool : public SimTestToolBase {


public:

  TileHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent);

  StatusCode initialize();

  StatusCode processEvent();

private:

  const TileID * m_tileID;
  const TileTBID * m_tileTBID;
  const TileDetDescrManager * m_tileMgr;

  TH1 *m_tile_eta, *m_tile_phi, *m_tile_energy, *m_tile_log_energy, *m_tile_time;
  TH2 *m_tile_rz, *m_tile_etaphi, *m_tile_energyphi, *m_tile_energyeta;

  TH1 *m_mbts_side, *m_mbts_eta, *m_mbts_phi;
  TH2 *m_mbts_sidetaphi;

  TH1 *m_etot;
};

#endif

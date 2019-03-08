/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileMuId2DBAlg.cxx
//  Author   : Aranzazu Ruiz
//  Created  : May 2009
//
//  DESCRIPTION:
//
//     Individual lower energy thresholds for each cell are computed as three
//     times the noise RMS value of the cell when the two PMTs are in High Gain
//     using CaloNoiseToolDB tool.
//
//     For DSP usage, thresholds in MeV are multiply by 2 and round off in order
//     to compare them with reconstructed energies at the DSP which have a 0.5 MeV
//     precision for High Gain. ASCII file with TileMuId thresholds is created to
//     be loaded in the DSPs.
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

#include "TileCalibAlgs/TileMuId2DBAlg.h"

#include <fstream>

using namespace std;


TileMuId2DBAlg::TileMuId2DBAlg(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name,pSvcLocator), 
  m_calo_id(0),
  m_noise(0.0),
  m_module(0)
{
}


  
TileMuId2DBAlg::~TileMuId2DBAlg()
{
} 



StatusCode TileMuId2DBAlg::initialize()
{
  ATH_CHECK( detStore()->retrieve( m_caloIdMgr ) );
  m_calo_id = m_caloIdMgr->getCaloCell_ID();

  ATH_CHECK( detStore()->retrieve(m_calodetdescrmgr) );
  ATH_CHECK( m_noiseTool.retrieve() );
  return StatusCode::SUCCESS;
}



StatusCode TileMuId2DBAlg::execute()
{
  // Open ASCII file
  string TileMuIdFile = "TileMuId_thresholds.dat";
  fstream *fl = new fstream(TileMuIdFile.c_str(), fstream::app| fstream::out);
  if (fl->is_open())
    ATH_MSG_INFO ( " TileMuId file open" );
  else
    ATH_MSG_INFO ( " TileMuId file didn't open succesfully" );

  float noise[4][64][24]; memset(noise,0,sizeof(noise));
  int DSP[4][64][40];     memset(DSP,0,sizeof(DSP));
  int Thr[4][64][40];     memset(Thr,0,sizeof(Thr));

  // A1, BC1, D0, A2, BC2, A3, BC3, D1, A4, BC4, A5, BC5, D2, A6, BC6, A7, BC7, D3, A8, BC8, A9, BC9, A10
  int Thr_barrel[23] = {
    1350, 2550, 1590, 960, 1980, 870, 1770, 1110, 750, 1890, 840, 1860, 1050, 840, 1800, 930, 1890, 1050, 840, 1860, 960, 1110, 900
  };

  // B11, D5, A12, B12, A13, B13, D6, A14, B14, A15, B15, A16
  int Thr_extbarrel[12] = {
    1050, 2100, 210, 1380, 1140, 1440, 2370, 1230, 1500, 1380, 1680, 1680
  };

  IdentifierHash caloCellMin=0;
  IdentifierHash caloCellMax=0;
  m_calo_id->calo_cell_hash_range(3, caloCellMin, caloCellMax);

  ATH_MSG_INFO ( "caloCellMin: " << caloCellMin );
  ATH_MSG_INFO ( "caloCellMax: " << caloCellMax );
  ATH_MSG_INFO ( "Start loop over TileCal cells " << caloCellMax-caloCellMin );

  for (unsigned int i=caloCellMin;i<caloCellMax;i++) {

    IdentifierHash idHash = i;
    Identifier id = m_calo_id->cell_id(idHash);
    const CaloDetDescrElement* calodde = m_calodetdescrmgr->get_element(id);
    int subCalo;
    IdentifierHash idSubHash = m_calo_id->subcalo_cell_hash (idHash, subCalo);

    //m_eta = calodde->eta();
    //m_phi = calodde->phi();
    m_module = m_calo_id->module(id);

    CaloGain::CaloGain gain=CaloGain::TILEHIGHHIGH;

    m_noise = m_noiseTool->totalNoiseRMS(calodde,gain);

    if( m_calo_id->is_tile_barrel(id)    && m_calo_id->is_tile_negative(id) ) noise[1][m_module][idSubHash-22*m_module] = 3*m_noise;

    if( m_calo_id->is_tile_barrel(id)    && m_calo_id->is_tile_positive(id) ) noise[0][m_module][idSubHash-(23*m_module+22*64)] = 3*m_noise;

    if( m_calo_id->is_tile_extbarrel(id) && m_calo_id->is_tile_negative(id) ) noise[3][m_module][idSubHash-(12*m_module+23*64+22*64)] = 3*m_noise;

    if( m_calo_id->is_tile_extbarrel(id) && m_calo_id->is_tile_positive(id) ) noise[2][m_module][idSubHash-(12*m_module+12*64+23*64+22*64)] = 3*m_noise;

  }

  // Barrel C-side

  int i=0;
  for(int j=0;j<64;j++) {
    for(int k=0;k<20;k++) {
      DSP[i][j][k] = (int)round(2*noise[i][j][k]);
    }
  }

  // Barrel A-side (include D0 cell)

  i=1;
  for(int j=0;j<64;j++) {
    for(int k=0;k<2;k++) {
      DSP[i][j][k] = (int)round(2*noise[i][j][k]);
    }
    int k=2;
    DSP[i][j][k] = (int)round(2*noise[0][j][k]);
    for(int k=2;k<19;k++) {
      DSP[i][j][k+1] = (int)round(2*noise[i][j][k]);
    }
  }

  // Ext Barrels
  for(int i=2;i<4;i++) {
    for(int j=0;j<64;j++) {
      for(int k=0;k<11;k++) {
	DSP[i][j][k] = (int)round(2*noise[i][j][k]);
      }
    }
  }

  for(int i=0;i<2;i++) {

    for(int j=0;j<64;j++) {

      int k=0;

      // D cells

      Thr[i][j][k]=DSP[i][j][2];k++;    Thr[i][j][k]=DSP[i][j][7];k++;    Thr[i][j][k]=DSP[i][j][12];k++;    Thr[i][j][k]=DSP[i][j][17];k++;
      Thr[i][j][k]=2*Thr_barrel[2];k++; Thr[i][j][k]=2*Thr_barrel[7];k++; Thr[i][j][k]=2*Thr_barrel[12];k++; Thr[i][j][k]=2*Thr_barrel[17];k++;

      // BC cells

      Thr[i][j][k]=DSP[i][j][1];k++;     Thr[i][j][k]=DSP[i][j][4];k++;     Thr[i][j][k]=DSP[i][j][6];k++;     Thr[i][j][k]=DSP[i][j][9];k++;
      Thr[i][j][k]=DSP[i][j][11];k++;    Thr[i][j][k]=DSP[i][j][14];k++;    Thr[i][j][k]=DSP[i][j][16];k++;    Thr[i][j][k]=DSP[i][j][19];k++;
      Thr[i][j][k]=2*Thr_barrel[1];k++;  Thr[i][j][k]=2*Thr_barrel[4];k++;  Thr[i][j][k]=2*Thr_barrel[6];k++;  Thr[i][j][k]=2*Thr_barrel[9];k++;
      Thr[i][j][k]=2*Thr_barrel[11];k++; Thr[i][j][k]=2*Thr_barrel[14];k++; Thr[i][j][k]=2*Thr_barrel[16];k++; Thr[i][j][k]=2*Thr_barrel[19];k++;

      // A cells

      Thr[i][j][k]=DSP[i][j][0];k++;     Thr[i][j][k]=DSP[i][j][3];k++;     Thr[i][j][k]=DSP[i][j][5];k++;     Thr[i][j][k]=DSP[i][j][8];k++;
      Thr[i][j][k]=DSP[i][j][10];k++;    Thr[i][j][k]=DSP[i][j][13];k++;    Thr[i][j][k]=DSP[i][j][15];k++;    Thr[i][j][k]=DSP[i][j][18];k++;
      Thr[i][j][k]=2*Thr_barrel[0];k++;  Thr[i][j][k]=2*Thr_barrel[3];k++;  Thr[i][j][k]=2*Thr_barrel[5];k++;  Thr[i][j][k]=2*Thr_barrel[8];k++;
      Thr[i][j][k]=2*Thr_barrel[10];k++; Thr[i][j][k]=2*Thr_barrel[13];k++; Thr[i][j][k]=2*Thr_barrel[15];k++; Thr[i][j][k]=2*Thr_barrel[18];k++;

    }

  }

  for(int i=2;i<4;i++) {

    for(int j=0;j<64;j++) {

      int k=0;

      // D cells

      Thr[i][j][k]=DSP[i][j][1];k++;       Thr[i][j][k]=DSP[i][j][6];k++;
      Thr[i][j][k]=2*Thr_extbarrel[1];k++; Thr[i][j][k]=2*Thr_extbarrel[6];k++;

      // BC cells

      Thr[i][j][k]=DSP[i][j][0];k++;       Thr[i][j][k]=DSP[i][j][3];k++;       Thr[i][j][k]=DSP[i][j][5];k++;       Thr[i][j][k]=DSP[i][j][8];k++;       Thr[i][j][k]=DSP[i][j][10];k++;
      Thr[i][j][k]=2*Thr_extbarrel[0];k++; Thr[i][j][k]=2*Thr_extbarrel[3];k++; Thr[i][j][k]=2*Thr_extbarrel[5];k++; Thr[i][j][k]=2*Thr_extbarrel[8];k++; Thr[i][j][k]=2*Thr_extbarrel[10];k++;

      // A cells

      Thr[i][j][k]=DSP[i][j][2];k++;       Thr[i][j][k]=DSP[i][j][4];k++;       Thr[i][j][k]=DSP[i][j][7];k++;       Thr[i][j][k]=DSP[i][j][9];k++;
      Thr[i][j][k]=2*Thr_extbarrel[2];k++; Thr[i][j][k]=2*Thr_extbarrel[4];k++; Thr[i][j][k]=2*Thr_extbarrel[7];k++; Thr[i][j][k]=2*Thr_extbarrel[9];k++;

    }

  }

  for(int i=0;i<4;i++) {

    for(int j=0;j<64;j++) {

      msg(MSG::INFO)
	  << "TileMuId  0x" << std::hex << (i+1)*0x100+j << std::dec << "  0";
      *fl << "TileMuId  0x" << std::hex << (i+1)*0x100+j << std::dec << "  0";

      for(int k=0;k<40;k++) {

	msg(MSG::INFO)
	    << "  " << Thr[i][j][k];
	*fl << "  " << Thr[i][j][k];

      }

      msg(MSG::INFO) << endmsg;
      *fl << endl;

    }

  }

  // Close ASCII file
  fl->close();

  return StatusCode::SUCCESS;
}



StatusCode TileMuId2DBAlg::finalize()
{
  ATH_MSG_INFO ( "in finalize()" );
  return StatusCode::SUCCESS;
}

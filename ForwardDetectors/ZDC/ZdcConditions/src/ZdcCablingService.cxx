/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcConditions/ZdcCablingService.h"
#include <string>
#include <iostream>
#include <algorithm>

// initialization of singleton instance pointer

ZdcCablingService * ZdcCablingService::s_zdcCablingService = 0 ;

// Singleton methods

//------------------------------------------------------------
ZdcCablingService * ZdcCablingService::getInstance()
{

    if(s_zdcCablingService == 0){
        std::cout << "==> New ZdcCablingService created" << std::endl;
        s_zdcCablingService = new ZdcCablingService() ;
    }
    return s_zdcCablingService ;
}

//-------------------------------------------------------------
void ZdcCablingService::deleteInstance()
{
    if(s_zdcCablingService != 0){
        delete s_zdcCablingService ;
        s_zdcCablingService = 0 ;
    }
}

// default constructor

//-------------------------------------------------------------
ZdcCablingService::ZdcCablingService()
  : m_dbFilled(0), m_zdcID(0), m_zdcHWID(0)
  , m_cablingType(ZdcCablingService::Sim)
{

  fillConnectionTables();

  fillDB();

}

void ZdcCablingService::fillDB()
{

  if (m_dbFilled==true) return;

  std::cout << "starting fillDB()" << std::endl;

  m_dbFilled = true;

  int side_db[16][16] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
  };

  /* //original version Nov 2009

  int module_db[8][16] =
    {
      {1,1,1,1,0,0,0,0,3,3,3,3,2,2,2,2},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,0,0,0,0,3,3,3,3,2,2,2,2},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };
  */

  int module_db[16][16] =
    {
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {3,3,3,3,0,0,0,0,1,1,1,1,2,2,2,2},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {3,3,3,3,0,0,0,0,1,1,1,1,2,2,2,2},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

  int type_db[16][16] =
    {
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

  int channel_db[16][16] =
    {
      {8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15},
      {24,24,25,25,26,26,27,27,28,28,29,29,30,30,31,31},
      {40,40,41,41,42,42,43,43,44,44,45,45,46,46,47,47},
      {56,56,57,57,58,58,59,59,60,60,61,61,62,62,63,63},
      {0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7},
      {16,16,17,17,18,18,19,19,20,20,21,21,22,22,23,23},
      {32,32,33,33,34,34,35,35,36,36,37,37,38,38,39,39},
      {48,48,49,49,50,50,51,51,52,52,53,53,54,54,55,55},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {1,1,5,5,0,0,4,4,3,3,7,7,2,2,6,6},
      {12,12,8,8,13,13,9,9,14,14,10,10,15,15,11,11},
      {20,20,16,16,21,21,17,17,22,22,18,18,23,23,19,19},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {1,1,5,5,0,0,4,4,3,3,7,7,2,2,6,6},
      {12,12,8,8,13,13,9,9,14,14,10,10,15,15,11,11},
      {20,20,16,16,21,21,17,17,22,22,18,18,23,23,19,19},
    };

  /*
    // the old one
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {4,4,0,0,5,5,1,1,6,6,2,2,7,7,3,3},
      {9,9,13,13,8,8,12,12,11,11,15,15,10,10,14,14},
      {17,17,21,21,16,16,20,20,19,19,23,23,18,18,22,22}
  */

  int gain_db[16][16] =
    {
      {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},   // EM Pix
      {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},   // EM Pix
      {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},   // EM Pix
      {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},   // EM Pix
      {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},   // EM Pix
      {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},   // EM Pix
      {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},   // EM Pix
      {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},   // EM Pix
      {1,1,0,0,0,0,1,1,1,1,0,0,1,1,0,0},   // HAD2,EM,HAD0,HAD1
      {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},   // HAD0 Pix
      {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},   // HAD0 Pix
      {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},   // HAD0 Pix
      {1,1,0,0,0,0,1,1,1,1,0,0,1,1,0,0},   // HAD2,EM,HAD0,HAD1
      {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},   // HAD0 Pix
      {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},   // HAD0 Pix
      {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1}    // HAD0 Pix
    };

  int delay_db[16][16] =
    {
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
      {1,0,1,0,0,1,0,1,1,0,1,0,1,0,1,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    //{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
      {1,0,1,0,0,1,0,1,1,0,1,0,1,0,1,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    };

  int hv_db[16][16] =
    {
      {224,224,224,224,212,212,212,212,213,213,213,213,214,214,214,214},
      {221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221},
      {222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222},
      {223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223},
      {224,224,224,224,212,212,212,212,213,213,213,213,214,214,214,214},
      {221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221},
      {222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222},
      {223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223},
      {224,224,224,224,212,212,212,212,213,213,213,213,214,214,214,214},
      {221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221},
      {222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222},
      {223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223},
      {224,224,224,224,212,212,212,212,213,213,213,213,214,214,214,214},
      {221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221},
      {222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222},
      {223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223}
    };

  int  ppm_db[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};

  // this is a mess.  need to derive crate_index and m_crate_index from the crate_db, but not right this second
  //int crate_db[8] = {12,12,12,12,14,14,14,14};
  int crate_index[16] ={0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3};
  m_ncrate = 4;
  m_crate_index[0] = 10;
  m_crate_index[1] = 11;
  m_crate_index[2] = 12;
  m_crate_index[3] = 14;

  // lookup table side/module/type/channel/gain/delay

  for (int i = 0 ; i < 16 ; i++)
    {
      for (int j = 0 ; j < 16 ; j++)
	{
	  int ic = crate_index[i]; // crate index
	  int icc = i*16+j-64*ic; // channel index
	  //std::cout << "ic=" << ic << "  icc=" << icc << std::endl;
	  m_side_db[ic][icc] = side_db[i][j];
	  m_module_db[ic][icc] = module_db[i][j];
	  m_type_db[ic][icc] = type_db[i][j];
	  m_channel_db[ic][icc] = channel_db[i][j];
	  m_gain_db[ic][icc] = gain_db[i][j];
	  m_delay_db[ic][icc] = delay_db[i][j];
	  m_hv_db[ic][icc] = hv_db[i][j];
	  m_ppm_db[ic] = ppm_db[i];
	  //m_crate_lookup[m_side_db[i][j]][m_module_db[i][j]][m_type_db[i][j]] = ic;
	  //m_channel_lookup[m_side_db[i][j]][m_module_db[i][j]][m_type_db[i][j]][j][m_gain_db[i][j]][m_delay_db[i][j]] = icc;
	}
    }

  std::cout << "finished fillDB()" << std::endl;
}

// destructor

//-------------------------------------------------------------
ZdcCablingService::~ZdcCablingService()
{
}

void
ZdcCablingService::setCablingType(ZdcCablingService::ZdcCablingType type)
{
// cabling type can be:
//  -1 for testbeam
//   0 for standard simulation
//   1 new configuration
  m_cablingType = type;
}

void
ZdcCablingService::setConnected(int ppm, int channel)
{
    m_connected[ppm][channel] = true;
}

//
// Convert ZdcID to Trigger Tower ID
//

/*
Identifier ZdcCablingService::h2s_side_id ( const HWIdentifier & hwid ) const
  throw (ZdcID_Exception)
{
  int ppm      = m_zdcHWID->ppm     (hwid);
  int channel  = m_zdcHWID->channel (hwid);
  int side     = hwid2side     (ppm,channel);
  return m_zdcID->side_id(side);
}

Identifier ZdcCablingService::h2s_type_id ( const HWIdentifier & hwid ) const
  throw (ZdcID_Exception)
{
  int ppm      = m_zdcHWID->ppm     (hwid);
  int channel  = m_zdcHWID->channel (hwid);
  int type    = hwid2type     (ppm,channel);
  return m_zdcID->type_id(side);
}

Identifier ZdcCablingService::h2s_module_id ( const HWIdentifier & hwid ) const
  throw (ZdcID_Exception)
{
  int ppm      = m_zdcHWID->ppm     (hwid);
  int channel  = m_zdcHWID->channel (hwid);
  int side     = hwid2side     (ppm,channel);
  return m_zdcID->module_id(side);
}

Identifier ZdcCablingService::h2s_gain_id ( const HWIdentifier & hwid ) const
  throw (ZdcID_Exception)
{
  int ppm      = m_zdcHWID->ppm     (hwid);
  int channel  = m_zdcHWID->channel (hwid);
  int side     = hwid2side     (ppm,channel);
  return m_zdcID->side_id(side);
}

Identifier ZdcCablingService::h2s_delay_id ( const HWIdentifier & hwid ) const
  throw (ZdcID_Exception)
{
  int ppm      = m_zdcHWID->ppm     (hwid);
  int channel  = m_zdcHWID->channel (hwid);
  int side     = hwid2side     (ppm,channel);
  return m_zdcID->side_id(side);
}
*/

/*
Identifier ZdcCablingService::h2s_channel_id ( const HWIdentifier & hwid ) const
  throw (ZdcID_Exception)
{
  int ppm      = m_zdcHWID->ppm     (hwid);
  int channel  = m_zdcHWID->channel (hwid);
  int side     = hwid2side     (ppm,channel);
  int module     = hwid2module    (ppm,channel);
  int type     = hwid2type     (ppm,channel);
  int channel     = hwid2channel     (ppm,channel);
  return m_zdcID->channel_id(side,module,type,channel);
}
*/

Identifier ZdcCablingService::h2s_channel_id ( int crate, int channel ) const
{

  int side     = hwid2side     (crate,channel);
  int module     = hwid2module    (crate,channel);
  int type     = hwid2type     (crate,channel);
  int chan     = hwid2channel     (crate,channel);

  //std::cout << "ZCS: side=" << side << " module=" << module << " type=" << type << " chan=" << chan << std::endl;
  return m_zdcID->channel_id(side,module,type,chan);
}

/*
HWIdentifier ZdcCablingService::s2h_channel_hwid (const Identifier & id) const
{
  int side = m_zdcID->side(id);
  int type = m_zdcID->type(id);
  int module = m_zdcID->module(id);
  int channel = m_zdcID->channel(id);
  int gain = 0;
  int delay = 0;

  int ppm = swid2ppm(side,module,type);
  int hwchannel = swid2channel(side,module,type,channel,gain,delay);

  return m_zdcHWID->channel_id(ppm,hwchannel);
}
*/

/*
HWIdentifier ZdcCablingService::s2h_ppm_hwid (const Identifier & id) const
{
  int side = m_zdcID->side(id);
  int module = m_zdcID->module(id);
  int type = m_zdcID->type(id);
  int channel = m_zdcID->channel(id);
  int ppm = swid2ppm(side,module,type);
  return m_zdcHWID->ppm_id(ppm);
}
*/

int
ZdcCablingService::hwid2side ( int crate, int channel ) const
{
  int ic = crate_index_lookup(crate);
  if (ic<0) return -1;

  int side = m_side_db[ic][channel];
  return side;
}

int ZdcCablingService::hwid2module ( int crate, int channel  ) const
{
  int ic = crate_index_lookup(crate);
  if (ic<0) return -1;

  int module = m_module_db[ic][channel];
  return module;
}

int ZdcCablingService::hwid2type ( int crate, int channel  ) const
{
  int ic = crate_index_lookup(crate);
  if (ic<0) return -1;

  int type = m_type_db[ic][channel];
  return type;
}


int ZdcCablingService::hwid2channel ( int crate, int channel  ) const
{
  int ic = crate_index_lookup(crate);
  if (ic<0) return -1;

  int chan = m_channel_db[ic][channel];
  return chan;
}


int ZdcCablingService::hwid2gain ( int crate, int channel  ) const
{
  int ic = crate_index_lookup(crate);
  if (ic<0) return -1;

  int gain = m_gain_db[ic][channel];
  //std::cout << "crate =" << crate << " channel="<<channel<< " ic="<<ic<<" channel="<<channel<<" gain=" << gain<<std::endl;
  return gain;
}

int ZdcCablingService::hwid2delay ( int crate, int channel  ) const
{

  int ic = crate_index_lookup(crate);
  if (ic<0) return -1;

  int delay = m_delay_db[ic][channel];
  //std::cout << "crate =" << crate << " channel="<<channel<< " ic="<<ic<<" channel="<<channel<<" delay=" << delay<<std::endl;
  return delay;
}

int ZdcCablingService::hwid2hv ( int crate, int channel  ) const
{
  int ic = crate_index_lookup(crate);
  if (ic<0) return -1;

  int hv = m_hv_db[ic][channel];
  return hv;
}



void ZdcCablingService::fillConnectionTables()
{
  // mapping from module (offline id) to drawer (onile id)
  // NC - not connected
  //const int NC = 65;
  //const int PB = 0;
  // PB - possibly broken E3, but for the moment assume that this channel is still alive

}


/*
int ZdcCablingService::swid2ppm( int side, int module, int type) const
{
  int ppm = m_ppm_lookup[side][module][type];
  return ppm;
}

int ZdcCablingService::swid2channel(int side, int module, int type, int channel, int gain, int delay) const
{
  int hwchannel = m_channel_lookup[side][module][type][channel][gain][delay];
  return hwchannel;
}
*/


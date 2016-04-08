/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "LArTBEvent/LArTBBeamChambers.h"

LArTBBeamChambers::LArTBBeamChambers(int flag, int TDC_up, int TDC_right, 
				     int TDC_left, int TDC_down, float x, 
				     float y, float res_x, float res_y, 
				     float clusterSize_x, float clusterSize_y)
{
  m_flag = flag; 
  m_TDC_up = TDC_up; 
  m_TDC_right = TDC_right;
  m_TDC_left = TDC_left;
  m_TDC_down = TDC_down;
  m_x = x; 
  m_y = y;
  m_residu_x = res_x; 
  m_residu_y = res_y;
  m_clusterSize_x = clusterSize_x; 
  m_clusterSize_y = clusterSize_y;
  m_cwireno.clear();
  m_nwires.clear();
  m_wire_xy.clear();
}

LArTBBeamChambers::LArTBBeamChambers( int flag, float x, float y, float res_x, 
				      float res_y, float clusterSize_x, 
				      float clusterSize_y)
{
  m_flag = flag; 
  m_TDC_up = 0; 
  m_TDC_right = 0;
  m_TDC_left = 0;
  m_TDC_down = 0;
  m_x = x; 
  m_y = y;
  m_residu_x = res_x; 
  m_residu_y = res_y;
  m_clusterSize_x = clusterSize_x; 
  m_clusterSize_y = clusterSize_y;
  m_cwireno.clear();
  m_nwires.clear();
  m_wire_xy.clear();
}

LArTBBeamChambers::~LArTBBeamChambers(){}

LArTBBeamChambers::operator std::string() const
{
  std::string error_message = "Unable to get LArTBBeamChambers string" ;
  return error_message ;
}





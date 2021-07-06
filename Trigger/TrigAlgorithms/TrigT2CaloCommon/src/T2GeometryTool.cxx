/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// Infrastructure
#include "TrigT2CaloCommon/T2GeometryTool.h"

T2GeometryTool::T2GeometryTool(const std::string &type, const std::string & name,
                    const IInterface* parent) : AthAlgTool(type,name,parent)
{

  declareProperty( "nStripEtaEM0nar" , m_nStripetaEMnar[0] = 4); // 0.1
  declareProperty( "nStripPhiEM0nar" , m_nStripphiEMnar[0] = 1); // 0.098
  declareProperty( "nStripEtaEM1nar" , m_nStripetaEMnar[1] = 32); // 0.1
  declareProperty( "nStripPhiEM1nar" , m_nStripphiEMnar[1] = 1); // 0.098
  declareProperty( "nStripEtaEM2nar" , m_nStripetaEMnar[2] = 4); // 0.1
  declareProperty( "nStripPhiEM2nar" , m_nStripphiEMnar[2] = 4); // 0.098
  declareProperty( "nStripEtaEM3nar" , m_nStripetaEMnar[3] = 2); // 0.1
  declareProperty( "nStripPhiEM3nar" , m_nStripphiEMnar[3] = 4); // 0.098

  declareProperty( "nStripEtaEM0wid" , m_nStripetaEMwid[0] = 8); // 0.2
  declareProperty( "nStripPhiEM0wid" , m_nStripphiEMwid[0] = 2); // 0.196
  declareProperty( "nStripEtaEM1wid" , m_nStripetaEMwid[1] = 64); // 0.2
  declareProperty( "nStripPhiEM1wid" , m_nStripphiEMwid[1] = 2); // 0.196
  declareProperty( "nStripEtaEM2wid" , m_nStripetaEMwid[2] = 8); // 0.2
  declareProperty( "nStripPhiEM2wid" , m_nStripphiEMwid[2] = 8); // 0.196
  declareProperty( "nStripEtaEM3wid" , m_nStripetaEMwid[3] = 4); // 0.2
  declareProperty( "nStripPhiEM3wid" , m_nStripphiEMwid[3] = 8); // 0.196

  declareProperty( "nStripEtaEM0nor" , m_nStripetaEMnor[0] = 16); // 0.4
  declareProperty( "nStripPhiEM0nor" , m_nStripphiEMnor[0] = 4); // 0.39
  declareProperty( "nStripEtaEM1nor" , m_nStripetaEMnor[1] = 128); // 0.4
  declareProperty( "nStripPhiEM1nor" , m_nStripphiEMnor[1] = 4); // 0.39
  declareProperty( "nStripEtaEM2nor" , m_nStripetaEMnor[2] = 16); // 0.4
  declareProperty( "nStripPhiEM2nor" , m_nStripphiEMnor[2] = 16); // 0.39
  declareProperty( "nStripEtaEM3nor" , m_nStripetaEMnor[3] = 8); // 0.4
  declareProperty( "nStripPhiEM3nor" , m_nStripphiEMnor[3] = 16); // 0.39


  declareProperty( "nStripEtaHAD0nar" , m_nStripetaHADnar[0] = 1); // 0.1
  declareProperty( "nStripPhiHAD0nar" , m_nStripphiHADnar[0] = 1); // 0.098
  declareProperty( "nStripEtaHAD1nar" , m_nStripetaHADnar[1] = 1); // 0.1
  declareProperty( "nStripPhiHAD1nar" , m_nStripphiHADnar[1] = 1); // 0.098
  declareProperty( "nStripEtaHAD2nar" , m_nStripetaHADnar[2] = 1); // 0.2
  declareProperty( "nStripPhiHAD2nar" , m_nStripphiHADnar[2] = 1); // 0.098

  declareProperty( "nStripEtaHAD0wid" , m_nStripetaHADwid[0] = 2); // 0.2
  declareProperty( "nStripPhiHAD0wid" , m_nStripphiHADwid[0] = 2); // 0.196
  declareProperty( "nStripEtaHAD1wid" , m_nStripetaHADwid[1] = 2); // 0.2
  declareProperty( "nStripPhiHAD1wid" , m_nStripphiHADwid[1] = 2); // 0.196
  declareProperty( "nStripEtaHAD2wid" , m_nStripetaHADwid[2] = 1); // 0.2
  declareProperty( "nStripPhiHAD2wid" , m_nStripphiHADwid[2] = 2); // 0.196

  declareProperty( "nStripEtaHAD0nor" , m_nStripetaHADnor[0] = 4); // 0.4
  declareProperty( "nStripPhiHAD0nor" , m_nStripphiHADnor[0] = 4); // 0.39
  declareProperty( "nStripEtaHAD1nor" , m_nStripetaHADnor[1] = 4); // 0.4
  declareProperty( "nStripPhiHAD1nor" , m_nStripphiHADnor[1] = 4); // 0.39
  declareProperty( "nStripEtaHAD2nor" , m_nStripetaHADnor[2] = 2); // 0.4
  declareProperty( "nStripPhiHAD2nor" , m_nStripphiHADnor[2] = 4); // 0.39



}


StatusCode T2GeometryTool::initialize()
{
  ATH_MSG_DEBUG("in initialize() by T2GeometryTool");
 return StatusCode::SUCCESS; 
}



const int T2GeometryTool::m_netareg[7] = {3,7,4,3,2,2,3};

const double T2GeometryTool::m_etareg[7][8] = {
       {0.0,1.4 ,1.5  ,1.8,0.0,0.0,0.0,0.0},
       {0.0,1.4 ,1.5  ,1.8,2.0,2.4,2.5,3.2},
       {0.0,1.4 ,1.425,2.5,3.2,0.0,0.0,0.0},
       {0.0,1.35,1.5  ,2.5,0.0,0.0,0.0,0.0},
       {0.0,2.5 ,3.2  ,0.0,0.0,0.0,0.0,0.0},
       {0.0,2.5 ,3.2  ,0.0,0.0,0.0,0.0,0.0},
       {0.0,1.5 ,2.5  ,3.2,0.0,0.0,0.0,0.0}};
const double T2GeometryTool::m_etaGran[7][7] = {
      {0.025   ,0.024,0.025   ,0.0      ,0.0    ,0.0  ,0.0},
      {0.003125,0.025,0.003125,0.0041667,0.00625,0.025,0.1},
      {0.025   ,0.05 ,0.025   ,0.1      ,0.0    ,0.0  ,0.0},
      {0.05    ,0.001,0.05    ,0.0      ,0.0    ,0.0  ,0.0},
      {0.1     ,0.2  ,0.0     ,0.0      ,0.0    ,0.0  ,0.0},
      {0.1     ,0.2  ,0.0     ,0.0      ,0.0    ,0.0  ,0.0},
      {0.2     ,0.1  ,0.2     ,0.0      ,0.0    ,0.0  ,0.0}};


const int T2GeometryTool::m_nphireg[7] = {1,3,2,1,2,2,2};

const double T2GeometryTool::m_phireg[7][4] = {
      {0.0,1.8,0.0  ,0.0},
      {0.0,1.4,1.475,3.2},
      {0.0,2.5,3.2  ,0.0},
      {0.0,2.5,0.0  ,0.0},
      {0.0,2.5,3.2  ,0.0},
      {0.0,2.5,3.2  ,0.0},
      {0.0,2.5,3.2  ,0.0}};

const double T2GeometryTool::m_phiGran[7][3] = {
      {0.0981748,0.0,0.0},
      {0.0981748,0.0245437,0.0981748},
      {0.0245437,0.0981748,0.0},
      {0.0245437,0.0      ,0.0},
      {0.0981748,0.19635  ,0.0},
      {0.0981748,0.19635  ,0.0},
      {0.0981748,0.19635  ,0.0}};


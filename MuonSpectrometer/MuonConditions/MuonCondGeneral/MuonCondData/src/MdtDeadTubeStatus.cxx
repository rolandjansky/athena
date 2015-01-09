/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Monica Verducci monica.verducci@cern.ch author
// class to read the conditiond data object stored with COOL 
// need to understand where to put it (Detector Descriptions??)

#include "MuonCondData/MdtDeadTubeStatus.h"
#include "MuonCondData/MdtDeadTubeStatusContainer.h"
MdtDeadTubeStatus::MdtDeadTubeStatus()
{ 
  // initialize class members
  m_name        = "";
  m_mlayer      = 0;
  m_layer       = 0;
  m_tubeid      = 0;
  m_status      = -1;
  m_mlayerArray.clear();
}


MdtDeadTubeStatus::~MdtDeadTubeStatus()
{ }

void 
MdtDeadTubeStatus::set(std::string name, int mlayer, int layer, int tubeid)
{
  m_name        = name;
  m_mlayer      = mlayer;
  m_layer       = layer;
  m_tubeid      = tubeid;
  
}

void 
MdtDeadTubeStatus::setstatus(std::string name, int mlayer, int layer, int tubeid, int status)
{
  m_name        = name;
  m_mlayer      = mlayer;
  m_layer       = layer;
  m_tubeid      = tubeid;
  m_status      = status;
  
}

void 

MdtDeadTubeStatus::setChamberStatus(std::string name,std::vector< unsigned int > mlayerArray)
{
  m_name        = name;
  m_mlayerArray = mlayerArray;
  
}

std::string MdtDeadTubeStatus::name() const
{
    return (m_name);
}

unsigned int MdtDeadTubeStatus::mlayer() const 
{
  return m_mlayer;
}

unsigned int MdtDeadTubeStatus::layer() const 
{
  return m_layer;
}
unsigned int MdtDeadTubeStatus::tubeid() const 
{
  return m_tubeid;
}
unsigned int MdtDeadTubeStatus::status() const 
{
  return m_status;
}

std::vector< unsigned int >  MdtDeadTubeStatus::mlayerArray() 
{
  return m_mlayerArray;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HECHitsTestTool.h"

#include <TH2D.h>
#include <TH1.h>
#include <TProfile.h>

HECHitsTestTool::HECHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent) 
  : LArHitsTestTool(type, name, parent)
{
}

StatusCode HECHitsTestTool::initialize()
{ 
  std::string origpath=m_path;

  m_path += "LAr/HEC/";

  // variables specific to sub detector (root histo names have to be unique!)
  _TH1D(m_eta,"HEC_eta",25,-3.5,3.5);
  _TH1D_WEIGHTED(m_time,"HEC_time",100,0,10);
  _TH1D(m_edep,"HEC_edep",100,0.,4.);
  _TH1D(m_etot,"HEC_etot",100,0.,20.);
  _TH1D_WEIGHTED(m_edep_eta,"HEC_edep_eta",25,-3.5,3.5);
  //_TH1D_WEIGHTED(m_edep_z,"HEC_edep_z",100,-5200.,5200.); 
  _TH1D_WEIGHTED(m_edep_r,"HEC_edep_r",100,300.,2200.);
  _TPROFILE(m_etot_eta,"HEC_etot_eta",25,-3.,3.); 

  m_path = origpath;

  return LArHitsTestTool::initialize();
}


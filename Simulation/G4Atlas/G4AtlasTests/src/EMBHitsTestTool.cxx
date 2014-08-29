/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EMBHitsTestTool.h"

#include <TH2D.h>
#include <TH1.h>
#include <TProfile.h>

EMBHitsTestTool::EMBHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent) 
  : LArHitsTestTool(type, name, parent)
{
}

StatusCode EMBHitsTestTool::initialize()
{ 
  std::string origpath=m_path;

  m_path += "LAr/EMB/";

  // variables specific to sub detector (root histo names have to be unique!)
  _TH1D(m_eta,"EMB_eta",25,-2.,2.);
  _TH1D_WEIGHTED(m_time,"EMB_time",100,0,2.);
  _TH1D(m_etot,"EMB_etot",100,0.,50.);
  _TH1D_WEIGHTED(m_edep_eta,"EMB_edep_eta",25,-2.,2.);
  _TH1D_WEIGHTED(m_edep_z,"EMB_edep_z",100,-4000.,4000.); 
  _TH1D_WEIGHTED(m_edep_r,"EMB_edep_r",100,1300.,2000.);
  _TPROFILE(m_etot_eta,"EMB_etot_eta",25,-2.,2.); 

  m_path = origpath;

  return LArHitsTestTool::initialize();
}


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FCALHitsTestTool.h"

#include <TH2D.h>
#include <TH1.h>
#include <TProfile.h>

FCALHitsTestTool::FCALHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent) 
  : LArHitsTestTool(type, name, parent)
{
}

StatusCode FCALHitsTestTool::initialize()
{ 
  std::string origpath=m_path;

  m_path += "LAr/FCAL/";

  // variables specific to sub detector (root histo names have to be unique!)
  //_TH1D(m_eta,"FCAL_eta",25,-5.,5.);
  //_TH1D_WEIGHTED(m_time,"FCAL_time",100,0,25);
  _TH1D(m_etot,"FCAL_etot",100,0.,10.);
  //_TH1D_WEIGHTED(m_edep_eta,"FCAL_edep_eta",25,-5.,5.);
  //_TH1D_WEIGHTED(m_edep_z,"FCAL_edep_z",100,-5200.,5200.); 
  _TH1D_WEIGHTED(m_edep_r,"FCAL_edep_r",100,0.,500.);
  _TPROFILE(m_etot_eta,"FCAL_etot_eta",25,-3.,3.); 

  m_path = origpath;

  return LArHitsTestTool::initialize();
}


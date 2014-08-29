/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EMECHitsTestTool.h"

#include <TH2D.h>
#include <TH1.h>
#include <TProfile.h>

EMECHitsTestTool::EMECHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent) 
  : LArHitsTestTool(type, name, parent)
{
}

StatusCode EMECHitsTestTool::initialize()
{ 
  std::string origpath=m_path;

  m_path += "LAr/EMEC/";

  // variables specific to sub detector (root histo names have to be unique!)
  _TH1D(m_eta,"EMEC_eta",25,-3.5,3.5);
  _TH1D_WEIGHTED(m_time,"EMEC_time",100,0,2.);
  _TH1D(m_etot,"EMEC_etot",100,0.,150.);
  _TH1D_WEIGHTED(m_edep_eta,"EMEC_edep_eta",25,-3.5,3.5);
  _TH1D_WEIGHTED(m_edep_z,"EMEC_edep_z",100,-4500.,4500.); 
  _TH1D_WEIGHTED(m_edep_r,"EMEC_edep_r",100,300.,2200.);
  _TPROFILE(m_etot_eta,"EMEC_etot_eta",25,-3.,3.); 

  m_path = origpath;

  return LArHitsTestTool::initialize();
}


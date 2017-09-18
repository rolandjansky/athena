/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSParametrization.h"

//=============================================
//======= TFCSParametrization =========
//=============================================

TFCSParametrization::TFCSParametrization(const char* name, const char* title):TFCSParametrizationBase(name,title),m_Ekin_nominal(0),m_Ekin_min(0),m_Ekin_max(0),m_eta_nominal(100),m_eta_min(100),m_eta_max(100)
{
}

void TFCSParametrization::set_pdgid(int id)
{
  m_pdgid.clear();
  m_pdgid.insert(id);
}

void TFCSParametrization::add_pdgid(int id)
{
  m_pdgid.insert(id);
}

void TFCSParametrization::clear_pdgid()
{
  m_pdgid.clear();
}

void TFCSParametrization::set_Ekin_nominal(double nominal)
{
  m_Ekin_nominal=nominal;
}

void TFCSParametrization::set_Ekin_min(double min)
{
  m_Ekin_min=min;
}

void TFCSParametrization::set_Ekin_max(double max)
{
  m_Ekin_max=max;
}


void TFCSParametrization::set_eta_nominal(double nominal)
{
  m_eta_nominal=nominal;
}

void TFCSParametrization::set_eta_min(double min)
{
  m_eta_min=min;
}

void TFCSParametrization::set_eta_max(double max)
{
  m_eta_max=max;
}

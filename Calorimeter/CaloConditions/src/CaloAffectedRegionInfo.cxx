/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloConditions/CaloAffectedRegionInfo.h"

#include <iostream>

CaloAffectedRegionInfo::CaloAffectedRegionInfo(void)
{
  m_eta_min=m_eta_max=m_phi_min=m_phi_max=m_rate=0.;
  m_layer_min=m_layer_max=0;
  m_problem = CaloAffectedRegionInfo::Unknown;
}

CaloAffectedRegionInfo::CaloAffectedRegionInfo(float parameter_eta_min,float parameter_eta_max,float parameter_phi_min,float parameter_phi_max,int parameter_layer_min,int parameter_layer_max,type_problem current_problem, float parameter_rate)
{
  m_eta_min=parameter_eta_min;
  m_eta_max=parameter_eta_max;
  m_phi_min=parameter_phi_min;
  m_phi_max=parameter_phi_max;
  m_layer_min=parameter_layer_min;
  m_layer_max=parameter_layer_max;
  m_problem=current_problem;
  m_rate = parameter_rate;
}

float CaloAffectedRegionInfo::get_eta_min() const {
  return m_eta_min;
}

float CaloAffectedRegionInfo::get_eta_max() const {
  return m_eta_max;
}

float CaloAffectedRegionInfo::get_phi_min() const {
  return m_phi_min;
}

float CaloAffectedRegionInfo::get_phi_max() const {
  return m_phi_max;
}

int CaloAffectedRegionInfo::get_problem() const {
  return m_problem;
}

int CaloAffectedRegionInfo::get_layer_min() const {
  return m_layer_min;
}

int CaloAffectedRegionInfo::get_layer_max() const {
  return m_layer_max;
}

float CaloAffectedRegionInfo::get_rate() const{
  return m_rate;
}

void CaloAffectedRegionInfo::FillCaloAffectedRegionInfo(float parameter_eta_min,float parameter_eta_max,float parameter_phi_min,float parameter_phi_max,int parameter_layer_min,int parameter_layer_max,type_problem current_problem, float parameter_rate)
{
  m_eta_min=parameter_eta_min;
  m_eta_max=parameter_eta_max;
  m_phi_min=parameter_phi_min;
  m_phi_max=parameter_phi_max;
  m_layer_min=parameter_layer_min;
  m_layer_max=parameter_layer_max;
  m_problem=current_problem;
  m_rate = parameter_rate;
}

void CaloAffectedRegionInfo::PrintInfo(void) const
{
  std::cout << "eta range : [" << m_eta_min << " ; " << m_eta_max << " ], phi range : [" << m_phi_min << " ; " << m_phi_max << " ], layer range : [" << m_layer_min << " ; " << m_layer_max << " ]" <<", problem=" << m_problem << ", rate " << m_rate << std::endl;
}


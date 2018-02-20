/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrization.h"
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"

//=============================================
//======= TFCSLateralShapeParametrization =========
//=============================================

TFCSLateralShapeParametrization::TFCSLateralShapeParametrization(const char* name, const char* title):TFCSParametrization(name,title),m_Ekin_bin(-1),m_calosample(-1)
{
}

void TFCSLateralShapeParametrization::set_Ekin_bin(int bin)
{
  m_Ekin_bin=bin;
}

void TFCSLateralShapeParametrization::set_calosample(int cs)
{
  m_calosample=cs;
}

void TFCSLateralShapeParametrization::Print(Option_t *option) const
{
  TString opt(option);
  if(!opt.IsWhitespace()) opt="";
  TFCSParametrization::Print(option);
  ATH_MSG_INFO(opt <<"  Ekin_bin="<<Ekin_bin());
  ATH_MSG_INFO(opt <<"  calosample="<<calosample());
}

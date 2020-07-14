/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaEvent/egDetail.h"
#include "egammaEvent/egammaParamDefs.h"
#include "AthenaKernel/ClassName.h"

egDetail::egDetail() 
{ }

egDetail::~egDetail()
{ }

double egDetail::parameter(egammaParameters::ParamDef) const 
{
  return egammaParameters::EgParamUndefined;
}

int egDetail::intParameter(egammaParameters::ParamDef) const 
{
  return (int) egammaParameters::EgParamUndefined;
}

bool egDetail::boolParameter(egammaParameters::ParamDef) const 
{
  return false;
}

bool egDetail::hasParameter(egammaParameters::ParamDef) const
{
  return false;
}

bool egDetail::hasIntParameter(egammaParameters::ParamDef) const
{
  return false;
}

bool egDetail::hasBoolParameter(egammaParameters::ParamDef) const
{
  return false;
}


const std::string& egDetail::className() const
{
  static const std::string name;
  return name;
}

int egDetail::linkIndex() const
{
  return (int) egammaParameters::EgParamUndefined;
}

void egDetail::set_linkIndex(int)
{}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MagFieldUtils/IdentityManipulator.h"

MagField::IdentityManipulator::IdentityManipulator(const std::string& name,
                                         const std::string& type,
					 const IInterface* parent)
          : AthAlgTool(name, type, parent)
{

  declareInterface<IMagFieldManipulator>(this);
}

MagField::IdentityManipulator::~IdentityManipulator()
{
}

StatusCode MagField::IdentityManipulator::initialize()
{
  ATH_MSG_INFO( "in initialize()" );
  return StatusCode::SUCCESS;
}

StatusCode MagField::IdentityManipulator::finalize()
{
  ATH_MSG_INFO( "in finalize()" );
  return StatusCode::SUCCESS;
}

void MagField::IdentityManipulator::modifyPosition(const double *xyz_old, double *xyz_new)
{
  xyz_new[0] = xyz_old[0];
  xyz_new[1] = xyz_old[1];
  xyz_new[2] = xyz_old[2];
}

void MagField::IdentityManipulator::modifyField(double */*bxyz*/, double */*deriv*/)
{
  // do nothing (i.e. leave bxyz untouched)
}

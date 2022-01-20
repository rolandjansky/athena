/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonNSWAsBuilt/ElementModelRigid.h"
#include <stdexcept>

using namespace NswAsBuilt;

//===============================================================================
// Transform a set of vectors expressed in local frame, stored in a matrix
void ElementModelRigid::transform(const ParameterVector& parvec, VectorSetRef local) const 
{
  if (!parvec.transformCacheValid) {
    throw std::runtime_error("Should call Element::cacheTransforms() first");
  }
  local = (parvec.transformCache * local).eval(); // Needs eval to avoid aliasing (?)
}

//===============================================================================
// Cache the rigid component of this deformation model
void ElementModelRigid::cacheTransform(ParameterVector& parvec) const 
{
  const auto& pars = parvec.parameters;
  parvec.transformCache
    = Eigen::Translation3d(pars[X], pars[Y], pars[Z])
    * Eigen::AngleAxisd(pars[THZ], Eigen::Vector3d::UnitZ())
    * Eigen::AngleAxisd(pars[THY], Eigen::Vector3d::UnitY())
    * Eigen::AngleAxisd(pars[THX], Eigen::Vector3d::UnitX());
  parvec.transformCacheValid = true;
}

//===============================================================================
// Helper methods to convert parameter index to string representation
ElementModelRigid::ipar_t ElementModelRigid::getParameterIndex(const std::string& parname) const 
{
  if (parname == "x") return parameter_t::X;
  if (parname == "y") return parameter_t::Y;
  if (parname == "z") return parameter_t::Z;
  if (parname == "thx") return parameter_t::THX;
  if (parname == "thy") return parameter_t::THY;
  if (parname == "thz") return parameter_t::THZ;
  throw std::runtime_error("Invalid parameter name "+parname);
}


//===============================================================================
std::string ElementModelRigid::getParameterName(ipar_t ipar) const 
{
  switch (ipar) {
    case X: return "x";
    case Y: return "y";
    case Z: return "z";
    case THX: return "thx";
    case THY: return "thy";
    case THZ: return "thz";
    default: throw std::runtime_error("Invalid parameter");
  }
}


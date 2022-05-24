/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonNSWAsBuilt/ElementModelSTGC.h"
#include <stdexcept>

using namespace NswAsBuilt;

ElementModelSTGC::ElementModelSTGC(double lenX, double lenY, Amg::Vector3D defo0)
  : m_lenX(lenX),
  m_lenY(lenY),
  m_defo0(defo0)
{
}

/**
 * Transform a set of vectors expressed in local frame, stored in a matrix
 */
void ElementModelSTGC::transform(const ParameterVector& parvec, VectorSetRef local) const {

  if (!parvec.transformCacheValid) {
    throw std::runtime_error("Should call Element::cacheTransforms() first");
  }

  // Apply the deformation component
  /*
   * old-style (reference) implementation: not optimized
  for (int i=0; i< local.cols(); ++i) {
    applyDeformation(parvec, local.col(i));
  }
  */
  // Eigen-style implementation: optimized
  applyDeformation2(parvec, local);

  // Apply the rigid component
  local = (parvec.transformCache * local).eval(); // Needs eval to avoid aliasing (?)
}

/**
 * Cache the rigid component of this deformation model
 * Add rotation and offset obtained from the sTGC fit 
 */
void ElementModelSTGC::cacheTransform(ParameterVector& parvec) const {
  const auto& pars = parvec.parameters;
  parvec.transformCache
    = Eigen::Translation3d(pars[X], pars[Y]+pars[OFF], pars[Z])
    * Eigen::AngleAxisd(pars[THZ]+pars[ROT], Eigen::Vector3d::UnitZ())
    * Eigen::AngleAxisd(pars[THY], Eigen::Vector3d::UnitY())
    * Eigen::AngleAxisd(pars[THX], Eigen::Vector3d::UnitX());
  parvec.transformCacheValid = true;
}

/*
 * Helper methods to convert parameter index to string representation
 */
ElementModelSTGC::ipar_t ElementModelSTGC::getParameterIndex(const std::string& parname) const {
  if (parname == "x") return parameter_t::X;
  if (parname == "y") return parameter_t::Y;
  if (parname == "z") return parameter_t::Z;
  if (parname == "thx") return parameter_t::THX;
  if (parname == "thy") return parameter_t::THY;
  if (parname == "thz") return parameter_t::THZ;
  if (parname == "rot") return parameter_t::ROT;
  if (parname == "off") return parameter_t::OFF;
  if (parname == "scl") return parameter_t::SCL;
  if (parname == "npar") return parameter_t::NPAR;
  throw std::runtime_error("Invalid parameter name "+parname);
}

std::string ElementModelSTGC::getParameterName(ipar_t ipar) const {
  switch (ipar) {
    case X: return "x";
    case Y: return "y";
    case Z: return "z";
    case THX: return "thx";
    case THY: return "thy";
    case THZ: return "thz";
    case ROT: return "rot";
    case OFF: return "off";
    case SCL: return "scl";
    case NPAR: return "npar";
    default: throw std::runtime_error("Invalid parameter");
  }
}

Amg::Vector3D ElementModelSTGC::stgcScale(double scl, Amg::Vector3D d0) const {
// Scale measured by CMM/Faro at construction sites
  return d0.array() * Eigen::Array3d{0., scl*0.001, 0.};
}

Amg::Vector3D ElementModelSTGC::stgcNonPar(double npar, Amg::Vector3D d0) const {
// Non-parallelism measured by CMM/Faro at construction sites
  double npar_eff = npar * d0[1]/(0.5*m_lenY);
  double delta = npar_eff * d0[0]/1000.;
  return Amg::Vector3D(0., delta, 0.);
}


void ElementModelSTGC::applyDeformation(const ParameterVector& parvec, Eigen::Ref<Amg::Vector3D> local) const {
  // Applies the deformation to the set of local vectors given as argument
  // This old-style implementation is the reference implementation
  Amg::Vector3D d0 = local - m_defo0;
  local = local
    + stgcScale(parvec[SCL], d0)
    + stgcNonPar(parvec[NPAR], d0)
    ;
}

void ElementModelSTGC::applyDeformation2(const ParameterVector& parvec, VectorSetRef local) const {
  // Applies the deformation to the set of local vectors given as argument
  // This implementation uses Eigen-style algebra (does the same as the method applyDeformation above, but benefits from Eigen's optimizations)

  // Temporaries allocated on the stack
  // d0 = local - defo0
  VectorSet d0 = local.colwise() - m_defo0;

  // r = {d0.x/(0.5*lenX), d0.y/(0.5*lenY), (d0.z)}
  // r.x is (-1,+1) at X=(-0.5*lenX, 0.5*lenX), similarly for Y
  VectorSet r = d0.array().colwise() * Eigen::Array3d{1.0/(0.5*m_lenX), 1.0/(0.5*m_lenY), 1.0};

  // p2.{x,y,z} = 1 - r.{x,y,z}^2
  // p2.x = 1 at r.x = 0 and p2.x = 0 at r.x = +-0.5*lenX, similarly for Y
//  VectorSet p2 = -1.0*(r.array().square().colwise() - Eigen::Array3d::Ones());

  double scl = parvec[SCL];
  double npar = parvec[NPAR];

  // SCL:
  local.array() += d0.array().colwise() * Eigen::Array3d{0., scl*0.001, 0.};

  // NPAR:
  local.topRows<2>().array() += (d0.topRows<2>().array() * r.topRows<2>().array().colwise().reverse()).colwise() * Eigen::Array2d{0., npar*0.001};

}


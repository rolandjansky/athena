/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonNSWAsBuilt/ElementModelScaleSag.h"
#include <stdexcept>

using namespace NswAsBuilt;

//===============================================================================
ElementModelScaleSag::ElementModelScaleSag(double lenX, double lenY, Amg::Vector3D defo0)
: m_lenX(lenX), m_lenY(lenY), m_defo0(defo0)
{ }

/**
 * Transform a set of vectors expressed in local frame, stored in a matrix
 */
void ElementModelScaleSag::transform(const ParameterVector& parvec, VectorSetRef local) const {

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
 */
void ElementModelScaleSag::cacheTransform(ParameterVector& parvec) const {
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
ElementModelScaleSag::ipar_t ElementModelScaleSag::getParameterIndex(const std::string& parname) const 
{
  if (parname == "x") return parameter_t::X;
  if (parname == "y") return parameter_t::Y;
  if (parname == "z") return parameter_t::Z;
  if (parname == "thx") return parameter_t::THX;
  if (parname == "thy") return parameter_t::THY;
  if (parname == "thz") return parameter_t::THZ;
  if (parname == "egx") return parameter_t::EGX;
  if (parname == "egy") return parameter_t::EGY;
  if (parname == "egz") return parameter_t::EGZ;
  if (parname == "sagx") return parameter_t::SAGX;
  if (parname == "sagy") return parameter_t::SAGY;
  if (parname == "degx") return parameter_t::DEGX;
  if (parname == "degy") return parameter_t::DEGY;
  if (parname == "pgx") return parameter_t::PGX;
  if (parname == "pgy") return parameter_t::PGY;
  if (parname == "dsagx") return parameter_t::DSAGX;
  if (parname == "dsagy") return parameter_t::DSAGY;
  throw std::runtime_error("Invalid parameter name "+parname);
}


//===============================================================================
std::string ElementModelScaleSag::getParameterName(ipar_t ipar) const 
{
  switch (ipar) {
    case X: return "x";
    case Y: return "y";
    case Z: return "z";
    case THX: return "thx";
    case THY: return "thy";
    case THZ: return "thz";
    case EGX: return "egx";
    case EGY: return "egy";
    case EGZ: return "egz";
    case SAGX: return "sagx";
    case SAGY: return "sagy";
    case DEGX: return "degx";
    case DEGY: return "degy";
    case PGX: return "pgx";
    case PGY: return "pgy";
    case DSAGX: return "dsagx";
    case DSAGY: return "dsagy";
    default: throw std::runtime_error("Invalid parameter");
  }
}

//===============================================================================
// The basic ingredients of the calculation
Amg::Vector3D ElementModelScaleSag::DEg( double egx, double egy, double egz, const Amg::Vector3D& d0) const 
{
  // Calculate thermal expansion
  return d0.array() * Eigen::Array3d{egx*0.001, egy*0.001, egz*0.001};
}


//===============================================================================
Amg::Vector3D ElementModelScaleSag::DSagX(double sagx, const Amg::Vector3D& d0) const 
{
  // Calculate sag along X
  double r = d0[1]/(0.5*m_lenY);
  // delta is sagx at y=y0, 0 at y=y0+-0.5*_lenY
  double delta = sagx * (1. - r*r);
  return Amg::Vector3D(delta, 0., 0.);
}


//===============================================================================
Amg::Vector3D ElementModelScaleSag::DSagY(double sagy, const Amg::Vector3D& d0) const 
{
  // Calculate sag along Y
  double r = d0[0]/(0.5*m_lenX);
  // delta is sagY at x=x0, 0 at x=x0+-0.5*_lenX
  double delta = sagy * (1. - r*r);
  return Amg::Vector3D(0., delta, 0.);
}


//===============================================================================
Amg::Vector3D ElementModelScaleSag::DDSagX(double dsagx, const Amg::Vector3D& d0) const 
{
  // Calculate sag along X
  double sagx = dsagx * d0[0]/(0.5*m_lenX);
  double r = d0[1]/(0.5*m_lenY);
  // delta is sagx at y=y0, 0 at y=y0+-0.5*_lenY
  double delta = sagx * (1. - r*r);
  return Amg::Vector3D(delta, 0., 0.);
}


//===============================================================================
Amg::Vector3D ElementModelScaleSag::DDSagY(double dsagy, const Amg::Vector3D& d0) const 
{
  // Calculate dsag along Y
  double sagy = dsagy * d0[1]/(0.5*m_lenY);
  double r = d0[0]/(0.5*m_lenX);
  // delta is sagY at x=x0, 0 at x=x0+-0.5*_lenX
  double delta = sagy * (1. - r*r);
  return Amg::Vector3D(0., delta, 0.);
}


//===============================================================================
Amg::Vector3D ElementModelScaleSag::DDegX(double degx, const Amg::Vector3D& d0) const 
{
  double egx_eff = degx * d0[1]/(0.5*m_lenY);
  double delta = egx_eff * d0[0]/1000.;
  return Amg::Vector3D(delta, 0., 0.);
}


//===============================================================================
Amg::Vector3D ElementModelScaleSag::DDegY(double degy, const Amg::Vector3D& d0) const 
{
  double egy_eff = degy * d0[0]/(0.5*m_lenX);
  double delta = egy_eff * d0[1]/1000.;
  return Amg::Vector3D(0., delta, 0.);
}


//===============================================================================
Amg::Vector3D ElementModelScaleSag::DPgX(double pgx, const Amg::Vector3D& d0) const 
{
  double delta = pgx * d0[1]/(0.5*m_lenY);
  return Amg::Vector3D(delta, 0., 0.);
}


//===============================================================================
Amg::Vector3D ElementModelScaleSag::DPgY(double pgy, const Amg::Vector3D& d0) const 
{
  double delta = pgy * d0[0]/(0.5*m_lenX);
  return Amg::Vector3D(0., delta, 0.);
}


//===============================================================================
void ElementModelScaleSag::applyDeformation(const ParameterVector& parvec, Eigen::Ref<Amg::Vector3D> local) const 
{
  // Applies the deformation to the set of local vectors given as argument
  // This old-style implementation is the reference implementation
  Amg::Vector3D d0 = local - m_defo0;
  local = local
    + DEg(parvec[EGX], parvec[EGY], parvec[EGZ], d0)
    + DSagX(parvec[SAGX], d0)
    + DSagY(parvec[SAGY], d0)
    + DDSagX(parvec[DSAGX], d0)
    + DDSagY(parvec[DSAGY], d0)
    + DDegX(parvec[DEGX], d0)
    + DDegY(parvec[DEGY], d0)
    + DPgX(parvec[PGX], d0)
    + DPgY(parvec[PGY], d0)
    ;
}


//===============================================================================
void ElementModelScaleSag::applyDeformation2(const ParameterVector& parvec, VectorSetRef local) const 
{
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
  VectorSet p2 = -1.0*(r.array().square().colwise() - Eigen::Array3d::Ones());

  double egx = parvec[EGX];
  double egy = parvec[EGY];
  double egz = parvec[EGZ];
  double sagx = parvec[SAGX];
  double sagy = parvec[SAGY];
  double dsagx = parvec[DSAGX];
  double dsagy = parvec[DSAGY];
  double degx = parvec[DEGX];
  double degy = parvec[DEGY];
  double pgx = parvec[PGX];
  double pgy = parvec[PGY];

  // EGX, EGY, EGZ:
  // d0.{x,y,z} * {egx, egy, egz}
  local.array() += d0.array().colwise() * Eigen::Array3d{egx*0.001, egy*0.001, egz*0.001};

  // SAGX, SAGY:
  // p2.{y,x} * {sagx, sagy}
  local.topRows<2>().array() += p2.topRows<2>().array().colwise().reverse().colwise() * Eigen::Array2d{sagx, sagy};

  // DSAGX, DSAGY:
  // p2.{y,x} * {dsagx*r.x, dsagy*r.y}
  local.topRows<2>().array() += (p2.topRows<2>().array().colwise().reverse() * r.topRows<2>().array()).colwise() * Eigen::Array2d{dsagx, dsagy};

  // DEGX, DEGY:
  // d0.{x,y} * {degx*r.y, degy*r.x}
  local.topRows<2>().array() += (d0.topRows<2>().array() * r.topRows<2>().array().colwise().reverse()).colwise() * Eigen::Array2d{degx*0.001, degy*0.001};

  // PGX, PGY:
  // r.{y,x} * {pgx, pgy}
  local.topRows<2>().array() += r.topRows<2>().array().colwise().reverse().colwise() * Eigen::Array2d{pgx, pgy};
}


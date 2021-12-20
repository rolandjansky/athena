/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************
// Liquid Argon detector description package
// -----------------------------------------
//
//****************************************************************************

#include "CaloDetDescr/CaloDepthTool.h"

#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include <vector>

#include "GaudiKernel/SystemOfUnits.h"

// Calo specific stuff :
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"

#include <cmath>
#include <iomanip>
#include <iostream>

namespace {
constexpr double s_default = 0;
}
static const InterfaceID IID_CaloDepthTool("CaloDepthTool", 1, 0);
const InterfaceID&
CaloDepthTool::interfaceID()
{
  return IID_CaloDepthTool;
}

CaloDepthTool::CaloDepthTool(const std::string& type,
                             const std::string& name,
                             const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_calo_id(nullptr)
{
  declareInterface<CaloDepthTool>(this);
}

CaloDepthTool::~CaloDepthTool() {}

StatusCode
CaloDepthTool::initialize()
{

  ATH_MSG_DEBUG(" getting started ");

  ATH_CHECK(detStore()->retrieve(m_calo_id, "CaloCell_ID"));

  if (m_depth_choice.empty()) {
    ATH_MSG_INFO(" CaloDepthTool "
                 << this->name()
                 << " successfully initialised, will provide entrance (default)"
                 << m_depth_choice);
  } else {
    ATH_MSG_INFO(" CaloDepthTool " << this->name()
                                   << " successfully initialised, will provide "
                                   << m_depth_choice);
  }
  return StatusCode::SUCCESS;
}

StatusCode
CaloDepthTool::finalize()
{
  return StatusCode::SUCCESS;
}

// the generic methods to be used by clients :

double
CaloDepthTool::radius(const CaloCell_ID::SUBCALO subcalo,
                      const int sampling_or_module,
                      const bool barrel,
                      const double eta,
                      const double phi,
                      const CaloDetDescrManager* caloDD) const
{
  CaloCell_ID::CaloSample sample;
  CaloDetDescrManager::build_sample(
    subcalo, barrel, sampling_or_module, sample);
  return radius(sample, eta, phi, caloDD);
}

double
CaloDepthTool::radius(const CaloCell_ID::CaloSample sample,
                      const double eta,
                      const double phi,
                      const CaloDetDescrManager* caloDD) const
{
  double radius = s_default;

  if (m_depth_choice == "egparam") {
    radius = egparametrized(sample, eta, phi, caloDD);
  } else if (m_depth_choice == "cscopt") {
    radius = cscopt_parametrized(sample, eta, phi, caloDD);
  } else if (m_depth_choice == "cscopt2") {
    radius = cscopt2_parametrized(sample, eta, phi, caloDD);
  } else if (m_depth_choice == "TBparam") {
    radius = TBparametrized(sample, eta, phi, caloDD);
  } else if (m_depth_choice == "entrance") {
    radius = entrance(sample, eta, phi, caloDD);
  } else if (m_depth_choice == "middle") {
    radius = middle(sample, eta, phi, caloDD);
  } else if (m_depth_choice == "flat") {
    if (eta >= 0)
      radius = flat(sample, 1, caloDD);
    else
      radius = flat(sample, -1, caloDD);
  } else {
    radius = entrance(sample, eta, phi, caloDD);
  }

  // outside DD, use parametrised radius as default
  //  it is OK, but choice should be left tothe user
  if (std::abs(radius) < 10.) {
    radius = egparametrized(sample, eta, phi, caloDD);
  }

  return radius;
}

double
CaloDepthTool::deta(const CaloCell_ID::SUBCALO subcalo,
                    const int sampling_or_module,
                    const bool barrel,
                    const double eta,
                    const CaloDetDescrManager* caloDD) 
{
  CaloCell_ID::CaloSample sample;
  CaloDetDescrManager::build_sample(
    subcalo, barrel, sampling_or_module, sample);
  return deta(sample, eta, caloDD);
}

double
CaloDepthTool::deta(const CaloCell_ID::CaloSample sample,
                    const double eta,
                    const CaloDetDescrManager* caloDD) 
{
  double deta = 99999.;
  /*bool result =*/caloDD->is_in(eta, 0., sample, deta);
  return deta;
}

// -----------------------
// the really dirty ones :
// -----------------------

double
CaloDepthTool::egparametrized(const CaloCell_ID::SUBCALO subcalo,
                              const int sampling_or_module,
                              const bool barrel,
                              const double eta,
                              const double phi,
                              const CaloDetDescrManager* caloDD) 
{
  CaloCell_ID::CaloSample sample;
  CaloDetDescrManager::build_sample(
    subcalo, barrel, sampling_or_module, sample);
  return egparametrized(sample, eta, phi, caloDD);
}

double
CaloDepthTool::egparametrized(const CaloCell_ID::CaloSample sample,
                              const double eta,
                              const double /*phi*/,
                              const CaloDetDescrManager* caloDD) 
{
  double radius = s_default;

  // note that only LAREM was foreseen in egammaqdepth
  // here we go ... cut and paste the code as is, including hardcoded
  // parametrisation :-(

  float aeta = static_cast<float>(std::abs(eta));

  if (sample == CaloCell_ID::PreSamplerB) {
    radius = 1422.3 * Gaudi::Units::millimeter;
  } else if (sample == CaloCell_ID::EMB1) {
    if (aeta < 0.8)
      radius = (1567.8 - 18.975 * aeta - 17.668 * aeta * aeta) *
               Gaudi::Units::millimeter;
    else
      radius = (1503.2 + 71.716 * aeta - 41.008 * aeta * aeta) *
               Gaudi::Units::millimeter;
  } else if (sample == CaloCell_ID::EMB2) {
    if (aeta < 0.8)
      radius = (1697.1 - 15.311 * aeta - 64.153 * aeta * aeta) *
               Gaudi::Units::millimeter;
    else
      radius = (1739.1 - 75.648 * aeta - 18.501 * aeta * aeta) *
               Gaudi::Units::millimeter;
  } else if (sample == CaloCell_ID::EMB3) {
    if (aeta < 0.8)
      radius = (1833.88 - 106.25 * aeta) * Gaudi::Units::millimeter;
    else
      radius = (2038.40 - 286. * aeta) * Gaudi::Units::millimeter;
  }

  else if (sample == CaloCell_ID::PreSamplerE) {
    radius = 3600. * Gaudi::Units::millimeter;
    if (eta < 0.)
      radius = -radius;
  } else if (sample == CaloCell_ID::EME1) {
    radius = 3760. * Gaudi::Units::millimeter;
    if (eta < 0.)
      radius = -radius;
  } else if (sample == CaloCell_ID::EME2) {
    radius = 3880. * Gaudi::Units::millimeter;
    if (eta < 0.)
      radius = -radius;
  } else if (sample == CaloCell_ID::EME3) {
    radius = 4150. * Gaudi::Units::millimeter;
    if (eta < 0.)
      radius = -radius;
  }

  // This was not in the egamma parametrization, but it does not cost much to
  // add it :
  else {
    if (eta > 0.)
      radius = flat(sample, 1, caloDD);
    else
      radius = flat(sample, -1, caloDD);
  }

  return radius;
}

// Depths of samplings 1 and 2 found by finding the depth where the
// eta resolution is optimal.  Done for CSC data.  These coefficents
// are from the E=100 GeV single-photon sample.
double
CaloDepthTool::cscopt_parametrized(const CaloCell_ID::CaloSample sample,
                                   const double eta,
                                   const double /*phi*/,
                                   const CaloDetDescrManager* caloDD) 
{
  double radius = s_default;

  float aeta = static_cast<float>(std::abs(eta));

  if (sample == CaloCell_ID::PreSamplerB) {
    radius = 1422.3 * Gaudi::Units::millimeter;
  } else if (sample == CaloCell_ID::EMB1) {
    if (aeta < 0.8)
      radius = (1558.859292 - 4.990838 * aeta - 21.144279 * aeta * aeta) *
               Gaudi::Units::millimeter;
    else
      radius = (1522.775373 + 27.970192 * aeta - 21.104108 * aeta * aeta) *
               Gaudi::Units::millimeter;
  } else if (sample == CaloCell_ID::EMB2) {
    radius = (1689.621619 + 2.682993 * aeta - 70.165741 * aeta * aeta) *
             Gaudi::Units::millimeter;
  } else if (sample == CaloCell_ID::EMB3) {
    if (aeta < 0.8)
      radius = (1833.88 - 106.25 * aeta) * Gaudi::Units::millimeter;
    else
      radius = (2038.40 - 286. * aeta) * Gaudi::Units::millimeter;
  }

  else if (sample == CaloCell_ID::PreSamplerE) {
    radius = 3600. * Gaudi::Units::millimeter;
    if (eta < 0.)
      radius = -radius;
  } else if (sample == CaloCell_ID::EME1) {
    if (aeta < 1.5)
      radius = (12453.297448 - 5735.787116 * aeta) * Gaudi::Units::millimeter;
    else
      radius = 3790.671754 * Gaudi::Units::millimeter;
    if (eta < 0.)
      radius = -radius;
  } else if (sample == CaloCell_ID::EME2) {
    if (aeta < 1.5)
      radius = (8027.574119 - 2717.653528 * aeta) * Gaudi::Units::millimeter;
    else
      radius = (3473.473909 + 453.941515 * aeta - 119.101945 * aeta * aeta) *
               Gaudi::Units::millimeter;
    if (eta < 0.)
      radius = -radius;
  } else if (sample == CaloCell_ID::EME3) {
    radius = 4150. * Gaudi::Units::millimeter;
    if (eta < 0.)
      radius = -radius;
  }

  // This was not in the egamma parametrization,
  // but it does not cost much to add it :
  else {
    if (eta > 0.)
      radius = flat(sample, 1, caloDD);
    else
      radius = flat(sample, -1, caloDD);
  }

  return radius;
}

// The same as cscopt, except that the outermost point in EMB2 was
// excluded.  That point was influenced by edge effects and was distorting
// the fit, pulling it down too much at the end.  Excluding it is shown
// to somewhat improve the measured resolution.
double
CaloDepthTool::cscopt2_parametrized(const CaloCell_ID::CaloSample sample,
                                    const double eta,
                                    const double /*phi*/,
                                    const CaloDetDescrManager* caloDD) 
{
  double radius = s_default;

  float aeta = static_cast<float>(std::abs(eta));

  if (sample == CaloCell_ID::PreSamplerB) {
    radius = 1422.3 * Gaudi::Units::millimeter;
  } else if (sample == CaloCell_ID::EMB1) {
    if (aeta < 0.8)
      radius = (1558.859292 - 4.990838 * aeta - 21.144279 * aeta * aeta) *
               Gaudi::Units::millimeter;
    else
      radius = (1522.775373 + 27.970192 * aeta - 21.104108 * aeta * aeta) *
               Gaudi::Units::millimeter;
  } else if (sample == CaloCell_ID::EMB2) {
    radius = (1698.990944 - 49.431767 * aeta - 24.504976 * aeta * aeta) *
             Gaudi::Units::millimeter;
  } else if (sample == CaloCell_ID::EMB3) {
    if (aeta < 0.8)
      radius = (1833.88 - 106.25 * aeta) * Gaudi::Units::millimeter;
    else
      radius = (2038.40 - 286. * aeta) * Gaudi::Units::millimeter;
  }

  else if (sample == CaloCell_ID::PreSamplerE) {
    radius = 3600. * Gaudi::Units::millimeter;
    if (eta < 0.)
      radius = -radius;
  } else if (sample == CaloCell_ID::EME1) {
    if (aeta < 1.5)
      radius = (12453.297448 - 5735.787116 * aeta) * Gaudi::Units::millimeter;
    else
      radius = 3790.671754 * Gaudi::Units::millimeter;
    if (eta < 0.)
      radius = -radius;
  } else if (sample == CaloCell_ID::EME2) {
    if (aeta < 1.5)
      radius = (8027.574119 - 2717.653528 * aeta) * Gaudi::Units::millimeter;
    else
      radius = (3473.473909 + 453.941515 * aeta - 119.101945 * aeta * aeta) *
               Gaudi::Units::millimeter;
    if (eta < 0.)
      radius = -radius;
  } else if (sample == CaloCell_ID::EME3) {
    radius = 4150. * Gaudi::Units::millimeter;
    if (eta < 0.)
      radius = -radius;
  }

  // This was not in the egamma parametrization,
  // but it does not cost much to add it :
  else {
    if (eta > 0.)
      radius = flat(sample, 1, caloDD);
    else
      radius = flat(sample, -1, caloDD);
  }

  return radius;
}

double
CaloDepthTool::TBparametrized(const CaloCell_ID::SUBCALO subcalo,
                              const int sampling_or_module,
                              const bool barrel,
                              const double eta,
                              const double phi,
                              const CaloDetDescrManager* caloDD) 
{
  CaloCell_ID::CaloSample sample;
  CaloDetDescrManager::build_sample(
    subcalo, barrel, sampling_or_module, sample);
  return TBparametrized(sample, eta, phi, caloDD);
}

double
CaloDepthTool::TBparametrized(const CaloCell_ID::CaloSample sample,
                              const double eta,
                              const double phi,
                              const CaloDetDescrManager* caloDD) 
{
  double radius = s_default;

  // release 10.0.1 : the analysis is not advanced enough to know what is
  // needed. right now, use the eg parametrisation everywhere possible, entrance
  // elsewere.
  if (sample == CaloCell_ID::PreSamplerB) {
    return egparametrized(sample, eta, phi, caloDD);
  } else if (sample == CaloCell_ID::EMB1) {
    return egparametrized(sample, eta, phi, caloDD);
  } else if (sample == CaloCell_ID::EMB2) {
    return egparametrized(sample, eta, phi, caloDD);
  } else if (sample == CaloCell_ID::EMB3) {
    return egparametrized(sample, eta, phi, caloDD);
  } else {
    return entrance(sample, eta, phi, caloDD);
  }

  return radius;
}

// ------------------
// the cleaner ones :
// ------------------

double
CaloDepthTool::entrance(const CaloCell_ID::SUBCALO subcalo,
                        const int sampling_or_module,
                        const bool barrel,
                        const double eta,
                        const double phi,
                        const CaloDetDescrManager* caloDD) 
{
  CaloCell_ID::CaloSample sample;
  CaloDetDescrManager::build_sample(
    subcalo, barrel, sampling_or_module, sample);
  return entrance(sample, eta, phi, caloDD);
}

double
CaloDepthTool::entrance(const CaloCell_ID::CaloSample sample,
                        const double eta,
                        const double phi,
                        const CaloDetDescrManager* caloDD) 
{
  double radius = s_default;

  // FIXME : the calodetdescr manager misses 1 generic get_entrance method
  if (sample == CaloCell_ID::PreSamplerE || sample == CaloCell_ID::EME1 ||
      sample == CaloCell_ID::EME2 || sample == CaloCell_ID::EME3 ||
      sample == CaloCell_ID::HEC0 || sample == CaloCell_ID::HEC1 ||
      sample == CaloCell_ID::HEC2 || sample == CaloCell_ID::HEC3 ||
      sample == CaloCell_ID::FCAL0 || sample == CaloCell_ID::FCAL1 ||
      sample == CaloCell_ID::FCAL2)
    radius = get_entrance_z(sample, eta, phi, caloDD);
  else
    radius = get_entrance_radius(sample, eta, phi, caloDD);

  return radius;
}

double
CaloDepthTool::middle(const CaloCell_ID::SUBCALO subcalo,
                      const int sampling_or_module,
                      const bool barrel,
                      const double eta,
                      const double phi,
                      const CaloDetDescrManager* caloDD) 
{
  CaloCell_ID::CaloSample sample;
  CaloDetDescrManager::build_sample(
    subcalo, barrel, sampling_or_module, sample);
  return middle(sample, eta, phi, caloDD);
}

double
CaloDepthTool::middle(const CaloCell_ID::CaloSample sample,
                      const double eta,
                      const double phi,
                      const CaloDetDescrManager* caloDD) 
{
  double radius = s_default;

  if (sample == CaloCell_ID::PreSamplerE || sample == CaloCell_ID::EME1 ||
      sample == CaloCell_ID::EME2 || sample == CaloCell_ID::EME3 ||
      sample == CaloCell_ID::HEC0 || sample == CaloCell_ID::HEC1 ||
      sample == CaloCell_ID::HEC2 || sample == CaloCell_ID::HEC3 ||
      sample == CaloCell_ID::FCAL0 || sample == CaloCell_ID::FCAL1 ||
      sample == CaloCell_ID::FCAL2)
    radius = get_middle_z(sample, eta, phi, caloDD);
  else
    radius = get_middle_radius(sample, eta, phi, caloDD);

  return radius;
}

double
CaloDepthTool::exit(const CaloCell_ID::SUBCALO subcalo,
                    const int sampling_or_module,
                    const bool barrel,
                    const double eta,
                    const double phi,
                    const CaloDetDescrManager* caloDD) 
{
  CaloCell_ID::CaloSample sample;
  CaloDetDescrManager::build_sample(
    subcalo, barrel, sampling_or_module, sample);
  return exit(sample, eta, phi, caloDD);
}

double
CaloDepthTool::exit(const CaloCell_ID::CaloSample sample,
                    const double eta,
                    const double phi,
                    const CaloDetDescrManager* caloDD) 
{
  double radius = s_default;

  if (sample == CaloCell_ID::PreSamplerE || sample == CaloCell_ID::EME1 ||
      sample == CaloCell_ID::EME2 || sample == CaloCell_ID::EME3 ||
      sample == CaloCell_ID::HEC0 || sample == CaloCell_ID::HEC1 ||
      sample == CaloCell_ID::HEC2 || sample == CaloCell_ID::HEC3 ||
      sample == CaloCell_ID::FCAL0 || sample == CaloCell_ID::FCAL1 ||
      sample == CaloCell_ID::FCAL2)
    radius = get_exit_z(sample, eta, phi, caloDD);
  else
    radius = get_exit_radius(sample, eta, phi, caloDD);

  return radius;
}

double
CaloDepthTool::flat(const CaloCell_ID::SUBCALO subcalo,
                    const int sampling_or_module,
                    const bool barrel,
                    const int side,
                    const CaloDetDescrManager* caloDD) 
{
  CaloCell_ID::CaloSample sample;
  CaloDetDescrManager::build_sample(
    subcalo, barrel, sampling_or_module, sample);
  return flat(sample, side, caloDD);
}

double
CaloDepthTool::flat(const CaloCell_ID::CaloSample sample,
                    const int side,
                    const CaloDetDescrManager* caloDD) 
{
  // FIXME : tiles is hardcoded !!!
  double radius = 2280.;

  if (sample == CaloCell_ID::TileBar0)
    return false;
  if (sample == CaloCell_ID::TileBar1)
    return false;
  if (sample == CaloCell_ID::TileBar2)
    return false;
  if (sample == CaloCell_ID::TileGap1)
    return false;
  if (sample == CaloCell_ID::TileGap2)
    return false;
  if (sample == CaloCell_ID::TileExt0)
    return false;
  if (sample == CaloCell_ID::TileExt1)
    return false;
  if (sample == CaloCell_ID::TileExt2)
    return false;

  radius = s_default;
  for (const CaloDetDescriptor* reg : caloDD->calo_descriptors_range()) {
    if (reg) {
      if (reg->getSampling(0) == sample && reg->calo_sign() * side > 0) {
        std::vector<double> depth;
        reg->get_depth_in(depth);
        for (unsigned int j = 0; j < depth.size(); j++)
          if (radius < depth[j])
            radius = depth[j];
      }
    }
  }

  // depth in are positive values, so should multiply by -1 for the HEC and FCAL
  // at eta<0. to get the corresponding z

  if (side < 0) {
    if (sample == CaloCell_ID::PreSamplerE || sample == CaloCell_ID::EME1 ||
        sample == CaloCell_ID::EME2 || sample == CaloCell_ID::EME3 ||
        sample == CaloCell_ID::HEC0 || sample == CaloCell_ID::HEC1 ||
        sample == CaloCell_ID::HEC2 || sample == CaloCell_ID::HEC3 ||
        sample == CaloCell_ID::FCAL0 || sample == CaloCell_ID::FCAL1 ||
        sample == CaloCell_ID::FCAL2)
      radius = -1. * radius;
  }
  return radius;
}

double
CaloDepthTool::depth(const CaloCell_ID::SUBCALO subcalo,
                     const int sampling_or_module,
                     const bool barrel,
                     const int side,
                     const CaloDetDescrManager* caloDD) 
{
  CaloCell_ID::CaloSample sample;
  CaloDetDescrManager::build_sample(
    subcalo, barrel, sampling_or_module, sample);
  return depth(sample, side, caloDD);
}

double
CaloDepthTool::depth(const CaloCell_ID::CaloSample sample,
                     const int side,
                     const CaloDetDescrManager* caloDD) 
{
  // FIXME : tiles is hardcoded !!!
  double radius = 1970.;

  if (sample == CaloCell_ID::TileBar0)
    return false;
  if (sample == CaloCell_ID::TileBar1)
    return false;
  if (sample == CaloCell_ID::TileBar2)
    return false;
  if (sample == CaloCell_ID::TileGap1)
    return false;
  if (sample == CaloCell_ID::TileGap2)
    return false;
  if (sample == CaloCell_ID::TileExt0)
    return false;
  if (sample == CaloCell_ID::TileExt1)
    return false;
  if (sample == CaloCell_ID::TileExt2)
    return false;

  radius = s_default;

  for (const CaloDetDescriptor* reg : caloDD->calo_descriptors_range()) {
    if (reg) {
      if (reg->getSampling(0) == sample && reg->calo_sign() * side > 0) {
        std::vector<double> depth;
        reg->get_depth_out(depth);
        for (unsigned int j = 0; j < depth.size(); j++)
          if (radius > depth[j])
            radius = depth[j];
      }
    }
  }

  return radius;
}

double
CaloDepthTool::get_entrance_radius(CaloCell_ID::CaloSample sample,
                                   double eta,
                                   double phi,
                                   const CaloDetDescrManager* caloDD) 
{
  const CaloDetDescrElement* elt = caloDD->get_element(sample, eta, phi);
  if (!elt) {
    return s_default;
  }
  return (elt->r() - elt->dr());
}

double
CaloDepthTool::get_entrance_z(CaloCell_ID::CaloSample sample,
                              double eta,
                              double phi,
                              const CaloDetDescrManager* caloDD) 
{
  const CaloDetDescrElement* elt = caloDD->get_element(sample, eta, phi);
  if (!elt) {
    return s_default;
  }
  return (elt->z() - (elt->z() < 0 ? -elt->dz() : elt->dz()));
}

double
CaloDepthTool::get_middle_radius(CaloCell_ID::CaloSample sample,
                                 double eta,
                                 double phi,
                                 const CaloDetDescrManager* caloDD) 
{

  const CaloDetDescrElement* elt = caloDD->get_element(sample, eta, phi);
  if (!elt) {
    return s_default;
  }
  return elt->r();
}

double
CaloDepthTool::get_middle_z(CaloCell_ID::CaloSample sample,
                            double eta,
                            double phi,
                            const CaloDetDescrManager* caloDD) 
{

  const CaloDetDescrElement* elt = caloDD->get_element(sample, eta, phi);
  if (!elt) {
    return s_default;
  }
  return elt->z();
}

double
CaloDepthTool::get_exit_radius(CaloCell_ID::CaloSample sample,
                               double eta,
                               double phi,
                               const CaloDetDescrManager* caloDD) 
{

  const CaloDetDescrElement* elt = caloDD->get_element(sample, eta, phi);
  if (!elt) {
    return s_default;
  }
  return (elt->r() + elt->dr());
}

double
CaloDepthTool::get_exit_z(CaloCell_ID::CaloSample sample,
                          double eta,
                          double phi,
                          const CaloDetDescrManager* caloDD) 
{

  const CaloDetDescrElement* elt = caloDD->get_element(sample, eta, phi);
  if (!elt) {
    return s_default;
  }
  return (elt->z() + (elt->z() < 0 ? -elt->dz() : elt->dz()));
}


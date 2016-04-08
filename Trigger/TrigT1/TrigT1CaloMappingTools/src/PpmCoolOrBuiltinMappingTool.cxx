/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "PpmCoolOrBuiltinMappingTool.h"

namespace LVL1 {


PpmCoolOrBuiltinMappingTool::PpmCoolOrBuiltinMappingTool(const std::string& type,
            const std::string& name, const IInterface*  parent)
          : AthAlgTool(type, name, parent),
            m_coolTool("LVL1::PpmCoolMappingTool/PpmCoolMappingTool"),
            m_builtinTool("LVL1::PpmMappingTool/PpmMappingTool"),
	    m_coolCheckDone(false)
{
  declareInterface<IL1CaloMappingTool>(this);

  declareProperty("CoolMappingTool", m_coolTool);
  declareProperty("BuiltinMappingTool", m_builtinTool);

}

PpmCoolOrBuiltinMappingTool::~PpmCoolOrBuiltinMappingTool()
{
}

// Initialise the mappings

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode PpmCoolOrBuiltinMappingTool::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endreq;

  // Retrieve the COOL mapping tool
  StatusCode sc = m_coolTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve tool " << m_coolTool << endreq;
    return sc;
  } else msg(MSG::INFO) << "Retrieved tool " << m_coolTool << endreq;

  // Retrieve the built-in mapping tool
  sc = m_builtinTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve tool " << m_builtinTool << endreq;
    return sc;
  } else msg(MSG::INFO) << "Retrieved tool " << m_builtinTool << endreq;

  return StatusCode::SUCCESS;
}

StatusCode PpmCoolOrBuiltinMappingTool::finalize()
{

  return StatusCode::SUCCESS;
}
 
// Return eta, phi and layer mapping for given crate/module/channel

bool PpmCoolOrBuiltinMappingTool::mapping(const int crate, const int module,
         const int channel, double& eta, double& phi, int& layer)
{
  bool rc = false;
  if (coolWorks()) {
    rc = m_coolTool->mapping(crate, module, channel, eta, phi, layer);
  } else {
    rc = m_builtinTool->mapping(crate, module, channel, eta, phi, layer);
  }
  return rc;
}

// Return crate, module and channel mapping for given eta/phi/layer

bool PpmCoolOrBuiltinMappingTool::mapping(const double eta, const double phi,
                    const int layer, int& crate, int& module, int& channel)
{
  bool rc = false;
  if (coolWorks()) {
    rc = m_coolTool->mapping(eta, phi, layer, crate, module, channel);
  } else {
    rc = m_builtinTool->mapping(eta, phi, layer, crate, module, channel);
  }
  return rc;
}

// Check if COOL mappings are working

bool PpmCoolOrBuiltinMappingTool::coolWorks()
{
  if ( !m_coolCheckDone ) {
    const double testEta   = 3.8375;
    const double testPhi   = M_PI/16.;
    const int    testLayer = 1;
    int crate   = 0;
    int module  = 0;
    int channel = 0;
    m_coolCheckResult = ((m_coolTool->mapping(testEta, testPhi, testLayer,
                          crate, module, channel)) &&
			 !(crate == 4 && module == 8 && channel == 48));
    if (m_coolCheckResult) {
      const double tolerance = 0.01;
      double eta   = 0.;
      double phi   = 0.;
      int    layer = 0;
      m_coolCheckResult = ((m_coolTool->mapping(crate, module, channel,
                            eta, phi, layer)) &&
			   (fabs(eta-testEta) < tolerance) &&
			   (fabs(phi-testPhi) < tolerance) &&
			   (layer == testLayer));
    }
    m_coolCheckDone = true;
    if (m_coolCheckResult) {
      msg(MSG::INFO)      << "COOL mappings selected"     << endreq;
    } else msg(MSG::INFO) << "Built-in mappings selected" << endreq;
  }
  return m_coolCheckResult;
}

} // end namespace

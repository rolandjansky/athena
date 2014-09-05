/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Athena includes
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileConditions/TileCondToolIntegrator.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileCalibBlobObjs/Exception.h"

//
//____________________________________________________________________
static const InterfaceID IID_TileCondToolIntegrator("TileCondToolIntegrator", 1, 0);

const InterfaceID& TileCondToolIntegrator::interfaceID() {
  return IID_TileCondToolIntegrator;
}

//
//____________________________________________________________________
TileCondToolIntegrator::TileCondToolIntegrator(const std::string& type, const std::string& name,
    const IInterface* parent)
    : AthAlgTool(type, name, parent)
    , m_pryIntegrator("TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_Integrator", this)
{
  declareInterface<TileCondToolIntegrator>(this);
  declareProperty("ProxyIntegrator", m_pryIntegrator);
}

//
//____________________________________________________________________
TileCondToolIntegrator::~TileCondToolIntegrator() {
}

//
//____________________________________________________________________
StatusCode TileCondToolIntegrator::initialize() {
  ATH_MSG_DEBUG( "In initialize()" );

  //=== Retrieve proxy
  CHECK( m_pryIntegrator.retrieve() );

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileCondToolIntegrator::finalize() {
  ATH_MSG_DEBUG(  "finalize called" );
  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
float TileCondToolIntegrator::getGain(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {

  return m_pryIntegrator->getCalibDrawer(drawerIdx)->getData(channel, adc, 0);
}

//
//____________________________________________________________________
float TileCondToolIntegrator::getGainError(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {

  return m_pryIntegrator->getCalibDrawer(drawerIdx)->getData(channel, adc, 1);
}

//
//____________________________________________________________________
float TileCondToolIntegrator::getChi2(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {

  return m_pryIntegrator->getCalibDrawer(drawerIdx)->getData(channel, adc, 2);
}

//
//____________________________________________________________________
float TileCondToolIntegrator::getPedestal(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {

  return m_pryIntegrator->getCalibDrawer(drawerIdx)->getData(channel, adc, 3);
}

//
//____________________________________________________________________
float TileCondToolIntegrator::getDACForPed(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {

  return m_pryIntegrator->getCalibDrawer(drawerIdx)->getData(channel, adc, 4);
}

//
//____________________________________________________________________
float TileCondToolIntegrator::getSigmaOfPed(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {

  return m_pryIntegrator->getCalibDrawer(drawerIdx)->getData(channel, adc, 5);
}

//
//____________________________________________________________________
float TileCondToolIntegrator::getRMSOfPed(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {

  return m_pryIntegrator->getCalibDrawer(drawerIdx)->getData(channel, adc, 6);
}

//
//____________________________________________________________________
float TileCondToolIntegrator::getSigmaOfRMS(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {

  return m_pryIntegrator->getCalibDrawer(drawerIdx)->getData(channel, adc, 7);
}


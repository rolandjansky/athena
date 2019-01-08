/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileConditions/TileCondToolOfcCool.h"
#include "TileCalibBlobObjs/TileCalibDrawerOfc.h"


// Athena includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadCondHandle.h"

//____________________________________________________________________
TileCondToolOfcCool::TileCondToolOfcCool(const std::string& type, const std::string& name,
    const IInterface* parent)
    : base_class(type, name, parent)
{
}

//
//____________________________________________________________________
TileCondToolOfcCool::~TileCondToolOfcCool() {

}

//
//____________________________________________________________________
StatusCode TileCondToolOfcCool::initialize() {
  ATH_MSG_DEBUG( "In initialize()" );

  ATH_CHECK( m_calibOfcKey.initialize() );

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileCondToolOfcCool::finalize() {
  ATH_MSG_DEBUG( "finalize called" );

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode
TileCondToolOfcCool::getOfcWeights(unsigned int drawerIdx,
                                   unsigned int channel,
                                   unsigned int adc,
                                   float& phase,
                                   bool /*of2*/,
                                   TileOfcWeightsStruct& weights) const
{
  std::fill (std::begin(weights.g), std::end(weights.g), 0);
  std::fill (std::begin(weights.dg), std::end(weights.dg), 0);
  std::fill (std::begin(weights.w_a), std::end(weights.w_a), 0);
  std::fill (std::begin(weights.w_b), std::end(weights.w_b), 0);
  std::fill (std::begin(weights.w_c), std::end(weights.w_c), 0);

  SG::ReadCondHandle<TileCalibData<TileCalibDrawerOfc>> calibOFC(m_calibOfcKey);

  calibOFC->getCalibDrawer(drawerIdx)->fillOfc(channel, adc, phase, weights.w_a, weights.w_b,
                                               weights.w_c, weights.g, weights.dg);

  weights.n_samples = calibOFC->getCalibDrawer(drawerIdx)->getNSamples();

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
int TileCondToolOfcCool::getOfcWeights(unsigned int drawerIdx,
                                       unsigned int channel,
                                       unsigned int adc,
                                       float& phase, 
                                       float *a, float *b, float *c, float *g, float *dg) {

  SG::ReadCondHandle<TileCalibData<TileCalibDrawerOfc>> calibOFC(m_calibOfcKey);

  calibOFC->getCalibDrawer(drawerIdx)->fillOfc(channel, adc, phase, a, b, c, g, dg);

  return calibOFC->getCalibDrawer(drawerIdx)->getNSamples();
}

//
//____________________________________________________________________
void TileCondToolOfcCool::getOfcParams(unsigned int drawerIdx
                                       , int &NPhases
                                       , int &NFields
                                       , int &Phamin
                                       , int &Phamax
                                       , int &NSamples) {

  SG::ReadCondHandle<TileCalibData<TileCalibDrawerOfc>> calibOFC(m_calibOfcKey);

  NPhases = calibOFC->getCalibDrawer(drawerIdx)->getNPhases();
  NFields = calibOFC->getCalibDrawer(drawerIdx)->getNFields();
  Phamin = round(calibOFC->getCalibDrawer(drawerIdx)->getPhase(0, 0, 0) * (1 / PHASE_PRECISION));
  Phamax = round(calibOFC->getCalibDrawer(drawerIdx)->getPhase(0, 0, abs(NPhases) - 1) * (1 / PHASE_PRECISION));
  NSamples = calibOFC->getCalibDrawer(drawerIdx)->getNSamples();
}

//
//____________________________________________________________________


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileConditions/TileCondToolOfcCool.h"
#include "TileCalibBlobObjs/TileCalibDrawerOfc.h"


// Athena includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadCondHandle.h"

//
//____________________________________________________________________
static const InterfaceID IID_TileCondToolOfcCool("TileCondToolOfcCool", 1, 0);

const InterfaceID& TileCondToolOfcCool::interfaceID() {
  return IID_TileCondToolOfcCool;
}

//
//____________________________________________________________________
TileCondToolOfcCool::TileCondToolOfcCool(const std::string& type, const std::string& name,
    const IInterface* parent)
    : AthAlgTool(type, name, parent)
    , m_weights(0)
    , m_NPhases(0)
    , m_NFields(0)
    , m_NSamples(0)
    , m_first(true)
{
  declareInterface<TileCondToolOfcCool>(this);
  declareInterface<ITileCondToolOfc>(this);
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

  //=== prepare structure for OFCs
  m_weights = new TileOfcWeightsStruct;
  memset(m_weights, 0, sizeof(TileOfcWeightsStruct));

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileCondToolOfcCool::finalize() {
  if (m_weights) delete m_weights;
  ATH_MSG_DEBUG( "finalize called" );

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
const TileOfcWeightsStruct* TileCondToolOfcCool::getOfcWeights(unsigned int drawerIdx, 
                                                               unsigned int channel, 
                                                               unsigned int adc,
                                                               float& phase, 
                                                               bool /* of2 */) {


  SG::ReadCondHandle<TileCalibData<TileCalibDrawerOfc>> calibOFC(m_calibOfcKey);

  if (m_first) {
    m_first = false;
    m_NPhases = calibOFC->getCalibDrawer(drawerIdx)->getNPhases();
    m_NFields = calibOFC->getCalibDrawer(drawerIdx)->getNFields();
    m_NSamples = calibOFC->getCalibDrawer(drawerIdx)->getNSamples();

    ATH_MSG_DEBUG( "OFC Blob Type " << calibOFC->getCalibDrawer(drawerIdx)->getType()
                   << " NPhases " << m_NPhases
                   << " NFields " << m_NFields );

  }

  calibOFC->getCalibDrawer(drawerIdx)->fillOfc(channel, adc, phase, m_weights->w_a, m_weights->w_b,
                                                   m_weights->w_c, m_weights->g, m_weights->dg);

  m_weights->n_samples = m_NSamples;

  return m_weights;
}

//
//____________________________________________________________________
int TileCondToolOfcCool::getOfcWeights(unsigned int drawerIdx,
                                       unsigned int channel,
                                       unsigned int adc,
                                       float& phase, 
                                       float *a, float *b, float *c, float *g, float *dg) {

  SG::ReadCondHandle<TileCalibData<TileCalibDrawerOfc>> calibOFC(m_calibOfcKey);

  if (m_first) {
    m_first = false;
    m_NPhases = calibOFC->getCalibDrawer(drawerIdx)->getNPhases();
    m_NFields = calibOFC->getCalibDrawer(drawerIdx)->getNFields();
    m_NSamples = calibOFC->getCalibDrawer(drawerIdx)->getNSamples();

    ATH_MSG_DEBUG( "OFC Blob Type " << calibOFC->getCalibDrawer(drawerIdx)->getType()
                  << " NPhases " << m_NPhases
                  << " NFields " << m_NFields );

  }

  calibOFC->getCalibDrawer(drawerIdx)->fillOfc(channel, adc, phase, a, b, c, g, dg);

  return m_NSamples;
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


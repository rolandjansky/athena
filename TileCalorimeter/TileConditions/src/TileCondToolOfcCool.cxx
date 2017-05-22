/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Athena includes
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileConditions/TileCondToolOfcCool.h"
#include "TileCalibBlobObjs/Exception.h"
#include "TileConditions/TileCondProxyWrapper.h"
#include "TileConditions/ITileCondToolOfc.h"


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
    , m_pryOfcCool("TileCondProxyCool_TileCalibDrawerOfc_/TileCondProxyDefault_OfcCool", this)
    , m_weights(0)
    , m_NPhases(0)
    , m_NFields(0)
    , m_Phamin(0)
    , m_Phamax(0)
    , m_NSamples(0)
    , m_first(true)
{
  declareInterface<TileCondToolOfcCool>(this);
  declareInterface<ITileCondToolOfc>(this);
  declareProperty("ProxyOfcCool", m_pryOfcCool);
}

//
//____________________________________________________________________
TileCondToolOfcCool::~TileCondToolOfcCool() {
  //  if(m_weights) delete m_weights;
}

//
//____________________________________________________________________
StatusCode TileCondToolOfcCool::initialize() {
  ATH_MSG_DEBUG( "In initialize()" );

  //=== Retrieve offline proxies
  CHECK( m_pryOfcCool.retrieve() );

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

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolOfcCool::getOfcWeights", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  if (m_first) {
    m_first = false;
    m_NPhases = m_pryOfcCool->getCalibDrawer(drawerIdx)->getNPhases();
    m_NFields = m_pryOfcCool->getCalibDrawer(drawerIdx)->getNFields();
    m_NSamples = m_pryOfcCool->getCalibDrawer(drawerIdx)->getNSamples();

    ATH_MSG_DEBUG( "OFC Blob Type " << m_pryOfcCool->getCalibDrawer(drawerIdx)->getType()
                   << " NPhases " << m_NPhases
                   << " NFields " << m_NFields );

  }

  m_pryOfcCool->getCalibDrawer(drawerIdx)->fillOfc(channel, adc, phase, m_weights->w_a, m_weights->w_b, 
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

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolOfcCool::getOfcWeights", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  if (m_first) {
    m_first = false;
    m_NPhases = m_pryOfcCool->getCalibDrawer(drawerIdx)->getNPhases();
    m_NFields = m_pryOfcCool->getCalibDrawer(drawerIdx)->getNFields();
    m_NSamples = m_pryOfcCool->getCalibDrawer(drawerIdx)->getNSamples();

    ATH_MSG_DEBUG( "OFC Blob Type " << m_pryOfcCool->getCalibDrawer(drawerIdx)->getType()
                  << " NPhases " << m_NPhases
                  << " NFields " << m_NFields );

  }

  m_pryOfcCool->getCalibDrawer(drawerIdx)->fillOfc(channel, adc, phase, a, b, c, g, dg);

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

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolOfcCool::getOfcWeights", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  NPhases = m_pryOfcCool->getCalibDrawer(drawerIdx)->getNPhases();
  NFields = m_pryOfcCool->getCalibDrawer(drawerIdx)->getNFields();
  Phamin = round(m_pryOfcCool->getCalibDrawer(drawerIdx)->getPhase(0, 0, 0) * (1 / PHASE_PRECISION));
  Phamax = round(m_pryOfcCool->getCalibDrawer(drawerIdx)->getPhase(0, 0, abs(NPhases) - 1) * (1 / PHASE_PRECISION));
  NSamples = m_pryOfcCool->getCalibDrawer(drawerIdx)->getNSamples();
}

//
//____________________________________________________________________


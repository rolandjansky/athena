/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
//#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileDCSSvc.h"
//#include "src/TileDCSTool.h"
#include "TileConditions/TileCablingService.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"


static const InterfaceID IID_TileDCSSvc("TileDCSSvc", 1, 0);
const InterfaceID& TileDCSSvc::interfaceID() {
  return IID_TileDCSSvc;
}

TileDCSSvc::TileDCSSvc(const std::string& name, ISvcLocator* pSvcLocator)
    : AthService(name, pSvcLocator)
  , m_cabling(nullptr)
  , m_version(2)
{

  declareProperty("Version", m_version); // 1 - RUN1 conditions DB, 2 - RUN2 conditions

  m_partName.push_back("AUX");
  m_partName.push_back("LBA");
  m_partName.push_back("LBC");
  m_partName.push_back("EBA");
  m_partName.push_back("EBC");

  m_ncalls = 0;

}

//_____________________________________________________________________________
StatusCode TileDCSSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (TileDCSSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<TileDCSSvc*>(this);
  } else {
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode TileDCSSvc::initialize() {

  //=== get TileDCSTool
  ATH_CHECK( m_dcsTool.retrieve());

  //=== get TileCablingSvc
  ServiceHandle<TileCablingSvc> cablingSvc("TileCablingSvc", name());
  ATH_CHECK( cablingSvc.retrieve());

  //=== cache pointers to cabling helpers
  m_cabling = cablingSvc->cablingService();
  if (!m_cabling) {
    ATH_MSG_ERROR( "Pointer to TileCablingService is zero: " << m_cabling);
    return StatusCode::FAILURE;
  }

  for (int i = 0; i < 2; ++i) {
    int ros = i + 2;
    int channel = 0;
    for (; channel < 48; ++channel) {
      int pmt = std::abs(m_cabling->channel2hole(ros, channel));
      m_pmt2channel[i][pmt - 1] = channel; // note - pmt-1 is used as index
    }
    for (; channel < 68; ++channel) {
      m_pmt2channel[i][channel] = channel;
    }
  }


  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode TileDCSSvc::finalize() {
  return StatusCode::SUCCESS;
}

float TileDCSSvc::getDCSHV(int ros,  int module,  int pmt) const {
  int lb_eb = (ros > 2) ? 1 : 0;
  int channel = m_pmt2channel[lb_eb][pmt - 1];
  return m_dcsTool->getChannelHV(ros, module - 1, channel);
}

float TileDCSSvc::getDCSHVSET(int ros,  int module,  int pmt) const {
  int lb_eb = (ros > 2) ? 1 : 0;
  int channel = m_pmt2channel[lb_eb][pmt - 1];
  return m_dcsTool->getChannelHVSet(ros, module - 1, channel);
}

int   TileDCSSvc::getDCSSTATES (int ros,  int module) const {
  return m_dcsTool->getDrawerStates(ros, module - 1);
}


//_____________________________________________________________________________
TileDCSSvc::TileDCSStatus TileDCSSvc::getDCSHVSTATUS(int ros, int drawer, int channel) const {
  return static_cast<TileDCSSvc::TileDCSStatus>(m_dcsTool->getDCSHVStatus(ros, drawer, channel));
}

//_____________________________________________________________________________
TileDCSSvc::TileDCSStatus TileDCSSvc::getDCSSTATUS(int ros, int drawer) const {
  return static_cast<TileDCSSvc::TileDCSStatus>(m_dcsTool->getDCSStatus(ros, drawer));
}


TileDCSSvc::TileDCSStatus TileDCSSvc::getDCSSTATUS(int ros, int drawer, int channel) const {
  return static_cast<TileDCSSvc::TileDCSStatus>(m_dcsTool->getDCSStatus(ros, drawer, channel));
}

bool TileDCSSvc::statusHVIsBad (int ros, int drawer, int channel) const {
  return m_dcsTool->isStatusHVBad(ros, drawer, channel);
}

bool TileDCSSvc::statusIsBad (int ros, int drawer) const {
  return m_dcsTool->isStatusBad(ros, drawer);
}

bool TileDCSSvc::statusIsBad (int ros, int drawer, int channel) const {
  return m_dcsTool->isStatusBad(ros, drawer, channel);
}

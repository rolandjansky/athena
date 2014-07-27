/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AthenaKernel/errorcheck.h"

#include "TileByteStream/TileRawChannelContByteStreamTool.h"
#include "TileByteStream/TileROD_Encoder.h"

#include "TileEvent/TileRawChannelCollection.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileRawChannel.h"
#include "TileEvent/TileFastRawChannel.h"

#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/ITileBadChanTool.h"

#include <map> 
#include <stdint.h>

static const InterfaceID IID_ITileRawChannelContByteStreamTool("TileRawChannelContByteStreamTool",
    1, 0);

const InterfaceID& TileRawChannelContByteStreamTool::interfaceID() {
  return IID_ITileRawChannelContByteStreamTool;
}

// default constructor

TileRawChannelContByteStreamTool::TileRawChannelContByteStreamTool(const std::string& type,
    const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent)
    , m_tileHWID(0)
    , m_verbose(false)
    , m_tileToolEmscale("TileCondToolEmscale")
    , m_tileBadChanTool("TileBadChanTool")
    , m_channels(0)
{
  declareInterface<TileRawChannelContByteStreamTool>(this);
  declareProperty("DoFragType4", m_doFragType4 = true);
  declareProperty("DoFragType5", m_doFragType5 = false);
}

// destructor 

TileRawChannelContByteStreamTool::~TileRawChannelContByteStreamTool() {
}

StatusCode TileRawChannelContByteStreamTool::initialize() {

  CHECK( detStore()->retrieve(m_tileHWID, "TileHWID") );

  m_hid2re.setTileHWID(m_tileHWID);
  m_fea.idMap().setTileHWID(m_tileHWID);

  // get TileCondToolEmscale
  CHECK( m_tileToolEmscale.retrieve() );

  // get TileBadChanTool
  CHECK( m_tileBadChanTool.retrieve() );

  m_channels = new TileFastRawChannel[m_tileHWID->channel_hash_max()];

  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelContByteStreamTool::finalize() {
  delete[] m_channels;

  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelContByteStreamTool::convert(CONTAINER* cont,
    FullEventAssembler<TileHid2RESrcID> *fea) {

  //fea->clear();
  // m_fea.idMap().setTileHWID(m_tileHWID);

  // NOTE: conversion from ADC counts to MeV is commented out currently
  // i.e. units for raw channels in BS remain ADC counts

  TileFragHash::TYPE contType = cont->get_type();
  TileRawChannelUnit::UNIT inputUnit = cont->get_unit();
  if (contType == TileFragHash::MF && inputUnit != TileRawChannelUnit::ADCcounts) {
    ATH_MSG_WARNING( " Wrong container passed to converter - ignoring it ");
    return StatusCode::SUCCESS;
  }
  TileRawChannelUnit::UNIT outputUnit = inputUnit; // TileRawChannelUnit::MegaElectronVolts;
  bool oflCont = (inputUnit < TileRawChannelUnit::OnlineOffset);

  FullEventAssembler<TileHid2RESrcID>::RODDATA* theROD;
  TileRawChannelContainer::const_iterator it_coll = cont->begin();
  TileRawChannelContainer::const_iterator it_coll_end = cont->end();

  std::map<uint32_t, TileROD_Encoder> mapEncoder;

  int nch = 0;
  ATH_MSG_DEBUG( " number of collections " << cont->size() );
  for (; it_coll != it_coll_end; ++it_coll) {
    const TileRawChannelCollection* coll = (*it_coll);

    // the same ROD id for all channels in collection
    TileRawChannelCollection::ID frag_id = coll->identify();
    uint32_t reid = m_hid2re.getRodID(frag_id);
    mapEncoder[reid].setTileHWID(m_tileHWID, m_verbose, 4);
    mapEncoder[reid].setTypeAndUnit(contType, outputUnit);

    HWIdentifier drawer_id = m_tileHWID->drawer_id(frag_id);
    int ros = m_tileHWID->ros(drawer_id);
    int drawer = m_tileHWID->drawer(drawer_id);
    int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

    TileRawChannelCollection::const_iterator it_b = coll->begin();
    TileRawChannelCollection::const_iterator it_e = coll->end();

    int n = 0;
    for (; it_b != it_e; ++it_b) {
      const TileRawChannel* rawChan = *it_b;
      HWIdentifier adc_id = rawChan->adc_HWID();
      int channel = m_tileHWID->channel(adc_id);
      int adc = m_tileHWID->adc(adc_id);
      float ampl = rawChan->amplitude();
      float time = rawChan->time();
      float qual = rawChan->quality();
      if (oflCont) {
        if (qual > 15.0) qual = 15.0;
        if (m_tileBadChanTool->getAdcStatus(drawerIdx, channel, adc).isBad()) qual += 16.;
      }
      //ampl = m_tileToolEmscale->channelCalib(drawerIdx, channel, adc, ampl, inputUnit, outputUnit);
      m_channels[nch].set(frag_id, channel, adc, ampl, time, qual);
      mapEncoder[reid].add(&m_channels[nch]);
      ++nch;
      ++n;
    }

    ATH_MSG_DEBUG( " collection " << MSG::hex << "0x" << frag_id
                  << " ROD " << "0x" << reid
                  << " number of channels " << MSG::dec << n );
  }

  std::map<uint32_t, TileROD_Encoder>::iterator it = mapEncoder.begin();
  std::map<uint32_t, TileROD_Encoder>::iterator it_end = mapEncoder.end();

  TileROD_Encoder* theEncoder;

  // TileROD_Encoder has collected all the channels, now can fill the
  // ROD block data.

  for (; it != it_end; ++it) {
    theROD = fea->getRodData((*it).first);
    theEncoder = &((*it).second);
    if (m_doFragType4) theEncoder->fillROD4(*theROD);
    if (m_doFragType5) theEncoder->fillROD5(*theROD);
  }

  return StatusCode::SUCCESS;
}

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CscRDO_Decoder.h"

#include "GaudiKernel/ListItem.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonRDO/CscRawData.h"

/** constructor */
Muon::CscRDO_Decoder::CscRDO_Decoder(const std::string& type, const std::string& name, const IInterface* parent) :
    base_class(type, name, parent) {}

StatusCode Muon::CscRDO_Decoder::initialize() {
    ATH_MSG_DEBUG("CscRDO_Decoder::initialize");
    ATH_CHECK(m_cabling.retrieve());
    ATH_CHECK(m_cscCalibTool.retrieve());
    ATH_MSG_DEBUG(" Parameters are from CscCalibTool ");
    m_samplingTime = m_cscCalibTool->getSamplingTime();
    m_signalWidth = m_cscCalibTool->getSignalWidth();
    m_timeOffset = m_cscCalibTool->getTimeOffset();
    ATH_MSG_DEBUG(" Initialization is done!");
    return StatusCode::SUCCESS;
}

void Muon::CscRDO_Decoder::getDigit(const CscRawData* rawData, const CscIdHelper* cscIdHelper, Identifier& moduleId, Identifier& channelId,
                                    double& adc, double& time) const {
    // get the raw data
    uint32_t address = rawData->address();

    // initialize some parameters
    CscRODReadOut rodReadOut;
    rodReadOut.set(cscIdHelper);
    rodReadOut.setChamberBitVaue(1);
    rodReadOut.setParams(m_timeOffset, m_signalWidth);

    adc = rodReadOut.findCharge(m_samplingTime, rawData->samples(), time);

    // now decode the encoded fragments
    // find the Identifier and charge
    moduleId = rodReadOut.decodeAddress(address);
    channelId = rodReadOut.decodeAddress(address, moduleId);
}

Identifier Muon::CscRDO_Decoder::stationIdentifier(const CscRawData* rawData, const CscIdHelper* cscIdHelper) const {
    /** get the raw data */
    uint32_t address = rawData->address();

    // initialize some parameters
    CscRODReadOut rodReadOut;
    rodReadOut.set(cscIdHelper);
    rodReadOut.setChamberBitVaue(1);
    rodReadOut.setParams(m_timeOffset, m_signalWidth);

    /** now decode the endcoded fragments find the Identifiers */
    return rodReadOut.decodeAddress(address);
}

Identifier Muon::CscRDO_Decoder::channelIdentifier(const CscRawData* rawData, const CscIdHelper* cscIdHelper, int j) const {
    /** get the raw data */
    uint32_t address = rawData->address();

    // initialize some parameters
    CscRODReadOut rodReadOut;
    rodReadOut.set(cscIdHelper);
    rodReadOut.setChamberBitVaue(1);
    rodReadOut.setParams(m_timeOffset, m_signalWidth);

    /** now decode the endcoded fragments find the Identifiers */
    Identifier moduleId = rodReadOut.decodeAddress(address);

    ATH_MSG_DEBUG(" CscRDO_Decoder OUTPUT ::: " << m_timeOffset << "  " << m_samplingTime << " " << m_signalWidth << " "
                                                << "  " << m_detdescr << "  " << address << "   " << moduleId << " " << j);

    return rodReadOut.decodeAddress(address, moduleId, j);
}

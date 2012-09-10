/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprLut.h"

L1CaloPprLut::L1CaloPprLut():
m_channelId(0),
m_lutStrategy(0),
m_lutOffset(0),
m_lutSlope(0),
m_lutNoiseCut(0),
m_lutParameters(0),
m_pedValue(0),
m_pedMean(0.)
{}


L1CaloPprLut::L1CaloPprLut(unsigned int channelId, unsigned short lutStrategy, unsigned short lutOffset, unsigned short lutSlope, unsigned short lutNoiseCut, coral::Blob lutParameters, unsigned int pedValue, double pedMean, ChanCalibErrorCode errorCode):
m_channelId(channelId),
m_lutStrategy(lutStrategy),
m_lutOffset(lutOffset),
m_lutSlope(lutSlope),
m_lutNoiseCut(lutNoiseCut),
m_lutParameters(lutParameters),
m_pedValue(pedValue),
m_pedMean(pedMean),
m_errorCode(errorCode)
{}


L1CaloPprLut::L1CaloPprLut(const L1CaloCoolChannelId& channelId, unsigned short lutStrategy, unsigned short lutOffset, unsigned short lutSlope, unsigned short lutNoiseCut, coral::Blob lutParameters, unsigned int pedValue, double pedMean, ChanCalibErrorCode errorCode):
m_channelId(channelId),
m_lutStrategy(lutStrategy),
m_lutOffset(lutOffset),
m_lutSlope(lutSlope),
m_lutNoiseCut(lutNoiseCut),
m_lutParameters(lutParameters),
m_pedValue(pedValue),
m_pedMean(pedMean),
m_errorCode(errorCode)
{}

std::ostream& operator<<(std::ostream& output, const L1CaloPprLut& ppmLut) {
    output << "channelId: "<< std::hex << ppmLut.m_channelId.id()<< std::dec
	   << ", lutStrategy: " << (int) ppmLut.m_lutStrategy
	   << ", lutOffset: " << ppmLut.m_lutOffset
	   << ", lutSlope: " << ppmLut.m_lutSlope
	   << ", lutNoiseCut: " << ppmLut.m_lutNoiseCut
	   << ", pedValue: " << ppmLut.m_pedValue
	   << ", pedMean: " << ppmLut.m_pedMean
	   <<", errorCode: " << ppmLut.m_errorCode
	   << ", chanValid: " << ppmLut.m_errorCode.chanValid();
    return output;
}

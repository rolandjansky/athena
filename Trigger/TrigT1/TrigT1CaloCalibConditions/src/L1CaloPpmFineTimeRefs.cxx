/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPpmFineTimeRefs.h"

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

L1CaloPpmFineTimeRefs::L1CaloPpmFineTimeRefs():
	m_channelId(0),
	m_refValue(0),
	m_criterion(0),
	m_calibValue(0)
{
}

L1CaloPpmFineTimeRefs::L1CaloPpmFineTimeRefs( const L1CaloCoolChannelId& channelId):
	m_channelId(channelId),
	m_refValue(0),
	m_criterion(0),
	m_calibValue(0)
{
}

L1CaloPpmFineTimeRefs::L1CaloPpmFineTimeRefs( unsigned int channelId, FineTimeErrorCode errorCode, double refValue, double criterion, double calibValue):
	m_channelId(channelId),
	m_errorCode(errorCode),
	m_refValue(refValue),
	m_criterion(criterion),
	m_calibValue(calibValue)
{
}

L1CaloPpmFineTimeRefs::L1CaloPpmFineTimeRefs( const L1CaloCoolChannelId& channelId, FineTimeErrorCode errorCode, double refValue, double criterion, double calibValue):
	m_channelId(channelId),
	m_errorCode(errorCode),
	m_refValue(refValue),
	m_criterion(criterion),
	m_calibValue(calibValue)
{
}

std::ostream& operator<<(std::ostream& output, const  L1CaloPpmFineTimeRefs& r) {
	output << "channelID: " << std::hex << r.m_channelId.id() << std::dec << ", " << "errorCode: " << r.m_errorCode<< ", " << "refValue: " << r.m_refValue << ", " << "criterion: " << r.m_criterion<< ", " << "calibValue: " << r.m_calibValue ;
    return output;
}

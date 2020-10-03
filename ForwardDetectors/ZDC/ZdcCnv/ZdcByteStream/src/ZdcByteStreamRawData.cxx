/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZdcByteStreamRawData.cxx
 *
 *  Created on: Jun 29, 2009
 *      Author: leite
 *
 *  Based on Athena Algorithm this class must provide 3 methods:
 *
 *  ZdcByteStreamRawData::initialize   (run once at run start)
 *  ZdcByteStreamRawData::execute      (run for every event)
 *  ZdcByteStreamRawData::finalize     (run one at run end)
 *
 *  plus some helpers
 */

#include <utility>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"

#include "ZdcEvent/ZdcDigits.h"
#include "ZdcEvent/ZdcDigitsCollection.h"

#include "ZdcByteStream/ZdcDefs.h"
#include "ZdcByteStream/ZdcByteStreamRawData.h"

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

//==================================================================================================
ZdcByteStreamRawData::ZdcByteStreamRawData(const std::string& name, ISvcLocator* pSvcLocator) :
	AthAlgorithm(name, pSvcLocator)
{
	declareProperty("ZdcDigitsCollectionLocation", m_ZdcDigitsCollectionLocation = ZdcDefs::ZdcDigitsCollectionLocation);
	declareProperty("ForceSlicesLUT", m_forceSlicesLut = 0);
	declareProperty("ForceSlicesFADC", m_forceSlicesFadc = 0);
}
//==================================================================================================


//==================================================================================================
ZdcByteStreamRawData::~ZdcByteStreamRawData()
{
}
//==================================================================================================


//==================================================================================================
StatusCode ZdcByteStreamRawData::initialize()
{
	msg(MSG::INFO) << "Initializing " << name() << " - package version " << PACKAGE_VERSION
			<< endmsg;

	return StatusCode::SUCCESS;
}
//==================================================================================================


//==================================================================================================
StatusCode ZdcByteStreamRawData::execute()
{
	if (!msgLvl(MSG::INFO)) return StatusCode::SUCCESS;
	msg(MSG::INFO);

	/// The collection must be already somewhere
	/// otherwise just skip this
	/// TODO keep record somewhere of this type of error?

	const ZdcDigitsCollection* ttCollection = nullptr;

	msg(MSG::DEBUG) << "Looking for ZDC Digits Collection at " << m_ZdcDigitsCollectionLocation << endmsg;
	//std::cout << "Looking for ZDC Digits Collection at " << m_ZdcDigitsCollectionLocation << std::endl;
	StatusCode sc = evtStore()->retrieve(ttCollection, m_ZdcDigitsCollectionLocation);
	if (sc.isFailure() || !ttCollection || ttCollection->empty())
	{
		msg() << "No Zdc Digits found" << endmsg;
		return StatusCode::SUCCESS;
	}

	return StatusCode::SUCCESS;
}
//==================================================================================================


//==================================================================================================
StatusCode ZdcByteStreamRawData::finalize()
{

	return StatusCode::SUCCESS;
}
//==================================================================================================


//==================================================================================================
//FIXME There is no such thing like ZdcDigitsMap
void ZdcByteStreamRawData::printZdcDigits() const
{
	msg() << "Number of ZdcDigits = " << m_ZdcDigitsMap.size() << endmsg;
// 	ZdcDigitsMap::const_iterator mapIter = m_ZdcDigitsMap.begin();
// 	ZdcDigitsMap::const_iterator mapEnd = m_ZdcDigitsMap.end();
}
//==================================================================================================


//==================================================================================================
void ZdcByteStreamRawData::printVec(const std::vector<int>& vec) const
{
	std::vector<int>::const_iterator pos;
	for (pos = vec.begin(); pos != vec.end(); ++pos)
	{
		if (pos != vec.begin()) msg() << ",";
		msg() << *pos;
	}
	msg() << "/";
}
//==================================================================================================

/* REMOVE

 // Set up trigger tower map
 void ZdcByteStreamRawData::setupZdcDigitsMap(const ZdcDigitsCollection* const ttCollection)
 {
 m_ZdcDigitsMap.clear();
 ZdcDigitsCollection::const_iterator pos  = ttCollection->begin();
 ZdcDigitsCollection::const_iterator pose = ttCollection->end();
 for (; pos != pose; ++pos) {
 const ZdcDigits* const tt = *pos;
 tt->get_ID();

 //const unsigned int key = m_towerKey->ttKey(tt->phi(), tt->eta());
 //m_ttMap.insert(std::make_pair(key, tt));
 }
 }
 */

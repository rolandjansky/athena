/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZdcByteStreamTester.cxx
 *
 *  Created on: Jun 29, 2009
 *      Author: leite
 *
 *  Based on Athena Algorithm this class must provide 3 methods:
 *
 *  ZdcByteStreamTester::initialize   (run once at run start)
 *  ZdcByteStreamTester::execute      (run for every event)
 *  ZdcByteStreamTester::finalize     (run one at run end)
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
#include "ZdcByteStream/ZdcByteStreamTester.h"

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

//==================================================================================================
ZdcByteStreamTester::ZdcByteStreamTester(const std::string& name, ISvcLocator* pSvcLocator) :
	AthAlgorithm(name, pSvcLocator)
{
	declareProperty("ZdcDigitsLocation", m_ZdcDigitsLocation = ZdcDefs::ZdcDigitsLocation);
	declareProperty("ForceSlicesLUT", m_forceSlicesLut = 0);
	declareProperty("ForceSlicesFADC", m_forceSlicesFadc = 0);
}
//==================================================================================================


//==================================================================================================
ZdcByteStreamTester::~ZdcByteStreamTester()
{
}
//==================================================================================================


//==================================================================================================
StatusCode ZdcByteStreamTester::initialize()
{
	msg(MSG::INFO) << "Initializing " << name() << " - package version " << PACKAGE_VERSION
			<< endmsg;

	return StatusCode::SUCCESS;
}
//==================================================================================================


//==================================================================================================
StatusCode ZdcByteStreamTester::execute()
{
	if (!msgLvl(MSG::INFO)) return StatusCode::SUCCESS;
	msg(MSG::INFO);

	/// The collection must be already somewhere
	/// otherwise just skip this
	/// TODO keep record somewhere of this type of error?

	const ZdcDigitsCollection* ttCollection = nullptr;
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
StatusCode ZdcByteStreamTester::finalize()
{

	return StatusCode::SUCCESS;
}
//==================================================================================================


//==================================================================================================
//FIXME There is no such thing like ZdcDigitsMap
void ZdcByteStreamTester::printZdcDigits() const
{
	msg() << "Number of ZdcDigits = " << m_ZdcDigitsMap.size() << endmsg;
// 	ZdcDigitsMap::const_iterator mapIter = m_ZdcDigitsMap.begin();
// 	ZdcDigitsMap::const_iterator mapEnd = m_ZdcDigitsMap.end();
}
//==================================================================================================


//==================================================================================================
void ZdcByteStreamTester::printVec(const std::vector<int>& vec) const
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
 void ZdcByteStreamTester::setupZdcDigitsMap(const ZdcDigitsCollection* const ttCollection)
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

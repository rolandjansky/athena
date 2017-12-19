/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZdcRecChannel.cxx
 *
 *  Created on: Nov 24, 2009
 *      Author: leite
 */

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
//#include "Identifier/Identifier.h"

#include "ZdcEvent/ZdcDigitsCollection.h"
#include "ZdcEvent/ZdcCalibEvent.h"
#include "ZdcRec/ZdcRec.h"
#include "ZdcRec/ZdcRecChannelTool.h"
#include "ZdcRec/ZdcRecNoiseTool.h"

//==================================================================================================
ZdcRec::ZdcRec(const std::string& name, ISvcLocator* pSvcLocator) :

	AthAlgorithm(name, pSvcLocator),
	m_storeGate("StoreGateSvc", name),
	m_ownPolicy(static_cast<int> (SG::OWN_ELEMENTS)),
	m_digitsContainerName("ZdcDigitsCollection"),
	m_rawContainerName("ZdcRawChannelCollection"),
	m_digitsCollection(0),
	m_rawCollection(0),
	m_eventCount(0),
	m_complainContain(1),
	m_complainRetrieve(1),
        m_ChannelTool("ZdcRecChannelTool"),
        m_NoiseTool("ZdcRecNoiseTool")

{
	declareProperty("OwnPolicy",m_ownPolicy) ;

	declareProperty("DigitsContainerName", m_digitsContainerName, "ZdcDigitsCollection");

	declareProperty("RawContainerName",    m_rawContainerName,    "ZdcRawChannelCollection");
}
//==================================================================================================

//==================================================================================================
ZdcRec::~ZdcRec() {}
//==================================================================================================

//==================================================================================================
StatusCode ZdcRec::initialize()
{
	MsgStream mLog(msgSvc(), name());

	// Look up the Storegate service
	StatusCode sc = m_storeGate.retrieve();
	if (sc.isFailure())
	{
		mLog << MSG::FATAL << "--> ZDC: Unable to retrieve pointer to StoreGateSvc" << endmsg;
		return sc;
	}


	// Reconstruction Tool
	StatusCode scTool = m_ChannelTool.retrieve();
	if (scTool.isFailure())
	{
		mLog << MSG::WARNING << "--> ZDC: Could not retrieve " << m_ChannelTool << endmsg;
		return StatusCode::FAILURE;
	}
	mLog << MSG::INFO << "--> ZDC: SUCCESS retrieving " << m_ChannelTool << endmsg;

	// Noise and Pedestal Tool
	scTool = m_NoiseTool.retrieve();
	if (scTool.isFailure())
	{
		mLog << MSG::WARNING << "--> ZDC: Could not retrieve " << m_NoiseTool << endmsg;
		return StatusCode::FAILURE;
	}
	mLog << MSG::INFO << "--> ZDC: SUCCESS retrieving " << m_NoiseTool << endmsg;

	// Container output name
	//TODO: change MESSAGE !!
	mLog << MSG::INFO << " Output Container Name " << m_rawContainerName << endmsg;
	if (m_ownPolicy == SG::OWN_ELEMENTS)
		mLog << MSG::INFO << "...will OWN its cells." << endmsg;
	else
		mLog << MSG::INFO << "...will VIEW its cells." << endmsg;


	mLog << MSG::INFO  << "--> ZDC: ZdcRec initialization complete" << endmsg;

	return StatusCode::SUCCESS;
}
//==================================================================================================

//==================================================================================================
StatusCode ZdcRec::execute()
{

  ZdcRawChannelCollection::iterator iter;
  int ncha = 0;

  MsgStream mLog(msgSvc(), name());
	mLog << MSG::DEBUG
	     << "--> ZDC: ZdcRec execute starting on "
	     << m_eventCount
	     << "th event"
		 << endmsg;

	m_eventCount++;

	//Look for the container presence
	bool dg = m_storeGate->contains<ZdcDigitsCollection>( m_digitsContainerName);
	if (!dg) {
	  if (m_complainContain) mLog << MSG::WARNING << "--> ZDC: StoreGate does not contain " << m_digitsContainerName << endmsg;
	  m_complainContain = 0;
	  return StatusCode::SUCCESS;
	}

	// Look up the Digits "ZdcDigitsCollection" in Storegate
	StatusCode digitsLookupSC = m_storeGate->retrieve(m_digitsCollection, m_digitsContainerName);
	if (digitsLookupSC.isFailure())
	{
	  if (m_complainRetrieve)
	    mLog << MSG::WARNING
		 << "--> ZDC: Could not retrieve "
		 << m_digitsContainerName
		 << " from StoreGate"
		 << endmsg;
	  m_complainRetrieve = 0;
	  return StatusCode::SUCCESS;
	}

	if (digitsLookupSC.isSuccess() && !m_digitsCollection)
	{
		mLog << MSG::ERROR
			 << "--> ZDC: Storegate returned zero pointer for "
			 << m_digitsContainerName
			 << endmsg;
		return StatusCode::SUCCESS;
	}

	//Create the containers to hold the reconstructed information
	m_rawCollection = new ZdcRawChannelCollection (static_cast<SG::OwnershipPolicy>(m_ownPolicy));
	ZdcRawChannelCollection *tmpCollection  = new ZdcRawChannelCollection (static_cast<SG::OwnershipPolicy>(SG::VIEW_ELEMENTS));

	ncha = m_ChannelTool->makeRawFromDigits(*m_digitsCollection, tmpCollection);


	// SG has the ownership of m_rawCollection, and it should be copyed intead of just
	// being passed around.
	for (iter=tmpCollection->begin();iter!=tmpCollection->end();iter++) {
	    m_rawCollection->push_back(*iter);
	}

	StatusCode sc = evtStore()->record(m_rawCollection, m_rawContainerName,false);

	mLog << MSG::DEBUG
		 << "--> ZDC: ZdcRawChannelCollection size recorded into SG: "
		 << m_rawCollection->size()
		 << ".. and from tool " << ncha
		 << endmsg;

	// For Debugging only
	/*
	ZdcRawChannel *z;
	int i  = 0;
	int ss = 0;
    for (iter=m_rawCollection->begin();iter!=m_rawCollection->end();iter++) {
        i++;
        z = *iter;
        ss = z->getSize();
        for (int cc = 0; cc < ss ; cc++) {
            std::cout << "...." << i << "  " << "  " << cc << "E = " <<  z->getEnergy(cc) << " T =  " << z->getTime(cc);
            }
        std::cout << std::endl;
    }
	*/


	if (sc.isFailure())
	{
		mLog << MSG::WARNING
				<< "execute() : cannot record Container "
				<< m_rawContainerName
				<< endmsg;

		return StatusCode::SUCCESS;
	}

	if (tmpCollection != NULL) {
		mLog << MSG::DEBUG
				<< "--> ZDC: Deleting tmpCollection"
				<< endmsg;
		delete tmpCollection;
	}

	return StatusCode::SUCCESS;

}
//==================================================================================================

//==================================================================================================
StatusCode ZdcRec::finalize()
{
  MsgStream mLog(msgSvc(),name());

  mLog << MSG::INFO
		  << "--> ZDC: ZdcRec finalize complete"
		  << endmsg;
  return StatusCode::SUCCESS;
}
//==================================================================================================


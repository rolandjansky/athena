/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArConditionsContainerBase.cxx
 *
 * @brief This file contain an implementation base class for
 * LArConditionsContainer 
 *
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 *
 * $Id: LArConditionsContainerBase.cxx,v 1.14 2008-10-03 17:46:39 schaffer Exp $
 */

#include "LArRawConditions/LArConditionsContainerBase.h"

// Services/helpers
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloCell_ID.h"

// Gaudi/Athena
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "AthenaKernel/getMessageSvc.h"
#include <atomic>

LArConditionsContainerBase::LArConditionsContainerBase( )
	:
	m_groupType(SubDetectorGrouping),
	m_onlineHelper(0),
	m_offlineHelper(0),
	m_isInitialized(false)
{}

LArConditionsContainerBase::LArConditionsContainerBase(GroupingType type)
	:
	m_groupType(type),
	m_onlineHelper(0),
	m_offlineHelper(0),
	m_isInitialized(false)
{}


LArConditionsContainerBase::~LArConditionsContainerBase()
{}



StatusCode 
LArConditionsContainerBase::initializeBase()
{
    MsgStream log(Athena::getMessageSvc(), "LArConditionsContainerBase");
    log << MSG::DEBUG << "initializeBase "<< endmsg;

    if (m_isInitialized) {
	log << MSG::DEBUG << "already initialized - returning "<< endmsg;
	return (StatusCode::SUCCESS);
    }
 
    //Get LArOnlineID....
    ISvcLocator* svcLoc = Gaudi::svcLocator( );
    StoreGateSvc* detStore;
    StatusCode sc = svcLoc->service("DetectorStore",detStore);
    if (sc.isFailure()) {
	log << MSG::ERROR << "Cannot get DetectorStore!" << endmsg;
	return sc;
    }
    sc = detStore->retrieve(m_onlineHelper,"LArOnlineID");
    if (sc.isFailure()) {
	log << MSG::ERROR << "Cannot get LArOnlineID!" << endmsg;
	return sc;
    }
    sc = detStore->retrieve(m_offlineHelper,"CaloCell_ID");
    if (sc.isFailure()) {
	log << MSG::ERROR << "Cannot get CaloCell_ID!" << endmsg;
	return sc;
    }

    // initialize the grouping
    sc = initGrouping();
    if (sc.isFailure()) {
	log << MSG::ERROR << "Cannot initialize the Grouping" << endmsg;
	return sc;
    }

    // Set initialized to true
    m_isInitialized = true;
    log << MSG::DEBUG << "end initializeBase " << endmsg;

    return (StatusCode::SUCCESS);
}

void
LArConditionsContainerBase::setGroupingType(GroupingType type)
{
    m_groupType = type;
}


StatusCode 
LArConditionsContainerBase::setGroupingType(const std::string& groupingStr, MsgStream& logStr)
{
 // Set grouping type - default is SubDet
  if (groupingStr == "Single") 
    {setGroupingType(LArConditionsContainerBase:: SingleGroup);
     return StatusCode::SUCCESS;
    } 
  else if (groupingStr == "SubDetector") 
    {setGroupingType(LArConditionsContainerBase:: SubDetectorGrouping);
     return StatusCode::SUCCESS;
    } 
  else if (groupingStr == "ExtendedSubDetector") 
    {setGroupingType(LArConditionsContainerBase::ExtendedSubDetGrouping);
    return StatusCode::SUCCESS;
    } 
  else if (groupingStr == "FeedThrough") 
    {setGroupingType(LArConditionsContainerBase:: FeedThroughGrouping);
     return StatusCode::SUCCESS;
    } 
  else if (groupingStr == "ExtendedFeedThrough") {
    setGroupingType(LArConditionsContainerBase:: ExtendedFTGrouping);
    return StatusCode::SUCCESS;
  }

  logStr << MSG::ERROR << "Unknown COOL Channel Grouping. Allowed values are: \n" << endmsg;
  logStr << MSG::ERROR << "'Single','SubDetector', 'ExtendedSubDetector','FeedThrough','ExtendedFeedThrough'" << endmsg;
  return StatusCode::FAILURE;
}


StatusCode
LArConditionsContainerBase::initGrouping()
{
    MsgStream log(Athena::getMessageSvc(), "LArConditionsContainerBase");
    log << MSG::DEBUG << "initGrouping "<< endmsg;

    // We allow for different groupings

    if (Unknown == m_groupType) {
	log << MSG::ERROR << "Unknown grouping "<< endmsg;

	// Not yet known
    }
    if (SingleGroup == m_groupType) {

	log << MSG::DEBUG << "Single group "<< endmsg;

	// Only one group - add all feb ids to channel 0
	std::vector<HWIdentifier>::const_iterator febIt  = m_onlineHelper->feb_begin();
	std::vector<HWIdentifier>::const_iterator febEnd = m_onlineHelper->feb_end();
	LArCondFEBIdChanMap::FEBIdVector febIds; 
	for (; febIt != febEnd; ++febIt) {
	    febIds.push_back((*febIt).get_identifier32().get_compact());
	}
	// add for LArCondFEBIdChanMap
	unsigned int nChanPerGain = 1;
	unsigned int minGain      = 0;
	unsigned int nGains       = 3;
	m_febIdChanMap = LArCondFEBIdChanMap(nChanPerGain, minGain, nGains);
	m_febIdChanMap.addFEBIdVector(0, febIds);
    }
    else if (SubDetectorGrouping == m_groupType) {

	log << MSG::DEBUG << "Subdetector  grouping "<< endmsg;

	// Set up LArCondFEBIdChanMap for four groups: EM barrel, EMEM, HEC, FCal 
	std::vector<HWIdentifier>::const_iterator febIt  = m_onlineHelper->feb_begin();
	std::vector<HWIdentifier>::const_iterator febEnd = m_onlineHelper->feb_end();

	LArCondFEBIdChanMap::FEBIdVector emBarrel; 
	LArCondFEBIdChanMap::FEBIdVector emEndcap; 
	LArCondFEBIdChanMap::FEBIdVector hec; 
	LArCondFEBIdChanMap::FEBIdVector fcal; 

	for (; febIt != febEnd; ++febIt) {
            unsigned int febId = (*febIt).get_identifier32().get_compact();
	    if (m_onlineHelper->isEMBchannel(*febIt)) {
		emBarrel.push_back(febId);
	    }
	    else if (m_onlineHelper->isEMECchannel(*febIt)) {
		emEndcap.push_back(febId);
	    }
	    else if (m_onlineHelper->isHECchannel(*febIt)) {
		hec.push_back(febId);
	    }
	    else if (m_onlineHelper->isFCALchannel(*febIt)) {
		fcal.push_back(febId);
	    }
	    else {
		    log << MSG::DEBUG << "Uassociated FEB id: "
			<< MSG::hex << (febId) << MSG::dec 
			<< " isCalib " << m_onlineHelper->isCalibration(*febIt)
			<< "  " << m_onlineHelper->show_to_string(*febIt)
			<< endmsg;
	    }
	}
	// add for LArCondFEBIdChanMap
	unsigned int nChanPerGain = 4;
	unsigned int minGain      = 0;
	unsigned int nGains       = 3;
	m_febIdChanMap = LArCondFEBIdChanMap(nChanPerGain, minGain, nGains);
	m_febIdChanMap.addFEBIdVector(0, emBarrel);
	m_febIdChanMap.addFEBIdVector(1, emEndcap);
	m_febIdChanMap.addFEBIdVector(2, hec);
	m_febIdChanMap.addFEBIdVector(3, fcal);

// 	unsigned int coolChannel;
// 	for (unsigned int gain = 0; gain < nGains; ++gain) {
// 	    for (unsigned int i = 0; i < emBarrel.size(); ++i) {
// 		if (!m_febIdChanMap.getChannel(emBarrel[i], gain, coolChannel)) {
// 		    log << MSG::DEBUG << "Unable to get cool channel for em barrel - i = "
// 			<< i << endmsg;
// 		    return (StatusCode::FAILURE);
// 		}
// 	    }
// 	    for (unsigned int i = 0; i < emEndcap.size(); ++i) {
// 		if (!m_febIdChanMap.getChannel(emEndcap[i], gain, coolChannel)) {
// 		    log << MSG::DEBUG << "Unable to get cool channel for em endcap - i = "
// 			<< i << endmsg;
// 		    return (StatusCode::FAILURE);
// 		}
// 	    }
// 	    for (unsigned int i = 0; i < hec.size(); ++i) {
// 		if (!m_febIdChanMap.getChannel(hec[i], gain, coolChannel)) {
// 		    log << MSG::DEBUG << "Unable to get cool channel for hec - i = "
// 			<< i << endmsg;
// 		    return (StatusCode::FAILURE);
// 		}
// 	    }
// 	    for (unsigned int i = 0; i < fcal.size(); ++i) {
// 		if (!m_febIdChanMap.getChannel(fcal[i], gain, coolChannel)) {
// 		    log << MSG::DEBUG << "Unable to get cool channel for fcal - i = "
// 			<< i << endmsg;
// 		    return (StatusCode::FAILURE);
// 		}
// 	    }
//      }
    }
    else if (ExtendedSubDetGrouping == m_groupType) {
      	log << MSG::DEBUG << "Extended Subdetector  grouping "<< endmsg;

	// Set up LArCondFEBIdChanMap for four groups: EM barrel, EM barrel PS ,EMEC EMEC PS, HEC, FCal 
	std::vector<HWIdentifier>::const_iterator febIt  = m_onlineHelper->feb_begin();
	std::vector<HWIdentifier>::const_iterator febEnd = m_onlineHelper->feb_end();

	LArCondFEBIdChanMap::FEBIdVector emBarrel[2];
 	LArCondFEBIdChanMap::FEBIdVector emBarrelPS[2];
	LArCondFEBIdChanMap::FEBIdVector emEndcap[2]; 
	LArCondFEBIdChanMap::FEBIdVector emEndcapPS[2]; 
	LArCondFEBIdChanMap::FEBIdVector hec[2]; 
	LArCondFEBIdChanMap::FEBIdVector fcal[2]; 

	for (; febIt != febEnd; ++febIt) {
            int iside = m_onlineHelper->pos_neg(*febIt);
            // Sanity check
            if(iside <0 || iside > 1) {
	      log << MSG::ERROR << "Wrong side id: "<< iside <<" from: "  
		  << MSG::hex << *febIt << MSG::dec << endmsg;
	      return (StatusCode::FAILURE);
	    }
            unsigned int febId = (*febIt).get_identifier32().get_compact();
	    if (m_onlineHelper->isEMBPS(*febIt)) {
	      emBarrelPS[iside].push_back(febId);
	    }
	    else if (m_onlineHelper->isEMECPS(*febIt)) {
	      emEndcapPS[iside].push_back(febId);
	    }
	    else if (m_onlineHelper->isEMBchannel(*febIt)) {
	      emBarrel[iside].push_back(febId);
	    }
	    else if (m_onlineHelper->isEMECchannel(*febIt)) {
	      emEndcap[iside].push_back(febId);
	    }
	    else if (m_onlineHelper->isHECchannel(*febIt)) {
	      hec[iside].push_back(febId);
	    }
	    else if (m_onlineHelper->isFCALchannel(*febIt)) {
	      fcal[iside].push_back(febId);
	    }
	    else {
	      log << MSG::DEBUG << "Uassociated FEB id: "
		  << MSG::hex << (febId) << MSG::dec 
		  << " isCalib " << m_onlineHelper->isCalibration(*febIt)
		  << "  " << m_onlineHelper->show_to_string(*febIt)
		  << endmsg;
	    }
	}
	// add for LArCondFEBIdChanMap
	unsigned int nChanPerGain = 12;
	unsigned int minGain      = 0;
	unsigned int nGains       = 3;
	m_febIdChanMap = LArCondFEBIdChanMap(nChanPerGain, minGain, nGains);
        for (int iside=0; iside<2; ++iside) {
	  m_febIdChanMap.addFEBIdVector(0+iside*6, emBarrel[iside]);
	  m_febIdChanMap.addFEBIdVector(1+iside*6, emBarrelPS[iside]);
	  m_febIdChanMap.addFEBIdVector(2+iside*6, emEndcap[iside]);
	  m_febIdChanMap.addFEBIdVector(3+iside*6, emEndcapPS[iside]);
	  m_febIdChanMap.addFEBIdVector(4+iside*6, hec[iside]);
	  m_febIdChanMap.addFEBIdVector(5+iside*6, fcal[iside]);
        }

    }
    else if (FeedThroughGrouping == m_groupType) {
        log << MSG::DEBUG << "FeedThrough  grouping "<< endmsg;

        // Set up LArCondFEBIdChanMap for four groups: EM barrel, EMEM, HEC, FCal
        std::vector<HWIdentifier>::const_iterator febIt  = m_onlineHelper->feb_begin();
        std::vector<HWIdentifier>::const_iterator febEnd = m_onlineHelper->feb_end();

        std::vector<LArCondFEBIdChanMap::FEBIdVector> ft(m_onlineHelper->feedthroughHashMax());


        for (; febIt != febEnd; ++febIt) {
            // Find the feedthru hash and add to vector
            HWIdentifier febId = (*febIt);
            int b_ec = m_onlineHelper->barrel_ec(febId);
            int p_n  = m_onlineHelper->pos_neg(febId);
            int f_t  = m_onlineHelper->feedthrough(febId);
            HWIdentifier ftid   = m_onlineHelper->feedthrough_Id(b_ec, p_n, f_t);
            IdentifierHash fthash = m_onlineHelper->feedthrough_Hash(ftid);
            if (fthash < ft.size()) {
                ft[fthash].push_back(febId.get_identifier32().get_compact());
            }
            else {
                log << MSG::ERROR << "Feedthru hash > channel map size: id, hash, size "
                    << MSG::hex << febId << MSG::dec << " "
                    << ftid << " " << ft.size()
                    << endmsg;
                return (StatusCode::FAILURE);
            }
        }
        // add for LArCondFEBIdChanMap
        unsigned int nChanPerGain = ft.size();
        unsigned int minGain      = 0;
        unsigned int nGains       = 3;
        m_febIdChanMap = LArCondFEBIdChanMap(nChanPerGain, minGain, nGains);
        for (unsigned int i = 0; i < ft.size(); ++i) {
            m_febIdChanMap.addFEBIdVector(i, ft[i]);
        }
    }
    else if (ExtendedFTGrouping == m_groupType) {

	log << MSG::DEBUG << "Extended FeedThrough  grouping (PS goes seperatly, EMEC in HEC as well)"<< endmsg;

	// Set up LArCondFEBIdChanMap for four groups: EM barrel, EMEM, HEC, FCal 
	std::vector<HWIdentifier>::const_iterator febIt  = m_onlineHelper->feb_begin();
	std::vector<HWIdentifier>::const_iterator febEnd = m_onlineHelper->feb_end();

	//Vectors of groups
	//Standard Febs (one group per FT)
	std::vector<LArCondFEBIdChanMap::FEBIdVector> ft(m_onlineHelper->feedthroughHashMax());
	//Presampler FEBs (one group per FT)
	std::vector<LArCondFEBIdChanMap::FEBIdVector> ftPS(m_onlineHelper->feedthroughHashMax());
	//EMEC channel in special crate:
	std::vector<LArCondFEBIdChanMap::FEBIdVector> ftSpecial(8); //How many are there?

	for (; febIt != febEnd; ++febIt) {
	    // Find the feedthru hash and add to vector
	    HWIdentifier febId = (*febIt);
	    int b_ec = m_onlineHelper->barrel_ec(febId);
	    int p_n  = m_onlineHelper->pos_neg(febId);
	    int f_t  = m_onlineHelper->feedthrough(febId);
	    HWIdentifier ftid   = m_onlineHelper->feedthrough_Id(b_ec, p_n, f_t);
	    IdentifierHash fthash = m_onlineHelper->feedthrough_Hash(ftid);

	    //sanity check
	    if (fthash >= ft.size()) {
	      log << MSG::ERROR << "Feedthru hash > channel map size: id, hash, size " 
		  << MSG::hex << febId << MSG::dec << " "
		  << ftid << " " << ft.size() 
		  << endmsg;
	      return (StatusCode::FAILURE);
	    }

	    //Subdivide into specific cases:
	    if (!m_onlineHelper->isEMECinHECchannel(febId)) { //this is taken care of further down
	      if (m_onlineHelper->isPS(febId))
		ftPS[fthash].push_back(febId.get_identifier32().get_compact());  //PS goes in separate channels
	      else
		ft[fthash].push_back(febId.get_identifier32().get_compact());
	    }
	}
	//EMEC in HEC crate: Do 'by hand'
	//FTs: 3,10,16,22 Slots: 1,2
	//parameters: ec,pn,ft,slot
	ftSpecial[0].push_back(m_onlineHelper->feb_Id(1,0,3,1).get_identifier32().get_compact()); 
	ftSpecial[0].push_back(m_onlineHelper->feb_Id(1,0,3,2).get_identifier32().get_compact()); 

	ftSpecial[1].push_back(m_onlineHelper->feb_Id(1,1,3,1).get_identifier32().get_compact()); 
	ftSpecial[1].push_back(m_onlineHelper->feb_Id(1,1,3,2).get_identifier32().get_compact()); 
		
	ftSpecial[2].push_back(m_onlineHelper->feb_Id(1,0,10,1).get_identifier32().get_compact()); 
	ftSpecial[2].push_back(m_onlineHelper->feb_Id(1,0,10,2).get_identifier32().get_compact()); 

	ftSpecial[3].push_back(m_onlineHelper->feb_Id(1,1,10,1).get_identifier32().get_compact()); 
	ftSpecial[3].push_back(m_onlineHelper->feb_Id(1,1,10,2).get_identifier32().get_compact()); 

	ftSpecial[4].push_back(m_onlineHelper->feb_Id(1,0,16,1).get_identifier32().get_compact()); 
	ftSpecial[4].push_back(m_onlineHelper->feb_Id(1,0,16,2).get_identifier32().get_compact()); 

	ftSpecial[5].push_back(m_onlineHelper->feb_Id(1,1,16,1).get_identifier32().get_compact()); 
	ftSpecial[5].push_back(m_onlineHelper->feb_Id(1,1,16,2).get_identifier32().get_compact()); 

	ftSpecial[6].push_back(m_onlineHelper->feb_Id(1,0,22,1).get_identifier32().get_compact()); 
	ftSpecial[6].push_back(m_onlineHelper->feb_Id(1,0,22,2).get_identifier32().get_compact()); 

	ftSpecial[7].push_back(m_onlineHelper->feb_Id(1,1,22,1).get_identifier32().get_compact()); 
	ftSpecial[7].push_back(m_onlineHelper->feb_Id(1,1,22,2).get_identifier32().get_compact()); 


	// add for LArCondFEBIdChanMap
	unsigned int nChanPerGain = ft.size()+ftPS.size()+ftSpecial.size();
	unsigned int minGain      = 0;
	unsigned int nGains       = 3;
	unsigned int iCoolChannel=0;
	m_febIdChanMap = LArCondFEBIdChanMap(nChanPerGain, minGain, nGains);
	for (unsigned int i = 0; i < ft.size(); ++i) {
	    m_febIdChanMap.addFEBIdVector(iCoolChannel++, ft[i]);
	}
	for (unsigned int i = 0; i < ftPS.size(); ++i) {
	    m_febIdChanMap.addFEBIdVector(iCoolChannel++, ftPS[i]);
	}
	for (unsigned int i = 0; i < ftSpecial.size(); ++i) {
	    m_febIdChanMap.addFEBIdVector(iCoolChannel++, ftSpecial[i]);
	}
    } else {
      log << MSG::ERROR << "Unknown COOL Channel grouping " << m_groupType << endmsg;
      return StatusCode::FAILURE;
    }
    // Resize the vector mapping the COOL channel to the
    // DataVector. 
    m_channelToMultChanCollIndex = std::vector<unsigned int>(m_febIdChanMap.totalChannels(), 9999);

    log << MSG::DEBUG << "Number of channels per gain " 
	<< m_febIdChanMap.channelsPerGain()
	<< endmsg;

    log << MSG::DEBUG << "Mininum gain value          "
	<< m_febIdChanMap.minGain() 
	<< endmsg;
    
    log << MSG::DEBUG << "Number of gain values       "
	<< m_febIdChanMap.nGains()
	<< endmsg;

    log << MSG::DEBUG << "Number of offset channels   "
	<< m_febIdChanMap.nOffsetChannels() 
	<< endmsg;

    log << MSG::DEBUG << "Total number of channels    "
	<< m_febIdChanMap.totalChannels()
	<< endmsg;

    return (StatusCode::SUCCESS);
    
}

std::string   
LArConditionsContainerBase::groupingTypeToString() const
{
    // Return grouping type in string form
    switch (m_groupType){
	case 0:
	    return ("Unknown");
	case 1:
	    return ("SingleGroup");
	case 2:
	    return ("SubDetectorGrouping");
        case 3:
	    return ("FeedThroughGrouping");
        case 4:
	    return ("ExtendedFTGrouping");
        case 5:
	    return ("ExtendedSubDetGrouping");
    }
    return ("Unknown");
}

const LArOnlineID*          
LArConditionsContainerBase::onlineHelper() const
{
    return (m_onlineHelper);
}

const CaloCell_ID*          
LArConditionsContainerBase::offlineHelper() const
{
    return (m_offlineHelper);
}


const LArConditionsContainerBase::FEBIdVector&
LArConditionsContainerBase::febIdVector(unsigned int gain, 
					unsigned int coolChannel) const
{
    return (m_febIdChanMap.febIdVector(gain, coolChannel));
}

void
LArConditionsContainerBase::correctionIndexAndCoolChannel (HWIdentifier id, 
                                                           unsigned int gain,
                                                           unsigned int& index,
                                                           unsigned int& coolChannel)
{
    MsgStream log(Athena::getMessageSvc(), "LArConditionsContainerBase");

    index       = 9999;
    coolChannel = 0;
    // First get cool channel number
    if (!m_onlineHelper) {
        log << MSG::ERROR << "correctionIndex> Could not get online id helper!" << endmsg;
        return;
    }

    int p_n  = m_onlineHelper->pos_neg(id);
    if (m_onlineHelper->isEMBPS(id)) {
        if (p_n) coolChannel = LArCondFEBIdChanMap::PSBA;  // PS Barrel
        else     coolChannel = LArCondFEBIdChanMap::PSBC;  // PS Barrel
    }
    else if (m_onlineHelper->isEMECPS(id)) {
        if (p_n) coolChannel = LArCondFEBIdChanMap::PSECA; // PS Endcap
        else     coolChannel = LArCondFEBIdChanMap::PSECC; // PS Endcap
    }
    else if (m_onlineHelper->isEMBchannel(id)) {
        if (p_n) coolChannel = LArCondFEBIdChanMap::EMBA;  // EM Barrel
        else     coolChannel = LArCondFEBIdChanMap::EMBC;  // EM Barrel
    }
    else if (m_onlineHelper->isEMECchannel(id)) {
        if (p_n) coolChannel = LArCondFEBIdChanMap::EMECA; // EM Endcap
        else     coolChannel = LArCondFEBIdChanMap::EMECC; // EM Endcap
    }
    else if (m_onlineHelper->isHECchannel(id)) {
        if (p_n) coolChannel = LArCondFEBIdChanMap::HECA;  // HEC A
        else     coolChannel = LArCondFEBIdChanMap::HECC;  // HEC C
    }
    else if (m_onlineHelper->isFCALchannel(id)) {
        if (p_n) coolChannel = LArCondFEBIdChanMap::FCALA; // FCAL A
        else     coolChannel = LArCondFEBIdChanMap::FCALC; // FCAL C
    }

    if (!coolChannel) {
        log << MSG::ERROR << "correctionIndex> coolChannel not found! Online ids: " 
            << "  " << m_onlineHelper->show_to_string(id)
            << endmsg;
        return;
    }

    // add on offset for gain
    unsigned int gainOffset = (gain - m_febIdChanMap.minGain())*LArCondFEBIdChanMap::NCORRTYPE;
    coolChannel += gainOffset;

    // look up the index in the table
    if (coolChannel >= m_channelToMultChanCollIndex.size()) {
        log << MSG::ERROR << "correctionIndex> coolChannel larger than lookup table: chan, size " 
            << coolChannel << " " << m_channelToMultChanCollIndex.size()
            << endmsg;
    }
    else {
        index = m_channelToMultChanCollIndex[coolChannel];
    }
}

bool             
LArConditionsContainerBase::applyCorrectionsAtInit(bool setFlag, bool flag)
{
    // If setFlag is true, change the value of static flag, otherwise
    // return current value

    // Default value is true
    static std::atomic<bool> applyCorrs = true;
    
    if (setFlag) applyCorrs = flag;
    
    return (applyCorrs);
}


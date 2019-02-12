/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArConditionsTestAlg.cxx
 *
 * @brief This file contains an algorithm for testing lar conditions
 * data access
 *
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 * @author Hong Ma      <hma@bnl.gov>
 *
 * $Id: LArConditionsTestAlg.cxx,v 1.21 2009-04-27 15:53:09 gunal Exp $ */

#include "LArConditionsTest/LArConditionsTestAlg.h"

#include "StoreGate/StoreGateSvc.h"

// #include "LArTools/LArFebRodMap.h"
// #include "LArTools/LArOnOffIdMap.h"
#include "LArRawUtils/LArRoI_Map.h" 
#include "LArCabling/LArCablingLegacyService.h" 
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloCell_ID.h"

//#include "AthenaKernel/IIOVSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include "LArRawConditions/LArRampMC.h" 
// #include "LArRawConditions/LArShapeMC.h" 
#include "LArRawConditions/LArConditionsChannelSet.h" 
//#include "LArRawConditions/LArNoiseDB.h" 
#include "LArElecCalib/ILArOFC.h" 

//#include "CondDBObjects/GenericDbTableClassDef.h"
//  #include "LArCondCnv/ExampleData.h"

/// POOL - temporary
// #include "POOLCore/POOLContext.h"

#include "AthenaKernel/errorcheck.h"

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

LArConditionsTestAlg::LArConditionsTestAlg(const std::string& name, ISvcLocator* pSvcLocator) :
	AthAlgorithm(name,pSvcLocator),
	m_cablingSvc(0),
	m_onlineID(0),
	m_testFill(false),
	m_testCondObjs(false),
	m_readCondObjs(false),
	m_writeCondObjs(false),
	m_writeCorrections(false),
	m_applyCorrections(false),
	m_testReadDB(false), 
	m_TB(false),
	m_testMC(false),
	m_tbin(0)
{
    // switch for testing Filling IOV. 
    declareProperty("TestCondObjs",      m_testCondObjs);
    declareProperty("ReadCondObjs",     m_readCondObjs);
    declareProperty("WriteCondObjs",    m_writeCondObjs);
    declareProperty("WriteCorrections", m_writeCorrections);
    declareProperty("ApplyCorrections", m_applyCorrections);
    declareProperty("TestFill",         m_testFill);
    declareProperty("TestReadDBDirect", m_testReadDB) ;
    declareProperty("TestMC",           m_testMC) ;
    declareProperty("Testbeam",         m_TB) ;
    declareProperty("Tbin",         m_tbin) ;
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////

LArConditionsTestAlg::~LArConditionsTestAlg()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
/////////////////////////////////////////////////////////////////////

StatusCode LArConditionsTestAlg::initialize()
{
    ATH_MSG_DEBUG ( " TestCondObjs flag         = " << m_testCondObjs );
    ATH_MSG_DEBUG ( " ReadCondObjs flag         = " << m_readCondObjs );
    ATH_MSG_DEBUG ( " WriteCondObjs flag        = " << m_writeCondObjs );
    ATH_MSG_DEBUG ( " WriteCorrections flag     = " << m_writeCorrections );
    ATH_MSG_DEBUG ( " ApplyCorrections flag     = " << m_applyCorrections );
    ATH_MSG_DEBUG ( " TestFill flag             = " << m_testFill );
    ATH_MSG_DEBUG ( " TestReadDBDirect flag     = " << m_testReadDB );
    ATH_MSG_DEBUG ( " Testbeam flag             = " << m_TB );

    ATH_CHECK( detStore()->retrieve(m_onlineID) );

    // Need to load authentication for RDBMS
    // log << MSG::INFO << "Loading XMLAuthenticationService " << endmsg;
    // pool::POOLContext::loadComponent( "POOL/Services/XMLAuthenticationService" );

    const CaloCell_ID* calocell_id = nullptr;
    ATH_CHECK( detStore()->retrieve(calocell_id) );

     if(m_testMC){ 

       IIncidentSvc* incSvc = nullptr;
       ATH_CHECK( service("IncidentSvc", incSvc) );

       int PRIORITY = 100;
       //start listening to "BeginRun"
       incSvc->addListener(this, "BeginRun", PRIORITY);
       // register handles or call back 
       ATH_MSG_DEBUG ( " register DataHandles  or callback " );
 	// retrieve handel is we are going to test cond db objects
       StatusCode sc =  detStore()->regFcn(&LArConditionsTestAlg::testCallBack1, 
                                           this, m_ramp, "LArRamp"); 
       if(sc.isFailure() ){
         ATH_MSG_ERROR ( " regFcn DataHandle<ILArRamp> failed " );
       }

 	ATH_CHECK( detStore()->regHandle(m_DAC2uA, "LArDAC2uA") );
 	ATH_CHECK( detStore()->regHandle(m_uA2MeV, "LAruA2MeV") );
 	ATH_CHECK( detStore()->regHandle(m_noise, "LArNoise") );
 	ATH_CHECK( detStore()->regHandle(m_autoCorr, "LArAutoCorr") );
 	ATH_CHECK( detStore()->regHandle(m_shape, "LArShape") );
 	ATH_CHECK( detStore()->regHandle(m_pedestal, "LArPedestal") );
 	ATH_CHECK( detStore()->regHandle(m_fSampl, "LArfSampl") );
 	ATH_CHECK( detStore()->regHandle(m_minBias, "LArMinBias") );
     }// end of m_testMC

     ATH_MSG_DEBUG ( "initialize done" );
     return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////
// EXECUTE:
/////////////////////////////////////////////////////////////////////

StatusCode LArConditionsTestAlg::execute()
{
    ATH_MSG_DEBUG ( " retrieve DataHandle<ILArRamp>  in execute " );

    if(m_testCondObjs){ 

	// create cache
        ATH_CHECK(  testCondObjects() );
// 	StatusCode sc = testDbObjectRead();
// 	if(sc.isFailure()) {
//   	    log << MSG::ERROR << "Failed testDbObjectRead " << endmsg;
//   	    return StatusCode::FAILURE;
//   	}
	
    }
    

//    if(m_testCondObject){ 
// 	std::string key = "LArRamp";
// 	const ILArRamp* ramp = 0 ;
// 	m_detStore->retrieve(ramp, key);
// 	if(!ramp) {
// 	    log<< MSG::ERROR<<" Failed to get LArRamp in execute " << endmsg;
// 	    return StatusCode::FAILURE ; 
// 	}
//     } 

    if(m_TB){ 
	const ILArOFC* ofc = nullptr;
	ATH_CHECK( detStore()->retrieve(ofc, "LArOFC") );

	const ILArRamp* ramp = nullptr;
	ATH_CHECK( detStore()->retrieve(ramp, "LArRamp") );
    } 

/* 
   log << MSG::DEBUG << " retrieve DataHandle<ExampleData>  in execute " <<endmsg;
   const ExampleData* example = 0 ;
   m_detStore->retrieve( example );
   if(!example) {
   log<< MSG::ERROR<<" Failed to get ExampleData in execute " << endmsg;
   }
*/


//      IToolSvc* toolSvc;
//      if( service("ToolSvc", toolSvc) != StatusCode::SUCCESS){
//  	log<< MSG::ERROR <<" Failed to get ToolSvc" <<endmsg;     
//  	return StatusCode::FAILURE; 
//      }

//      LArRoI_Map* roiMap;
//      if(StatusCode::SUCCESS != toolSvc->retrieveTool("LArRoI_Map",roiMap )) {
//  	log << MSG::ERROR << " Can't get LArRoI_Map " << endmsg;
//  	return StatusCode::FAILURE;
//      }

    return StatusCode::SUCCESS;
}


/////////////////////////////////////////////////////////////////////
// FINALIZE:
// Note that it is NOT NECESSARY to run the finalize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////

StatusCode LArConditionsTestAlg::finalize()
{
    if(m_testFill)    testFillIOVDb(); 
    if(m_testReadDB)  testDbObjectRead(); 
    return StatusCode::SUCCESS; 
} 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode
LArConditionsTestAlg::createCompareObjects()
{
    // StatusCode sc;
    ATH_MSG_INFO ("in createCompareObjects()" );

    // Create set of ids, LArRampComplete::LArCondObj

    std::vector<HWIdentifier>::const_iterator chanIt  = m_onlineID->channel_begin();
    std::vector<HWIdentifier>::const_iterator chanEnd = m_onlineID->channel_end();
    int ichan           = -1;
    int icorr           = 0;
    float vramp         = 0;
    int gain            = 0;
    for (; chanIt != chanEnd; ++chanIt, ++ichan) {
	// add channels with downscale factor
	if (ichan % 1000 != 5) continue;

	// Create ramp with 3 vRamp elements
	LArRampPTmp ramp((*chanIt), gain);
	ramp.m_vRamp.push_back(vramp);
	vramp += 1.0;
	ramp.m_vRamp.push_back(vramp);
	vramp += 1.0;
	ramp.m_vRamp.push_back(vramp);
	vramp += 1.0;
	// add to cache
	m_rampCache.push_back(ramp);
	
	// Change gain each time
	gain = (gain == 2) ? 0 : gain + 1;
	
	if (m_writeCorrections || m_applyCorrections) {
	    // Create downscaled corrections
	    ++icorr;
	    if (icorr % 10 != 5) continue;
	    // Just change sign of ramp values
	    for (unsigned int i = 0; i < 3; ++i)ramp.m_vRamp[i] = -ramp.m_vRamp[i];
	    m_rampCorrections.push_back(ramp);
	}
    }
    
    // Print out cache and corrections
    for (unsigned int i = 0; i < m_rampCache.size(); ++i) {
      ATH_MSG_DEBUG ("Cache: chan, gain, ramps "
                     << m_onlineID->show_to_string(m_rampCache[i].m_channelID) << " " 
                     << m_rampCache[i].m_gain << " " 
                     << m_rampCache[i].m_vRamp[0] << " " 
                     << m_rampCache[i].m_vRamp[1] << " " 
                     << m_rampCache[i].m_vRamp[2] << " " );
    }
    for (unsigned int i = 0; i < m_rampCorrections.size(); ++i) {
      ATH_MSG_DEBUG ("Corrections: chan, gain, ramps "
                     << m_onlineID->show_to_string(m_rampCorrections[i].m_channelID) << " " 
                     << m_rampCorrections[i].m_gain << " " 
                     << m_rampCorrections[i].m_vRamp[0] << " " 
                     << m_rampCorrections[i].m_vRamp[1] << " " 
                     << m_rampCorrections[i].m_vRamp[2] << " " );
    }
    
    ATH_MSG_DEBUG ( "End of create comparison objects " );
    return StatusCode::SUCCESS; 
} 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

inline bool operator == (const LArRampComplete::LArCondObj& r1, const LArRampPTmp& r2) 
{
    // Comparison of two LArRampComplete::LArCondObj objects
    if (r1.m_vRamp.size() != r2.m_vRamp.size()) return (false);
    for (unsigned int i = 0; i < r1.m_vRamp.size(); ++i) {
	if (r1.m_vRamp[i] != r2.m_vRamp[i]) return (false);
    }
    return (true);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

inline bool CorrectionCompare (const LArRampComplete::LArCondObj& r1, const LArRampPTmp& r2) 
{
    // Comparison of two LArRampComplete::LArCondObj objects
    if (r1.m_vRamp.size() != r2.m_vRamp.size()) return (false);
    for (unsigned int i = 0; i < r1.m_vRamp.size(); ++i) {
	if (r1.m_vRamp[i] != -r2.m_vRamp[i]) return (false);
    }
    return (true);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

inline bool operator != (const LArRampComplete::LArCondObj& r1, const LArRampPTmp& r2) 
{
    if(r1 == r2)return (false);
    return (true);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode
LArConditionsTestAlg::testCondObjects()
{
    ATH_MSG_INFO ("in testCondObjects()" );

    static bool first = true;
    if (!first) {
        ATH_MSG_INFO ("Multiple entries - returning" );
	return StatusCode::SUCCESS; 
    }
    first = false;
    
    typedef LArRampMC::CONTAINER            CONTAINER; 
    typedef CONTAINER::chan_const_iterator  chan_const_iterator;
    typedef CONTAINER::iov_const_iterator   iov_const_iterator;
    
    ATH_CHECK( createCompareObjects() );
    ATH_CHECK( testChannelSet() );

    const LArRampMC* ramps = 0;
    
    // Create SingleGroup
    if (m_readCondObjs) {
	const ILArRamp* iramps = 0;
	ATH_CHECK( detStore()->retrieve(iramps, "/LArCalorimeter/LArTests/LArRampsSingleGroup") );
	ATH_MSG_INFO ( "Retrieved ramps for LArRampsSingleGroup " );
	ramps = dynamic_cast<const LArRampMC*>(iramps);
	if (!ramps) {
            ATH_MSG_ERROR ("Could not dynamic cast ILArRamp to LArRampMC" );
	    return( StatusCode::FAILURE);
	}
    }
    else {
	LArRampMC* ramps_rw = new LArRampMC;
	ramps = ramps_rw;
	ATH_MSG_INFO ( "Created ramps for LArRampsSingleGroup "  );
	ramps_rw->setGroupingType(LArConditionsContainerBase::SingleGroup);
	ATH_CHECK( ramps_rw->initialize() );
    }
    
    ATH_CHECK( testEachCondObject(ramps) );
    ATH_MSG_INFO ( "Succeeded SingleGroup test  " );

    if (!m_readCondObjs) {
	// Save in DetectorStore
        ATH_CHECK( detStore()->record(ramps, "/LArCalorimeter/LArTests/LArRampsSingleGroup") );
	const ILArRamp* iramps = 0;
        ATH_CHECK( detStore()->symLink(ramps, iramps) );

	/// Statistics: total number of conditions 
        if (ramps) {
          ATH_MSG_DEBUG  ( "Total number of conditions objects"
                           << ramps->totalNumberOfConditions() );
        }
	if (!m_writeCondObjs) {
	    // Remove conditions objects if not writing out
	    LArRampMC* ramps_rw = const_cast<LArRampMC*>(ramps);
	    if (!ramps_rw) {
                ATH_MSG_ERROR ( "Could not const cast to LArRampMC " );
		return StatusCode::FAILURE;
	    }
	    ramps_rw->removeConditions();
	    ATH_MSG_DEBUG  ( "Removed conditions objects" );
	}
	ATH_MSG_DEBUG  ( "Total number of conditions objects "
                         << ramps->totalNumberOfConditions() );
	ATH_MSG_DEBUG  ( "Total number of correction objects"
                         << ramps->totalNumberOfCorrections() );
    }
    
    // Create SubDetectorGrouping
    if (m_readCondObjs) {
	const ILArRamp* iramps = 0;
	ATH_CHECK( detStore()->retrieve(iramps, "/LArCalorimeter/LArTests/LArRampsSubDetectorGrouping") );
	ATH_MSG_INFO ( "Retrieved ramps for LArRampsSubDetectorGrouping "  );
	ramps = dynamic_cast<const LArRampMC*>(iramps);
	if (!ramps) {
          ATH_MSG_ERROR ("Could not dynamic cast ILArRamp to LArRampMC" );
          return( StatusCode::FAILURE);
	}
    }
    else {
	LArRampMC* ramps_rw = new LArRampMC;
	ramps = ramps_rw;
	//ramps_rw->setGroupingType(LArConditionsContainerBase::SubDetectorGrouping);
	ATH_CHECK( ramps_rw->initialize() );
    }
    
    ATH_CHECK( testEachCondObject(ramps) );
    ATH_MSG_INFO ( "Succeeded SubDetectorGrouping test  " );

    if (!m_readCondObjs) {
	// Save in DetectorStore
        ATH_CHECK( detStore()->record(ramps, "/LArCalorimeter/LArTests/LArRampsSubDetectorGrouping") );
	const ILArRamp* iramps = 0;
	ATH_CHECK( detStore()->symLink(ramps, iramps) );
	/// Statistics: total number of conditions 
        if (ramps) {
          ATH_MSG_DEBUG  ( "Total number of conditions objects"
                           << ramps->totalNumberOfConditions() );
        }
	if (!m_writeCondObjs) {
	    // Remove conditions objects if not writing out
	    LArRampMC* ramps_rw = const_cast<LArRampMC*>(ramps);
	    if (!ramps_rw) {
                ATH_MSG_ERROR ( "Could not const cast to LArRampMC " );
		return StatusCode::FAILURE;
	    }
	    ramps_rw->removeConditions();
	    ATH_MSG_DEBUG  ( "Removed conditions objects" );
	}
	ATH_MSG_DEBUG  ( "Total number of conditions objects "
                         << ramps->totalNumberOfConditions() );
	ATH_MSG_DEBUG  ( "Total number of correction objects"
                         << ramps->totalNumberOfConditions() );
    }
    
    // Create FeedThroughGrouping
    if (m_readCondObjs) {
	const ILArRamp* iramps = 0;
	ATH_CHECK( detStore()->retrieve(iramps, "/LArCalorimeter/LArTests/LArRampsFeedThroughGrouping") );
	ATH_MSG_INFO ( "Retrieved ramps for LArRampsFeedThroughGrouping " );
	ramps = dynamic_cast<const LArRampMC*>(iramps);
	if (!ramps) {
            ATH_MSG_ERROR ("Could not dynamic cast ILArRamp to LArRampMC" );
	    return( StatusCode::FAILURE);
	}
    }
    else {
	LArRampMC* ramps_rw = new LArRampMC;
	ramps = ramps_rw;
	ramps_rw->setGroupingType(LArConditionsContainerBase::FeedThroughGrouping);
	ATH_CHECK( ramps_rw->initialize() );
    }
    
    ATH_CHECK( testEachCondObject(ramps) );
    ATH_MSG_INFO ( "Succeeded FeedThroughGrouping test  " );

    if (!m_readCondObjs) {
	// Save in DetectorStore
      ATH_CHECK( detStore()->record(ramps, "/LArCalorimeter/LArTests/LArRampsFeedThroughGrouping") );
	const ILArRamp* iramps = 0;
	ATH_CHECK( detStore()->symLink(ramps, iramps) );
	/// Statistics: total number of conditions 
        if (ramps) {
          ATH_MSG_DEBUG  ( "Total number of conditions objects"
                           << ramps->totalNumberOfConditions() );
        }
	if (!m_writeCondObjs) {
	    // Remove conditions objects if not writing out
	    LArRampMC* ramps_rw = const_cast<LArRampMC*>(ramps);
	    if (!ramps_rw) {
                ATH_MSG_ERROR ( "Could not const cast to LArRampMC " );
		return StatusCode::FAILURE;
	    }
	    ramps_rw->removeConditions();
	    ATH_MSG_DEBUG  ( "Removed conditions objects" );
	}
	ATH_MSG_DEBUG  ( "Total number of conditions objects "
                         << ramps->totalNumberOfConditions() );
	ATH_MSG_DEBUG  ( "Total number of correction objects"
                         << ramps->totalNumberOfCorrections() );
    }

    ATH_MSG_DEBUG ( "Statistics for LArRampsFeedThroughGrouping " );
    ATH_MSG_DEBUG ( "Number of channels, iovs "
                    << ramps->chan_size() << " " << ramps->iov_size() );

    iov_const_iterator iovIt  = ramps->iov_begin();
    iov_const_iterator iovEnd = ramps->iov_end  ();
    msg() << MSG::DEBUG << "IOVs found:  ";
    for (; iovIt != iovEnd; ++iovIt) {
      msg() << MSG::DEBUG << (*iovIt) << ", ";
    }
    msg() << MSG::DEBUG << endmsg;
    
    chan_const_iterator chIt  = ramps->chan_begin();
    chan_const_iterator chEnd = ramps->chan_end  ();
    for (; chIt != chEnd; ++chIt) {
      ATH_MSG_DEBUG ( "Channel:  " << (*chIt) 
                      << " number of conditions: " << ramps->conditionsPerChannel((*chIt)) );
    }
    
    for (unsigned int i = 0; i < ramps->nGroups(); ++i) {
      ATH_MSG_DEBUG ( "Group:  " << i 
                      << " number of conditions: " << ramps->conditionsPerGroup(i) );
    }
    ATH_MSG_DEBUG ("");
    
    for (unsigned int i = 0; i < ramps->nGains(); ++i) {
      ATH_MSG_DEBUG ( "Gain:  " << i 
                      << " number of conditions: " << ramps->conditionsPerGain(i) );
    }
    ATH_MSG_DEBUG  ( "Total number of conditions objects "
                     << ramps->totalNumberOfConditions() );
    ATH_MSG_DEBUG  ( "Total number of correction objects "
                     << ramps->totalNumberOfCorrections() );

    
    ATH_MSG_DEBUG ( "End of testCondObjects " );

    return StatusCode::SUCCESS; 
} 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode
LArConditionsTestAlg::testEachCondObject(const LArRampMC* ramps)
{
    ATH_MSG_INFO ("in testEachCondObject()" );
    bool error = false;
    
    typedef LArRampMC::CONTAINER         CONTAINER; 
    //typedef CONTAINER::ConstCorrectionIt ConstCorrectionIt;

    // Cast into r/w for tests
    LArRampMC* ramps_rw = const_cast<LArRampMC*>(ramps);
    if (!ramps_rw) {
        ATH_MSG_ERROR ( "Could not const cast to LArRampMC " );
	return StatusCode::FAILURE;
    }

    if (ramps_rw->correctionsApplied())
      ATH_CHECK(ramps_rw->undoCorrections());

    if (!m_readCondObjs) {
	if (m_writeCondObjs) {
	    
	    for (unsigned int i = 0; i < m_rampCache.size(); ++i) {
              ATH_MSG_DEBUG ("setPdata for chan, chan id, gain " << i << " "
                             << m_onlineID->show_to_string(m_rampCache[i].m_channelID) << " " 
                             << m_rampCache[i].m_gain << " " );

		// Must copy LArRampPTmp into a LArRampComplete::LArCondObj
		LArRampComplete::LArCondObj ramp;
		ramp.m_vRamp = m_rampCache[i].m_vRamp;

		ramps_rw->setPdata(m_rampCache[i].m_channelID, 
				   ramp,
				   m_rampCache[i].m_gain);
	    }
	}
	
	ATH_MSG_DEBUG ( "Finished conditions, now write corrections " );

	if (m_writeCorrections) {
	    for (unsigned int i = 0; i < m_rampCorrections.size(); ++i) {

              ATH_MSG_DEBUG ("insert corr for chan, chan id, gain " << i << " "
                             << m_onlineID->show_to_string(m_rampCorrections[i].m_channelID) << " " 
                             << m_rampCorrections[i].m_gain << " " );

		// Must copy LArRampPTmp into a LArRampComplete::LArCondObj
		LArRampComplete::LArCondObj ramp;
		ramp.m_vRamp = m_rampCorrections[i].m_vRamp;

		ATH_CHECK( ramps_rw->insertCorrection(m_rampCorrections[i].m_channelID, 
                                                      ramp,
                                                      m_rampCorrections[i].m_gain) );
	    }
	}
    }
    
    ATH_MSG_DEBUG ("Number of channels, iovs "
                   << ramps->chan_size() << " " << ramps->iov_size() );

    CONTAINER::chan_const_iterator   chanIt1  = ramps->chan_begin();
    CONTAINER::chan_const_iterator   endChan1 = ramps->chan_end  ();
    for (unsigned int i = 0; chanIt1 != endChan1; ++chanIt1, ++i) {
	const CONTAINER::Subset* subset = ramps->at(i);
	ATH_MSG_DEBUG ( "Index " << i 
	    << " channel "           << subset->channel() 
	    << " gain "              << subset->gain() 
	    << " groupingType "      << subset->groupingType()
	    << " subsetSize "        << subset->subsetSize()
	    << " correctionVecSize " << subset->correctionVecSize() );
	if ((*chanIt1) != subset->channel()) {
          ATH_MSG_ERROR ( "Channel numbers not the same for MultChanColl and subset: " 
		<< i 
		<< " multchan "           << (*chanIt1)
		<< " subset   "           << subset->channel() );
	    error = true;
	}
    }

    ATH_MSG_DEBUG ("Number of channels, iovs, subsets "
                   << ramps->chan_size() << " " 
                   << ramps->iov_size()  << " "
                   << ramps->size()  << " " );

    ATH_MSG_DEBUG ("Compare LArRampMC with cache " );
    // Now loop over ramps and compare with cache
    for (unsigned int i = 0; i < m_rampCache.size(); ++i) {

	LArRampComplete::LArCondObj rampP           = ramps->get(m_rampCache[i].m_channelID, 
					      m_rampCache[i].m_gain);
	unsigned int coolChannel = ramps->coolChannel(m_rampCache[i].m_channelID, 
						      m_rampCache[i].m_gain);

	if (!rampP.isEmpty()) {
          ATH_MSG_DEBUG ("New  : cool chan, chan id, gain, ramps "
                         << coolChannel << " "
                         << m_onlineID->show_to_string(m_rampCache[i].m_channelID) << " " 	
                         << m_rampCache[i].m_gain << " " 
                         << rampP.m_vRamp[0] << " " 
                         << rampP.m_vRamp[1] << " " 
                         << rampP.m_vRamp[2] << " " );
	}
	else {
          ATH_MSG_DEBUG ("New  : isEmpty " );
	}
	ATH_MSG_DEBUG ("Cache: cool chan, chan id, gain, ramps "
                       << coolChannel << " "
                       << m_onlineID->show_to_string(m_rampCache[i].m_channelID) << " " 	
                       << m_rampCache[i].m_gain << " " 
                       << m_rampCache[i].m_vRamp[0] << " " 
                       << m_rampCache[i].m_vRamp[1] << " " 
                       << m_rampCache[i].m_vRamp[2] << " " 
                       << " Compare = " << (rampP == m_rampCache[i]) );
	if (rampP != m_rampCache[i] && !rampP.isEmpty()) {
            ATH_MSG_ERROR ("LArRampMC and cache NOT equal" );
	    error = true;
	}
    }


    // Now loop over ramps using generic iterator and compare with cache
    ATH_MSG_DEBUG ("Compare LArRampMC with cache using iterator " );
    CONTAINER::ConstConditionsMapIterator rampIt;
    CONTAINER::ConstConditionsMapIterator rampEnd;
    for (unsigned int gain = 0; gain < 3; ++gain) {
	rampIt  = ramps->begin(gain);	    
	rampEnd = ramps->end  (gain);
	for (unsigned int i = 0; i < m_rampCache.size(); ++i) {
	    // cache is not in order for gains, select the current gain
	    if (gain != m_rampCache[i].m_gain) continue; 
	    LArRampComplete::LArCondObj rampP;
	    HWIdentifier rampId;
	    while(rampIt != rampEnd) {
		rampP = *rampIt;
		rampId = rampIt.channelId();
		++rampIt;
		if (!rampP.isEmpty()) break; // break out for first non-empty ramp
	    }
	    unsigned int coolChannel = ramps->coolChannel(m_rampCache[i].m_channelID, 
							  m_rampCache[i].m_gain);
	    if (!rampP.isEmpty()) {
              ATH_MSG_DEBUG ("New  : cool chan, chan id, gain, ramps "
                             << coolChannel << " "
                             << m_onlineID->show_to_string(rampId) << " " 
                             << m_rampCache[i].m_gain << " " 
                             << rampP.m_vRamp[0] << " " 
                             << rampP.m_vRamp[1] << " " 
                             << rampP.m_vRamp[2] << " " );
	    }
	    else {
              ATH_MSG_DEBUG ("New  : isEmpty " );
	    }
	    ATH_MSG_DEBUG ("Cache: cool chan, chan id, gain, ramps "
                           << coolChannel << " "
                           << m_onlineID->show_to_string(m_rampCache[i].m_channelID) << " " 	
                           << m_rampCache[i].m_gain << " " 
                           << m_rampCache[i].m_vRamp[0] << " " 
                           << m_rampCache[i].m_vRamp[1] << " " 
                           << m_rampCache[i].m_vRamp[2] << " " 
                           << " Compare = " << (rampP == m_rampCache[i]) );
	    if (rampP != m_rampCache[i] && !rampP.isEmpty()) {
                ATH_MSG_ERROR ("LArRampMC and cache NOT equal" );
		error = true;
	    }
	}
    }
    


    // Now loop over ramps in pieces using the selector on febids to
    // iterate and compare with cache
    ATH_MSG_DEBUG ("Compare LArRampMC with cache using iterator and febid selection " );
    // Loop over cache and divide the febids into three sets, where
    // each set is an array of size 3 for the separate gains
    std::vector<unsigned int> ids1[3];
    std::vector<unsigned int> ids2[3];
    std::vector<unsigned int> ids3[3];
    for (unsigned int i = 0; i < m_rampCache.size(); ++i) {
	if (i < m_rampCache.size()/3) {
	    unsigned int id =  m_onlineID->feb_Id(m_rampCache[i].m_channelID).get_identifier32().get_compact();
	    ids1[m_rampCache[i].m_gain].push_back(id);
	}
	else if (i < 2*m_rampCache.size()/3) {
	    unsigned int id =  m_onlineID->feb_Id(m_rampCache[i].m_channelID).get_identifier32().get_compact();
	    ids2[m_rampCache[i].m_gain].push_back(id);
	}
	else {
	    unsigned int id =  m_onlineID->feb_Id(m_rampCache[i].m_channelID).get_identifier32().get_compact();
	    ids3[m_rampCache[i].m_gain].push_back(id);
	}
    }

    for (unsigned int gain = 0; gain < 3; ++gain) {
	for (unsigned int febSet = 0; febSet < 3; ++febSet) {
	    unsigned int i0   = 0; 
	    unsigned int iend = m_rampCache.size()/3;
	    if (febSet < m_rampCache.size()/3) {
		rampIt  = ramps->begin(gain, ids1[gain]);	    
		msg() << MSG::DEBUG <<"FebID vec 1  : ";
		for (unsigned int i = 0; i < ids1[gain].size(); ++i) {
                  msg() << MSG::DEBUG << m_onlineID->show_to_string(HWIdentifier(ids1[gain][i]))
			<< " ";
		}
		msg() << MSG::DEBUG << endmsg;
	    }
	    else if (febSet < 2*m_rampCache.size()/3) {
		rampIt  = ramps->begin(gain, ids2[gain]);	    
		i0   = m_rampCache.size()/3 + 1;
		iend = 2*m_rampCache.size()/3;
		msg() << MSG::DEBUG <<"FebID vec 2  : ";
		for (unsigned int i = 0; i < ids2[gain].size(); ++i) {
                  msg() << MSG::DEBUG << m_onlineID->show_to_string(HWIdentifier(ids2[gain][i]))
			<< " ";
		}
                msg() << MSG::DEBUG << endmsg;
	    }
	    else {
		rampIt  = ramps->begin(gain, ids3[gain]);	    
		i0   = 2*m_rampCache.size()/3 + 1;
		iend = m_rampCache.size();
		msg() << MSG::DEBUG <<"FebID vec 3  : ";
		for (unsigned int i = 0; i < ids3[gain].size(); ++i) {
                  msg() << MSG::DEBUG << m_onlineID->show_to_string(HWIdentifier(ids3[gain][i]))
			<< " ";
		}
		msg() << MSG::DEBUG << endmsg;
	    }
	    
	    rampEnd = ramps->end  (gain);
	    ATH_MSG_DEBUG ("After ramps->end " );
	    for (unsigned int i = i0; i < iend; ++i) {
		// cache is not in order for gains, select the current gain
		if (gain != m_rampCache[i].m_gain) continue; 
		LArRampComplete::LArCondObj rampP;
		HWIdentifier rampId;

		ATH_MSG_DEBUG ("Looking for "
                               << m_onlineID->show_to_string(m_rampCache[i].m_channelID) );

		// Skip the empty channels
		while(rampIt != rampEnd) {
		    rampP = *rampIt;
		    rampId = rampIt.channelId();
		    ++rampIt;
		    if (!rampP.isEmpty()) break; // break out for first non-empty ramp
		}
		unsigned int coolChannel = ramps->coolChannel(m_rampCache[i].m_channelID, 
							      m_rampCache[i].m_gain);
		if (!rampP.isEmpty()) {
                  ATH_MSG_DEBUG ("New  : cool chan, chan id, gain, ramps "
                                 << coolChannel << " "
                                 << m_onlineID->show_to_string(rampId) << " " 
                                 << m_rampCache[i].m_gain << " " 
                                 << rampP.m_vRamp[0] << " " 
                                 << rampP.m_vRamp[1] << " " 
                                 << rampP.m_vRamp[2] << " "  );
		}
		else {
                  ATH_MSG_DEBUG ("New  : isEmpty " );
		}
		ATH_MSG_DEBUG ("Cache: cool chan, chan id, gain, ramps "
                               << coolChannel << " "
                               << m_onlineID->show_to_string(m_rampCache[i].m_channelID) << " " 	
                               << m_rampCache[i].m_gain << " " 
                               << m_rampCache[i].m_vRamp[0] << " " 
                               << m_rampCache[i].m_vRamp[1] << " " 
                               << m_rampCache[i].m_vRamp[2] << " " 
                               << " Compare = " << (rampP == m_rampCache[i]) );
		if (rampP != m_rampCache[i] && !rampP.isEmpty()) {
                    ATH_MSG_ERROR ("LArRampMC and cache NOT equal" );
		    error = true;
		}
	    }
	}
    }

    ATH_MSG_DEBUG ("Compare LArRampMC with corrections " );

    if (m_applyCorrections) {
	for (unsigned int i = 0; i < m_rampCorrections.size(); ++i) {
	    LArRampComplete::LArCondObj rampP           = ramps->get(m_rampCorrections[i].m_channelID, 
						  m_rampCorrections[i].m_gain);
	    unsigned int coolChannel = ramps->coolChannel(m_rampCorrections[i].m_channelID, 
							  m_rampCorrections[i].m_gain);
	    if (!rampP.isEmpty()) {
              ATH_MSG_DEBUG ("New        : cool chan, chan id, gain, ramps "
                             << coolChannel << " "
                             << m_onlineID->show_to_string(m_rampCorrections[i].m_channelID) << " " 
                             << m_rampCorrections[i].m_gain << " " 
                             << rampP.m_vRamp[0] << " " 
                             << rampP.m_vRamp[1] << " " 
                             << rampP.m_vRamp[2] << " "  );
	    }
	    else {
              ATH_MSG_DEBUG ("New  : isEmpty " );
	    }
	    
	    ATH_MSG_DEBUG ("Corrections: cool chan, chan id, gain, ramps "
                           << coolChannel << " "
                           << m_onlineID->show_to_string(m_rampCorrections[i].m_channelID) << " " 
                           << m_rampCorrections[i].m_gain << " " 
                           << m_rampCorrections[i].m_vRamp[0] << " " 
                           << m_rampCorrections[i].m_vRamp[1] << " " 
                           << m_rampCorrections[i].m_vRamp[2] << " " 
                           << " Compare = " << (CorrectionCompare(rampP, m_rampCorrections[i])) );
	    if (!CorrectionCompare(rampP, m_rampCorrections[i]) && !rampP.isEmpty()) {
		
                ATH_MSG_ERROR ("Before correction: LArRampMC and correction DO NOT compare - should have opposite signs for rampes" );
		error = true;
	    }
	}
    

	ATH_MSG_DEBUG ("Apply corrections and compare LArRampMC with corrections " );
	ATH_CHECK( ramps_rw->applyCorrections() );
	ATH_MSG_DEBUG ("Corrections applied: " << ramps->correctionsApplied() );

	for (unsigned int i = 0; i < m_rampCorrections.size(); ++i) {
	    LArRampComplete::LArCondObj rampP           = ramps->get(m_rampCorrections[i].m_channelID, 
						  m_rampCorrections[i].m_gain);
	    unsigned int coolChannel = ramps->coolChannel(m_rampCorrections[i].m_channelID, 
							  m_rampCorrections[i].m_gain);
	    if (!rampP.isEmpty()) {
              ATH_MSG_DEBUG ("New        : cool chan, chan id, gain, ramps "
                             << coolChannel << " "
                             << m_onlineID->show_to_string(m_rampCorrections[i].m_channelID) << " " 
                             << m_rampCorrections[i].m_gain << " " 
                             << rampP.m_vRamp[0] << " " 
                             << rampP.m_vRamp[1] << " " 
                             << rampP.m_vRamp[2] << " "  );
	    }
	    else {
              ATH_MSG_DEBUG ("New  : isEmpty " );
	    }
	    ATH_MSG_DEBUG ("Corrections: cool chan, chan id, gain, ramps "
                           << coolChannel << " "
                           << m_onlineID->show_to_string(m_rampCorrections[i].m_channelID) << " " 
                           << m_rampCorrections[i].m_gain << " " 
                           << m_rampCorrections[i].m_vRamp[0] << " " 
                           << m_rampCorrections[i].m_vRamp[1] << " " 
                           << m_rampCorrections[i].m_vRamp[2] << " " 
                           << " Compare = " << (rampP == m_rampCorrections[i]) );
	    if (rampP != m_rampCorrections[i] && !rampP.isEmpty()) {
                ATH_MSG_ERROR ("After correction: LArRampMC and correction NOT equal" );
		error = true;
	    }
	}

	ATH_MSG_DEBUG ("Undo corrections and compare LArRampMC with corrections " );
	ATH_CHECK( ramps_rw->undoCorrections() );
	ATH_MSG_DEBUG ("Corrections applied: " << ramps->correctionsApplied() );

	for (unsigned int i = 0; i < m_rampCorrections.size(); ++i) {
	    LArRampComplete::LArCondObj rampP           = ramps->get(m_rampCorrections[i].m_channelID, 
						  m_rampCorrections[i].m_gain);
	    unsigned int coolChannel = ramps->coolChannel(m_rampCorrections[i].m_channelID, 
							  m_rampCorrections[i].m_gain);
	    if (!rampP.isEmpty()) {
              ATH_MSG_DEBUG ("New        : cool chan, chan id, gain, ramps "
                             << coolChannel << " "
                             << m_onlineID->show_to_string(m_rampCorrections[i].m_channelID) << " " 
                             << m_rampCorrections[i].m_gain << " " 
                             << rampP.m_vRamp[0] << " " 
                             << rampP.m_vRamp[1] << " " 
                             << rampP.m_vRamp[2] << " "  );
	    }
	    else {
              ATH_MSG_DEBUG ("New  : isEmpty " );
	    }
	    ATH_MSG_DEBUG ("Corrections: cool chan, chan id, gain, ramps "
                           << coolChannel << " "
                           << m_onlineID->show_to_string(m_rampCorrections[i].m_channelID) << " " 
                           << m_rampCorrections[i].m_gain << " " 
                           << m_rampCorrections[i].m_vRamp[0] << " " 
                           << m_rampCorrections[i].m_vRamp[1] << " " 
                           << m_rampCorrections[i].m_vRamp[2] << " " 
                           << " Compare = " << (CorrectionCompare(rampP, m_rampCorrections[i])) );
	    if (!CorrectionCompare(rampP, m_rampCorrections[i]) && !rampP.isEmpty()) {
		
                ATH_MSG_ERROR ("After undo: LArRampMC and correction DO NOT compare - should have opposite signs for ramps" );
		error = true;
	    }
	}

	ATH_MSG_DEBUG ("2nd Apply corrections and compare LArRampMC with corrections " );
	ATH_CHECK( ramps_rw->applyCorrections() );
	ATH_MSG_DEBUG ("Corrections applied: " << ramps->correctionsApplied() );

	for (unsigned int i = 0; i < m_rampCorrections.size(); ++i) {
	    LArRampComplete::LArCondObj rampP           = ramps->get(m_rampCorrections[i].m_channelID, 
						  m_rampCorrections[i].m_gain);
	    unsigned int coolChannel = ramps->coolChannel(m_rampCorrections[i].m_channelID, 
							  m_rampCorrections[i].m_gain);
	    if (!rampP.isEmpty()) {
              ATH_MSG_DEBUG ("New        : cool chan, chan id, gain, ramps "
                             << coolChannel << " "
                             << m_onlineID->show_to_string(m_rampCorrections[i].m_channelID) << " " 
                             << m_rampCorrections[i].m_gain << " " 
                             << rampP.m_vRamp[0] << " " 
                             << rampP.m_vRamp[1] << " " 
                             << rampP.m_vRamp[2] << " "  );
	    }
	    else {
              ATH_MSG_DEBUG ("New  : isEmpty " );
	    }
	    ATH_MSG_DEBUG ("Corrections: cool chan, chan id, gain, ramps "
                           << coolChannel << " "
                           << m_onlineID->show_to_string(m_rampCorrections[i].m_channelID) << " " 
                           << m_rampCorrections[i].m_gain << " " 
                           << m_rampCorrections[i].m_vRamp[0] << " " 
                           << m_rampCorrections[i].m_vRamp[1] << " " 
                           << m_rampCorrections[i].m_vRamp[2] << " " 
                           << " Compare = " << (rampP == m_rampCorrections[i]) );
	    if (rampP != m_rampCorrections[i] && !rampP.isEmpty()) {
                ATH_MSG_ERROR ("After correction: LArRampMC and correction NOT equal" );
		error = true;
	    }
	}

	ATH_MSG_DEBUG ("2nd Undo corrections and compare LArRampMC with corrections " );
	ATH_CHECK( ramps_rw->undoCorrections() );
	ATH_MSG_DEBUG ("Corrections applied: " << ramps->correctionsApplied() );

	for (unsigned int i = 0; i < m_rampCorrections.size(); ++i) {
	    LArRampComplete::LArCondObj rampP           = ramps->get(m_rampCorrections[i].m_channelID, 
						  m_rampCorrections[i].m_gain);
	    unsigned int coolChannel = ramps->coolChannel(m_rampCorrections[i].m_channelID, 
							  m_rampCorrections[i].m_gain);
	    if (!rampP.isEmpty()) {
              ATH_MSG_DEBUG ("New        : cool chan, chan id, gain, ramps "
                             << coolChannel << " "
                             << m_onlineID->show_to_string(m_rampCorrections[i].m_channelID) << " " 
                             << m_rampCorrections[i].m_gain << " " 
                             << rampP.m_vRamp[0] << " " 
                             << rampP.m_vRamp[1] << " " 
                             << rampP.m_vRamp[2] << " "  );
	    }
	    else {
              ATH_MSG_DEBUG ("New  : isEmpty " );
	    }
	    ATH_MSG_DEBUG ("Corrections: cool chan, chan id, gain, ramps "
                           << coolChannel << " "
                           << m_onlineID->show_to_string(m_rampCorrections[i].m_channelID) << " " 
                           << m_rampCorrections[i].m_gain << " " 
                           << m_rampCorrections[i].m_vRamp[0] << " " 
                           << m_rampCorrections[i].m_vRamp[1] << " " 
                           << m_rampCorrections[i].m_vRamp[2] << " " 
                           << " Compare = " << (CorrectionCompare(rampP, m_rampCorrections[i])) );
	    if (!CorrectionCompare(rampP, m_rampCorrections[i]) && !rampP.isEmpty()) {
 		
                ATH_MSG_ERROR ("After undo: LArRampMC and correction DO NOT compare - should have opposite signs for ramps" );
		error = true;
	    }
	}
    }
    

    /*
    log << MSG::DEBUG <<"Find each correction "
	<< endmsg;

    for (unsigned int i = 0; i < m_rampCorrections.size(); ++i) {
	HWIdentifier id   = m_rampCorrections[i].m_channelID;
	unsigned int gain = m_rampCorrections[i].m_gain;

	ConstCorrectionIt  it    = ramps->findCorrection(id, gain);
	// May not have any corrections
	if (it != ramps->correctionsEnd(gain)) {
		
	    unsigned int coolChannel = ramps->coolChannel(id, gain);
	    HWIdentifier id1((*it).first);
	    LArRampComplete::LArCondObj rampP        = (*it).second;
	    if (id != id1 || rampP != m_rampCorrections[i]) {
		log << MSG::ERROR <<"Correction retrieved with findCorrection does not match: " 
		    << " i = " << i << endmsg;
		error = true;
		log << MSG::DEBUG <<"New        : cool chan, chan id, gain, ramps "
		    << coolChannel << " "
		    << m_onlineID->show_to_string(m_rampCorrections[i].m_channelID) << " " 
		    << m_rampCorrections[i].m_gain << " " 
		    << rampP.m_vRamp[0] << " " 
		    << rampP.m_vRamp[1] << " " 
		    << rampP.m_vRamp[2] << " " 
		    << endmsg;
		log << MSG::DEBUG <<"Corrections: cool chan, chan id, gain, ramps "
		    << coolChannel << " "
		    << m_onlineID->show_to_string(m_rampCorrections[i].m_channelID) << " " 
		    << m_rampCorrections[i].m_gain << " " 
		    << m_rampCorrections[i].m_vRamp[0] << " " 
		    << m_rampCorrections[i].m_vRamp[1] << " " 
		    << m_rampCorrections[i].m_vRamp[2] << " " 
		    << " Compare = " << (rampP == m_rampCorrections[i])
		    << endmsg;
	    }
	}
	else {
	    log << MSG::DEBUG <<"No corrections found "
		<< endmsg;
	}	    
    }
    log << MSG::DEBUG <<"End - Find each correction "
	<< endmsg;


    // Count the number of corrections per gain
    unsigned int gains[3] = {0,0,0};
    for (unsigned int i = 0; i < m_rampCorrections.size(); ++i) {
	unsigned int gain = m_rampCorrections[i].m_gain;
	gains[gain]++;
    }
    for (unsigned int i = 0; i < 3; ++i) {
	if (gains[i] != ramps->correctionsSize(i)) {
	    log << MSG::ERROR <<"Number of corrections not same as number inserted: " 
		<< gains[i] << " " 
		<< ramps->correctionsSize(i) << " gain " << i
		<< endmsg;
	    error = true;
	}

	// Check that each correction is the same using container iterator
	unsigned int nit = 0;
	ConstCorrectionIt  it  = ramps->correctionsBegin(i);
	ConstCorrectionIt  end = ramps->correctionsEnd(i);
	unsigned int icorr = 0;
	for (; it != end && icorr <  m_rampCorrections.size(); ++it, ++nit, ++icorr) {
	    while (m_rampCorrections[icorr].m_gain != i) ++icorr;
	    HWIdentifier id          = m_rampCorrections[icorr].m_channelID;
	    unsigned int gain        = m_rampCorrections[icorr].m_gain;
	    unsigned int coolChannel = ramps->coolChannel(id, gain);
	    HWIdentifier id1((*it).first);
	    LArRampComplete::LArCondObj rampP           = (*it).second;
	    if (id != id1 || rampP != m_rampCorrections[icorr]) {
		log << MSG::ERROR <<"Correction retrieved with iterator does not match: " 
		    << " gain  = " << i 
		    << " icorr = " << icorr
		    << " nit   = " << nit
		    << endmsg;
		error = true;
		log << MSG::DEBUG <<"New        : cool chan, chan id, gain, ramps "
		    << coolChannel << " "
		    << m_onlineID->show_to_string(m_rampCorrections[icorr].m_channelID) << " " 
		    << m_rampCorrections[icorr].m_gain << " " 
		    << rampP.m_vRamp[0] << " " 
		    << rampP.m_vRamp[1] << " " 
		    << rampP.m_vRamp[2] << " " 
		    << endmsg;
		log << MSG::DEBUG <<"Corrections: cool chan, chan id, gain, ramps "
		    << coolChannel << " "
		    << m_onlineID->show_to_string(m_rampCorrections[icorr].m_channelID) << " " 
		    << m_rampCorrections[icorr].m_gain << " " 
		    << m_rampCorrections[icorr].m_vRamp[0] << " " 
		    << m_rampCorrections[icorr].m_vRamp[1] << " " 
		    << m_rampCorrections[icorr].m_vRamp[2] << " " 
		    << " Compare = " << (rampP == m_rampCorrections[icorr])
		    << endmsg;
	    }
	}
    }

    removed.  
    */

    ATH_MSG_DEBUG ("Number of channels, iovs "
                   << ramps->chan_size() << " " << ramps->iov_size() );

    std::set<unsigned int> channelNumbers;
    CONTAINER::chan_const_iterator   chanIt  = ramps->chan_begin();
    CONTAINER::chan_const_iterator   endChan = ramps->chan_end  ();
    for (unsigned int i = 0; chanIt != endChan; ++chanIt, ++i) {
	const CONTAINER::Subset* subset = ramps->at(i);
	ATH_MSG_DEBUG ( "Index " << i 
                        << " channel "           << subset->channel() 
                        << " gain "              << subset->gain() 
                        << " groupingType "      << subset->groupingType()
                        << " subsetSize "        << subset->subsetSize()
                        << " correctionVecSize " << subset->correctionVecSize() );
	if ((*chanIt) != subset->channel()) {
          ATH_MSG_ERROR ( "Channel numbers not the same for MultChanColl and subset: " 
                          << i 
                          << " multchan "           << (*chanIt)
                          << " subset   "           << subset->channel()  );
	    error = true;
	}
	if (!(channelNumbers.insert(subset->channel()).second)) {
            ATH_MSG_ERROR ( "Duplicate channel number - Index " << i 
                            << " channel "           << subset->channel()  );
	    error = true;
	}
    }
    ATH_MSG_DEBUG ( "Channel numbers size " << channelNumbers.size()
                    << " ramps size " << ramps->chan_size() );
    
    if (error) {
        ATH_MSG_ERROR ("Failing check of LArRamp - see above" );
	return (StatusCode::FAILURE);
    }

    ATH_MSG_DEBUG ( "End of testEachCondObject " );
    return StatusCode::SUCCESS; 
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode 
LArConditionsTestAlg::testChannelSet()
{

    typedef LArConditionsChannelSet<LArRampComplete::LArCondObj>    ChanSet;
    typedef ChanSet::ConstChannelIt              ConstChannelIt;

    ATH_MSG_INFO ("in testChannelSet" );

    ChanSet chanSet;
    
    // corrections not always available:
    if (m_writeCorrections || m_applyCorrections) {
	// add corrections to channel set 
	for (unsigned int i = 0; i < m_rampCorrections.size(); ++i) {
	    // Must copy LArRampPTmp into a LArRampComplete::LArCondObj
	    LArRampComplete::LArCondObj ramp;
	    ramp.m_vRamp = m_rampCorrections[i].m_vRamp;
	    chanSet.insert(m_rampCorrections[i].m_channelID.get_identifier32().get_compact(), ramp);
	}
	// Now loop over corrections and check that they agree
	bool error = false;
	if (m_rampCorrections.size() != chanSet.size()) {
            ATH_MSG_ERROR ("Corrections not the same size as channel set: " 
                           << m_rampCorrections.size() << " " << chanSet.size() );
	    return (StatusCode::FAILURE);
	}
	else {
          ATH_MSG_DEBUG ("Sizes OK: "  << chanSet.size() );
	}

	ConstChannelIt it    = chanSet.begin();
	ConstChannelIt itEnd = chanSet.end();

	
	unsigned int i = 0;
	for (; it != itEnd; ++it, ++i) {

	    HWIdentifier id          = m_rampCorrections[i].m_channelID;
	    HWIdentifier id1((*it).first);
	    LArRampComplete::LArCondObj rampP           = (*it).second;
	    if (id != id1 || rampP != m_rampCorrections[i]) {
                ATH_MSG_ERROR ("Correction retrieved with iterator does not match: " 
                               << " i = " << i );
		error = true;
	    }
	    ATH_MSG_DEBUG ("New        : chan id, gain, ramps "
                           << m_onlineID->show_to_string(id1) << " " 
                           << m_rampCorrections[i].m_gain << " " 
                           << rampP.m_vRamp[0] << " " 
                           << rampP.m_vRamp[1] << " " 
                           << rampP.m_vRamp[2] << " " 
                           );
	    ATH_MSG_DEBUG ("Corrections: chan id, gain, ramps "
                           << m_onlineID->show_to_string(m_rampCorrections[i].m_channelID) << " " 
                           << m_rampCorrections[i].m_gain << " " 
                           << m_rampCorrections[i].m_vRamp[0] << " " 
                           << m_rampCorrections[i].m_vRamp[1] << " " 
                           << m_rampCorrections[i].m_vRamp[2] << " " 
                           << " Compare = " << (rampP == m_rampCorrections[i])
                           );
	}
	if (!error) {
          ATH_MSG_DEBUG ("Iteration check OK "  );
	}
	
	i = 0;
	for (; i < m_rampCorrections.size(); ++i) {

	    unsigned int id = m_rampCorrections[i].m_channelID.get_identifier32().get_compact();
	    it = chanSet.find(id);
	    if (it == itEnd) {
                ATH_MSG_ERROR ("Could not find correction: " 
                               << " i = " << i );
		error = true;
		ATH_MSG_DEBUG ("Corrections: cool chan, chan id, gain, ramps "
                               << m_onlineID->show_to_string(m_rampCorrections[i].m_channelID) << " " 
                               << m_rampCorrections[i].m_gain << " " 
                               << m_rampCorrections[i].m_vRamp[0] << " " 
                               << m_rampCorrections[i].m_vRamp[1] << " " 
                               << m_rampCorrections[i].m_vRamp[2] << " " 
                               );
	    }
	}
	if (!error) {
          ATH_MSG_DEBUG ("Find check OK "  );
	}

	if (error) {
            ATH_MSG_ERROR ("Failing check of channel set - see above" );
	    return (StatusCode::FAILURE);
	}
    }

    return StatusCode::SUCCESS; 

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode 
LArConditionsTestAlg::printCondObjects()
{
  ATH_MSG_INFO ("in printCondObjects()" );
  return StatusCode::SUCCESS; 
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode 
LArConditionsTestAlg::streamOutCondObjects()
{
  ATH_MSG_INFO ("in streamOutCondObjects()" );
  return StatusCode::SUCCESS; 
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode 
LArConditionsTestAlg::registerCondObjects()
{
  ATH_MSG_INFO ("in registerCondObjects()" );
  return StatusCode::SUCCESS; 
}



StatusCode  
LArConditionsTestAlg::testCallBack1( int& i , std::list<std::string>& l  ) 
{
  
    ATH_MSG_DEBUG ( " testing Call back function1 " );
    ATH_MSG_DEBUG(" int =  " << i );
    ATH_MSG_DEBUG(" list<string> size =  " << l.size() );

//     if(m_testCondObject){ 
// 	const ILArRamp* ramp = m_ramp; 
// 	log<< MSG::DEBUG<<" Pointer to Ramp = " << ramp <<endmsg;
//     }  
 
    return StatusCode::SUCCESS;
}


StatusCode  LArConditionsTestAlg::testCallBack2( int& i , std::list<std::string>& l  ) 
{
    ATH_MSG_DEBUG ( " testing Call back function2 " );
    ATH_MSG_DEBUG(" int =  " << i );
    ATH_MSG_DEBUG(" list<string> size =  " << l.size() );
    return StatusCode::SUCCESS;
}

StatusCode LArConditionsTestAlg::testFillIOVDb()
{
    IToolSvc* toolSvc = nullptr;
    ATH_CHECK( service("ToolSvc", toolSvc) );

//      IFillNovaIOVTool* fill; 
//      if(StatusCode::SUCCESS != toolSvc->retrieveTool("FillNovaIOVTool",fill ))
//  	{
//  	    log << MSG::ERROR << " Can't get FillNovaIOVTool " << endmsg;
//  	    return StatusCode::FAILURE;
//  	}

//      // Make a new ExampleData object 
//      ExampleData  v;

//      // prepare some fake data.
//      int n = 3 ;
//      for( int i = 0; i<n; ++i ) {
//  	ExampleData_t p ;
//  	p.chan_id = i;
//  	p.fArray[0]  = i*100+0;
//  	p.fArray[1]  = i*100+1;
//  	p.fArray[2]  = i*100+2;
//  	p.fArray[3]  = i*100+3;
//  	p.fArray[4]  = i*100+4;
//  	p.i = i;
//  	p.d = i;
//  	v.push_back(p);
//      }

//      typedef ExampleData::const_iterator IT;
//      IT it = v.begin();
//      void* vArray = (void*) &(*it);
  
//      typedef ExampleData::value_type STRUCT ;  
//      std::string typenm   = System::typeinfoName(typeid(STRUCT)) ;
//      std::string folder = "/lar/test/ExampleData1/ExampleDataName"; 
//      int nrow = v.size(); 

//      log<< MSG::DEBUG <<" writing " <<  typenm <<" through FillNovaIOVTool " <<endmsg;     
//      log<< MSG::DEBUG <<" to folder  " << folder <<endmsg;     
//      log<< MSG::DEBUG <<" number of rows  " << nrow <<endmsg; 

    // Fill object to Nova_IOV, using validity specified in the tool. 
//      StatusCode sc=  fill->fillNovaIOV(vArray, folder,typenm,nrow) ; 
//      if(!sc.isSuccess())  return sc ; 

//      // put the same reference in a different folder. 
//      std::string folder2 = "/lar/test/ExampleData2/ExampleDataName"; 
//      return fill->fillLastIOV(folder2); 

    return StatusCode::SUCCESS;
}




void LArConditionsTestAlg::handle ( const Incident& /* inc*/ )
{ // we get BeginRun incident here. 
    ATH_MSG_DEBUG(" in LArConditionsTestAlg::handle ");

    const LArEM_ID*  emid = nullptr;
    StatusCode sc = detStore()->retrieve( emid);
    if(sc!=StatusCode::SUCCESS){ 
        ATH_MSG_ERROR ( " Can not retrieve LArEM_ID" );
	return;
    }

    // retrieve DetDescrManager and LArCablingService
    IToolSvc* toolSvc = nullptr;
    if( service("ToolSvc", toolSvc) != StatusCode::SUCCESS){
        ATH_MSG_ERROR (" Failed to get ToolSvc" );
	// return StatusCode::FAILURE ; 
	return; 
    }


    if(StatusCode::SUCCESS != toolSvc->retrieveTool("LArCablingLegacyService",m_cablingSvc) ) {
        ATH_MSG_ERROR (" Failed to get LArCablingService" );
	// return StatusCode::FAILURE ; 
	return; 
    }



    // Testing mapping objects. 
    // LArFebRodMap
//      const LArFebRodMap * febRodMap;
//      sc=m_detStore->retrieve(febRodMap); 
//      if(sc!=StatusCode::SUCCESS){ 
//  	log<< MSG::ERROR<<" Can not find LArFedRodMap"<<endmsg;
//  	// return sc;
//      }

    // LArOnOffIdMap
//      const LArOnOffIdMap * onOffIdMap;
//      sc=m_detStore->retrieve(onOffIdMap); 
//      if(sc!=StatusCode::SUCCESS){ 
//  	log<< MSG::ERROR<<" Can not find LArOnOffIdMap"<<endmsg;
//  	// return sc;
//      }


//     if(m_testCondObjectNoReg){ 
// 	std::string key = "LArRamp";
// 	sc = m_detStore->retrieve(m_ramp, key);
// 	//if(m_ramp) {
// 	if(!sc.isSuccess()){
// 	    log<< MSG::ERROR<<" Failed to get LArRamp in handle " << endmsg;
// 	    return;
// 	    // return StatusCode::FAILURE ; 
// 	}
// 	else {
// 	    log<< MSG::DEBUG << "Handle: Retrieved LArRamp " << endmsg;
// 	}
//     } 


    // Access LArCablingService, which should use LArFebRodMap and LArOnOffIdMap.

    const std::vector<HWIdentifier>& roms = const_cast<LArCablingLegacyService*>(m_cablingSvc)->getLArRoModIDvec(); 
    ATH_MSG_DEBUG ( " Number of LArReadoutModuleIDs= " << roms.size() );

//    std::vector<HWIdentifier>::const_iterator it = febs.begin();
//    std::vector<HWIdentifier>::const_iterator it_end = febs.end();
    std::vector<HWIdentifier>::const_iterator it = m_onlineID->channel_begin();
    std::vector<HWIdentifier>::const_iterator it_end = m_onlineID->channel_end();

    int ntot = it_end-it; 
    ATH_MSG_DEBUG( " Total number of online channels from LArOnlineID " <<ntot);

    int nch = 0; 
    int nconnected = 0; 
    int nerr=0; 

    int n_err_uA2MeV=0;
    int n_err_DAC2uA=0;


    const ILArShape* pShape=0; 
    if(!m_shape) {
       // try retrieving it 
       sc = detStore()->retrieve(pShape);
       if (sc.isSuccess() ){
         ATH_MSG_INFO(" use ILArShape pointer retrieved from DetStore");
       } 
    } else 
       pShape = m_shape; 


    for(;it!=it_end;++it) {
//	HWIdentifier feb = *it;
//        for(int i=0;i<128;++i)
//	  {
//	   HWIdentifier sid =m_onlineID->channel_Id(feb,i);
	HWIdentifier sid = *it; 
	try { 

	    if(! m_cablingSvc->isOnlineConnected(sid))
		continue;

	    ++nconnected; 
	    Identifier id = m_cablingSvc->cnvToIdentifier(sid);
	    HWIdentifier sid2 =m_cablingSvc->createSignalChannelID(id);
	    ++nch ;
	    if( sid  !=sid2  ) { 		
              ATH_MSG_ERROR( " HWIdentifier mismatch,  sid "
		   <<" "<<sid<<" "<<m_onlineID->show_to_string(sid)
		   <<" offline id = "<< id <<" "<<m_onlineID->show_to_string(id) 
		   <<" sid2 = "<< sid2 <<" "<<m_onlineID->show_to_string(sid2) 
                             );
		++nerr; 
	    } 
	    else { // good identifier, test conditions objects

		// check Calibration Slot and Channel
		const std::vector<HWIdentifier>&
		    calib = const_cast<LArCablingLegacyService*>(m_cablingSvc)->calibSlotLine(sid) ; 
		if(calib.size()==0) {
                  ATH_MSG_ERROR( " No calibration for this channel,hdw id="
                                 <<sid.get_compact() );
		} 
		else {
                  ATH_MSG_VERBOSE( " Calib ID ="<<m_onlineID->show_to_string(calib[0])
                                   );
		}

 		if(m_testMC)  { 
		  ILArRamp::RampRef_t v = m_ramp->ADC2DAC(sid, 0 ); 

 		    if(v.size()<2) {
                      ATH_MSG_ERROR( " Failed to find ramp, hdw id = " 
                                     <<sid.get_compact() <<" "<< id.get_compact() );
 			emid->print(id); 
 		    } 

                    float f = m_uA2MeV->UA2MEV( sid ) ;
                    if(f == ILAruA2MeV::ERRORCODE) ++n_err_uA2MeV;
                    f = m_DAC2uA->DAC2UA( sid ) ;
                    if(f == ILArDAC2uA::ERRORCODE) ++n_err_DAC2uA;

  		    f = m_noise->noise( sid , 0 ) ; 

		    ILArAutoCorr::AutoCorrRef_t v2 = m_autoCorr->autoCorr(sid, 0 ); 
  		    if(v2.size()!=4) {
                      ATH_MSG_ERROR( " Failed to find AutoCorr, hdw id = " 
                                     <<sid.get_compact() );
 		    } 


  		    if(!pShape) { 
  			// log<< MSG::ERROR<< " ILArShape invalid " << endmsg;
  			// skip if LArShape is not valid
  		    } 
 		    else {
                        ILArShape::ShapeRef_t vShape=pShape->Shape(sid, 0,m_tbin);
                        ILArShape::ShapeRef_t vShapeDer=pShape->ShapeDer(sid, 0,m_tbin );
  			if(vShape.size() ==0 || vShapeDer.size() == 0 ) { 
                          ATH_MSG_ERROR( " Failed to get Shape or ShapeDer,  hdw id = " 
                                         <<sid.get_compact() << " size = " << vShape.size() << " " << vShapeDer.size());
  			}
                        else {
                          if (msgLvl (MSG::VERBOSE)) {
                            msg()<<MSG::VERBOSE<< " hdw id "<<sid.get_compact() <<endmsg;
                            msg()<<MSG::VERBOSE<<" Shape= " ;
                            for (unsigned int i=0;i<vShape.size();++i){
                              msg()<<" " << vShape[i] ; 
                            }
                            msg()<<endmsg;
                            msg()<<MSG::VERBOSE<<" ShapeDer=" ;
                            for (unsigned int i=0;i<vShapeDer.size();++i){
                              msg()<<" " << vShapeDer[i] ; 
                            }
                            msg()<<endmsg;
                          }
			}

 		    } 

  				// pedestal 
  		    float vPed = m_pedestal->pedestal( sid,0  ) ; 
 		    if(vPed <= (1.0+LArElecCalib::ERRORCODE)) {
                      ATH_MSG_ERROR( " Failed to find pedestal, hdw id = " 
                                     <<sid.get_compact() );
  		    } 

  				// fSampl
  		    float fs = m_fSampl->FSAMPL( sid  ) ; 
  		    if( fs==0 ) {
                      ATH_MSG_ERROR( " Failed to find fSampl, hdw id = " 
                                     <<sid.get_compact() );
 		    } 

 				// MinBias
  		    float mbs = m_minBias->minBiasRMS( sid  ) ; 
 		    if( mbs==0 ) {
                      ATH_MSG_ERROR( " Failed to find MinBias, hdw id = " 
                                     <<sid.get_compact() );
 		    } 

		}// end of m_testMC


	    } 
	} 
	catch (LArID_Exception& except) {
	    // this is allowed.
	    std::string err = m_onlineID->print_to_string(sid); 
	    ATH_MSG_VERBOSE( (std::string)except << sid.get_identifier32().get_compact());
	    ATH_MSG_VERBOSE( err );
	} 
// 	  }
    }
    ATH_MSG_DEBUG (" Number of Connected Channel ID = " <<nconnected );
    ATH_MSG_DEBUG (" Number of Valid Channel ID = " <<nch );
    if(nerr>0) ATH_MSG_ERROR (" Number channels with incorrect mapping= " <<nerr );

    if (n_err_uA2MeV!=0) 
      ATH_MSG_DEBUG (" Number of channels without uA2MeV "<<n_err_uA2MeV);
    if (n_err_DAC2uA!=0) 
      ATH_MSG_DEBUG (" Number of channels without DAC2uA"<<n_err_DAC2uA);
// test DCS data 
//    testDCS_Objects() ;

    return; 
} 



StatusCode LArConditionsTestAlg::testDbObjectRead()
{

    typedef LArRampMC::CONTAINER  CONTAINER; 
    typedef CONTAINER::Subset     Subset;

    const LArRampMC* ramp = 0 ;
    ATH_CHECK( detStore()->retrieve(ramp, "LArRamp") );
  
    ATH_MSG_DEBUG ( " Found LArRampMC, key LArRamp." );

    // Print out channels
    ATH_MSG_DEBUG ( " Number of channels " << ramp->chan_size() );

    // Print out first 10 elements of each gain for subset
    CONTAINER::chan_const_iterator   chanIt  = ramp->chan_begin();
    CONTAINER::chan_const_iterator   endChan = ramp->chan_end  ();
    for (unsigned int i = 0; chanIt != endChan; ++chanIt, ++i) {
	unsigned int coolChan = *chanIt;
	const Subset* subset = ramp->at(i);

	ATH_MSG_DEBUG ( " Channel " << coolChan << " "
                        << " Subset size " << subset->subsetSize() 
                        << " gain, channel, grouping type " << subset->gain() << " "
                        << MSG::hex << subset->channel() << " "  << MSG::dec
                        << subset->groupingType() << " "
                        );

	Subset::ConstSubsetIt   first = subset->subsetBegin();
	Subset::ConstSubsetIt   last  = subset->subsetEnd();
	//for (int i = 0; i < 10 && first != last; ++i, ++first) {
	for (; first != last; ++first) {

	    // select non-zero subsets
	    if ((*first).second.size()) {

              ATH_MSG_DEBUG ( " FEB id " 
                              << m_onlineID->show_to_string(HWIdentifier((*first).first)) << " " 
                              );
		for (unsigned int k = 0; k < 5; ++k) {
                    msg() << MSG::DEBUG << " vramp " ;
//			<< m_onlineID->show_to_string((*first).second[k].m_channelID) << " " 
//			<< (*first).second[k].m_gain << " ";
		    for (unsigned int j = 0; j < (*first).second[k].m_vRamp.size(); ++j) {
                      msg() << MSG::DEBUG << (*first).second[k].m_vRamp[j] << " ";
		    }
		    msg() << MSG::DEBUG << endmsg;
		}

	    }

	}
    }
    

    /*

    // Print out first 10 elements of each gain for corrections
    for (unsigned int gain = 0; gain < 3; ++gain) {
	log << MSG::DEBUG << " Gain, size " 
	    << gain << " "
	    << ramp->correctionsSize(gain) << endmsg;
	CONTAINER::ConstCorrectionIt  first = ramp->correctionsBegin(gain);
	CONTAINER::ConstCorrectionIt  last  = ramp->correctionsEnd(gain);
	for (int i = 0; i < 10 && first != last; ++i, ++first) {
	    log << MSG::DEBUG << " id, vramp " 
		<< m_onlineID->show_to_string(HWIdentifier((*first).first)) << " ";
//		<< m_onlineID->show_to_string((*first).second.m_channelID) << " " 
//		<< (*first).second.m_gain << " ";
	    for (unsigned int j = 0; j < (*first).second.m_vRamp.size(); ++j) {
		log << MSG::DEBUG << (*first).second.m_vRamp[j] << " ";
	    }
	    log << MSG::DEBUG << endmsg;
	}
    }

    */

    return StatusCode::SUCCESS; 

}


StatusCode LArConditionsTestAlg::testDCS_Objects()
{

//      StatusCode sc;
//      MsgStream log(msgSvc(), name());
//      log << MSG::INFO <<"in testDCS_Objects()" <<endmsg;

//      log << MSG::DEBUG << "RETRIEVING ALL CONDDBMYSQLOBJECT" << endmsg;

//      const DataHandle<GenericDbTable> dh_b, dh_e;

//      sc = m_detStore->retrieve(dh_b, dh_e);
//      if (sc.isFailure()) {
//  	log << MSG::WARNING <<"Could not find GenericDbTable DetectorStore" <<endmsg;
//  	return( StatusCode::SUCCESS);
//      }

//      for(; dh_b!=dh_e;++dh_b) {
//  	const GenericDbTable* obj1 = dh_b;

//  	//lets dump the object contents
//  	int ncolumns = obj1->getNumColumns();
//  	int nrows    = obj1->getNumRows();

//  	log << MSG::INFO << " Object " << dh_b.ID() << " has "
//  	    << obj1->getNumColumns() << " parameters and "
//  	    << obj1->getNumRows() << " objects" << endmsg;

//  	std::vector<std::string> dNames;
//  	std::vector<GenericDbTable::dataTypes> dTypes;

//  	obj1->getNames(dNames);
//  	obj1->getTypes(dTypes);

//  	if (!(dNames.size()))
//  	    {
//  		log << MSG::FATAL << "Esta tabela nao tem nada (This Table hasnothing =)" << endmsg;
//  	    }

//  	log << MSG::INFO << "X" << dNames.size() <<"X The parameters name/types are: |";
//  	for (unsigned int i=0; i< dNames.size(); i++) { 
//  	    log << dNames[i] << " / ";
//  	    log << dTypes[i] << " | ";
//  	    log << endmsg;
//  	} 
//  	printTable(obj1);

//      }

    return StatusCode::SUCCESS;
}

//  void LArConditionsTestAlg::printTable(const GenericDbTable *table)
//  {
//      //********* Function that prints the ICondDBTable table ******//
//      //    It uses the proper methos to access to the data 
//      //    that is in memory
//      MsgStream log(msgSvc(), name());


//      int ncolumns = table->getNumColumns();
//      int nrows    = table->getNumRows();

//    // Now we want to handle NULL values
//      std::string nullVal;


//      if (ncolumns && nrows) {
//  	std::vector<std::string> names;                     //A vector for table names
//  	std::vector<GenericDbTable::dataTypes> types;    //A vector for table types

//  	table->getNames(names);                   //Retrieving column names
//  	table->getTypes(types);                   //Retrieving column types
      
//  	log << MSG::INFO  << "-------------------  Table BEGIN  -------------" << endmsg;
//  	log << MSG::INFO  << "Table [" << table->getNumRows() << "]x[" << table->getNumColumns() << "]" << endmsg; 
 
//  	for (unsigned int i=0; i< table->getNumRows(); i++){  //for each row, 

	    
//  	    log << MSG::INFO << "{ Row  " << i+1 << " Begin }" << endmsg;
	
//  	    std::vector<std::string> tmpStr;        // temporary vector for row storage
	
//  	    table->getRow(i, tmpStr);
	
//  	    long pos=0;                  //Set the position in the 
//  	    //tmpSrt vector
	
//  	    for (unsigned columnNumber=0;columnNumber<names.size();columnNumber++){
//  		log << MSG::INFO << "Column name: " << names[columnNumber]  << endmsg;
	  
//  		table->getNull(columnNumber, nullVal);
//  		int n_vals;
//  		if (GenericDbTable::kLongLong <types[columnNumber]){           //If the values in 
//  		    //the column cells are 
//  		    n_vals=atol(tmpStr[pos].c_str()); //arrays of some type
//  		    pos++;                            //Incrementing the 
//  		    //position 
//  		    //in the tmpStr vector
//  		}
//  		else {
//  		    n_vals=1;     //Otherwise, the number of values is only 1
//  		}
		
//  		log << "Data in cell ["<<i+1<< "]x["<<columnNumber+1<< "]: ";
//  		//printing the data for each cell
//  		for (long c=pos; c< pos+n_vals; c++){
//  		    if (tmpStr[c] != nullVal) {
//  			log << tmpStr[c];
//  		    }
//  		    else {
//  			log << "NULL";
//  		    }
//  		    if (n_vals!=1) log << " ; ";
//  		}
	  
//  		pos+=n_vals;        //Incrementing the position in the 
//  		//tmpStr vector
//  		log << endmsg;
//  	    }
//  	    log << MSG::INFO << "{ Row  " << i +1  << " End }" << endmsg << endmsg;
//  	}
//  	log << MSG::INFO << "------------------ Table END  --------------------" << endmsg;
      
//      }
//      else {
//  	log << MSG::INFO << "Table empty" << endmsg;
//      }
    
//  }














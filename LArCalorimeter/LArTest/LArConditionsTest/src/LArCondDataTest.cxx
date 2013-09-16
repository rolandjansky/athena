/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArCondDataTest.cxx
 *
 * @brief This file contains an algorithm for testing lar conditions
 * data access
 *
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 * @author Hong Ma      <hma@bnl.gov>
 *
 * $Id: LArCondDataTest.cxx,v 1.7 2009-04-27 15:53:09 gunal Exp $ */

#include "LArConditionsTest/LArCondDataTest.h"

#include "StoreGate/StoreGateSvc.h"

#include "LArTools/LArCablingService.h" 
#include "LArIdentifier/LArOnlineID.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include "LArElecCalib/ILArShape.h" 
#include "LArElecCalib/ILArNoise.h" 
#include "LArElecCalib/ILArRamp.h" 
#include "LArElecCalib/ILArDAC2uA.h" 
#include "LArElecCalib/ILAruA2MeV.h" 
#include "LArElecCalib/ILArAutoCorr.h" 
#include "LArElecCalib/ILArPedestal.h" 
#include "LArElecCalib/ILArfSampl.h" 
#include "LArElecCalib/ILArMinBias.h" 
#include "LArElecCalib/ILArOFC.h" 

#include "LArRawConditions/LArRampMC.h" 
#include "LArRawConditions/LArShapeMC.h" 
#include "LArRawConditions/LArConditionsChannelSet.h" 


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

LArCondDataTest::LArCondDataTest(const std::string& name, ISvcLocator* pSvcLocator) :
	Algorithm(name,pSvcLocator),
	m_detStore(0),
	m_cablingSvc(0),
	m_onlineID(0),
        m_emid(0),
        m_tbin(0),m_fixShape(false)

{
    // switch for testing Filling IOV. 
    declareProperty("Tbin",         m_tbin) ;
    declareProperty("FixShape",         m_fixShape) ;
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////

LArCondDataTest::~LArCondDataTest()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
/////////////////////////////////////////////////////////////////////

StatusCode LArCondDataTest::initialize()
{
    MsgStream  log(messageService(),name());


    // Pointer to StoreGate (cached)
    StatusCode sc = service("DetectorStore", m_detStore);
    if (sc.isFailure()) {
	log << MSG::ERROR
	    << "Unable to retrieve pointer to DetectorStore "
	    << endreq;
	return sc;
    }
    log << MSG::DEBUG << "Retrieved DetectorStore" << endreq;

    // retrieve LArOnlineID
    sc = m_detStore->retrieve(m_onlineID); 
    if (sc.isFailure()) {
 	log << MSG::ERROR
 	    << "Unable to retrieve pointer to LArOnlineID  "
 	    << endreq;
 	return sc;
    }
    log << MSG::DEBUG << "Retrieved LArOnlineID" << endreq;

    IToolSvc* toolSvc;
    if( service("ToolSvc", toolSvc) != StatusCode::SUCCESS){
	log<< MSG::ERROR <<" Failed to get ToolSvc" <<endreq;     
	return StatusCode::FAILURE; 
    }

    if(StatusCode::SUCCESS != toolSvc->retrieveTool("LArCablingService",m_cablingSvc) ) {
	log<< MSG::ERROR <<" Failed to get LArCablingService" <<endreq;     
	// return StatusCode::FAILURE ; 
	return StatusCode::FAILURE; 
    }

 
    sc = m_detStore->retrieve( m_emid);
    if(sc!=StatusCode::SUCCESS){ 
	log << MSG::ERROR << " Can not retrieve LArEM_ID" << endreq;
	return StatusCode::FAILURE; 
    }

    log << MSG::DEBUG << "initialize done" <<endreq; 

    return StatusCode::SUCCESS;

}

/////////////////////////////////////////////////////////////////////
// EXECUTE:
/////////////////////////////////////////////////////////////////////

StatusCode LArCondDataTest::execute()
{
    MsgStream  log(messageService(),name());  

    StatusCode sc; 

    log<< MSG::DEBUG<<" LArCondDataTest::execute() "<<endreq;


    // Access LArCablingService, which should use LArFebRodMap and LArOnOffIdMap.

    const std::vector<HWIdentifier>& roms = m_cablingSvc->getLArRoModIDvec(); 
    log << MSG::DEBUG << " Number of LArReadoutModuleIDs= " << roms.size() << endreq;

    std::vector<HWIdentifier>::const_iterator it = m_onlineID->channel_begin();
    std::vector<HWIdentifier>::const_iterator it_end = m_onlineID->channel_end();

    int ntot = it_end-it; 
    log<< MSG::DEBUG<< " Total number of online channels from LArOnlineID "
       <<ntot << endreq;

    int nch = 0; 
    int nconnected = 0; 
    int nerr=0; 

    int n_err_uA2MeV=0;
    int n_err_DAC2uA=0;


    const ILArShape* pShape=0; 
    sc = m_detStore->retrieve(pShape);
    if (!sc.isSuccess() ){
	log<<MSG::ERROR <<" Failed to retrieve ILArShape from DetStore"<<endreq;
    } 


    const ILArNoise* pNoise=0; 
    sc = m_detStore->retrieve(pNoise);
    if (!sc.isSuccess() ){
	log<<MSG::ERROR <<" Failed to retrieve ILArNoise from DetStore"<<endreq;
    } 


    const ILArRamp* pRamp=0; 
    sc = m_detStore->retrieve(pRamp);
    if (!sc.isSuccess() ){
	log<<MSG::ERROR <<" Failed to retrieve ILArRamp from DetStore"<<endreq;
    } 

    const ILArDAC2uA* pDAC2uA=0; 
    sc = m_detStore->retrieve(pDAC2uA);
    if (!sc.isSuccess() ){
	log<<MSG::ERROR <<" Failed to retrieve ILArDAC2uA from DetStore"<<endreq;
    } 

    const ILAruA2MeV* puA2MeV=0; 
    sc = m_detStore->retrieve(puA2MeV);
    if (!sc.isSuccess() ){
	log<<MSG::ERROR <<" Failed to retrieve ILAruA2MeV from DetStore"<<endreq;
    } 

    const ILArAutoCorr* pAutoCorr=0; 
    sc = m_detStore->retrieve(pAutoCorr);
    if (!sc.isSuccess() ){
	log<<MSG::ERROR <<" Failed to retrieve ILArAutoCorr from DetStore"<<endreq;
    } 

    const ILArPedestal* pPedestal=0; 
    sc = m_detStore->retrieve(pPedestal);
    if (!sc.isSuccess() ){
	log<<MSG::ERROR <<" Failed to retrieve ILArPedestal from DetStore"<<endreq;
    } 

    const ILArfSampl* pfSampl=0; 
    sc = m_detStore->retrieve(pfSampl);
    if (!sc.isSuccess() ){
	log<<MSG::ERROR <<" Failed to retrieve ILArfSampl from DetStore"<<endreq;
    } 


    const ILArMinBias* pMinBias=0; 
    sc = m_detStore->retrieve(pMinBias);
    if (!sc.isSuccess() ){
	log<<MSG::ERROR <<" Failed to retrieve ILArMinBias from DetStore"<<endreq;
    } 


    for(;it!=it_end;++it) {
	HWIdentifier sid = *it; 
	try { 

	    if(! m_cablingSvc->isOnlineConnected(sid))
		continue;

	    if(m_onlineID->barrel_ec(sid)==1)
	     {// endcap 
	        if(m_onlineID->feedthrough(sid)==6)
		 { // fcal
		   log<< MSG::DEBUG<< " FCAL  sid "<<sid<<endreq;
		   m_emid->print(sid);
		 }
	     }
	
	    ++nconnected; 
	    Identifier id = m_cablingSvc->cnvToIdentifier(sid);
	    HWIdentifier sid2 =m_cablingSvc->createSignalChannelID(id);
	    ++nch ;
	    if( sid  !=sid2  ) { 		
		log<< MSG::ERROR<< " HWIdentifier mismatch,  sid "
		   <<" "<<sid<<" "<<m_onlineID->show_to_string(sid)
		   <<" offline id = "<< id <<" "<<m_onlineID->show_to_string(id) 
		   <<" sid2 = "<< sid2 <<" "<<m_onlineID->show_to_string(sid2) 
		   <<endreq;
		++nerr; 
	    } 
	    else { // good identifier, test conditions objects

		// check Calibration Slot and Channel
		const std::vector<HWIdentifier>&
		    calib = m_cablingSvc->calibSlotLine(sid) ; 
		if(calib.size()==0) {
		    log<< MSG::ERROR<< " No calibration for this channel,hdw id="
		       <<sid.get_compact() << endreq;
		} 
		else {
		    log<< MSG::VERBOSE<< " Calib ID ="<<m_onlineID->show_to_string(calib[0])
		       <<endreq;
		}

		const ILArRamp::RampRef_t v = pRamp->ADC2DAC(sid, 0 ); 

 		if(v.size()!=3) {
 			log<< MSG::ERROR<< " Failed to find ramp, hdw id = " 
 			   <<sid.get_compact() <<" "<< id.get_compact() << endreq;
 			m_emid->print(id); 
 		} 

                float f = puA2MeV->UA2MEV( sid ) ;
                if(f == ILAruA2MeV::ERRORCODE) 
		 {
 			log<< MSG::ERROR<< " Failed to find uA2MeV, hdw id = " 
 			   <<sid.get_compact() << endreq;
			++n_err_uA2MeV;
		 }
                f = pDAC2uA->DAC2UA( sid ) ;
                if(f == ILArDAC2uA::ERRORCODE) 
		 {
 			log<< MSG::ERROR<< " Failed to find DAC2uA, hdw id = " 
 			   <<sid.get_compact() << endreq;	
			++n_err_DAC2uA;
		 }
  		f = pNoise->noise( sid , 0 ) ; 
		if( f == ILArNoise::ERRORCODE ) 
		{
 			log<< MSG::ERROR<< " Failed to find Noise, hdw id = " 
 			   <<sid.get_compact() << endreq;
		}

		ILArAutoCorr::AutoCorrRef_t v2 = pAutoCorr->autoCorr(sid, 0 ); 
  		if(v2.size()!=4) {
 			log<< MSG::ERROR<< " Failed to find AutoCorr, hdw id = " 
 			   <<sid.get_compact() << endreq;
 		} 


                ILArShape::ShapeRef_t vShape=pShape->Shape(sid, 0,m_tbin);
                ILArShape::ShapeRef_t vShapeDer=pShape->ShapeDer(sid, 0,m_tbin );
		if(vShape.size() ==0 || vShapeDer.size() == 0 ) { 
 			    log<< MSG::ERROR<< " Failed to get Shape or ShapeDer,  hdw id = " 
 			       <<sid.get_compact() << " size = " << vShape.size() << " " << vShapeDer.size()<< endreq;
		} else 
		{
			  log<<MSG::VERBOSE<< " hdw id "<<sid.get_compact() <<endreq;
		          log<<MSG::VERBOSE<<" Shape= " ;
			  for (unsigned int i=0; i < vShape.size(); ++i){
			    log<<" " << vShape[i] ; 
			  }
			  log<<endreq;
		          log<<MSG::VERBOSE<<" ShapeDer=" ;
			  for (unsigned int i=0; i < vShapeDer.size(); ++i){
			    log<<" " << vShapeDer[i] ; 
			  }
			  log<<endreq;

		}


  		// pedestal 
  		float vPed = pPedestal->pedestal( sid,0  ) ; 
 		if(vPed <= (1.0+LArElecCalib::ERRORCODE)) {
  			log<< MSG::ERROR<< " Failed to find pedestal, hdw id = " 
  			   <<sid.get_compact() << endreq;
  		} 

  				// fSampl
  		float fs = pfSampl->FSAMPL( sid  ) ; 
  	        log<<MSG::VERBOSE<< " fsample and hdw id ="<<fs<<" " <<sid.get_compact() <<endreq;

  		if( fs==ILArfSampl::ERRORCODE ) {
  			log<< MSG::ERROR<< " Failed to find fSampl, hdw id = " 
  			   <<sid.get_compact() << endreq;
 		} 

 		// MinBias
  		float mbs = pMinBias->minBiasRMS( sid  ) ; 
 		    if( mbs== ILArMinBias::ERRORCODE) {
 			log<< MSG::ERROR<< " Failed to find MinBias, hdw id = " 
 			   <<sid.get_compact() << endreq;
 		    } 

	    } 
	} 
	catch (LArID_Exception& except) {
	    // this is allowed.
	    std::string err = m_onlineID->print_to_string(sid); 
	    log<< MSG::VERBOSE<< (std::string)except << sid.get_identifier32().get_compact()<<endreq;
	    log<< MSG::VERBOSE<< err <<endreq;
	} 
    }
    log<< MSG::DEBUG <<" Number of Connected Channel ID = " <<nconnected <<endreq;
    log<< MSG::DEBUG <<" Number of Valid Channel ID = " <<nch <<endreq;
    if(nerr>0) log<< MSG::ERROR <<" Number channels with incorrect mapping= " <<nerr <<endreq;

    if (n_err_uA2MeV!=0) 
	log <<MSG::DEBUG <<" Number of channels without uA2MeV "<<n_err_uA2MeV<<endreq;
    if (n_err_DAC2uA!=0) 
	log <<MSG::DEBUG <<" Number of channels without DAC2uA"<<n_err_DAC2uA<<endreq;


    return StatusCode::SUCCESS;
}


/////////////////////////////////////////////////////////////////////
// FINALIZE:
// Note that it is NOT NECESSARY to run the finalize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////

StatusCode LArCondDataTest::finalize()
{
  MsgStream  log(messageService(),name());

  StatusCode sc;

  log<<MSG::DEBUG << " finalize "<<endreq;

  if(!m_fixShape)
    return StatusCode::SUCCESS; 

  log<<MSG::DEBUG << " fix LArShapeMC "<<endreq;

  const LArShapeMC* shape ;
  sc = m_detStore->retrieve(shape);
  if(!sc.isSuccess()){
	log<<MSG::ERROR<<" Fail to retrieve LArShapeMC" <<endreq; 
        return StatusCode::FAILURE; 
  }


  LArShapeMC* newShape= new LArShapeMC();
  newShape->setGroupingType( LArShapeMC::SingleGroup );
  sc = newShape->initialize(); 

  if(!sc.isSuccess()){
	log<<MSG::ERROR<<" Fail to initialize LArShapeMC" <<endreq; 
        return StatusCode::FAILURE; 
  }

  sc = m_detStore->record(newShape,"LArFullShape"); 
  if(!sc.isSuccess()){
	log<<MSG::ERROR<<" record LArShapeMC" <<endreq; 
        return StatusCode::FAILURE; 
  }

  for(unsigned int g=0;g<3;++g)
  {
     LArShapeMC::ConstConditionsMapIterator  it = shape->begin( g )  ; 
     LArShapeMC::ConstConditionsMapIterator  it_e = shape->end( g )  ; 

     for(;it!=it_e;++it)
	{
	 LArShapeP1  t = *it; 

         unsigned int feb_id = it.getFebId();
         unsigned int chan_id = it.getChannel();
         HWIdentifier hw_chan_id =
	  m_onlineID->channel_Id(HWIdentifier(feb_id),chan_id);

	 if(hw_chan_id!=it.channelId()) {
	  log<<MSG::DEBUG<< " no match "<<endreq;
	  m_onlineID->print(hw_chan_id);
	  m_onlineID->print(it.channelId());
	  continue ;  
	 } 

         HWIdentifier hw_id = it.channelId(); 
          HWIdentifier new_id = hw_id;
          if(m_onlineID->barrel_ec(hw_id)==1)
	  { // endcap 
	   int ft = m_onlineID->feedthrough(hw_id) ; 
	   if(ft==6)
	     { // FCAL ft
	     int slot = m_onlineID->slot(hw_id);
	     if(slot>=8)
		{ // this one needs fix
		 std::cout<<" Fixin id , old and new "<<std::endl;
		 m_onlineID->print(hw_id);
	 	 int pn = m_onlineID->pos_neg(hw_id);
	 	 int chan = m_onlineID->channel(hw_id);
		 slot = slot +1 ;	
		 new_id = m_onlineID->channel_Id(1,pn,ft,slot,chan);
		 m_onlineID->print( new_id );
		}
	      }
	   }
           
	  newShape->setPdata(it.channelId(),t,g);
	}     

  }


  return StatusCode::SUCCESS; 

} 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

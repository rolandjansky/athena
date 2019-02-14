/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
	AthAlgorithm(name,pSvcLocator),
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
    ATH_CHECK( detStore()->retrieve(m_onlineID) );
    ATH_CHECK( detStore()->retrieve( m_emid) );
    ATH_CHECK( m_cablingKey.initialize() );
    ATH_CHECK( m_CLKey.initialize() );
    ATH_CHECK( m_RodKey.initialize() );

    ATH_MSG_DEBUG ( "initialize done" );
    return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////
// EXECUTE:
/////////////////////////////////////////////////////////////////////

StatusCode LArCondDataTest::execute()
{
     ATH_MSG_DEBUG(" LArCondDataTest::execute() ");

     SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
     const LArOnOffIdMapping* cabling{*cablingHdl};
     if(!cabling){
        ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key() );
        return StatusCode::FAILURE;
     }
     SG::ReadCondHandle<LArCalibLineMapping> clHdl{m_CLKey};
     const LArCalibLineMapping *clCont {*clHdl};
     if(!clCont){
        ATH_MSG_ERROR("Do not have calib mapping object " << m_CLKey.key() );
        return StatusCode::FAILURE;
     }
     SG::ReadCondHandle<LArFebRodMapping> rodHdl{m_RodKey};
     const LArFebRodMapping *rodCont {*rodHdl};
     if(!rodCont){
        ATH_MSG_ERROR("Do not have ROD mapping object " << m_RodKey.key() );
        return StatusCode::FAILURE;
     }

    const std::vector<HWIdentifier>& roms = rodCont->getLArRoModIDvec(); 
    ATH_MSG_DEBUG ( " Number of LArReadoutModuleIDs= " << roms.size() );

    std::vector<HWIdentifier>::const_iterator it = m_onlineID->channel_begin();
    std::vector<HWIdentifier>::const_iterator it_end = m_onlineID->channel_end();

    int ntot = it_end-it; 
    ATH_MSG_DEBUG( " Total number of online channels from LArOnlineID "
                   <<ntot );

    int nch = 0; 
    int nconnected = 0; 
    int nerr=0; 

    int n_err_uA2MeV=0;
    int n_err_DAC2uA=0;


    const ILArShape* pShape=0; 
    StatusCode sc = detStore()->retrieve(pShape);
    if (!sc.isSuccess() ){
      ATH_MSG_ERROR (" Failed to retrieve ILArShape from DetStore");
    } 


    const ILArNoise* pNoise=0; 
    sc = detStore()->retrieve(pNoise);
    if (!sc.isSuccess() ){
      ATH_MSG_ERROR (" Failed to retrieve ILArNoise from DetStore");
    } 


    const ILArRamp* pRamp=0; 
    sc = detStore()->retrieve(pRamp);
    if (!sc.isSuccess() ){
      ATH_MSG_ERROR (" Failed to retrieve ILArRamp from DetStore");
    } 

    const ILArDAC2uA* pDAC2uA=0; 
    sc = detStore()->retrieve(pDAC2uA);
    if (!sc.isSuccess() ){
      ATH_MSG_ERROR (" Failed to retrieve ILArDAC2uA from DetStore");
    } 

    const ILAruA2MeV* puA2MeV=0; 
    sc = detStore()->retrieve(puA2MeV);
    if (!sc.isSuccess() ){
      ATH_MSG_ERROR (" Failed to retrieve ILAruA2MeV from DetStore");
    } 

    const ILArAutoCorr* pAutoCorr=0; 
    sc = detStore()->retrieve(pAutoCorr);
    if (!sc.isSuccess() ){
      ATH_MSG_ERROR (" Failed to retrieve ILArAutoCorr from DetStore");
    } 

    const ILArPedestal* pPedestal=0; 
    sc = detStore()->retrieve(pPedestal);
    if (!sc.isSuccess() ){
      ATH_MSG_ERROR (" Failed to retrieve ILArPedestal from DetStore");
    } 

    const ILArfSampl* pfSampl=0; 
    sc = detStore()->retrieve(pfSampl);
    if (!sc.isSuccess() ){
      ATH_MSG_ERROR (" Failed to retrieve ILArfSampl from DetStore");
    } 


    const ILArMinBias* pMinBias=0; 
    sc = detStore()->retrieve(pMinBias);
    if (!sc.isSuccess() ){
      ATH_MSG_ERROR (" Failed to retrieve ILArMinBias from DetStore");
    } 


    for(;it!=it_end;++it) {
	HWIdentifier sid = *it; 
	try { 

	    if(! cabling->isOnlineConnected(sid))
		continue;

	    if(m_onlineID->barrel_ec(sid)==1)
	     {// endcap 
	        if(m_onlineID->feedthrough(sid)==6)
		 { // fcal
		   ATH_MSG_DEBUG( " FCAL  sid "<<sid);
		   m_emid->print(sid);
		 }
	     }
	
	    ++nconnected; 
	    Identifier id = cabling->cnvToIdentifier(sid);
	    HWIdentifier sid2 =cabling->createSignalChannelID(id);
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
		    calib = clCont->calibSlotLine(sid) ; 
		if(calib.size()==0) {
                  ATH_MSG_ERROR( " No calibration for this channel,hdw id="
                                 <<sid.get_compact() );
		} 
		else {
                  ATH_MSG_VERBOSE( " Calib ID ="<<m_onlineID->show_to_string(calib[0]) );
		}

		const ILArRamp::RampRef_t v = pRamp->ADC2DAC(sid, 0 ); 

 		if(v.size()!=3) {
                  ATH_MSG_ERROR( " Failed to find ramp, hdw id = " 
                                 <<sid.get_compact() <<" "<< id.get_compact() );
                  m_emid->print(id); 
 		} 

                float f = puA2MeV->UA2MEV( sid ) ;
                if(f == ILAruA2MeV::ERRORCODE) 
		 {
                        ATH_MSG_ERROR( " Failed to find uA2MeV, hdw id = " 
                                       <<sid.get_compact() );
			++n_err_uA2MeV;
		 }
                f = pDAC2uA->DAC2UA( sid ) ;
                if(f == ILArDAC2uA::ERRORCODE) 
		 {
                        ATH_MSG_ERROR( " Failed to find DAC2uA, hdw id = " 
                                       <<sid.get_compact() );
			++n_err_DAC2uA;
		 }
  		f = pNoise->noise( sid , 0 ) ; 
		if( f == ILArNoise::ERRORCODE ) 
		{
                  ATH_MSG_ERROR( " Failed to find Noise, hdw id = " 
                                 <<sid.get_compact() );
		}

		ILArAutoCorr::AutoCorrRef_t v2 = pAutoCorr->autoCorr(sid, 0 ); 
  		if(v2.size()!=4) {
                  ATH_MSG_ERROR( " Failed to find AutoCorr, hdw id = " 
                                 <<sid.get_compact() );
 		} 


                ILArShape::ShapeRef_t vShape=pShape->Shape(sid, 0,m_tbin);
                ILArShape::ShapeRef_t vShapeDer=pShape->ShapeDer(sid, 0,m_tbin );
		if(vShape.size() ==0 || vShapeDer.size() == 0 ) { 
                  ATH_MSG_ERROR( " Failed to get Shape or ShapeDer,  hdw id = " 
                                 <<sid.get_compact() << " size = " << vShape.size() << " " << vShapeDer.size());
		} else 
		{
                  if (msgLvl(MSG::VERBOSE)) {
                    msg()<<MSG::VERBOSE<< " hdw id "<<sid.get_compact() <<endmsg;
                    msg()<<MSG::VERBOSE<<" Shape= " ;
                    for (unsigned int i=0; i < vShape.size(); ++i){
                      msg()<<" " << vShape[i] ; 
                    }
                    msg()<<endmsg;
                    msg()<<MSG::VERBOSE<<" ShapeDer=" ;
                    for (unsigned int i=0; i < vShapeDer.size(); ++i){
                      msg()<<" " << vShapeDer[i] ; 
                    }
                    msg()<<endmsg;
                  }
		}


  		// pedestal 
  		float vPed = pPedestal->pedestal( sid,0  ) ; 
 		if(vPed <= (1.0+LArElecCalib::ERRORCODE)) {
                  ATH_MSG_ERROR( " Failed to find pedestal, hdw id = " 
                                 <<sid.get_compact() );
  		} 

  				// fSampl
  		float fs = pfSampl->FSAMPL( sid  ) ; 
  	        ATH_MSG_VERBOSE( " fsample and hdw id ="<<fs<<" " <<sid.get_compact() );

  		if( fs==ILArfSampl::ERRORCODE ) {
                  ATH_MSG_ERROR( " Failed to find fSampl, hdw id = " 
                                 <<sid.get_compact() );
 		} 

 		// MinBias
  		float mbs = pMinBias->minBiasRMS( sid  ) ; 
 		    if( mbs== ILArMinBias::ERRORCODE) {
                      ATH_MSG_ERROR( " Failed to find MinBias, hdw id = " 
                                     <<sid.get_compact() );
 		    } 

	    } 
	} 
	catch (LArID_Exception& except) {
	    // this is allowed.
	    std::string err = m_onlineID->print_to_string(sid); 
	    ATH_MSG_VERBOSE( (std::string)except << sid.get_identifier32().get_compact());
	    ATH_MSG_VERBOSE( err );
	} 
    }
    ATH_MSG_DEBUG (" Number of Connected Channel ID = " <<nconnected );
    ATH_MSG_DEBUG (" Number of Valid Channel ID = " <<nch );
    if(nerr>0) ATH_MSG_ERROR (" Number channels with incorrect mapping= " <<nerr );

    if (n_err_uA2MeV!=0) 
      ATH_MSG_DEBUG (" Number of channels without uA2MeV "<<n_err_uA2MeV);
    if (n_err_DAC2uA!=0) 
      ATH_MSG_DEBUG (" Number of channels without DAC2uA"<<n_err_DAC2uA);

    return StatusCode::SUCCESS;
}


/////////////////////////////////////////////////////////////////////
// FINALIZE:
// Note that it is NOT NECESSARY to run the finalize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////

StatusCode LArCondDataTest::finalize()
{
  ATH_MSG_DEBUG ( " finalize ");

  if(!m_fixShape)
    return StatusCode::SUCCESS; 

  ATH_MSG_DEBUG ( " fix LArShapeMC ");

  const LArShapeMC* shape  = 0;
  ATH_CHECK( detStore()->retrieve(shape) );

  LArShapeMC* newShape= new LArShapeMC();
  newShape->setGroupingType( LArShapeMC::SingleGroup );
  ATH_CHECK( newShape->initialize() );
  ATH_CHECK( detStore()->record(newShape,"LArFullShape") );

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
           ATH_MSG_DEBUG( " no match ");
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

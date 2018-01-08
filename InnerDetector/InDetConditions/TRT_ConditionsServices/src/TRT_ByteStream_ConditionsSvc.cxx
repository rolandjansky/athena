/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef SIMULATIONBASE

/** @file TRT_ByteStream_ConditionsSvc.cxx
 *  @Service for accessing TRT ByteStream conditions information
 *  @author Denver Whittington
 *  @author Paul Keener
 */

#include "GaudiKernel/ToolFactory.h"
#include "TRT_ByteStream_ConditionsSvc.h"

#include "InDetByteStreamErrors/TRT_BSErrContainer.h"
#include "InDetByteStreamErrors/TRT_BSIdErrContainer.h"

#include "Identifier/Identifier.h"


//////////
/// Constructor
/////
TRT_ByteStream_ConditionsSvc::TRT_ByteStream_ConditionsSvc( const std::string& name,
							    ISvcLocator* pSvcLocator ) :
  AthService( name, pSvcLocator ),
  m_evtStore("StoreGateSvc",name),
  m_num_l1id_errors(0),
  m_num_bcid_errors(0),
  m_num_missing_errors(0),
  m_num_error_errors(0),
  m_num_sid_errors(0),
  m_num_robStatus_errors(0),
  m_tot_num_l1id_errors(0),
  m_tot_num_bcid_errors(0),
  m_tot_num_missing_errors(0),
  m_tot_num_error_errors(0),
  m_tot_num_sid_errors(0),
  m_tot_num_robStatus_errors(0),
  m_IdCont(0),
  m_cont(0) 
{
  declareProperty( "writeBcidError", m_writeBCIDError = true );
  declareProperty( "writeL1idError", m_writeL1IDError = true );
  declareProperty( "writeMissingError", m_writeMISSINGError = true );
  declareProperty( "writeErrorError", m_writeERRORError = true );
  declareProperty( "writeSidError", m_writeSIDError = true );
  declareProperty( "readCondFromESD", m_readCondFromESD = false );
}

//////////
/// Destructor
/////
TRT_ByteStream_ConditionsSvc::~TRT_ByteStream_ConditionsSvc() {}

//////////
/// Initialize
/////
StatusCode TRT_ByteStream_ConditionsSvc::initialize()
{
  if(msgLvl(MSG::DEBUG)) msg() << "TRT_ByteStream_ConditionsSvc: Initialize." << endmsg;


  /*
   * Ask to be informed at the beginning of each new event so that we
   * can reset.
   */
  IIncidentSvc* incsvc;
  StatusCode sc = service("IncidentSvc", incsvc);
  int priority = 100;
  if( sc.isSuccess() ) 
  {
    incsvc->addListener( this, "BeginEvent", priority);
  }

  // Retrieve the EventStore
  if ( StatusCode::SUCCESS!=m_evtStore.retrieve() ) {
    ATH_MSG_FATAL("Couldn't retrieve " << m_evtStore);
    return StatusCode::FAILURE;
  }

  m_num_l1id_errors=0;
  m_num_bcid_errors=0;
  m_num_missing_errors=0;
  m_num_error_errors=0;
  m_num_sid_errors=0;
  m_num_robStatus_errors=0;

  m_tot_num_l1id_errors=0;
  m_tot_num_bcid_errors=0;
  m_tot_num_missing_errors=0;
  m_tot_num_error_errors=0;
  m_tot_num_sid_errors=0;
  m_tot_num_robStatus_errors=0;

  ATH_MSG_INFO( "writeBcidError    =" << (m_writeBCIDError?"TRUE":"FALSE") );
  ATH_MSG_INFO( "writeL1idError    =" << (m_writeL1IDError?"TRUE":"FALSE") );
  ATH_MSG_INFO( "writeMissingError =" << (m_writeMISSINGError?"TRUE":"FALSE") );
  ATH_MSG_INFO( "writeErrorError   =" << (m_writeERRORError?"TRUE":"FALSE") );
  ATH_MSG_INFO( "writeSidError     =" << (m_writeSIDError?"TRUE":"FALSE") );

  return StatusCode::SUCCESS;
}

//////////
/// StatusFlag for TRT_ConditionsSummarySvc (by Identifier)
/////
InDet::TRT_CondFlag 
TRT_ByteStream_ConditionsSvc::condSummaryStatus( const Identifier& /*ident*/ ) 
{
  return InDet::TRT_COND_GOOD;
}

//////////
/// Finalize
/////
StatusCode 
TRT_ByteStream_ConditionsSvc::finalize() 
{
   this->resetSets();
   this->resetCounts();

   msg(MSG::INFO) << " " << endmsg;
   msg(MSG::INFO) << "---------------------------------" << endmsg;
   msg(MSG::INFO) << "      TRT BS Error Summary" << endmsg;
   msg(MSG::INFO) << " " << endmsg;
   msg(MSG::INFO) << " Number of L1ID Mismatch errors     : " << m_tot_num_l1id_errors << endmsg;
   msg(MSG::INFO) << " Number of BCID Mismatch errors     : " << m_tot_num_bcid_errors << endmsg;
   msg(MSG::INFO) << " Number of Missing FE errors        : " << m_tot_num_missing_errors << endmsg;
   msg(MSG::INFO) << " Number of FE with error bit        : " << m_tot_num_error_errors << endmsg;
   msg(MSG::INFO) << " Number of FE with SID bit          : " << m_tot_num_sid_errors << endmsg;
   msg(MSG::INFO) << " Number of ROB with non-zero status : " << m_tot_num_robStatus_errors << endmsg;
   msg(MSG::INFO) << "---------------------------------" << endmsg;
   msg(MSG::INFO) << " " << endmsg;


   return StatusCode::SUCCESS;
}

/*
 * Clear Sets and Counters at each new event
 */
void
TRT_ByteStream_ConditionsSvc::handle(const Incident&) 
{
   this->resetSets();
   this->resetCounts();

   if ( m_readCondFromESD )
      this->readData();
   else
   {
      StatusCode sc(StatusCode::SUCCESS);

      m_IdCont = new TRT_BSIdErrContainer();
      sc = m_evtStore->record(m_IdCont,"TRT_ByteStreamIdErrs");
      if (sc.isFailure() ) 
	 msg(MSG::ERROR) <<"Failed to record BSIdErrors to SG"<<endmsg;


      m_cont = new TRT_BSErrContainer();
      sc = m_evtStore->record(m_cont,"TRT_ByteStreamErrs");
      if (sc.isFailure() ) 
	 msg(MSG::ERROR) <<"Failed to record BSErrors to SG"<<endmsg;
   }


   return;
}

void 
TRT_ByteStream_ConditionsSvc::resetSets( void ) 
{
   /*
    * Clear the sets and release the memory they hold
    */

   std::set<uint32_t>().swap (m_l1id_errors);
   std::set<uint32_t>().swap (m_bcid_errors);
   std::set<uint32_t>().swap (m_missing_errors);
   std::set<uint32_t>().swap (m_error_errors);
   std::set<uint32_t>().swap (m_sid_errors);

   std::set<std::pair<uint32_t,uint32_t> >().swap (m_l1id_pair_errors);
   std::set<std::pair<uint32_t,uint32_t> >().swap (m_bcid_pair_errors);

   std::set<std::pair<uint32_t,uint32_t> >().swap (m_rob_status_errors);
 
  return;
}

/** The following methods are for HLT, to get the numbers of errors 
 *  per RoI, so won't necessarily match up with the sizes of the sets. 
 */

void 
TRT_ByteStream_ConditionsSvc::resetCounts( void ) 
{
  m_num_l1id_errors=0;
  m_num_bcid_errors=0;
  m_num_missing_errors=0;
  m_num_error_errors=0;
  m_num_sid_errors=0;
  m_num_robStatus_errors=0;

  return;
}

int 
TRT_ByteStream_ConditionsSvc::getNumberOfErrors( TRTByteStreamErrors::errorTypes ErrType )
{
   switch( ErrType )
   {
     case TRTByteStreamErrors::BCIDError:
       return m_num_bcid_errors;
     case TRTByteStreamErrors::L1IDError:
       return m_num_l1id_errors;
     case TRTByteStreamErrors::MISSINGError:
       return m_num_missing_errors;
     case TRTByteStreamErrors::ERRORError:
       return m_num_error_errors;
     case TRTByteStreamErrors::SIDError:
       return m_num_sid_errors;
     case TRTByteStreamErrors::RobStatusError:
       return m_num_robStatus_errors;
    }

  return 0;
}


const std::set<std::pair<uint32_t,uint32_t> > &
TRT_ByteStream_ConditionsSvc::getL1idErrorSet( void )
{

  const std::set<std::pair<uint32_t,uint32_t> > & ref = 
    m_l1id_pair_errors;

  return ref;
 }

const std::set<std::pair<uint32_t,uint32_t> > &
TRT_ByteStream_ConditionsSvc::getBcidErrorSet( void )
{

  const std::set<std::pair<uint32_t,uint32_t> > & ref = 
    m_bcid_pair_errors;

  return ref;
 }


const std::set<std::pair<uint32_t,uint32_t> > &
TRT_ByteStream_ConditionsSvc::getRobErrorSet( void )
{

  const std::set<std::pair<uint32_t,uint32_t> > & ref = 
    m_rob_status_errors;

  return ref;
 }


const std::set<uint32_t> &
TRT_ByteStream_ConditionsSvc::getMissingErrorSet( void )
{

  const std::set<uint32_t> & ref = m_missing_errors;

  return ref;
 }


const std::set<uint32_t> &
TRT_ByteStream_ConditionsSvc::getErrorErrorSet( void )
{

  const std::set<uint32_t> & ref = m_error_errors;

  return ref;
 }


const std::set<uint32_t> &
TRT_ByteStream_ConditionsSvc::getSidErrorSet( void )
{

  const std::set<uint32_t> & ref = m_sid_errors;

  return ref;
 }



// void 
// TRT_ByteStream_ConditionsSvc::add_l1id_error( uint32_t DTMROC_idx ) 
// { 
//   m_l1id_errors.insert( DTMROC_idx ); 
//   m_num_l1id_errors++;
// }

// void 
// TRT_ByteStream_ConditionsSvc::add_bcid_error( uint32_t DTMROC_idx ) 
// { 
//   m_bcid_errors.insert( DTMROC_idx ); 
//   m_num_bcid_errors++;
// }

void 
TRT_ByteStream_ConditionsSvc::add_missing_error( uint32_t DTMROC_idx ) 
{ 
  m_missing_errors.insert( DTMROC_idx ); 
  m_num_missing_errors++;
  m_tot_num_missing_errors++;
}

void 
TRT_ByteStream_ConditionsSvc::add_error_error( uint32_t DTMROC_idx ) 
{ 
  m_error_errors.insert( DTMROC_idx ); 
  m_num_error_errors++;
  m_tot_num_error_errors++;
}

void 
TRT_ByteStream_ConditionsSvc::add_sid_error( uint32_t DTMROC_idx ) 
{ 
  m_sid_errors.insert( DTMROC_idx ); 
  m_num_sid_errors++;
  m_tot_num_sid_errors++;
}

/*
 * Versions with head info
 */
void 
TRT_ByteStream_ConditionsSvc::add_l1id_error( uint32_t DTMROC_idx, 
					       uint32_t l1id ) 
{ 
  std::pair<uint32_t, uint32_t> l1id_error( DTMROC_idx, l1id );

  m_l1id_pair_errors.insert( l1id_error ); 
  m_num_l1id_errors++;
  m_tot_num_l1id_errors++;
}

void 
TRT_ByteStream_ConditionsSvc::add_bcid_error( uint32_t DTMROC_idx,
					       uint32_t bcid ) 
{ 
  std::pair<uint32_t, uint32_t> bcid_error( DTMROC_idx, bcid );

  m_bcid_pair_errors.insert( bcid_error ); 
  m_num_bcid_errors++;
  m_tot_num_bcid_errors++;
}

void 
TRT_ByteStream_ConditionsSvc::add_rob_error( uint32_t robSourceId, 
					     uint32_t robStatus )
{
  //  ATH_MSG_INFO( "ROB status     =" << std::hex << robStatus );

  std::pair<uint32_t, uint32_t> rob_error( robSourceId, robStatus );

  m_rob_status_errors.insert( rob_error ); 
  m_num_robStatus_errors++;
  m_tot_num_robStatus_errors++;
}


std::set<std::pair<uint32_t,uint32_t> > * 
TRT_ByteStream_ConditionsSvc::getRodRobErrorSet( TRTByteStreamErrors::errorTypes ErrType )
{
  switch(ErrType) 
  {
     case TRTByteStreamErrors::RobStatusError:
       return &m_rob_status_errors;  
     default: 
       return 0;
  }

  return 0;
}

std::set<std::pair<uint32_t,uint32_t> > * 
TRT_ByteStream_ConditionsSvc::getIdErrorSet( TRTByteStreamErrors::errorTypes ErrType )
{
  switch(ErrType) 
  {
     case TRTByteStreamErrors::BCIDError:
       return &m_bcid_pair_errors;
     case TRTByteStreamErrors::L1IDError:
       return &m_l1id_pair_errors;   
     default: 
       return 0;
  }

  return 0;
}

std::set<uint32_t> * 
TRT_ByteStream_ConditionsSvc::getErrorSet( TRTByteStreamErrors::errorTypes ErrType )
{
  switch(ErrType) 
  {
     case TRTByteStreamErrors::MISSINGError:
       return &m_missing_errors;
     case TRTByteStreamErrors::ERRORError:
       return &m_error_errors;   
     case TRTByteStreamErrors::SIDError:
       return &m_sid_errors;   
     default: 
       return 0;
  }

  return 0;
}


StatusCode
TRT_ByteStream_ConditionsSvc::readData() 
{

  StatusCode sc(StatusCode::SUCCESS);  

  /*
   * ByteStream ID (L1ID, BCID) errors
   */
  {
     const TRT_BSIdErrContainer* errCont;
     sc = m_evtStore->retrieve(errCont,"TRT_ByteStreamIdErrs");
     if (sc.isFailure() ) 
     {
	ATH_MSG_WARNING( "Failed to retrieve TRT BS Id error container from SG" );

	return sc;
     }


     // = new TRT_BSIdErrContainer();


     for (const auto* elt : *errCont)
     {
	int errorType = elt->first;

	//	std::cout << "TRT BSerr: " << errorType << " " << ((*it)->second).first
	//		  << " " << (uint32_t) ((*it)->second).second << std::endl;

	switch(errorType) 
	{
	
	case TRTByteStreamErrors::BCIDError:
	   add_bcid_error( elt->second.first,  elt->second.second );
	   break;

	case TRTByteStreamErrors::L1IDError:
	   add_l1id_error( elt->second.first,  elt->second.second );
	   break;
	}
     }
  }

  /*
   * ByteStream (Missing, Error, SID) errors
   */
  {
     const TRT_BSErrContainer* errCont;
     sc = m_evtStore->retrieve(errCont,"TRT_ByteStreamErrs");
     if (sc.isFailure() ) 
     {
	ATH_MSG_WARNING( "Failed to retrieve TRT BS error container from SG" );

	return sc;
     }


     for (const auto* elt : *errCont)
     {
	int errorType = elt->first;

	//	std::cout << "TRT BSerr: " << errorType << " " << (*it)->second << std::endl;

	switch(errorType) 
	{
	
	case TRTByteStreamErrors::MISSINGError:
	   add_missing_error( elt->second );
	   break;

	case TRTByteStreamErrors::ERRORError:
	   add_error_error( elt->second );
	   break;

	case TRTByteStreamErrors::SIDError:
	   add_sid_error( elt->second );
	   break;
	}
     }
  }

   return sc;
 }


StatusCode 
TRT_ByteStream_ConditionsSvc::recordData() 
{

  StatusCode sc(StatusCode::SUCCESS);

  /*
   * ByteStream ID (L1ID, BCID) errors
   */
  {
     TRTByteStreamErrors::errorTypes IdPairErrorTypes[2] = { TRTByteStreamErrors::BCIDError,
							     TRTByteStreamErrors::L1IDError };

  for (int errIdx=0; errIdx<2; errIdx++)
  {
     TRTByteStreamErrors::errorTypes errType = IdPairErrorTypes[errIdx];

     if ( ((errType == TRTByteStreamErrors::BCIDError) && m_writeBCIDError) ||
	  ((errType == TRTByteStreamErrors::L1IDError) && m_writeL1IDError) )
     {
	std::set<std::pair<uint32_t,uint32_t> >* errors = getIdErrorSet(errType);
	if ( ! errors )
	{
#ifdef TRT_BS_ERR_DEBUG
	   msg(MSG::INFO) << "No TRT BS errors of type " << errType << " to record" << endmsg;
#endif // TRT_BS_ERR_DEBUG
	   continue;
	}

	std::set<std::pair<uint32_t,uint32_t> >::iterator it = errors->begin();
	std::set<std::pair<uint32_t,uint32_t> >::iterator itEnd = errors->end();
	for (; it != itEnd; ++it) 
	{
	   std::pair<uint8_t, std::pair<uint32_t,uint8_t> >* err = 
	      new std::pair<uint8_t, std::pair<uint32_t,uint8_t> >
	      (std::make_pair( errType, *it ));
	   m_IdCont->push_back(err);
	}
     }
  }

  }

  /*
   * ByteStream (Missing, Error, SID) errors
   */
  {
     TRTByteStreamErrors::errorTypes IdErrorTypes[3] = { TRTByteStreamErrors::MISSINGError,
							 TRTByteStreamErrors:: ERRORError,
							 TRTByteStreamErrors::SIDError };
  for (int errIdx=0; errIdx<3; errIdx++)
  {
     TRTByteStreamErrors::errorTypes errType = IdErrorTypes[errIdx];

     if ( ((errType == TRTByteStreamErrors::MISSINGError) && m_writeMISSINGError) ||
	  ((errType == TRTByteStreamErrors::ERRORError) && m_writeERRORError) ||
	  ((errType == TRTByteStreamErrors::SIDError) && m_writeSIDError) )
     { 

	std::set<uint32_t>* errors = getErrorSet(errType);
	if ( ! errors )
	{
#ifdef TRT_BS_ERR_DEBUG
	   msg(MSG::INFO) << "No TRT BS errors of type " << errType << " to record" << endmsg;
#endif // TRT_BS_ERR_DEBUG
	   continue;
	}
	std::set<uint32_t>::iterator it = errors->begin();
	std::set<uint32_t>::iterator itEnd = errors->end();
	for (; it != itEnd; ++it)
	{
	   std::pair<uint8_t, uint32_t>* err = 
	      new std::pair<uint8_t, uint32_t> (std::make_pair( errType, *it ));
	   m_cont->push_back(err);
	}
     }
  }

  }

  return sc;
 }

#endif //ifndef SIMULATIONBASE

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RPC_RDOToxAOD.cxx
//   Implementation file for class RPC_RDOToxAOD
///////////////////////////////////////////////////////////////////

#include "RPC_RDOToxAOD.h"

#include "xAODTracking/TrackMeasurementValidation.h"
#include "xAODTracking/TrackMeasurementValidationAuxContainer.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include <iostream>

typedef unsigned short int ubit16;

// Constructor with parameters:
RPC_RDOToxAOD::RPC_RDOToxAOD(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator)
{  
    
}

StatusCode RPC_RDOToxAOD::initialize() 
{
  m_trackMeasVal="RPC_RDO_Measurements";
  m_rpcPad="RPCPAD";
  ATH_CHECK(m_trackMeasVal.initialize());
  ATH_CHECK(m_rpcPad.initialize());
  return StatusCode::SUCCESS;
}


// Execute method:
StatusCode RPC_RDOToxAOD::execute() 
{
  SG::ReadHandle<RpcPadContainer> rpccol(m_rpcPad);
  if(!rpccol.isValid()){
    ATH_MSG_WARNING(m_rpcPad.key()<<" not valid");
    return StatusCode::FAILURE;
  }
  if(!rpccol.isPresent()){
    ATH_MSG_DEBUG("no RPC pad container");
    return StatusCode::SUCCESS;
  }

  SG::WriteHandle<xAOD::TrackMeasurementValidationContainer> xaod(m_trackMeasVal);
  ATH_CHECK(xaod.record(std::make_unique<xAOD::TrackMeasurementValidationContainer>(),std::make_unique<xAOD::TrackMeasurementValidationAuxContainer>()));

  // loop over the RpcPad
  ATH_MSG_DEBUG("Looping over RpcPad...");
  RpcPadContainer::const_iterator itpad = rpccol->begin(), itpadE = rpccol->end();
  for (; itpad != itpadE; itpad++) {
    const RpcPad *aPad = *itpad;
    ATH_MSG_DEBUG("A RpcPad, size = " << aPad->size());

    // information from RpcPad
    Identifier id_Pad         = aPad->identify();
    IdentifierHash idHash_Pad = aPad->identifyHash();

    ubit16 status    = aPad->status();
    ubit16 errorCode = aPad->errorCode();
    ubit16 padId  = aPad->onlineId();
    ubit16 lvl1Id    = aPad->lvl1Id();
    ubit16 padbcId      = aPad->bcId();
    int sector       = aPad->sector();

    ATH_MSG_DEBUG("A RpcPad, Identifier " << id_Pad.getString());
    std::cout << idHash_Pad << std::endl;
    ATH_MSG_DEBUG("and status = " << status << ", errorCode = " << errorCode 
		  << ", padId = " << padId  
		  << ", lvl1Id = " << lvl1Id
		  << ", padbcid = " << padbcId
		  << ", sector = " << sector);

     // loop over Coincidence Matrices
     ATH_MSG_DEBUG("Looping over Coincidence Matrices...");
     RpcPad::const_iterator itmat = (*itpad)->begin(), itmatE = (*itpad)->end();
     for (; itmat != itmatE; itmat++) {

       const RpcCoinMatrix *aMat = *itmat;
       ATH_MSG_DEBUG("A Coincidence Matrix, size = " << aMat->size());

       // information from Coincidence Matrix
       Identifier id_Mat         = aMat->identify();
       ubit16 cmaId    = aMat->onlineId();
       ubit16 crc    = aMat->crc();
       ubit16 fel1Id    = aMat->fel1Id();
       ubit16 febcId    = aMat->febcId();

       ATH_MSG_DEBUG("A Coincidence Matrix, Identifier " << id_Mat.getString());
       std::cout << id_Mat << std::endl;
       ATH_MSG_DEBUG("and cmaId = " << cmaId << ", crc = " << crc 
		     << ", fel1Id = " << fel1Id  
		     << ", febcId = " << febcId);

       // loop over Fired Channels
       ATH_MSG_DEBUG("Looping over Coincidence Matrix...");
       RpcCoinMatrix::const_iterator itchan = (*itmat)->begin(), itchanE = (*itmat)->end();
       for (; itchan != itchanE; itchan++) {

	 const RpcFiredChannel *aChan = *itchan;

	 // information from Fired Channel
	 ubit16 bcid    = aChan->bcid();
	 ubit16 ticks    = aChan->time();
	 ubit16 ijk    = aChan->ijk();
	 ubit16 cmachan    = aChan->channel();
	 int overlap = -1;  
	 int threshold = -1;
	 if ( (aChan->ijk()) == 7){
	     overlap    = (int) aChan->ovl();
	     threshold  = (int) aChan->thr();
	 }
	 ATH_MSG_DEBUG("and bcid = " << bcid << ", ticks = " << ticks 
		       << ", ijk = " << ijk  
		       << ", cmachan = " << cmachan
		       << ", overlap = " << overlap  
		       << ", threshold = " << threshold);

	 // fill the xAOD
	 xAOD::TrackMeasurementValidation* xprd = new xAOD::TrackMeasurementValidation();
	 xaod->push_back(xprd);
	 //Rpc Pad
	 xprd->auxdata<int>("status")    = (int)status;
	 xprd->auxdata<int>("errorCode") = (int)errorCode;
	 xprd->auxdata<int>("padId")  = (int)padId;
	 xprd->auxdata<int>("lvl1Id")    = (int)lvl1Id;
	 xprd->auxdata<unsigned int>("padbcid") = (unsigned int)padbcId;
	 xprd->auxdata<int>("sector")    = sector;
	 //Co Mat
	 xprd->auxdata<int>("cmaId")  = (int)cmaId;
	 xprd->auxdata<int>("crc")  = (int)crc;
	 xprd->auxdata<int>("fel1Id")  = (int)fel1Id;
	 xprd->auxdata<int>("febcId")  = (int)febcId;
	 //Fired Channel
	 xprd->auxdata<unsigned int>("bcid")  = (unsigned int)bcid;
	 xprd->auxdata<int>("ticks")  = (int)ticks;
	 xprd->auxdata<int>("ijk")  = (int)ijk;
	 xprd->auxdata<int>("cmachan")  = (int)cmachan;
	 xprd->auxdata<int>("overlap")  = (int)overlap;
	 xprd->auxdata<int>("threshold ")  = (int)threshold;  
	 
       }// end of loop over Fired Channels

     }// end of loop over Coincidence Matrices

  }// end of loop on the RpcPads

  
  
  return StatusCode::SUCCESS;
}


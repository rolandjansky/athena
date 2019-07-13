/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondTest/MDTConditionsTestAlgMT.h"
#include "MuonCondSvc/MuonHierarchy.h"


//Gaudi includes
#include "GaudiKernel/StatusCode.h"

//Athena includes
#include "Identifier/Identifier.h"


MDTConditionsTestAlgMT::MDTConditionsTestAlgMT(
                         const std::string& name,
                         ISvcLocator* pSvcLocator ) :
                         AthAlgorithm( name, pSvcLocator ){
			
}

MDTConditionsTestAlgMT::~MDTConditionsTestAlgMT()
{
  ATH_MSG_VERBOSE( "Calling destructor"  );
}

//Initialize
StatusCode
MDTConditionsTestAlgMT::initialize(){
 
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO)<< "Calling initialize" << endmsg;
  ATH_CHECK(m_readKey.initialize());
 
  return sc;
}

//Execute
StatusCode
MDTConditionsTestAlgMT::execute(){
  StatusCode sc(StatusCode::SUCCESS);

  msg(MSG::INFO)<< "Calling execute" << endmsg;
  msg(MSG::INFO)<< "Now setting up read handle: "; 
  SG::ReadCondHandle<MdtCondDbData> readHandle{m_readKey};
  const MdtCondDbData* readCdo{*readHandle};
  if(readCdo==0){
    msg(MSG::INFO)<< "DID NOT WORK!"<< endmsg;
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  else {
    msg(MSG::INFO)<< "worked!"<< endmsg;
  }


  msg(MSG::INFO)<<"Reading Dead Tubes: ";
  int size = readCdo->getDeadTubesId().size();
  msg(MSG::INFO)<<"found "<<size<<" from Id"<<endmsg;
  for(int k=0;k<size;++k){
    msg(MSG::INFO)<<"\t"<<k<<": "<<readCdo->getDeadTubes()[k]<<endmsg;
  } 

  msg(MSG::INFO)<<"Reading Dead Layers: ";
  size = readCdo->getDeadLayersId().size();
  msg(MSG::INFO)<<"found "<<size<<" from Id"<<endmsg;
  for(int k=0;k<size;++k){
    msg(MSG::INFO)<<"\t"<<k<<": "<<readCdo->getDeadLayers()[k]<<endmsg;
  } 

  msg(MSG::INFO)<<"Reading Dead Multilayers: ";
  size = readCdo->getDeadMultilayersId().size();
  msg(MSG::INFO)<<"found "<<size<<" from Id"<<endmsg;
  for(int k=0;k<size;++k){
    msg(MSG::INFO)<<"\t"<<k<<": "<<readCdo->getDeadMultilayers()[k]<<endmsg;
  } 

  msg(MSG::INFO)<<"Reading Dead Stations: ";
  size = readCdo->getDeadStationsId().size();
  msg(MSG::INFO)<<"found "<<size<<" from Id"<<endmsg;
  for(int k=0;k<size;++k){
    msg(MSG::INFO)<<"\t"<<k<<": "<<readCdo->getDeadStations()[k]<<endmsg;
  } 

  msg(MSG::INFO)<<"Reading Chambers with Dead Channels: ";
  size = readCdo->getDeadChambersId().size();
  msg(MSG::INFO)<<"found "<<size<<" from Id"<<endmsg;
  for(int k=0;k<size;++k){
    msg(MSG::INFO)<<"\t"<<k<<": "<<readCdo->getDeadChambersId()[k]<<endmsg;
  } 


  msg(MSG::INFO)<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"<<endmsg;

  msg(MSG::INFO)<<"Now probing Ids"<<endmsg;
  msg(MSG::INFO)<<"ID=1699348480; isGood? "<<readCdo->isGood(Identifier(1699348480))<<endmsg;

  msg(MSG::INFO)<<"MADE IT TO THE END"<<endmsg;
  return sc;
}


//Finalize
StatusCode
MDTConditionsTestAlgMT::finalize(){
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO)<< "Calling finalize" << endmsg;
  return sc;
}

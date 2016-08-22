/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscCoolTest.cxx - simple test of parameter retrieval function getParameter from CscCoolStrSvc
// January 9, 2007, lampen@physics.arizona.edu


#include <fstream>
#include <string>
#include <stdlib.h>
#include <math.h>
#include "MuonIdHelpers/CscIdHelper.h"

//#include "MuonCondData/CscCondParType.h"

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"


#include "CscCoolTest.h"

using namespace std;
using namespace MuonCalib; //So we don't need to type MuonCalib:: in front of CscCondParType::

CscCoolTest::CscCoolTest(const std::string& name, 
    ISvcLocator* pSvcLocator) :AthAlgorithm(name,pSvcLocator),
                               m_log(msgSvc(),name),
                               p_cscCoolStrSvc(0),
                               m_cscId(0)
{

  declareProperty("StripHash",m_stripHash = 800);
  declareProperty("DoStatusTest", m_doStatusTest = false);
}


CscCoolTest::~CscCoolTest() {}

StatusCode CscCoolTest::initialize()
{
  m_log << MSG::DEBUG << "CscCoolTest::initialize() called" << endmsg;

  if (StatusCode::SUCCESS!=service("MuonCalib::CscCoolStrSvc",p_cscCoolStrSvc))
  {
    m_log << MSG::FATAL << "Cannot get CscCoolStrSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  StoreGateSvc* detstore;
  if (StatusCode::SUCCESS!=service("DetectorStore",detstore)) {
    m_log << MSG::FATAL << "Detector store not found" << endmsg;
    return StatusCode::FAILURE;
  }
  StatusCode sc = detstore->retrieve(m_cscId,"CSCIDHELPER");
  if(sc.isFailure())
  {
    m_log << MSG::FATAL << "Cannot retrieve CscIdHelper from detector store" << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}

StatusCode CscCoolTest::execute()
{
  m_log << MSG::INFO << "Starting CscCoolTest execute" << endmsg;

  float  ped(0), pulseSlope(0.0), noise(0),rms(0),f001(0);
  //float gain, ped, runSlope, pulseSlope, peakt,width,sat1,sat2,noise,thold;
  uint32_t status =0;

  StatusCode sc;

  //Using the get parameter method of CscCoolStrSvc to retrieve the parameters.
  m_log << MSG::INFO << "Retrieveing constants from strip with hash " <<  m_stripHash  << endmsg;
  sc = p_cscCoolStrSvc->getParameter(ped,"ped",m_stripHash);
  sc = p_cscCoolStrSvc->getParameter(noise,"noise",m_stripHash);
  sc = p_cscCoolStrSvc->getParameter(f001,"f001",m_stripHash);
  sc = p_cscCoolStrSvc->getParameter(rms,"rms",m_stripHash);
  sc = p_cscCoolStrSvc->getParameter(pulseSlope,"pslope",m_stripHash);
  sc = p_cscCoolStrSvc->getParameter(status,"status",m_stripHash);  	
  //sc = p_cscCoolStrSvc->getParameter(thold,"thold",m_stripHash);
  //sc = p_cscCoolStrSvc->getParameter(peakt,"peakt",m_stripHash);
  //sc = p_cscCoolStrSvc->getParameter(width,"width",m_stripHash);
  //sc = p_cscCoolStrSvc->getParameter(sat1,CscCondParType::SAT1,m_stripHash);
  //sc = p_cscCoolStrSvc->getParameter(sat2,CscCondParType::SAT2,m_stripHash);

  //Test depreciated version 

  //sc = p_cscCoolStrSvc->getStatus(status,m_stripHash);  	

  if(!sc.isSuccess())
  {
    m_log << MSG::FATAL << "couldn't retrieve parameter!" << endmsg;
    return StatusCode::FAILURE;
  }

  m_log << MSG::INFO << "***data from first retrieval method is:" 
    << "\n\tslope from pulser:\t"   << pulseSlope 
    << "\n\tpedestal:\t"            << ped 
    << "\n\tnoise:\t"               << noise 
    << "\n\trms\t"                  << rms
    << "\n\tf001\t"                 << f001
    << "\n\tstatus:\t"              << hex << static_cast<unsigned int>(status) << dec 
    << endmsg;

  //Applying bit masks to bit-masked status
  m_log << MSG:: INFO << "Status flags taken from bit-maksed status" << endmsg;

  m_log << MSG::INFO <<"noisy/stuck-bit channel: "                        << (status & 0x1) << endmsg;
  m_log << MSG::INFO <<"dead channel: "                       << ((status >> 1 )& 0x1 )<< endmsg;



  //Count how man of each bad status result we have
  if(m_doStatusTest){
    IdContext channelContext = m_cscId->channel_context();   

    const unsigned int nIds = 61440;
    const unsigned int expectedChamberLayer = 2;

    unsigned int numNoisy(0);
    unsigned int numDead(0);

    for( unsigned int idItr = 0; idItr < nIds; idItr++){
      Identifier stripId;
      m_cscId->get_id(idItr, stripId, &channelContext);
      unsigned int chamLayer = m_cscId->chamberLayer(stripId);

      if( chamLayer == expectedChamberLayer){ //skip missing layer
        sc = p_cscCoolStrSvc->getParameter(status,"status",idItr);
        if(!sc.isSuccess())
        {
          m_log << MSG::FATAL << "couldn't retrieve status for stripHash " << stripId << endmsg;
          return StatusCode::FAILURE;
        }
        if(status&0x1)
          numNoisy++;
        if(status&0x2)
          numDead++;
      }//end if chamber layer is expected chamber layer
    }//end loop over channels
    m_log << MSG::INFO << "====Status Counts===" << endmsg;
    m_log << MSG::INFO << "Num Noisy Channels:\t" << numNoisy << endmsg;
    m_log << MSG::INFO << "Num Dead Channels:\t" << numDead << endmsg;
    m_log << MSG::INFO << "===================="<< endmsg;
  }//end if m_doStatusTest

  return StatusCode::SUCCESS;
}		

StatusCode CscCoolTest::finalize()
{ 
  return StatusCode::SUCCESS;
}

//} // end namespace MuonCalib


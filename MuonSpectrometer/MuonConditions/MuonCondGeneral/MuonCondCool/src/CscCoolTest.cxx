/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CscCoolTest.h"

#include <fstream>
#include <string>
#include <stdlib.h>
#include <cmath>

using namespace MuonCalib; //So we don't need to type MuonCalib:: in front of CscCondParType::

CscCoolTest::CscCoolTest(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name,pSvcLocator),
    p_cscCoolStrSvc(nullptr) {
  declareProperty("StripHash",m_stripHash = 800);
  declareProperty("DoStatusTest", m_doStatusTest = false);
}

StatusCode CscCoolTest::initialize()
{
  ATH_MSG_DEBUG("CscCoolTest::initialize() called");
  ATH_CHECK(service("MuonCalib::CscCoolStrSvc",p_cscCoolStrSvc));
  StoreGateSvc* detstore=nullptr;
  ATH_CHECK(service("DetectorStore",detstore));
  ATH_CHECK(m_idHelperSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode CscCoolTest::execute()
{
  ATH_MSG_DEBUG("Starting CscCoolTest execute");
  float ped(0);
  float pulseSlope(0);
  float noise(0);
  float rms(0);
  float f001(0);
  //float gain, ped, runSlope, pulseSlope, peakt,width,sat1,sat2,noise,thold;
  uint32_t status=0;

  StatusCode sc;
  //Using the get parameter method of CscCoolStrSvc to retrieve the parameters.
  ATH_MSG_DEBUG("Retrieveing constants from strip with hash " <<  m_stripHash);
  sc = p_cscCoolStrSvc->getParameter(ped,"ped",m_stripHash);
  sc = p_cscCoolStrSvc->getParameter(noise,"noise",m_stripHash);
  sc = p_cscCoolStrSvc->getParameter(f001,"f001",m_stripHash);
  sc = p_cscCoolStrSvc->getParameter(rms,"rms",m_stripHash);
  sc = p_cscCoolStrSvc->getParameter(pulseSlope,"pslope",m_stripHash);
  sc = p_cscCoolStrSvc->getParameter(status,"status",m_stripHash);  	
  if(!sc.isSuccess()) {
    ATH_MSG_FATAL("couldn't retrieve parameter!");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("***data from first retrieval method is:" 
    << "\n\tslope from pulser:\t"   << pulseSlope 
    << "\n\tpedestal:\t"            << ped 
    << "\n\tnoise:\t"               << noise 
    << "\n\trms\t"                  << rms
    << "\n\tf001\t"                 << f001
    << "\n\tstatus:\t"              << std::hex << static_cast<unsigned int>(status) << std::dec);

  //Applying bit masks to bit-masked status
  ATH_MSG_DEBUG("Status flags taken from bit-maksed status");
  ATH_MSG_DEBUG("noisy/stuck-bit channel: " << (status & 0x1));
  ATH_MSG_DEBUG("dead channel: " << ((status >> 1 )& 0x1 ));

  //Count how man of each bad status result we have
  if(m_doStatusTest){
    IdContext channelContext = m_idHelperSvc->cscIdHelper().channel_context();   

    const unsigned int nIds = 61440;
    const unsigned int expectedChamberLayer = 2;

    unsigned int numNoisy(0);
    unsigned int numDead(0);

    for( unsigned int idItr = 0; idItr < nIds; idItr++){
      Identifier stripId;
      m_idHelperSvc->cscIdHelper().get_id(idItr, stripId, &channelContext);
      unsigned int chamLayer = m_idHelperSvc->cscIdHelper().chamberLayer(stripId);

      if( chamLayer == expectedChamberLayer){ //skip missing layer
        sc = p_cscCoolStrSvc->getParameter(status,"status",idItr);
        if(!sc.isSuccess()) {
          ATH_MSG_FATAL("couldn't retrieve status for stripHash " << stripId);
          return StatusCode::FAILURE;
        }
        if(status&0x1)
          numNoisy++;
        if(status&0x2)
          numDead++;
      }//end if chamber layer is expected chamber layer
    }//end loop over channels
    ATH_MSG_DEBUG("====Status Counts===");
    ATH_MSG_DEBUG("Num Noisy Channels:\t" << numNoisy);
    ATH_MSG_DEBUG("Num Dead Channels:\t" << numDead);
    ATH_MSG_DEBUG("====================");
  }//end if m_doStatusTest

  return StatusCode::SUCCESS;
}		

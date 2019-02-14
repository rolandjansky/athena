/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Implementation of LArRodEncoder class
#include "LArByteStream/LArRodEncoder.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "eformat/Version.h"
#include <cstdio>
#include <stdio.h>
#include <Identifier/HWIdentifier.h>
#include "CaloInterface/ICaloNoiseTool.h"

// static variables
LArCablingLegacyService*    LArRodEncoder::m_cablingSvc=NULL;
const LArOnlineID*    LArRodEncoder::m_onlineHelper=NULL; 
LArRodBlockStructure* LArRodEncoder::m_BlStruct=NULL;
const CaloDetDescrManager* LArRodEncoder::m_CaloDetDescrManager=NULL;

void LArRodEncoder::setRodBlockStructure(LArRodBlockStructure* BlStructPtr)
{
  m_BlStruct=BlStructPtr; 
}


// constructor 
LArRodEncoder::LArRodEncoder()
{
  if(m_cablingSvc==NULL || m_onlineHelper==NULL) {
    // Message service
    IMessageSvc*  msgSvc;
    StatusCode sc = Gaudi::svcLocator()->service( "MessageSvc", msgSvc  );
    MsgStream log(msgSvc, "LArRodEncoder");
    if ( log.level() <= MSG::DEBUG )
      log << MSG::DEBUG << "initialize" << endmsg;
    // Cabling Service
    IToolSvc* toolSvc;
    sc   = Gaudi::svcLocator()->service( "ToolSvc",toolSvc  );
    if(sc.isSuccess())
      {
	sc = 
	  toolSvc->retrieveTool("LArCablingLegacyService",m_cablingSvc);
      } else {  // check if it fails
        // what do you want to do if it fails...
	log << MSG::FATAL << "Could not get LArCablingService !" << endmsg;
	exit(1);
      }
    //m_cablingSvc = LArCablingService::getInstance(); 
    // retrieve onlineHelper
    const LArOnlineID* online_id = 0;
    StoreGateSvc* detStore = 0;
    sc =Gaudi::svcLocator()->service( "DetectorStore", detStore );
    if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to locate DetectorStore" << endmsg;
      exit(1);
    } else {
      if ( log.level() <= MSG::INFO ) 
        log << MSG::INFO << "Successfully located DetectorStore" << endmsg;
    }     
    sc = detStore->retrieve(online_id, "LArOnlineID");
    if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get LArOnlineID helper !" << endmsg;
      exit(1);
    } 
    else {
      m_onlineHelper=online_id;
      if ( log.level() <= MSG::DEBUG ) 
        log << MSG::DEBUG << " Found the LArOnlineID helper. " << endmsg;
    }
  }

  m_CaloDetDescrManager = CaloDetDescrManager::instance();
}

// destructor 
LArRodEncoder::~LArRodEncoder()
{
  clear();
}

// Add LArRawChannel
void LArRodEncoder::add(const LArRawChannel* rc)
{
 uint32_t FEB_ID = (m_onlineHelper->feb_Id(rc->channelID()).get_identifier32().get_compact());
 if ( m_mFEB[FEB_ID].vLArRC.empty() ){
	m_mFEB[FEB_ID].vLArRC.resize(128,0);
	//for(int i=0;i<128;i++) m_mFEB[FEB_ID].vLArRC.push_back(0);
 }
 uint32_t chan = m_BlStruct->FebToRodChannel(m_onlineHelper->channel( rc->channelID() ) );
 m_mFEB[FEB_ID].vLArRC[chan]=rc;
}

// Add free gain digits
void LArRodEncoder::add(const LArDigit* dg)
{uint32_t FEB_ID = (m_onlineHelper->feb_Id(dg->hardwareID()).get_identifier32().get_compact());
 m_mFEB[FEB_ID].vLArDigit.push_back(dg);
}
//Add fixed gain digits
void LArRodEncoder::add(const LArDigit* dg, const int gain)
{uint32_t FEB_ID = (m_onlineHelper->feb_Id(dg->hardwareID()).get_identifier32().get_compact());
 m_mFEB[FEB_ID].vLArDigitFixed[gain].push_back(dg);
}
//Add calibration digits
void LArRodEncoder::add(const LArCalibDigit* dg, const int gain)
{uint32_t FEB_ID = (m_onlineHelper->feb_Id(dg->hardwareID()).get_identifier32().get_compact());
 m_mFEB[FEB_ID].vLArCalibDigit[gain].push_back(dg);
}





void LArRodEncoder::clear()
{
  m_mFEB.clear();
}

// convert all LArRawChannels and LArDigits
// in the current list to a vector of 32bit words
void LArRodEncoder::fillROD(std::vector<uint32_t>& v, MsgStream& logstr, ICaloNoiseTool* calonoisetool, double nsigma)
{
 if (!m_BlStruct)
  {logstr << MSG::ERROR << "No LArRodBlockStructure defined! Can't encode fragment!" << endmsg; 
   return;
  }
 //int febcounter=0; 
 //std::cout << "Going to init Fragment..." << std::endl;
 m_BlStruct->initializeFragment(v); //Makes new Fragment or splits v into existing Feb-Blocks
 std::map<uint32_t,FebData_t>::iterator it=m_mFEB.begin();
 std::map<uint32_t,FebData_t>::iterator it_end=m_mFEB.end();
 // The sort alorithm for the vectors inside the data object of the map fails
 // when I use const_iterator at this point. Don't ask me why
 //std::cout << "Have " << m_mFEB.size() << " FEBs in this ROD" << std::endl;
 for(;it!=it_end;it++) {
   //std::cout << "Setting values for feb # " << febcounter << " ID=" << it->first << std::endl;
    m_BlStruct->initializeFEB(it->first);                                     // Set FEB id
    //std::cout << "Setting Energy" << std::endl;
    // ************* Energy Block *************
    if (m_BlStruct->canSetEnergy() && it->second.vLArRC.size()>0) {
      //int j=0;
//      m_BlStruct->sortDataVector(it->second.vLArRC);
      std::vector<const LArRawChannel*>::const_iterator chit=it->second.vLArRC.begin();
      std::vector<const LArRawChannel*>::const_iterator chit_e=it->second.vLArRC.end();
      // compute Ex, Ey for this FEB : initialisation phase
      
      {
	double Ex=0;
	double Ey=0;
	double Ez=0;
	double SumE=0;
	Identifier myofflineID;
	const CaloDetDescrElement* caloDDE;

	for (;chit!=chit_e;chit++) {
	  if ( ((LArRawChannel*)(*chit))!=0 ){
	  int cId =  m_onlineHelper->channel((*chit)->hardwareID());
	  const LArRawChannel *theChannel = *chit;

	  int e=theChannel->energy();
          uint32_t quality = theChannel->quality();
	  m_BlStruct->setNextEnergy(cId,e, theChannel->time(), 
				    quality,theChannel->gain());
	  
	  // you convert from hardwareID to offline channle ID hash (???)
	  myofflineID = m_cablingSvc->cnvToIdentifier(theChannel->hardwareID()) ;
	  //std::cout << "Got Offile id 0x" << std::hex << myofflineID.get_compact() << std::dec << std::endl;
	  caloDDE = m_CaloDetDescrManager->get_element(myofflineID);
	  // This is probably NOT what one wants. You want the cell gain!
          double cellnoise = calonoisetool->totalNoiseRMS(caloDDE,theChannel->gain());
          if( e > (nsigma*cellnoise) && (quality != 65535 ) ){
	    double aux=caloDDE->sinTh();
	    double aux_z=tanh(caloDDE->eta());
            Ex += e*aux*caloDDE->cosPhi(); 
	    Ey += e*aux*caloDDE->sinPhi();
	    Ez += e*aux_z;
	    SumE += e;
          }
	 } // End of if to check for the LArRawChannel existence
        } // end of chit loop
	// Set after the loop finishes
	m_BlStruct->setEx(Ex);
	m_BlStruct->setEy(Ey);
	m_BlStruct->setEz(Ez);
	m_BlStruct->setSumE(SumE);
      }
    }//End canSetEnergy
    // try to include samples together with data
    if (m_BlStruct->canIncludeRawData() && it->second.vLArDigit.size()>0) {
      // Order channels according to ROD numbering
      m_BlStruct->sortDataVector(it->second.vLArDigit);
      std::vector<const LArDigit*>::const_iterator digit_it = it->second.vLArDigit.begin();
      std::vector<const LArDigit*>::const_iterator digit_it_end=it->second.vLArDigit.end();
      if(digit_it!=digit_it_end) { //Container not empty
        //m_BlStruct->setNumberOfSamples((*digit_it)->samples().size());
        for (;digit_it!=digit_it_end;digit_it++)  {
          int cId =  m_onlineHelper->channel((*digit_it)->hardwareID());
          m_BlStruct->setRawData(cId, (*digit_it)->samples(), (*digit_it)->gain());
        } // end of for digits
      }// end container not empty
    } // End of can Include Raw Data check
    //std::cout << "Setting Raw Data" << std::endl;
    // ************** Raw Data Block ***********
    if (m_BlStruct->canSetRawData() && it->second.vLArDigit.size()>0) {
      //sort(it->second.vLArDigit.begin(), it->second.vLArDigit.end(), *m_BlStruct);  // Order channels according to ROD numbering
      m_BlStruct->sortDataVector(it->second.vLArDigit);
      std::vector<const LArDigit*>::const_iterator digit_it = it->second.vLArDigit.begin();
      std::vector<const LArDigit*>::const_iterator digit_it_end=it->second.vLArDigit.end();
      if(digit_it!=digit_it_end) { //Container not empty
	m_BlStruct->setNumberOfSamples((*digit_it)->samples().size());
	for (;digit_it!=digit_it_end;digit_it++)  {
	  int cId =  m_onlineHelper->channel((*digit_it)->hardwareID());
	  //cId = m_BlStruct->FebToRodChannel(cId);
	  m_BlStruct->setRawData(cId, (*digit_it)->samples(), (*digit_it)->gain());
	} 
      }// end container not empty
    }// end if can set raw data    

    //std::cout << "Setting fixed gain data" << std::endl;
    // ************** Fixed Gain Data Block ***********
    if (m_BlStruct->canSetRawDataFixed()) {
      for (int i=0;i<3;i++) { //Loop over three gains
	//sort(it->second.vLArDigitFixed[i].begin(), it->second.vLArDigitFixed[i].end(), *m_BlStruct);
	m_BlStruct->sortDataVector(it->second.vLArDigitFixed[i]);
	std::vector<const LArDigit*>::const_iterator digit_it = it->second.vLArDigitFixed[i].begin();
	std::vector<const LArDigit*>::const_iterator digit_it_end=it->second.vLArDigitFixed[i].end();
	if(digit_it!=digit_it_end) {//Container not empty
	  m_BlStruct->setNumberOfSamples((*digit_it)->samples().size());
	  for (;digit_it!=digit_it_end;digit_it++)  {
	    int cId =  m_onlineHelper->channel((*digit_it)->hardwareID()); 
	    //cId = m_BlStruct->FebToRodChannel(cId);
	    m_BlStruct->setRawDataFixed(cId, (*digit_it)->samples(), (*digit_it)->gain());
	  }
	} //end Container not empty
      } // end loop over three gains
    } // end can set raw data fixed
  
    // ************** Calibration Digit Block ***********
    if (m_BlStruct->canSetCalibration()) { 
        for (int i=0;i<3;i++) { //Loop over three gains
	  //sort(it->second.vLArCalibDigit[i].begin(), it->second.vLArCalibDigit[i].end(), *m_BlStruct);
	  m_BlStruct->sortDataVector(it->second.vLArCalibDigit[i]);
	  std::vector<const LArCalibDigit*>::const_iterator digit_it = it->second.vLArCalibDigit[i].begin();
	  std::vector<const LArCalibDigit*>::const_iterator digit_it_end=it->second.vLArCalibDigit[i].end();
	  if(digit_it!=digit_it_end) {//Container not empty
	    m_BlStruct->setNumberOfSamples((*digit_it)->samples().size());
	    m_BlStruct->setDelay((*digit_it)->delay());
	    m_BlStruct->setDAC((*digit_it)->DAC());
	    for (;digit_it!=digit_it_end;digit_it++) { 
	      int cId =  m_onlineHelper->channel((*digit_it)->hardwareID()); 
	      //cId = m_BlStruct->FebToRodChannel(cId);
	      m_BlStruct->setRawDataFixed(cId, (*digit_it)->samples(), (*digit_it)->gain());
	      if ((*digit_it)->isPulsed())
		m_BlStruct->setPulsed(cId);
	    }
	  } //End Container not empty
	}// end loop over three gains
    } //end can set calibration
    
    /*
    // ************** Averaged Calibration Digit Block ***********
    if (m_BlStruct->canSetAverages()) { 
      for (int i=0;i<3;i++) { //Loop over three gains
	sort(it->second.vLArCalibDigit[i].begin(), it->second.vLArCalibDigit[i].end(), *m_BlStruct);
	std::vector<const LArAverageDigit*>::const_iterator digit_it = it->second.vLArAverageDigit.begin();
	std::vector<const LArAverageDigit*>::const_iterator digit_it_end=it->second.vLArAverageDigit.end();
	if(digit_it!=digit_it_end) {//Container not empty
	  m_BlStruct->setNumberOfSamples((*digit_it)->samples().size());
	  m_BlStruct->setDAC((*digit_it)->DAC());
	  m_BlStruct->setDelay((*digit_it)->delay());
	  for (;digit_it!=digit_it_end;digit_it++) { 
	    int cId =  m_cablingSvc->channel((*digit_it)->channelID()); 
	    cId = m_BlStruct->FebToRodChannel(cId);
	    m_BlStruct->setRawData(cId, (*digit_it)->samples(), (*digit_it)->gain());
	    m_BlStruct->setIsPulsed(cId);
	  }
	} //End Container not empty
      }// end loop over three gains
    }// can set averages
    */ 
    //std::cout << "Going to finalize FEB" << std::endl;
    m_BlStruct->finalizeFEB();
 }// end iterating over FEB-map
 //std::cout << "Going to finalize ROD" << std::endl;
 m_BlStruct->concatinateFEBs();
 //std::cout << "final size of fragment " << v.size() << std::endl;
 //m_BlStruct->dumpFragment(); // For testing purpose
 return; 
}

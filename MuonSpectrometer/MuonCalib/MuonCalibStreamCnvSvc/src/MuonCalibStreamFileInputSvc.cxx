/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//===================================================================
//  Implementation of MuonCalibStreamFileInputSvc
//  Revision:  July 11, 2002
//      Modified for eformat
//             
//===================================================================
//

// Include files.
#include "MuonCalibStreamCnvSvc/MuonCalibStreamFileInputSvc.h"
#include "MuCalDecode/CalibUti.h"

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include <iostream>

#include "EventStorage/pickDataReader.h" 
//#include "EventStorage/DataReaderBase.h" 
#include "EventStorage/EventStorageRecords.h" 
#include "EventStorage/DataReader.h"
#include "EventStorage/fRead.h" 
#include "EventStorage/loadfRead.h"


// Constructor.
MuonCalibStreamFileInputSvc::MuonCalibStreamFileInputSvc(const std::string& name, ISvcLocator* svcloc)
  :   
  MuonCalibStreamInputSvc(name,svcloc), 
  m_fileEventCounter(0),
  m_totalEventCounter(0),
  m_EoF(false), m_re(0), 
  m_dataLoader(0)
{
  m_DumpStream=0;
  declareProperty("InputFiles", m_inputFiles);
  declareProperty("DumpStream", m_DumpStream);
}

// Destructor.
MuonCalibStreamFileInputSvc::~MuonCalibStreamFileInputSvc()
{
}

// Open the first input file and read the first event.
StatusCode MuonCalibStreamFileInputSvc::initialize()
{
  StatusCode sc=  MuonCalibStreamInputSvc::initialize();
  if(!sc.isSuccess())   return sc;

  MsgStream log(messageService(), name() );
   
  // Check that the vector of input file names is not empty.

  if (m_inputFiles.empty())
    {
      log<<MSG::ERROR << " initialize: No input event files " <<endreq;
      return StatusCode::FAILURE;
    } 
  
  // Set the iterator to the first input file.
  m_inputFilesIt = m_inputFiles.begin();


  // instantiate the data loader

std::cout<<"QUI"<<std::endl;
  m_dataLoader=new LVL2_MUON_CALIBRATION::CalibDataLoader(m_inputFilesIt->c_str(), false,0xffffffff,0,DEFAULT_BUFFER_SIZE);
std::cout<<"QUI22"<<std::endl;
  
  m_EoF=false;

  return StatusCode::SUCCESS;
}

// Read the next event.
const LVL2_MUON_CALIBRATION::CalibEvent* MuonCalibStreamFileInputSvc::nextEvent()
{
  MsgStream log(messageService(), name() );

 // std::cout<<" in nextEvent "<< m_fRead<<m_fRead<<std::endl;

  if (m_re)
    {
       delete m_re;
       m_re=NULL;
    }

  
  if( m_EoF ) //EOF reached, need to change input file
    { 
      log << MSG::INFO << "nextEvent: end of file reached " << endreq; 
      m_totalEventCounter += m_fileEventCounter;

      log << MSG::INFO << "nextEvent: finished with file  " << *m_inputFilesIt << endreq;
      log << MSG::INFO << " Number of Events in this file " << m_fileEventCounter << endreq;
      log << MSG::INFO << " Total number of Events        " << m_totalEventCounter << endreq;

      // Reinitialize the file event counter.
      m_fileEventCounter = 0;

      ++m_inputFilesIt ; 

      if (m_inputFilesIt == m_inputFiles.end())
	{
	  log << MSG::INFO << "no more file to read  " << endreq;
	  return 0;
	} 

      // do the next File 

      delete m_dataLoader;
      m_dataLoader=new LVL2_MUON_CALIBRATION::CalibDataLoader(m_inputFilesIt->c_str(), false,0xffffffff,0,DEFAULT_BUFFER_SIZE);

      m_EoF=false;

      return nextEvent();
    }    

   ++m_fileEventCounter;

   // try to read next event

   if (m_dataLoader->next(m_dataBuffer)) 
     {

       m_re=new LVL2_MUON_CALIBRATION::CalibEvent(m_dataBuffer);

       if(m_re){
	 if(m_re->mdt()!=0){
	   if(m_DumpStream!=0) std::cout<<" MuonCalibStreamFileInputSvc::next event -- eta="<< m_re->eta()<<" mdt "<<*(m_re->mdt())<<std::endl;
	 }
	 if(m_re->rpc()!=0){
	   if(m_DumpStream!=0) std::cout<<" MuonCalibStreamFileInputSvc::next event -- eta="<< m_re->eta()<<" rpc "<<*(m_re->rpc())<<std::endl;
	 }
       } else { 

       }
     } else {
       // file must be over
       m_EoF=true;
       return nextEvent();
     }
   
    // Return 
   return m_re; 
}

/**  Get a pointer to the current event.
*/ 
const LVL2_MUON_CALIBRATION::CalibEvent* MuonCalibStreamFileInputSvc::currentEvent() const
{
   // Return a pointer to the  raw event.
   return m_re;
}


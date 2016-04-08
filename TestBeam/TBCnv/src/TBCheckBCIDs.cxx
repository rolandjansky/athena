//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TBCnv/TBCheckBCIDs.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "ByteStreamData/RawEvent.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 
#include "StoreGate/StoreGate.h"
//#include "CLIDSvc/CLASS_DEF.h"
#include <map> 


TBCheckBCIDs::TBCheckBCIDs(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_count(0),
    //m_ByteStreamEventAccess(0),
    m_rdpSvc(0),
    m_ntuplePtr(0)
{
}

TBCheckBCIDs::~TBCheckBCIDs() {}

StatusCode TBCheckBCIDs::initialize()
{
  StatusCode sc;
  MsgStream log(messageService(), name());
  log << MSG::DEBUG<< " initialize " <<endreq; 
  IService* svc;
  //Get ByteStreamInputSvc (only necessary for reading of digits, not for writing and for channels)
  
  if(StatusCode::SUCCESS != serviceLocator()->getService("ROBDataProviderSvc",svc)){
    log << MSG::WARNING << " Can't get ByteStreamInputSvc interface." << endreq;
     return StatusCode::FAILURE;
  }
  m_rdpSvc=dynamic_cast<IROBDataProviderSvc*>(svc);
  if(m_rdpSvc == 0 ) {
    log<<MSG::ERROR<< "  LArDigitContByteStreamCnv: Can't cast to  ByteStreamInputSvc " <<endreq; 
    return StatusCode::FAILURE;
  }
  //Book Ntuple

  
  NTupleFilePtr file1(ntupleSvc(),"/NTUPLES/FILE1");
  if (!file1)
    {log << MSG::ERROR << "Booking of NTuple failed" << endreq;
    return StatusCode::FAILURE;
   }
  NTuplePtr nt(ntupleSvc(),"/NTUPLES/FILE1/BCIDS");
  if (!nt) {
    nt=ntupleSvc()->book("/NTUPLES/FILE1/BCIDS",CLID_ColumnWiseTuple,"BCIDs");
  }
  if (!nt)
    {log << MSG::ERROR << "Booking of NTuple failed" << endreq;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("GlobalEvtID",m_EventID,0,50000);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'GlobalEvtID' failed" << endreq;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("LVL1ID",m_LVL1ID,0,50000);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'LVL1ID' failed" << endreq;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("RODIndex",m_RODIndex,0,100);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'ROD Index' failed" << endreq;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("RODID",m_RODIndex,m_RODID);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'RODID' failed" << endreq;
     return StatusCode::FAILURE;
    }
  
  sc=nt->addItem("BCID",m_RODIndex,m_BCID);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'BCID' failed" << endreq;
     return StatusCode::FAILURE;
    }
  m_ntuplePtr=nt;
  return StatusCode::SUCCESS;
}


//LArRawDataContByteStreamTool::ReadLArDigits(const RawEvent* re,LArDigitContainer* digitCont, const int gain, MsgStream& log)
StatusCode TBCheckBCIDs::execute()
{ MsgStream logstr(msgSvc(), name());
  logstr <<MSG::DEBUG<< "Executing event #"<< m_count++ <<endreq;

  unsigned RODCounter=0;
  const RawEvent* re = m_rdpSvc->getEvent();

  if (!re)
    {logstr <<MSG::FATAL << "RawEvent retrieved by TBCheckBCIDs is a null pointer!" << endreq;
     return StatusCode::FAILURE;
    }
  
  //Get ROB fragements from the raw event. 
  const size_t MAX_ROBFRAGMENTS = 2048*1024;
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType robF[MAX_ROBFRAGMENTS];
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType rePointer;
  re->start(rePointer);
  size_t robcount = re->children(robF,MAX_ROBFRAGMENTS);
  if (robcount == MAX_ROBFRAGMENTS)
    {
      logstr <<MSG::FATAL << "ROB buffer overflow" << endreq;
      return StatusCode::FAILURE;
    }
  

  m_LVL1ID=re->lvl1_id();
  m_EventID=re->global_id();
  

  
  for (size_t irob=0; irob<robcount; ++irob) //Loop over all ROB-Fragments
    {
      OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment rob(robF[irob]);
      
      try
	{
	  rob.check();
	}
      catch (...)
	{
	  logstr << MSG::ERROR << "Got invalid ROB fragment!" << endreq;
	  return StatusCode::FAILURE;
	}
      
      logstr << MSG::VERBOSE << MSG::hex << "\t\tFull source ID: " << rob.source_id()<<MSG::dec<< endreq;
      logstr << MSG::VERBOSE << "This ROD-Fragment consists of " << rob.rod_fragment_size_word() << " words\n";
      
      m_RODIndex=RODCounter;
      m_RODID[m_RODIndex]=rob.rod_source_id();
      m_BCID[m_RODIndex] =rob.rod_bc_id();
      logstr << MSG::DEBUG << "RODID 0x" << MSG::hex << rob.rod_source_id() << " BCID 0x" 
	     << rob.rod_bc_id() << MSG::dec << endreq;
      RODCounter++;
    }
  
  StatusCode sc=ntupleSvc()->writeRecord(m_ntuplePtr);
  if (sc!=StatusCode::SUCCESS) {
    logstr << MSG::ERROR << "writeRecord failed" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode TBCheckBCIDs::finalize() 
{return StatusCode::SUCCESS;}

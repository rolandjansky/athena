/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArROD/ReadLArDigits.h"
#include "LArRawEvent/LArDigit.h"
#include <vector>
// #include "GaudiKernel/IToolSvc.h"
#include "CaloIdentifier/CaloIdManager.h"


ReadLArDigits::SortDigits::SortDigits(const LArOnlineID* onlineHelper)
{m_onlineHelper=onlineHelper;
}

ReadLArDigits::ReadLArDigits(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator),
  m_emId(NULL),
  m_onlineHelper(NULL),
  m_ntuplePtr(NULL)
       
{m_count=0;
 declareProperty("ContainerKey",m_containerKey="");
 declareProperty("DumpFile",m_dumpFile="");
 declareProperty("PrintCellLocation",m_printCellLoc=false);
 declareProperty("PrintFebChan",m_printFebChan=true);
 
}

ReadLArDigits::~ReadLArDigits() 
{//empty
}

StatusCode ReadLArDigits::initialize()
{ MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize" << endmsg;

  const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
  m_emId=caloIdMgr->getEM_ID();

  ATH_CHECK(m_cablingKey.initialize());

  StatusCode sc = detStore()->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID helper !" << endmsg;
    return StatusCode::FAILURE;
  }

  if (m_dumpFile.size()>0)
    m_outfile.open(m_dumpFile.c_str(),std::ios::out);
  //Ntuple booking
 

  NTupleFilePtr file1(ntupleSvc(),"/NTUPLES/FILE1");
  if (!file1)
    {log << MSG::ERROR << "Booking of NTuple failed" << endmsg;
    return StatusCode::FAILURE;
   }
  NTuplePtr nt(ntupleSvc(),"/NTUPLES/FILE1/LARDIGITS");
  if (!nt) {
    nt=ntupleSvc()->book("/NTUPLES/FILE1/LARDIGITS",CLID_ColumnWiseTuple,"LArDigits");
  }
  if (!nt)
    {log << MSG::ERROR << "Booking of NTuple failed" << endmsg;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("icell",m_cellIndex,0,3600);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Cell Index' failed" << endmsg;
     return StatusCode::FAILURE;
    }

  //sc=nt->addItem("layer",m_layer,0,4);
  sc=nt->addItem("layer",m_cellIndex,m_layer);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Layer' failed" << endmsg;
     return StatusCode::FAILURE;
    }

  //sc=nt->addItem("ieta",m_eta,0,510);
  sc=nt->addItem("ieta",m_cellIndex,m_eta);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Eta' failed" << endmsg;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("iphi",m_cellIndex,m_phi);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Phi' failed" << endmsg;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("barrel_ec",m_cellIndex,m_barrel_ec);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'barrel_ec' failed" << endmsg;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("pos_neg",m_cellIndex,m_pos_neg);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'pos_neg' failed" << endmsg;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("FT",m_cellIndex,m_FT);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'FT' failed" << endmsg;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("slot",m_cellIndex,m_slot);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'slot' failed" << endmsg;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("channel",m_cellIndex,m_channel);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'channel' failed" << endmsg;
     return StatusCode::FAILURE;
    }

  sc=nt->addItem("gain",m_cellIndex,m_gain);
  if (sc!=StatusCode::SUCCESS)
    {log << MSG::ERROR << "addItem 'Gain' failed" << endmsg;
     return StatusCode::FAILURE;
    }  
  
  sc=nt->addItem("NSamples",m_Nsamples,0,32);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem 'sampleIndex' failed" << endmsg;
    return StatusCode::FAILURE;
  }
  
  sc=nt->addItem("Samples",m_cellIndex,m_samples,32);
  //sc=nt->addItem("Samples",m_cellIndex,m_samples,m_Nsamples);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem failed" << endmsg;
    return StatusCode::FAILURE;
  }
  
  m_ntuplePtr=nt;
  m_count=0;
  log << MSG::INFO << "======== ReadLArDigits initialize successfully ========" << endmsg;
  return StatusCode::SUCCESS;
}


StatusCode ReadLArDigits::execute()
{MsgStream log(msgSvc(), name());
 m_count++; 
 StatusCode sc; 
 log << MSG::VERBOSE << "======== executing event "<< m_count << " ========" << endmsg;
 log << MSG::VERBOSE << "Retrieving LArDigitContainer. Key= " << m_containerKey << std::endl; 
 LArDigitContainer* larDigitCont;
 if (m_containerKey.size())
   sc = evtStore()->retrieve(larDigitCont ,m_containerKey);
 else
   sc = evtStore()->retrieve(larDigitCont);
 if (sc.isFailure()) 
   {log << MSG::FATAL << " Cannot read LArDigitContainer from StoreGate! key=" << m_containerKey << endmsg;
    return StatusCode::FAILURE;
   }
 
 SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
 const LArOnOffIdMapping* cabling{*cablingHdl};
 if(!cabling) {
    ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key());
    return StatusCode::FAILURE;
 }
 if (m_outfile.is_open()) {
   SortDigits sortDigits(m_onlineHelper);
   std::sort(larDigitCont->begin(),larDigitCont->end(),sortDigits);
 }
 unsigned cellCounter=0;
 m_cellIndex=0;
 LArDigitContainer::const_iterator it=larDigitCont->begin();
 LArDigitContainer::const_iterator it_end=larDigitCont->end();
 if (larDigitCont->size()>0)
   m_Nsamples=(*it)->samples().size();
 else                       
   m_Nsamples=0;
 for (;it!=it_end;it++)
   {HWIdentifier chid=(*it)->hardwareID();
   const std::vector<short>& vSamples=(*it)->samples();
   m_cellIndex++;
    try {
      const Identifier id=cabling->cnvToIdentifier(chid);
      if (m_emId->is_lar_em(id))
	{m_eta[cellCounter]=m_emId->eta(id); 
	 m_phi[cellCounter]=m_emId->phi(id);
	 m_layer[cellCounter]=m_emId->sampling(id);
	}
      else {
	m_eta[cellCounter]=0;
	m_phi[cellCounter]=0;
	m_layer[cellCounter]=0;
      }
    }
    catch (LArID_Exception & except) {
      	m_eta[cellCounter]=-999;
	m_phi[cellCounter]=-999;
	m_layer[cellCounter]=-999;
    }
    m_barrel_ec[cellCounter]=m_onlineHelper->barrel_ec(chid);
    m_pos_neg[cellCounter] = m_onlineHelper->pos_neg(chid);
    m_FT[cellCounter] = m_onlineHelper->feedthrough(chid);
    m_slot[cellCounter] = m_onlineHelper->slot(chid);
    m_channel[cellCounter]   = m_onlineHelper->channel(chid);
    m_gain[cellCounter]=(*it)->gain();
    
    int nSamples=vSamples.size();
    for (int i=0;i<nSamples && i<32;i++)
      m_samples[cellCounter][i]=vSamples[i];
  
    if (m_outfile.is_open()) {
      if (m_printCellLoc) {
	if (m_eta[cellCounter]==-999 && m_phi[cellCounter]==-999 && m_layer[cellCounter]==-999)
	  m_outfile << "Cell l/e/p= <ILLEGAL IDENTIFIER> ";
	else
	  m_outfile << "Cell l/e/p= " << m_layer[cellCounter] << "/" << m_eta[cellCounter] 
		    << "/" << m_phi[cellCounter] << " ";
      }
      if (m_printFebChan)
	m_outfile << "FebId= 0x" << std::hex << m_onlineHelper->feb_Id(chid).get_compact() 
		  << std::dec << " Ch= " << m_channel[cellCounter] << " ";
      for (int i=0;i<nSamples;i++) {
	m_outfile << " " << vSamples[i];
      }
      m_outfile << " G=" << m_gain[cellCounter] << std::endl;
    }
    cellCounter++;
   }
 sc=ntupleSvc()->writeRecord(m_ntuplePtr);
 if (sc!=StatusCode::SUCCESS) {
   log << MSG::ERROR << "writeRecord failed" << endmsg;
   return StatusCode::FAILURE;
 }
 
 if (m_count%1000==0) 
   log << MSG::INFO  << "Event " << m_count << " contains " << cellCounter << " channels\n";
 return StatusCode::SUCCESS;
}

StatusCode ReadLArDigits::finalize()
{ MsgStream log(msgSvc(), name());
  if (m_outfile.is_open()) 
    m_outfile.close();
  log << MSG::INFO << "finalize ReadLarDigits" << endmsg;
  return StatusCode::SUCCESS;
}

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TBCnv/ReadTBLArDigits.h"
#include "LArRawEvent/LArDigit.h"
#include <vector>
#include "GaudiKernel/IToolSvc.h"
#include "CaloIdentifier/CaloCell_ID.h"

ReadTBLArDigits::SortDigits::SortDigits(const LArOnlineID* onlineHelper)
{
  m_onlineHelper = onlineHelper;
}

ReadTBLArDigits::ReadTBLArDigits(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_count(0),
    m_larCablingSvc(nullptr),
    m_emId(0),
    m_fcalId(0),
    m_hecId(0),
    m_onlineHelper(0),
    m_ntuplePtr(0)
{
 declareProperty("ContainerKey",m_containerKey="");
 declareProperty("DumpFile",m_dumpFile="");
 declareProperty("PrintCellLocation",m_printCellLoc=false);
 declareProperty("PrintFebChan",m_printFebChan=true);
 
}

ReadTBLArDigits::~ReadTBLArDigits() 
{//empty
}

StatusCode ReadTBLArDigits::initialize()
{ MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize" << endmsg;

  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );
  m_emId=idHelper->em_idHelper();
  m_fcalId=idHelper->fcal_idHelper();
  m_hecId=idHelper->hec_idHelper();

  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );

  ATH_CHECK( toolSvc()->retrieveTool("LArCablingLegacyService",m_larCablingSvc) );

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

  ATH_CHECK( nt->addItem("icell",m_cellIndex,0,3600) );
  ATH_CHECK( nt->addItem("layer",m_cellIndex,m_layer) );
  ATH_CHECK( nt->addItem("ieta",m_cellIndex,m_eta) );
  ATH_CHECK( nt->addItem("iphi",m_cellIndex,m_phi) );
  ATH_CHECK( nt->addItem("barrel_ec",m_cellIndex,m_barrel_ec) );
  ATH_CHECK( nt->addItem("pos_neg",m_cellIndex,m_pos_neg) );
  ATH_CHECK( nt->addItem("FT",m_cellIndex,m_FT) );
  ATH_CHECK( nt->addItem("slot",m_cellIndex,m_slot) );
  ATH_CHECK( nt->addItem("channel",m_cellIndex,m_channel) );
  ATH_CHECK( nt->addItem("gain",m_cellIndex,m_gain) );
  ATH_CHECK( nt->addItem("NSamples",m_Nsamples,0,32) );
  ATH_CHECK( nt->addItem("Samples",m_cellIndex,m_samples,32) );
  
  m_ntuplePtr=nt;
  m_count=0;
  log << MSG::INFO << "======== ReadTBLArDigits initialize successfully ========" << endmsg;
  return StatusCode::SUCCESS;
}


StatusCode ReadTBLArDigits::execute()
{MsgStream log(msgSvc(), name());
 m_count++; 
 StatusCode sc; 
 log << MSG::DEBUG << "======== executing event "<< m_count << " ========" << endmsg;
 log << MSG::DEBUG << "Retrieving TBLArDigitContainer. Key= " << m_containerKey << endmsg; 
 TBLArDigitContainer* larDigitCont;
 if (m_containerKey.size())
   sc = evtStore()->retrieve(larDigitCont ,m_containerKey);
 else
   sc = evtStore()->retrieve(larDigitCont);
 if (sc.isFailure()) 
   {log << MSG::FATAL << " Cannot read TBLArDigitContainer from StoreGate! key=" << m_containerKey << endmsg;
    return StatusCode::FAILURE;
   }
 

 if (m_outfile.is_open()) {
   log << MSG::DEBUG << "Sorting digits" << endmsg; 
   SortDigits sortDigits(m_onlineHelper);
   std::sort(larDigitCont->begin(),larDigitCont->end(),sortDigits);
 }
 
 log << MSG::DEBUG << "Finished sorting" << endmsg; 
 unsigned cellCounter=0;
 TBLArDigitContainer::const_iterator it=larDigitCont->begin();
 TBLArDigitContainer::const_iterator it_end=larDigitCont->end();
 if (larDigitCont->size()>0)
   m_Nsamples=(*it)->samples().size();
 else                       
   m_Nsamples=0;
 log << MSG::DEBUG << "Now loop over digits" << endmsg; 
 for (;it!=it_end;it++) {
   HWIdentifier chid=(*it)->hardwareID();
   log << MSG::DEBUG << "Get offline ID" << endmsg; 
   const Identifier id=m_larCablingSvc->cnvToIdentifier(chid);
   const std::vector<short>& vSamples=(*it)->samples();
   m_cellIndex=cellCounter;
   log << MSG::DEBUG << "Now find eta/phi (EM only right now)" << endmsg; 
   if (m_emId->is_lar_em(id)) {
     m_eta[m_cellIndex]=m_emId->eta(id); 
     m_phi[m_cellIndex]=m_emId->phi(id);
     m_layer[m_cellIndex]=m_emId->sampling(id);
   } else if (m_fcalId->is_lar_fcal(id)) {
     m_eta[m_cellIndex]=m_fcalId->eta(id); 
     m_phi[m_cellIndex]=m_fcalId->phi(id);
     m_layer[m_cellIndex]=m_fcalId->module(id);     
   } else if (m_hecId->is_lar_hec(id)) {
     m_eta[m_cellIndex]=m_hecId->eta(id); 
     m_phi[m_cellIndex]=m_hecId->phi(id);
     m_layer[m_cellIndex]=m_hecId->sampling(id);
   } else {
     m_eta[m_cellIndex]=0;
     m_phi[m_cellIndex]=0;
     m_layer[m_cellIndex]=0;
   }
   log << MSG::DEBUG << "Store identifier arrays" << endmsg;
   m_barrel_ec[m_cellIndex]=m_onlineHelper->barrel_ec(chid);
   m_pos_neg[m_cellIndex] = m_onlineHelper->pos_neg(chid);
   m_FT[m_cellIndex] = m_onlineHelper->feedthrough(chid);
   m_slot[m_cellIndex] = m_onlineHelper->slot(chid);
   m_channel[m_cellIndex]   = m_onlineHelper->channel(chid);
   m_gain[m_cellIndex]=(*it)->gain();
   
   log << MSG::DEBUG << "Loop over samples" << endmsg; 
   int nSamples=vSamples.size();
   for (int i=0;i<nSamples && i<32;i++)
     m_samples[m_cellIndex][i]=vSamples[i];
   
   if (m_outfile.is_open()) {
     m_outfile << "Evt="<< m_count << " ";
     if (m_printCellLoc)
       m_outfile << "Cell l/e/p= " << m_layer[m_cellIndex] << "/" << m_eta[m_cellIndex] 
                 << "/" << m_phi[m_cellIndex] << " ";
     if (m_printFebChan)
       m_outfile << "FebId= 0x" << std::hex << m_onlineHelper->feb_Id(chid).get_compact()
                 << std::dec << " Ch= " << m_onlineHelper->channel(chid) << " ";
     for (int i=0;i<nSamples;i++) {
       m_outfile << " " << vSamples[i];
     }
     m_outfile << " G=" << m_gain[m_cellIndex] << std::endl;
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

StatusCode ReadTBLArDigits::finalize()
{ MsgStream log(msgSvc(), name());
  if (m_outfile.is_open()) 
    m_outfile.close();
  log << MSG::INFO << "finalize ReadTBLArDigits" << endmsg;
  return StatusCode::SUCCESS;
}

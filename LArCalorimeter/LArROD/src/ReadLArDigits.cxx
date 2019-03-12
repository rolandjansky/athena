/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArROD/ReadLArDigits.h"
#include "LArRawEvent/LArDigit.h"
#include <vector>
// #include "GaudiKernel/IToolSvc.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "StoreGate/ReadHandle.h"


ReadLArDigits::SortDigits::SortDigits(const LArOnlineID* onlineHelper)
{m_onlineHelper=onlineHelper;
}

ReadLArDigits::ReadLArDigits(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator),
  m_emId(NULL),
  m_onlineHelper(NULL),
  m_ntuplePtr(NULL)
       
{m_count=0;
 declareProperty("DumpFile",m_dumpFile="");
 declareProperty("PrintCellLocation",m_printCellLoc=false);
 declareProperty("PrintFebChan",m_printFebChan=true);
 
}

ReadLArDigits::~ReadLArDigits() 
{//empty
}

StatusCode ReadLArDigits::initialize()
{
  ATH_MSG_INFO( "Initialize"  );

  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );
  m_emId=idHelper->em_idHelper();

  ATH_CHECK(m_cablingKey.initialize());
  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );

  if (m_dumpFile.size()>0)
    m_outfile.open(m_dumpFile.c_str(),std::ios::out);
  //Ntuple booking
 

  NTupleFilePtr file1(ntupleSvc(),"/NTUPLES/FILE1");
  if (!file1) {
    ATH_MSG_ERROR( "Booking of NTuple failed"  );
    return StatusCode::FAILURE;
  }
  NTuplePtr nt(ntupleSvc(),"/NTUPLES/FILE1/LARDIGITS");
  if (!nt) {
    nt=ntupleSvc()->book("/NTUPLES/FILE1/LARDIGITS",CLID_ColumnWiseTuple,"LArDigits");
  }
  if (!nt) {
    ATH_MSG_ERROR( "Booking of NTuple failed"  );
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

  ATH_CHECK( m_containerKey.initialize() );
  
  m_ntuplePtr=nt;
  m_count=0;
  ATH_MSG_INFO( "======== ReadLArDigits initialize successfully ========"  );
  return StatusCode::SUCCESS;
}


StatusCode ReadLArDigits::execute()
{
 m_count++; 
 ATH_MSG_VERBOSE( "======== executing event "<< m_count << " ========"  );
 ATH_MSG_VERBOSE( "Retrieving LArDigitContainer. Key= " << m_containerKey.key()  );
 const EventContext& ctx = Gaudi::Hive::currentContext();
 SG::ReadHandle<LArDigitContainer> larDigitContIn (m_containerKey, ctx);

 // View container copy.
 LArDigitContainer larDigitCont (*larDigitContIn);

 SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
 const LArOnOffIdMapping* cabling{*cablingHdl};
 if(!cabling) {
    ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key());
    return StatusCode::FAILURE;
 }


 if (m_outfile.is_open()) {
   SortDigits sortDigits(m_onlineHelper);
   std::sort(larDigitCont.begin(),larDigitCont.end(),sortDigits);
 }
 unsigned cellCounter=0;
 m_cellIndex=0;
 if (larDigitCont.size()>0)
   m_Nsamples=larDigitCont.front()->samples().size();
 else                       
   m_Nsamples=0;
 for (const LArDigit* digit : larDigitCont) {
   HWIdentifier chid=digit->hardwareID();
   const std::vector<short>& vSamples=digit->samples();
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
    m_gain[cellCounter]=digit->gain();
    
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
 ATH_CHECK( ntupleSvc()->writeRecord(m_ntuplePtr) );
 
 if (m_count%1000==0) 
   ATH_MSG_INFO( "Event " << m_count << " contains " << cellCounter << " channels" );
 return StatusCode::SUCCESS;
}

StatusCode ReadLArDigits::finalize()
{
  if (m_outfile.is_open()) 
    m_outfile.close();
  ATH_MSG_INFO( "finalize ReadLarDigits"  );
  return StatusCode::SUCCESS;
}

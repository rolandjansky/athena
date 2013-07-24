/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArOFPhaseFill.h"
#include "CaloIdentifier/CaloGain.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include <fstream>
#include <map>
#include <sstream>

typedef unsigned int uint;

LArOFPhaseFill::LArOFPhaseFill(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  m_detStore(0),
  m_lar_on_id(0),
  m_OFCbin(0),
  m_groupingType(0)
{
  declareProperty("InputFile",m_InputFile);
  declareProperty("keyOFCbin",m_keyOFCbin);
  declareProperty("GroupingType",m_groupingName="Unknown");
  declareProperty("DefaultPhase",m_defaultPhase=0);
}

LArOFPhaseFill::~LArOFPhaseFill()
{  }

StatusCode LArOFPhaseFill::initialize()
{
  MsgStream  log(messageService(),name());

  // retrieve detector Store
  StatusCode sc = service("DetectorStore", m_detStore);
  if (sc.isFailure()) 
  {
    log << MSG::ERROR
	<< "Unable to retrieve pointer to DetectorStore "
	<< endreq;
    return sc;
  }

  sc = m_detStore->retrieve(m_lar_on_id,"LArOnlineID");
  if (sc.isFailure()) {
    log  << MSG::ERROR << "Unable to retrieve  LArOnlineID from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }

  if ( m_groupingName == "Unknown" ) {
     m_groupingType = LArConditionsContainerBase::Unknown ;
  } else if ( m_groupingName == "SingleGroup" ) {
     m_groupingType = LArConditionsContainerBase::SingleGroup ;
  } else if ( m_groupingName == "SubDetector" ) {
     m_groupingType = LArConditionsContainerBase::SubDetectorGrouping ;
  } else if ( m_groupingName == "FeedThrough" ) {
     m_groupingType = LArConditionsContainerBase::FeedThroughGrouping ;
  } else if ( m_groupingName == "ExtendedFeedThrough" ) {
     m_groupingType = LArConditionsContainerBase::ExtendedFTGrouping ;
  } else if ( m_groupingName == "ExtendedSubDet" ) {
     m_groupingType = LArConditionsContainerBase::ExtendedSubDetGrouping ;
  } else {
     log << MSG::ERROR << "Grouping type " << m_groupingName << " is not foreseen!" << endreq ;
     log << MSG::ERROR << "Only \"Unknown\", \"SingleGroup\", \"SubDetector\", \"FeedThrough\", \"ExtendedFeedThrough\" and \"ExtendedSubDet\" are allowed" << endreq ;
     return StatusCode::FAILURE ;
  }

  return StatusCode::SUCCESS;
}

StatusCode LArOFPhaseFill::execute()
{
  MsgStream  log(messageService(),name());  
  
  log<<MSG::DEBUG<<" In execute() " << endreq;

 

  return StatusCode::SUCCESS;
}



StatusCode LArOFPhaseFill::stop()
{

  MsgStream  log(messageService(),name());  
  
  log<<MSG::DEBUG<<" In stop() " << endreq;

  typedef std::pair<HWIdentifier, int> idi;
  std::map<idi, uint > inmap;
  uint b_ec, p_n, ft, sl, ch, g;
  uint count; 
  // Open a file and read it - if exists
  if(m_InputFile.size() > 0) {
     std::ifstream *in = new std::ifstream(m_InputFile.c_str());
     if(!in->good()) {
        log<<MSG::ERROR<<"Could not open map file "<<m_InputFile<<endreq;
        log<<MSG::ERROR<<"Using default phase " << m_defaultPhase << " for all channels"<<endreq;
     } else {
        uint phase;
        count = 0;
        char line[100];
        do {
           in->getline(line,99);
           if((!in->good()) || in->eof()) break;
           if(line[0]=='#') continue;
           std::istringstream iss(line);
           iss>>std::dec>>b_ec>>p_n>>ft>>sl>>ch>>g>>phase;
           if(!iss.good()) {
              log<<MSG::WARNING<<"Wrong line: "<<line<<endreq;
              continue;
           }
           if(b_ec > 1) {
               log<<MSG::ERROR<<"Wrong barrel_ec: "<<b_ec<<", not taken"<<endreq;
               continue;
           }
           if(p_n > 1) {
               log<<MSG::ERROR<<"Wrong pos_neg: "<<p_n<<", not taken"<<endreq;
               continue;
           }
           if(ft > 31) {
               log<<MSG::ERROR<<"Wrong FTH: "<<ft<<", not taken"<<endreq;
               continue;
           }
           if(sl == 0 || sl > 15) {
               log<<MSG::ERROR<<"Wrong slot: "<<sl<<", not taken"<<endreq;
               continue;
           }
           if(ch > 127) {
               log<<MSG::ERROR<<"Wrong channel: "<<ch<<", not taken"<<endreq;
               continue;
           }
           if((int)g < (int)CaloGain::LARHIGHGAIN || g > CaloGain::LARLOWGAIN) {
               log<<MSG::ERROR<<"Wrong gain: "<<g<<", not taken"<<endreq;
               continue;
           }
           if(phase > 50) {
               log<<MSG::ERROR<<"Wrong phase: "<<phase<<", not taken"<<endreq;
               continue;
           }
           HWIdentifier oc = m_lar_on_id->channel_Id(b_ec, p_n, ft, sl, ch); 
           inmap[std::make_pair(oc,g)] = phase;
           ++count;
       }while(!in->eof());
     }
  }

  std::vector<HWIdentifier>::const_iterator it   = m_lar_on_id->channel_begin();
  std::vector<HWIdentifier>::const_iterator it_e = m_lar_on_id->channel_end();

  StatusCode sc;
  m_OFCbin = new LArOFCBinComplete();
  m_OFCbin->setGroupingType( static_cast<LArConditionsContainerBase::GroupingType>(m_groupingType) );
  sc  = m_OFCbin->initialize(); 
  if ( sc.isFailure() ) {
     log << MSG::ERROR << "Could not initialize data object - exit!" << endreq ;
     return sc ;
  }

  int fphase;
  count = 0;
  for(; it!=it_e;++it){
    const HWIdentifier id = *it;
    //if(! m_lar_on_id->isHECchannel(id)) continue;
    sl  = m_lar_on_id->slot(id);
    ch = m_lar_on_id->channel(id);
    ft = m_lar_on_id->feedthrough(id);
    p_n = m_lar_on_id->pos_neg(id);
    b_ec = m_lar_on_id->barrel_ec(id);
    HWIdentifier oc = m_lar_on_id->channel_Id(b_ec, p_n, ft, sl, ch); 
    ++count;
    for(g = CaloGain::LARHIGHGAIN; g <= CaloGain::LARLOWGAIN; ++g) {
      if(inmap.find(std::make_pair(oc,g)) != inmap.end()) 
	fphase = inmap[std::make_pair(oc,g)]; 
      else 
	fphase = m_defaultPhase;
       log<<MSG::DEBUG<<"B_EC: "<<b_ec<<" Pos_Neg: "<<p_n<<" FT: "<<ft<<" Slot: "<<sl<<" Chan: "<<ch<<" Phase: "<<fphase<<endreq;
       m_OFCbin->set(id,g,fphase);
    }
  }  

  log<<MSG::DEBUG<<" Loop over " << count << " cells" << endreq;

  sc = m_detStore->record(m_OFCbin, m_keyOFCbin);


  return StatusCode::SUCCESS;
  
}


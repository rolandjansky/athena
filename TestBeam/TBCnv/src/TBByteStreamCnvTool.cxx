/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBCnv/TBByteStreamCnvTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "eformat/Status.h"

#include "GaudiKernel/ListItem.h"

#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloGain.h"
#include "Identifier/HWIdentifier.h"

#include <list>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

const eformat::SubDetector TBByteStreamCnvTool::m_DontCheckRodSubDetID=(eformat::SubDetector)0xff;


const InterfaceID& TBByteStreamCnvTool::interfaceID( )
{ 
  static const InterfaceID IID_ITBByteStreamCnvTool
    ("TBByteStreamCnvTool", 1, 0);
  return IID_ITBByteStreamCnvTool; 
}

// default contructor
 
TBByteStreamCnvTool::TBByteStreamCnvTool
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool(type,name,parent), m_lastEventID(0, UINT_MAX), m_subdet_id(eformat::TDAQ_BEAM_CRATE),
     m_theRodBlock(0), m_theROB(0)
{ 
  declareInterface< TBByteStreamCnvTool  >( this );
  m_tbtdc=0;
  declareProperty("ForceHchoice",m_force_Hchoice=false);
  declareProperty("Dump",m_dump=false);
  declareProperty("isH6Run",m_H6run=true);
  declareProperty("isH8Run",m_H8run=false);
  declareProperty("Keys",m_keys);
  declareProperty("SubDetID",m_subdet_key);
  declareProperty("SummaryPath",m_summary_path);

}
 
StatusCode TBByteStreamCnvTool::initialize()
{StatusCode sc=AthAlgTool::initialize();
 if (sc!=StatusCode::SUCCESS)
   return sc;
 MsgStream logstr(msgSvc(), name());
 logstr << MSG::DEBUG << "Initialize" << endmsg;

 const LArOnlineID* online_id;
 sc = detStore()->retrieve(online_id, "LArOnlineID");
 if (sc.isFailure()) {
   logstr << MSG::FATAL << "Could not get LArOnlineID helper !" << endmsg;
    return StatusCode::FAILURE;
 } 
 else {
   m_onlineHelper=online_id;
   logstr << MSG::DEBUG << " Found the LArOnlineID helper. " << endmsg;
 }

 IToolSvc* toolSvc;
 sc=service( "ToolSvc",toolSvc  );
 if (sc.isFailure()) {
   logstr << MSG::ERROR << "Unable to retrieve ToolSvc" << endmsg;
   return StatusCode::FAILURE;
 }
 
 sc=toolSvc->retrieveTool("LArCablingService",m_larCablingSvc);
 if (sc.isFailure()) {
   logstr << MSG::ERROR << "Unable to retrieve LArCablingService" << endmsg;
   return StatusCode::FAILURE;
 }
 
 IService* svc;
 sc= service("ByteStreamCnvSvc",svc);
 if (sc!=StatusCode::SUCCESS)
   {logstr << MSG::ERROR << " Can't get ByteStreamEventAccess interface " << endmsg;
    return StatusCode::FAILURE;
   }
 
  m_ByteStreamEventAccess=dynamic_cast<ByteStreamCnvSvc*>(svc);
  if (m_ByteStreamEventAccess==NULL)
    {
      logstr <<MSG::ERROR<< " Can't cast to  ByteStreamCnvSvc " <<endmsg; 
      return StatusCode::FAILURE ;
    }
 
  sc=service("ROBDataProviderSvc",svc);
  if (sc!=StatusCode::SUCCESS)
   {logstr << MSG::WARNING << " Can't get ROBDataProviderSvc. Reading of ByteStream Data not possible " << endmsg;
     m_rdpSvc=0;
   }
  else
    {m_rdpSvc=dynamic_cast<IROBDataProviderSvc*>(svc);
     if(m_rdpSvc == 0 )
       {logstr <<MSG::ERROR<< "Can't cast to ROBDataProviderSvc " <<endmsg; 
        return StatusCode::FAILURE;
       }
    }

  
  std::vector<std::string>::const_iterator it   = m_keys.begin();
  std::vector<std::string>::const_iterator it_e = m_keys.end();
  std::vector<std::string> keys;
  keys.resize(24);
  for(; it!=it_e;++it) {
    ListItem item(*it);
    std::string t = item.type();
    std::string nm = item.name();
    logstr << MSG::DEBUG << " type "<<t<<" name="<<nm<<endmsg;
    if(t=="TBTDC") keys[0]=nm;
    if(t=="TBTriggerPatternUnit") keys[1]=nm;
    if(t=="TBTDCRawCont") keys[2]=nm;
    if(t=="TBADCRawCont") keys[3]=nm;
    if(t=="TBTailCatcherRaw") keys[4]=nm;
    if(t=="TBBPCRawCont") keys[5]=nm;
    if(t=="TBScintillatorRawCont") keys[6]=nm;
    if(t=="TBMWPCRawCont") keys[7]=nm;
  }
  // Fixed name.
  keys[10]="FREE";
  keys[11]="LOW";
  keys[12]="MEDIUM";
  keys[13]="HIGH";
  // name for Calib Digits
  keys[20]="FREE";
  keys[21]="LOW";
  keys[22]="MEDIUM";
  keys[23]="HIGH";

  
  m_keys=keys;
  m_subdet_id=(eformat::SubDetector)m_subdet_key;

  m_isCalib = false;

  m_febgain.clear();

  return StatusCode::SUCCESS;
}

StatusCode TBByteStreamCnvTool::finalize()
{
  return StatusCode::SUCCESS;
}

/* ----------------------------------------------------------------------

        Write ByteStream Methods

------------------------------------------------------------------------*/

StatusCode TBByteStreamCnvTool::WriteFragment()
{
  /** This creates the fragment for beam detectors
      calling BuildRodBlock method   **/

  MsgStream logstr(msgSvc(), name());
 logstr << MSG::DEBUG << "WriteFragment" << endmsg;
 //Check if Fragment is already written
 const DataHandle<EventInfo> thisEventInfo;
   StatusCode sc=evtStore()->retrieve(thisEventInfo);
   if (sc!=StatusCode::SUCCESS)
     {logstr << MSG::WARNING << "No EventInfo object found!" << endmsg;
      return sc;
     }
   const EventID* thisEvent=thisEventInfo->event_ID();
   if (m_lastEventID==*thisEvent)
     {logstr << MSG::WARNING << "LArByteStrem persistency representation for Event #"<<thisEvent->event_number() 
	     << " already created!" << endmsg;
     return StatusCode::SUCCESS;
     }
   else //New event ID
     m_lastEventID=*thisEvent;

   // Get the already existing part of the RawEvent 
   RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent(); 

   // delete previous fragments
   if (m_theRodBlock) delete m_theRodBlock;
   if (m_theROB) delete m_theROB;

   //Make a new ROD-Fragment
   m_theRodBlock=new std::vector<uint32_t>;

   sc=BuildRODBlock(m_theRodBlock);

   if (sc!=StatusCode::SUCCESS)
     return sc;

   //Header elements for new Fragments
   const uint8_t module_id=0;
   const uint32_t lvl1_id=re->lvl1_id(); 
   const uint32_t bc_id=0;
   const uint32_t lvl1_type = 0;
   const uint32_t detev_type=0; 
   const bool status_place=eformat::STATUS_BACK;
   const uint32_t run_no=re->run_no(); 

   //Make a new ROB-Fragment...
   eformat::helper::SourceIdentifier robsid(m_subdet_id, module_id);
   m_theROB = new OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment
     (robsid.code(),run_no,lvl1_id,bc_id,lvl1_type,detev_type,m_theRodBlock->size(),
      &(*m_theRodBlock)[0],status_place);

   re->append(m_theROB); //Append the newly created Subdetector-Fragment to the full event
   logstr << MSG::DEBUG << "Appended TestBeam fragment to ByteStream" << endmsg;
   return StatusCode::SUCCESS;
}
     
StatusCode TBByteStreamCnvTool::BuildRODBlock(std::vector<uint32_t> *theRodBlock)
{
  /** Build a ROD data block for all Beam detector classes
      found in SG. Working only with TBTDC class for testing **/

  MsgStream logstr(msgSvc(), name());
  //Get necessary Data objects
  StatusCode sc;

  // TBTDCRaw part of the block : 
  const TBTDC* tbtdc;
  std::string tbtdc_Key("TBTDC");
  sc=evtStore()->retrieve(tbtdc,tbtdc_Key);
  if (sc!=StatusCode::SUCCESS)
    {logstr << MSG::ERROR << "Can't retrieve TBTDC with key "<< tbtdc_Key << " from StoreGate" << endmsg;
     return sc;
    }
  else {
    theRodBlock->push_back(4);  // TIME_SIZE
    theRodBlock->push_back(0x03);  // TIME_ID
    theRodBlock->push_back(tbtdc->tdc());
    theRodBlock->push_back(tbtdc->tdcmin());
  }
  

  // TBBPCRaw :
//   TBBPCRawCont *  bpcrawCont;
//   sc = m_storeGate->retrieve(bpcrawCont, "BPCRawCont");
//   if (sc.isFailure()){
//     logstr << MSG::DEBUG << "BeamDetectorMonitoring: Retrieval of BPCRaw failed" << endmsg;   
//   }else {
//     TBBPCRawCont::const_iterator it_bc   = bpcrawCont->begin();
//     TBBPCRawCont::const_iterator last_bc   = bpcrawCont->end();
// //     theRodBlock->push_back(2+bpcrawCont->size()*6);  // BPC_SIZE
// //     theRodBlock->push_back(0x05);                  // BPC_ID (BPCRaw)

//     for(it_bc= bpcrawCont->begin();it_bc!=last_bc;it_bc++){
// //       theRodBlock->push_back((*it_bc)->getSignal((int) TBBPCRaw::tdcLeft));
// //       theRodBlock->push_back((*it_bc)->getSignal((int) TBBPCRaw::tdcRight));
// //       theRodBlock->push_back((*it_bc)->getSignal((int) TBBPCRaw::tdcUp));
// //       theRodBlock->push_back((*it_bc)->getSignal((int) TBBPCRaw::tdcDown));
// //       theRodBlock->push_back((*it_bc)->getSignal((int) TBBPCRaw::adcHorizontal));
// //       theRodBlock->push_back((*it_bc)->getSignal((int) TBBPCRaw::adcVertical));
//     }
//   }

  // Trigger Pattern :
//   TBTriggerPatternUnit *trigpat;
//     sc = m_storeGate->retrieve(trigpat, "TBTrigPat");
//   if (sc.isFailure()){
//     logstr << MSG::DEBUG << "BeamDetectorMonitoring: Retrieval of TrigPat failed" << endmsg;
    
//   }else {
//     theRodBlock->push_back(3);           // TrigPat_SIZE
//     theRodBlock->push_back(0x01);        // TrigPat_ID (TrigPatRaw)
//     theRodBlock->push_back(trigpat->getTriggerWord());
//   }


  return StatusCode::SUCCESS;
}




/* ----------------------------------------------------------------------

        Read ByteStream Methods

------------------------------------------------------------------------*/

StatusCode TBByteStreamCnvTool::ReadFragment(int unrec_code){

  StatusCode sc;
  if(m_H6run) sc=H6BuildObjects(unrec_code); // build all objects, do not record object unrec_code
  if(m_H8run) sc=H8BuildObjects(unrec_code); // build all objects, do not record object unrec_code
  return sc;
}

StatusCode TBByteStreamCnvTool::ReadFragment(TBTDC*& tbtdc,std::string key)
{
  /** This method is used only for testing **/
  MsgStream logstr(msgSvc(), name());
  logstr << MSG::DEBUG <<" in ReadFragment (TBTDC) key="<<key<<endmsg;
  StatusCode sc=GetRODBlock(m_subdet_id); //Possible improvement: Check if vector is already valid. 
  if (sc!=StatusCode::SUCCESS)
    return sc;
  if (m_rodBlock.size()<2)
    {
      logstr << "ReadRodBlock: RodBlock too small!" << endmsg;
      return StatusCode::FAILURE;
   }
  int tdc=m_rodBlock[0];
  int tdcmin=m_rodBlock[1];
  logstr << MSG::DEBUG <<" tdc =  "<< tdc <<endmsg;
  logstr << MSG::DEBUG <<" tdcmin =  "<< tdcmin <<endmsg;
  tbtdc=new TBTDC(tdc,tdcmin);
  return StatusCode::SUCCESS;
  // return sc;
}

StatusCode TBByteStreamCnvTool::ReadFragment(TBBPCRawCont*& bpcrawCont,std::string key)
{MsgStream logstr(msgSvc(), name());
  logstr << MSG::DEBUG <<" in ReadFragment (TBBPCRawCont) key="<<key<<endmsg;
  StatusCode sc=ReadFragment(5);
  if(sc) bpcrawCont = m_bpcrawCont;
  logstr << MSG::DEBUG <<" End ReadFragment (TBBPCRawCont) key="<<key<<endmsg;
  return sc;
}

StatusCode TBByteStreamCnvTool::ReadFragment(TBMWPCRawCont*& mwpcrawCont,std::string key)
{MsgStream logstr(msgSvc(), name());
  logstr << MSG::DEBUG <<" in ReadFragment (TBMWPCRawCont) key="<<key<<endmsg;
  StatusCode sc=ReadFragment(7);
  if(sc) mwpcrawCont = m_mwpcrawCont;
  return sc;
}

StatusCode TBByteStreamCnvTool::ReadFragment(TBADCRawCont*& adcrawCont,std::string key)
{MsgStream logstr(msgSvc(), name());
  logstr << MSG::DEBUG <<" in ReadFragment (TBADCRawCont) key="<<key<<endmsg;
  StatusCode sc=ReadFragment(3);
  if(sc) adcrawCont = m_adcrawCont;
  return sc;
}
StatusCode TBByteStreamCnvTool::ReadFragment(TBTDCRawCont*& tdcrawCont,std::string key)
{MsgStream logstr(msgSvc(), name());
  logstr << MSG::DEBUG <<" in ReadFragment (TBTDCRawCont) key="<<key<<endmsg;
  StatusCode sc=ReadFragment(2);
  if(sc) tdcrawCont = m_tdcrawCont;
  return sc;
}

StatusCode TBByteStreamCnvTool::ReadFragment(TBTriggerPatternUnit*& trigpat,std::string key)
{MsgStream logstr(msgSvc(), name());
  logstr << MSG::DEBUG <<" in ReadFragment (TBTriggerPatternUnit) key="<<key<<endmsg;
  StatusCode sc=ReadFragment(1);
  if(sc) trigpat = m_trigpat;
  return sc;
}

StatusCode TBByteStreamCnvTool::ReadFragment(TBScintillatorRawCont*& scintrawCont,std::string key)
{MsgStream logstr(msgSvc(), name());
  logstr << MSG::DEBUG <<" in ReadFragment (TBScintrawcont) key="<<key<<endmsg;
  StatusCode sc=ReadFragment(6);
  if(sc) scintrawCont = m_scintrawCont;
  return sc;
}

StatusCode TBByteStreamCnvTool::ReadFragment(TBTailCatcherRaw*& tailcatchraw,std::string key)
{MsgStream logstr(msgSvc(), name());
  logstr << MSG::DEBUG <<" in ReadFragment (TBTailCatcherRaw) key="<<key<<endmsg;
  StatusCode sc=ReadFragment(4);
  if(sc) tailcatchraw = m_tailcatchraw;
  return sc;
}
StatusCode TBByteStreamCnvTool::ReadFragment(TBEventInfo*& tbeventinfo,std::string key)
{MsgStream logstr(msgSvc(), name());
  logstr << MSG::DEBUG <<" in ReadFragment (TBEventInfo) key="<<key<<endmsg;
  StatusCode sc=ReadFragment(30);
  if(sc) tbeventinfo = m_eventinfo;
  return sc;
}

StatusCode TBByteStreamCnvTool::ReadFragment(TBLArDigitContainer*& tblardigitcont,std::string key)
{MsgStream logstr(msgSvc(), name());
  logstr << MSG::DEBUG <<" in ReadFragment (TBLArDigitContainer) key="<<key<<endmsg;
  int code=0;
  if(key=="FREE") code = 10;
  if(key=="LOW") code = 11;
  if(key=="HIGH") code = 12;
  if(key=="MEDIUM") code = 13;
  if(code){
    StatusCode sc=ReadFragment(code);
    if(sc) tblardigitcont = m_tblardigitcont[code-10];
    return sc;
  }else{
    logstr << MSG::ERROR <<" Can not create a LArDigitContainer with key="<<key<<endmsg;
    tblardigitcont =0;
    return StatusCode::FAILURE;
  }
}

StatusCode TBByteStreamCnvTool::ReadFragment(TBLArCalibDigitContainer*& tblarcalibdigitcont,std::string key)
{MsgStream logstr(msgSvc(), name());
  logstr << MSG::DEBUG <<" in ReadFragment (TBLArCalibDigitContainer) key="<<key<<endmsg;
  int code=0;
  if(key=="FREE") code = 20;
  if(key=="LOW")  code = 21;
  if(key=="HIGH") code = 22;
  if(key=="MEDIUM") code = 23;
  if(code){
    StatusCode sc=ReadFragment(code);
    if(sc) tblarcalibdigitcont = m_tblarcalibdigitcont[code-20];
    return sc;
  }else{
    logstr << MSG::ERROR <<" Can not create a LArCalibDigitContainer with key="<<key<<endmsg;
    tblarcalibdigitcont =0;
    return StatusCode::FAILURE;
  }
}



StatusCode TBByteStreamCnvTool::GetRODBlock(eformat::SubDetector subdet_id, 
					    eformat::SubDetector subdet_rod_id)//=m_DontCheckRodSubDetID)

{MsgStream logstr(msgSvc(), name());
 logstr << MSG::DEBUG << "GetRODBlock" << endmsg;
 if (!m_rdpSvc)
   {logstr << MSG::ERROR << "ROBDataProviderSvc not loaded. Can't read ByteStream" << endmsg;
    return StatusCode::FAILURE;
   }

  const RawEvent* re = m_rdpSvc->getEvent();
 
  if (!re)
    {logstr <<MSG::FATAL << "Can't get RawEvent!" << endmsg;
    return StatusCode::FAILURE;
    }
  
  const size_t MAX_ROBFRAGMENTS = 2048*1024;
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType robF[MAX_ROBFRAGMENTS];
  size_t robcount = re->children(robF,MAX_ROBFRAGMENTS);
  //  re->start(robF);
  if (robcount == MAX_ROBFRAGMENTS)
    {
      logstr <<MSG::FATAL << "ROB buffer overflow" << endmsg;
      return StatusCode::FAILURE;
    }
  
  for (size_t irob=0; irob<robcount; ++irob) //Loop over all ROB-Fragments
    {
      OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment rob(robF[irob]);
      
      logstr << MSG::DEBUG << "ROB Frag *****************************" << endmsg;
      MSG::hex(logstr) << MSG::DEBUG <<"Marker     : " << rob.marker() << endmsg;
      MSG::hex(logstr) << MSG::DEBUG <<"Frag Size  : " << rob.fragment_size_word() << endmsg;
      MSG::hex(logstr) << MSG::DEBUG <<"Header Size: " << rob.header_size_word() << endmsg;
      MSG::hex(logstr) << MSG::DEBUG <<"Source  ID : " << rob.source_id() << endmsg;
      MSG::hex(logstr) << MSG::DEBUG <<"Version    : " << rob.version() << endmsg;
      try
	{
	  rob.check();
	}
      catch (...)
	{
	  logstr << MSG::ERROR << "Got invalid ROB fragment!" << endmsg;
	  return StatusCode::FAILURE;
	}
      
      logstr << MSG::DEBUG << "ROD Frag *****************************" << endmsg;
      MSG::hex(logstr) << MSG::DEBUG <<"Frag Size  : " << rob.rod_fragment_size_word() << endmsg;
      MSG::hex(logstr) << MSG::DEBUG <<"Header Size: " << rob.rod_header_size_word() << endmsg;
      MSG::hex(logstr) << MSG::DEBUG <<"Source  ID : " << rob.rod_source_id() << endmsg;
      MSG::hex(logstr) << MSG::DEBUG <<"Run num    : " << rob.rod_run_no() << endmsg;
      MSG::hex(logstr) << MSG::DEBUG <<"Version    : " << rob.rod_version() << endmsg;
      
      eformat::helper::SourceIdentifier rod_sid(rob.rod_source_id());
      if (subdet_rod_id==m_DontCheckRodSubDetID || rod_sid.subdetector_id()==subdet_rod_id)
	{
	  MSG::hex(logstr) << MSG::DEBUG << "Found requested ROD (id=" 
			   << rod_sid.subdetector_id() << ")"  << endmsg;
	  // Set H6 or H8 mode :
	  if (!m_force_Hchoice)
	    {
	      logstr<< MSG::INFO << "Guessing from ROD header -> " ;
	      if (rob.rod_detev_type()&1)
		{
		  logstr<< " H6 !" << endmsg;
		  m_H6run = true;
		  m_H8run = false;
		}
	      else 
		{
		  logstr<< " H8 !" << endmsg;
		  m_H6run = false;
		  m_H8run = true;
		}
	    }
	  m_h8_triggword = rob.rod_lvl1_trigger_type();
	  // Event number ? 
	  m_ev_number = rob.rod_lvl1_id();
	  if(m_dump) MSG::dec(logstr) << MSG::INFO << " Ev. number(header)=" << m_ev_number <<endmsg;
	  
	  OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodPointer;
	  rob.rod_data(rodPointer);
	  m_rodBlock.clear();
	  for(size_t i=0; i<rob.rod_ndata(); ++i)
	    {
	      m_rodBlock.push_back(*rodPointer);
	      ++rodPointer; 
	    }
	  
	  if (m_rodBlock.size()==0)
	    {
	      logstr << MSG::FATAL << "Error reading bytestream event: "
		     << "Empty ROD block" << endmsg;
	      return StatusCode::FAILURE;
	    }
	  logstr << MSG::DEBUG << "Got Rod Block for Test Beam Instrumentation data from ByteStream" << endmsg;
	  return StatusCode::SUCCESS;
	}// end if requested ROD
    }//end loop over ROBs

  MSG::hex(logstr) << MSG::WARNING << " Didn't find SubDet ID for Beam Instruments. Should be "<< subdet_id<<endmsg;
  return StatusCode::FAILURE;
}

/*---------------------------------------------------------------------------*/

StatusCode TBByteStreamCnvTool::H6BuildObjects(int unrec_code)

/*---------------------------------------------------------------------------*/
{
  /** Build all beam detectors raw objects from m_rodBlock using H6 convention.
      Objects are recorded in SG if they do not correspond to unrec_code
      Will return faillure if : -one of the object could not be recorded in SG
                                -the requested object (unrec_code) was not found

  **/

  MsgStream logstr(msgSvc(), name());
  logstr << MSG::DEBUG << "H6BuildObject called for " << unrec_code<< endmsg;

  StatusCode sc=StatusCode::FAILURE;
  //bool gotobject=false;
  //bool recordfailure=false;

  // for string conversion
  std::ostringstream os;

  // Get ROD block.
  sc=GetRODBlock(m_subdet_id); //Possible improvement: Check if vector is already valid. 
  if (sc!=StatusCode::SUCCESS)
    return sc;
  if (m_rodBlock.size()<2)
    {MsgStream logstr(msgSvc(), name());
      logstr << MSG::ERROR << "ReadRodBlock: RodBlock too small!" << endmsg;
      return StatusCode::FAILURE;
   }
 



  m_trigpat      = new TBTriggerPatternUnit();
  m_tailcatchraw = 0;
  m_bpcrawCont   = new TBBPCRawCont();
  m_scintrawCont = new TBScintillatorRawCont();
  m_mwpcrawCont  = new TBMWPCRawCont();
  
  for(int i=0;i<4;i++) m_tblardigitcont[i] = new TBLArDigitContainer();
  for(int i=0;i<4;i++) m_tblarcalibdigitcont[i] = new TBLArCalibDigitContainer();
  
  std::string name1,name2;
  m_tdcrawCont = new TBTDCRawCont();
  TBTDCRaw* dummytdc = new TBTDCRaw("dummy",true,0,true);
  m_tdcrawCont->push_back(dummytdc);
  
  m_adcrawCont = new TBADCRawCont();
  TBADCRaw* dummyadc = new TBADCRaw("dummy",true,0);
  m_adcrawCont->push_back(dummyadc);


  // with this initialisation, first call of NextSubFrag will initialise correctly the index :
  m_subfrag_id=0;
  m_subfrag_size=2;    
  m_subfrag_firstdata=0;
  while(NextSubFrag()){
    MSG::dec(logstr) << MSG::DEBUG << "size   "<<m_subfrag_size<<endmsg;
    MSG::hex(logstr) << MSG::DEBUG << "ID "<< m_subfrag_id<<endmsg;    
    switch(m_subfrag_id){
    case 0x01: // HEADER_ID -----------------------------------------
      {
	logstr << MSG::DEBUG << "H6BuildObject : HEADER_ID "<< m_subfrag_size << endmsg;
	//m_trigpat = new TBTriggerPatternUnit();
	m_trigpat->setTriggerWord(m_rodBlock[m_subfrag_firstdata+3]);
	MSG::hex(logstr) << MSG::DEBUG << "Header   "<<m_rodBlock[m_subfrag_firstdata];
	MSG::hex(logstr) <<" "<<m_rodBlock[m_subfrag_firstdata+1];
	MSG::hex(logstr) <<" "<<m_rodBlock[m_subfrag_firstdata+2];
	MSG::hex(logstr) <<" "<<m_rodBlock[m_subfrag_firstdata+3]<<endmsg;
        // if(unrec_code == 30 ) gotobject=true;
	if(m_dump) MSG::dec(logstr) << MSG::INFO << " Ev. number(0x01)="<< m_rodBlock[m_subfrag_firstdata] <<endmsg;
	m_eventinfo = new TBEventInfo(m_ev_number,
				      m_rodBlock[m_subfrag_firstdata+2],
				      m_rodBlock[m_subfrag_firstdata+1],
				      m_run_num,
				      m_beam_moment,
				      m_beam_part,
				      m_cryoX,
				      m_cryoAngle,
				      m_tableY);
      }
      break;

    case 0x02: // MINI-ROD Data Fragment ... oh what fun ... ------------
      {
        logstr << MSG::DEBUG << " Found mini-ROD data in beam crate fragment, size " <<  m_subfrag_size << endmsg;

	//     bool calibdigit_requested = (unrec_code==20)||(unrec_code==21)||(unrec_code==22)||(unrec_code==23);
	// 
	short m_feedthrough[8] = {0,0,0,0,1,1,1,1};
	int m_sampleOrder[32] = {
	  0,1,2,3,4,5,6,7,8,9,10,
	  11,12,13,14,15,16,17,18,19,20,
	  21,22,23,24,25,26,27,28,29,30,
	  31};
	int m_sort[64] = { 55, 39, 23, 7, 119, 103, 87, 71,
			   54, 38, 22, 6, 118, 102, 86, 70,
			   53, 37, 21, 5, 117, 101, 85, 69,
			   52, 36, 20, 4, 116, 100, 84, 68,
			   51, 35, 19, 3, 115,  99, 83, 67,
			   50, 34, 18, 2, 114,  98, 82, 66,
			   49, 33, 17, 1, 113,  97, 81, 65,
			   48, 32, 16, 0, 112,  96, 80, 64
	} ;
	
	
	short m_slot[8]={5,7,9,11,3,4,5,6};
	
	const int NWREC = 8;
	int pos=m_subfrag_firstdata;
	for(unsigned int nfeb=0;nfeb<m_boards.size();nfeb++){ // FEB loop ----------------------------------
	  pos += NWREC*3;    // skip FEB header
	  if(m_dump) logstr << MSG::DEBUG << " Board "<< nfeb << endmsg;
	  for(int s=0;s<m_samples[0];s++){                   // sample loop ----------------------------------
	    pos +=NWREC;
	    int samp;
	    for(unsigned int g=0;g<m_gains.size();g++){     // gain loop ----------------------------------
	      int gainmode;

	      // According to 'FebAuto' keys,  sort sample (cf Petr G. doc)
	      if(m_febgain[nfeb]==0){ // Default behavior
		gainmode=m_gains[g];		
		if((gainmode==0)&&((m_sampleOrder[0]=m_firstsamples[0])!=0))
		  {
		    for(int j=0;j<m_firstsamples[0];j++) {m_sampleOrder[j+1]=j;}
		  }
	      }
	      else { // this FEB is AutoGain and first sample given in m_febgain
		gainmode=0;
		if((m_sampleOrder[0]=m_febgain[nfeb])!=0){
		  for(int j=0;j<m_febgain[nfeb];j++) {m_sampleOrder[j+1]=j;}
		}
	      }
	      samp=m_sampleOrder[s];

	      if(m_dump&&(gainmode==2)) logstr << MSG::DEBUG << " channels ";
	      for( int i= 0 ;i<64; i++){                   // channel loop ----------------------------------
		int chan= m_sort[i];
		
		unsigned short tmp1 = firstword(m_rodBlock[pos]);
		unsigned short gain1 = ((tmp1>>12)&3) - 1;
		m_arrayofsample[chan+8][gainmode][samp]=firstnbit(12,tmp1);

		unsigned short tmp2 = secondword(m_rodBlock[pos]);
		unsigned short gain2 = ((tmp2>>12)&3) - 1;
		m_arrayofsample[chan][gainmode][samp]=firstnbit(12,tmp2);

		m_arrayofgain[chan+8][gainmode] = getCaloGain(gain1);
		m_arrayofgain[chan][gainmode] = getCaloGain(gain2);


		// if(m_dump&(gainmode==2)) logstr << MSG::INFO << " " <<firstnbit(12,tmp1); 
		if(m_dump&&(gainmode==2)) logstr << MSG::DEBUG << " (," << chan << ","<< gainmode <<"," << samp <<","<< m_arrayofsample[chan+8][gainmode][samp]<<")";
		pos++;
	      } // end channel loop

	      if(m_dump&&(gainmode==2)) logstr << MSG::DEBUG <<endmsg;
	    }  // end gain loop
	  }// end sample loop
	  pos+= NWREC*2;
	  logstr << MSG::DEBUG << " Creating LArDigit for board  "<< nfeb << ".  m_febgain=" << m_febgain[nfeb] <<endmsg;
	  // create and store LArDigit.
	  for(unsigned int g=0;g<m_gains.size();g++){ // loop on reqested gains
	    int gainmode;
	    if(m_febgain[nfeb]==0){ // Default behavior
	      gainmode= m_gains[g];
	    } else { // this FEB is AutoGain and first sample given in m_febgain
	      gainmode=0;
	    }

	    logstr << MSG::DEBUG << "Gain mode="<< gainmode <<endmsg; 
	    for(int i=0;i<128;i++) { // loop on channels
         
              std::vector<short> samplevec(m_arrayofsample[i][gainmode]);
              samplevec.resize(m_samples[0]);
	      CaloGain::CaloGain  gain= m_arrayofgain[i][gainmode];
              HWIdentifier hwid=m_onlineHelper->channel_Id(1,1,m_feedthrough[m_boards[nfeb]-1],m_slot[m_boards[nfeb]-1],i);
	      LArDigit * lardig= new LArDigit(hwid,gain,samplevec);
	      m_tblardigitcont[gainmode]->push_back(lardig);
	      if(m_isCalib) {
                bool isPulsed=false;
                const std::vector<HWIdentifier>& calibChannelIDs=m_larCablingSvc->calibSlotLine(hwid);
                if (calibChannelIDs.size() != 0) {
                  // Now figure out if any calibration line connected to this channel is pulsed.
                  // I'm going to cheat and use the fact that we only pulsed one board at a time
                  // and only wrote the corresponding digits (EMEC/HEC/FCAL)
                  std::vector<HWIdentifier>::const_iterator csl_it=calibChannelIDs.begin();
                  std::vector<HWIdentifier>::const_iterator csl_it_e=calibChannelIDs.end();
                  for(;csl_it!=csl_it_e;csl_it++) {
                    HWIdentifier calChan = *csl_it;
                    int chan = m_onlineHelper->channel(calChan);
                    int bit  = chan%8;
                    int byte = chan/8;
                    //  if( 1<<bit & m_calib_pattern[byte] ) {
                    if( 128>>bit & m_calib_pattern[byte] ) {
                      isPulsed = true;
                    }
                  }
                }
                LArCalibDigit * larcalibdig= new LArCalibDigit(hwid,gain,samplevec,m_calib_dac,m_calib_delay,isPulsed);
                m_tblarcalibdigitcont[gainmode]->push_back(larcalibdig);
              }
	    }
	    if(m_febgain[nfeb]!=0){ // this FEB is AutoGain ignore every gain in FebGain
	      break;
	    }
	  }
	}// FEB loop
	
      }
      break;
      
    case 0x03: // TIME_ID -----------------------------------------
      { 
	logstr << MSG::DEBUG << "H6BuildObject : TIME_ID "<< m_subfrag_size << endmsg;
	name1="word1frag0x03chan";
	name2="word2frag0x03chan";

        
        if(m_subfrag_size!=5) {
          logstr<<MSG::ERROR<< "Beam counter subfrag (0x03) has not expected size" <<endmsg;
          sc=StatusCode::FAILURE;
          break;
        }
	int pos=m_subfrag_firstdata;
	int tmp1,tmp2;
        
	tmp1 = firstword(m_rodBlock[pos]);
	tmp2 = secondword(m_rodBlock[pos]);
	
	if(m_dump) logstr << MSG::INFO << " sub frag 0x03 : word1="<<tmp1<<"    word2="<< tmp2 <<endmsg;

	TBTDCRaw * clocktdc = new TBTDCRaw(name1+'0',false,tmp1,false);	
	TBTDCRaw * clock_deltdc = new TBTDCRaw(name2+'0',false,tmp2,false);	
	m_tdcrawCont->push_back(clocktdc); 
	m_tdcrawCont->push_back(clock_deltdc); 
	
	tmp1 = firstword(m_rodBlock[pos+1]);
	tmp2 = secondword(m_rodBlock[pos+1]);

	if(m_dump) logstr << MSG::INFO << " sub frag 0x03 : word1="<<tmp1<<"    word2="<< tmp2 <<endmsg;

	TBTDCRaw * scale1 = new TBTDCRaw(name1+'1',false,tmp1,false);	
	TBTDCRaw * scale1bis = new TBTDCRaw(name2+'1',false,tmp2,false);	
	m_tdcrawCont->push_back(scale1); 
	m_tdcrawCont->push_back(scale1bis); 

	tmp1 = firstword(m_rodBlock[pos+2]);
	tmp2 = secondword(m_rodBlock[pos+2]);

	if(m_dump) logstr << MSG::INFO << " sub frag 0x03 : word1="<<tmp1<<"    word2="<< tmp2 <<endmsg;

	TBTDCRaw * scale2 = new TBTDCRaw(name1+'2',false,tmp1,false);	
	TBTDCRaw * scale2bis = new TBTDCRaw(name2+'2',false,tmp2,false);	
	m_tdcrawCont->push_back(scale2); 
	m_tdcrawCont->push_back(scale2bis); 
	
      }
      break;
    case 0x04: // TAIL_ID -----------------------------------------
      {
	name1="word1frag0x06chan";
	name2="word2frag0x06chan";
	std::string tcname="tc";
	std::vector<TBScintillatorRaw *> theScints;
	if(m_subfrag_size!=26) {
          logstr<<MSG::ERROR<< "Beam counter subfrag (0x04) has not expected size" <<endmsg;
          sc=StatusCode::FAILURE;
          break;
        }
        logstr << MSG::DEBUG << "H6BuildObject : building tailcatcher " << endmsg;
	for(unsigned int i=0;i<24;i++){
	  int tmp1,tmp2;
	  int pos=m_subfrag_firstdata+i;
	  unsigned short word1=firstword(m_rodBlock[pos]);
	  unsigned short word2=secondword(m_rodBlock[pos]);
	  
	  tmp1 = firstnbit(10,word1);
	  tmp2 = firstnbit(10,word2);

	  os.str("");
	  os << name1 << i;
	  TBADCRaw*  adc1 = new TBADCRaw(os.str(),false,abs(tmp1));
	  os.str("");
	  os << name2 << i;
	  TBADCRaw*  adc2 = new TBADCRaw(os.str(),false,abs(tmp2));
	  if(testbit(10,word1)) adc1->setOverflow(); // if bit10=0 -> Overflow  
	  if(testbit(10,word2)) adc2->setOverflow(); // if bit10=0 -> Overflow  
	  m_adcrawCont->push_back(adc1);
	  m_adcrawCont->push_back(adc2);
	  os.str("");
	  os << tcname << 2*i;
	  TBScintillatorRaw* s1 = new TBScintillatorRaw(os.str(),m_tdcrawCont,dummytdc,m_adcrawCont,adc1);
	  os.str("");
	  os << tcname << 2*i+1;
	  TBScintillatorRaw* s2 = new TBScintillatorRaw(os.str(),m_tdcrawCont,dummytdc,m_adcrawCont,adc2);
	  theScints.push_back(s1);
	  theScints.push_back(s2);
	}
        
	m_tailcatchraw = new TBTailCatcherRaw("TailCatherRaw",false,theScints);
	//if(unrec_code == 4 ) gotobject=true;
      }      
      break;

    case 0x05: // BPC_ID -----------------------------------------
      {      
	name1="word1frag0x05chan";
	name2="word2frag0x05chan";
	//m_bpcrawCont = new TBBPCRawCont();
	int bpcnum = (m_subfrag_size - 2) / 3; // cause we store 6 tdc/adc in 16bits word
	logstr << MSG::DEBUG << "H6BuildObject : number of BPCRaw "<< bpcnum << endmsg;
	// get bpc raw information for each chamber:
	for(int i=0;i<bpcnum;i++){
          std::string bpcname = "BPC";
	  os.str("");
	  os << bpcname << i;
	  bpcname= os.str();
	  std::list<const TBTDCRaw*> theTDCs;
	  std::list<const TBADCRaw*> theADCs;
	  int pos=m_subfrag_firstdata;

	  unsigned int tmp1,tmp2;
	  tmp1=firstword(m_rodBlock[pos+i*3]); 
	  tmp2=secondword(m_rodBlock[pos+i*3]);
	  os.str("");
	  os << name1 << (i*3);
	  TBADCRaw*  tbadcH = new TBADCRaw(os.str(),(tmp1>1023),tmp1);
	  os.str("");
	  os << name2 << (i*3);
	  TBADCRaw*  tbadcV = new TBADCRaw(os.str(),(tmp1>1023),tmp2);
	  m_adcrawCont->push_back(tbadcH);
	  m_adcrawCont->push_back(tbadcV);

	  // first word is right :
	  tmp1=firstword(m_rodBlock[pos+i*3+1]); 
	  tmp2=secondword(m_rodBlock[pos+i*3+1]);
	  os.str("");
	  os << name2 << (i*3+1);
	  TBTDCRaw * tbtdcL = new TBTDCRaw(os.str(),(tmp2>2047),tmp2,0);
	  os.str("");
	  os << name1 << (i*3+1);
	  TBTDCRaw * tbtdcR = new TBTDCRaw(os.str(),(tmp1>2047),tmp1,0);
	  // no under threshold ??
	  m_tdcrawCont->push_back(tbtdcR); 
	  m_tdcrawCont->push_back(tbtdcL); 
	  MSG::hex(logstr) << MSG::DEBUG << bpcname<< "  "<< tmp1;
	  MSG::hex(logstr)                      << "  "<< tmp2<<"  ";

	  tmp1=firstword(m_rodBlock[pos+i*3+2]); 
	  tmp2=secondword(m_rodBlock[pos+i*3+2]);
	  os.str("");
	  os << name1 << (i*3+2);
	  TBTDCRaw * tbtdcU = new TBTDCRaw(os.str(),(tmp1>2047),tmp1,0);
	  os.str("");
	  os << name2 << (i*3+2);
	  TBTDCRaw * tbtdcD = new TBTDCRaw(os.str(),(tmp2>2047),tmp2,0);
	  // no under threshold ??
	  m_tdcrawCont->push_back(tbtdcU);
	  m_tdcrawCont->push_back(tbtdcD);
	  MSG::hex(logstr) << MSG::DEBUG << bpcname<< "  "<< tmp1;
	  MSG::hex(logstr)                      << "  "<< tmp2<<endmsg;
	  

	  theTDCs.push_back(tbtdcL);
	  theTDCs.push_back(tbtdcR);  // Fill the tdc/adc list
	  theTDCs.push_back(tbtdcU);  // for this BPC
	  theTDCs.push_back(tbtdcD);  // !! THE ORDER MATTERS !!
	  theADCs.push_back(tbadcH);
	  theADCs.push_back(tbadcV);
	  TBBPCRaw *bpcraw = new TBBPCRaw(bpcname,m_tdcrawCont,theTDCs,m_adcrawCont,theADCs);
	  m_bpcrawCont->push_back(bpcraw);
	}
        //if( unrec_code == 5 ) gotobject=true;

      }
      break;

    case 0x06: // SCINTILLATOR_ID -----------------------------------------
      {
      name1="word1frag0x06chan";
      name2="word2frag0x06chan";
      if(m_subfrag_size!=11) {
        logstr<<MSG::ERROR<< "Beam counter subfrag (0x06) has not expected size" <<endmsg;
        sc=StatusCode::FAILURE;
        break;
      }

      logstr << MSG::DEBUG << " Building TBScintillatorRawCont with key " << m_keys[6] << endmsg;
      
      TBScintillatorRaw * S1 = new TBScintillatorRaw("S1");
      TBScintillatorRaw * S2 = new TBScintillatorRaw("S2");
      TBScintillatorRaw * S3 = new TBScintillatorRaw("S3");
      TBScintillatorRaw * B = new TBScintillatorRaw("B");
      TBScintillatorRaw * Veto = new TBScintillatorRaw("Veto");
      TBScintillatorRaw * Halo = new TBScintillatorRaw("Halo");
      TBScintillatorRaw * muon[8];

      logstr << MSG::DEBUG << " building muon counters:";
      for(int i=0;i<8;i++) {
	  std::string scintname="muon";
	  os.str("");
	  os << scintname << (i+1);
	  scintname=os.str();
          logstr << MSG::DEBUG << " " << scintname;
	  muon[i] =  new TBScintillatorRaw(scintname);
      }
      logstr << endmsg;
      
      int tmp1,tmp2;
      int pos=m_subfrag_firstdata;
      unsigned short word1=firstword(m_rodBlock[pos]);
      unsigned short word2=secondword(m_rodBlock[pos]);

      tmp1 = firstnbit(10,word1);
      tmp2 = firstnbit(10,word2);

      TBADCRaw*  Badc = new TBADCRaw(name1+'0',false,abs(tmp1));
      TBADCRaw*  Haloadc = new TBADCRaw(name2+'0',false,abs(tmp2));
      if(testbit(10,word1)) Badc->setOverflow(); // if bit10=0 -> Overflow  
      if(testbit(10,word2)) Haloadc->setOverflow(); // if bit10=0 -> Overflow  
      m_adcrawCont->push_back(Badc);
      m_adcrawCont->push_back(Haloadc);

      tmp1 = firstnbit(10,firstword(m_rodBlock[pos+1]));
      tmp2 = firstnbit(10,secondword(m_rodBlock[pos+1]));
      TBADCRaw*  S3adc = new TBADCRaw(name1+'1',false,abs(tmp1));
      TBADCRaw*  S2adc = new TBADCRaw(name2+'1',false,abs(tmp2));
      if(testbit(10,word1)) S3adc->setOverflow(); // if bit10=0 -> Overflow  
      if(testbit(10,word2)) S2adc->setOverflow(); // if bit10=0 -> Overflow  
      m_adcrawCont->push_back(S3adc);
      m_adcrawCont->push_back(S2adc);

      tmp1 = firstnbit(10,firstword(m_rodBlock[pos+2]));
      TBADCRaw*  S1adc = new TBADCRaw(name1+'2',false,abs(tmp1));
      if(testbit(10,word1)) S1adc->setOverflow(); // if bit10=0 -> Overflow  
      m_adcrawCont->push_back(S1adc);

      for(int i=0;i<4;i++) {
	tmp1 = firstnbit(10,secondword(m_rodBlock[pos+2+i]));
	tmp2 = firstnbit(10,firstword(m_rodBlock[pos+3+i]));
	os.str("");
	os << name1 << (0+2+i);
	TBADCRaw*  adc1 = new TBADCRaw(os.str(),false,abs(tmp1));
	os.str("");
	os << name2 << (0+3+i);
	TBADCRaw*  adc2 = new TBADCRaw(os.str(),false,abs(tmp2));
	//	muon[i]
	//	muon[i+1]->setSignal(abs(tmp2));
	if(testbit(10,word1)) adc1->setOverflow(); // if bit10=0 -> Overflow  
	if(testbit(10,word2)) adc2->setOverflow(); // if bit10=0 -> Overflow  
	m_adcrawCont->push_back(adc1);
	m_adcrawCont->push_back(adc2);
	muon[2*i]->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,adc1);
	muon[2*i+1]->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,adc2);
      }


      tmp1 = firstnbit(11,secondword(m_rodBlock[pos+6]));
      TBTDCRaw*  S2tdc = new TBTDCRaw(name2+'6',false,abs(tmp1),0);
      m_tdcrawCont->push_back(S2tdc);

      // Overflows ?? underthreshold ??
      tmp1 = firstnbit(11,firstword(m_rodBlock[pos+7]));
      tmp2 = firstnbit(11,secondword(m_rodBlock[pos+7]));
      TBTDCRaw*  S3tdc = new TBTDCRaw(name1+'7',false,abs(tmp1),0);
      TBTDCRaw*  Btdc = new TBTDCRaw(name2+'7',false,abs(tmp2),0);
      m_tdcrawCont->push_back(S3tdc);
      m_tdcrawCont->push_back(Btdc);

      tmp1 = firstnbit(11,firstword(m_rodBlock[pos+8]));
      tmp2 = firstnbit(11,secondword(m_rodBlock[pos+8]));
      TBTDCRaw*  Halotdc = new TBTDCRaw(name1+'8',false,abs(tmp1),0);
      TBTDCRaw*  VetoORtdc = new TBTDCRaw(name2+'8',false,abs(tmp2),0);
      m_tdcrawCont->push_back(Halotdc);
      m_tdcrawCont->push_back(VetoORtdc);

      
      // Fill container
      S1->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,S1adc);
      S2->setSignals(m_tdcrawCont,S2tdc,m_adcrawCont,S2adc);


      //       logstr << MSG::INFO << "S1 adc ovf = "<< S1->isADCOverflow() << " // S1adc = "<< S1adc->isOverflow() <<endmsg;
      //       logstr << MSG::INFO << "S2 adc ovf = "<< S2->isADCOverflow() << " // S2adc = "<< S2adc->isOverflow() <<endmsg;

      S3->setSignals(m_tdcrawCont,S3tdc,m_adcrawCont,S3adc);
      Veto->setSignals(m_tdcrawCont,VetoORtdc,m_adcrawCont,dummyadc);
      Halo->setSignals(m_tdcrawCont,Halotdc,m_adcrawCont,Haloadc);
      B->setSignals(m_tdcrawCont,Btdc,m_adcrawCont,Badc);

      m_scintrawCont->push_back(S1);
      m_scintrawCont->push_back(S2);
      m_scintrawCont->push_back(S3);
      m_scintrawCont->push_back(Veto);
      m_scintrawCont->push_back(Halo);
      m_scintrawCont->push_back(B);
      for(int i=0;i<8;i++) {
	  m_scintrawCont->push_back(muon[i]);
      }

      logstr << MSG::DEBUG << " Should I record TBScintillatorRawCont with key " << m_keys[6] << " ? " << endmsg;
      //if(unrec_code == 6 ) gotobject=true;
      }
      break;

    case 0x07: // MWPC_ID -----------------------------------------
      {
	int pos=m_subfrag_firstdata;
	int nword = m_subfrag_size - 2;
	logstr << MSG::DEBUG << " Found MWPC subfragment. Nword= "<<nword << endmsg;

	std::string mwpcnames[8]= {"X2","Y2","X3","Y3","X4","Y4","X5","Y5"};
	bool isX[9]={true,false,true,false,true,false,true,false};
	TBMWPCRaw * mwpcraw[8];
	for( int i=0;i<8;i++){
	  mwpcraw[i] = new TBMWPCRaw(mwpcnames[i]);
	}

	unsigned short status= secondword(m_rodBlock[pos+nword-1]);
	//bool error = false ;
	if(status==0)  status= firstword(m_rodBlock[pos+nword-1]);
	else { // first 16 bit word is data
	  unsigned short word1=firstword(m_rodBlock[pos+nword-1]);
	  int w; // cluster width 
	  int c; // the starting channel of a cluster (0<start<4096) 
	  int chamber; // 0=X2, 1=Y2, 2=X3, 3=Y3, 4=X4, 5=Y4, 6=X5, 7=Y5 
	  int wire; // wire number 

	  w = (int) (word1>>12); c = (int) ((0xfff & word1) - w/2 + !(w&1)); 
	  if( c < 768 ) {chamber = c/128; wire=c-chamber*128;} 
	  else {chamber=6+(c-768)/64; wire=c-768-(chamber-6)*64;}

	  if(chamber>7){logstr << MSG::DEBUG << "Wrong MWPC chamber number : "<< chamber   <<endmsg;}
	  else{
	    mwpcraw[chamber]->addCwireno(wire);
	    mwpcraw[chamber]->addNwires(w);
	    mwpcraw[chamber]->setXchambers(isX[chamber]);
	    logstr << MSG::DEBUG << " MWPC chamber="<< chamber<< "  wire=" << wire << " (w="<<w<<")" << "  c="<<c<< endmsg;
	  }
	}
	
	if(status != 0x1000) {MSG::hex(logstr)<< MSG::DEBUG << "MWPC status word error =" << status <<endmsg; /*error=true;*/}

	for( int i=0;i<m_subfrag_size-2;i++){ // "-2" because last full word contains status word (see above)
	  unsigned short word1=firstword(m_rodBlock[pos+i]);
	  unsigned short word2=secondword(m_rodBlock[pos+i]);
	  int w; // cluster width 
	  int c; // the starting channel of a cluster (0<start<4096) 
	  int chamber; // 0=X2, 1=Y2, 2=X3, 3=Y3, 4=X4, 5=Y4, 6=X5, 7=Y5 
	  int wire; // wire number 

	  w = (int) (word1>>12); c = (int) ((0xfff & word1) - w/2 + !(w&1)); 
	  if( c < 768 ) {chamber = c/128; wire=c-chamber*128;} 
	  else {chamber=6+(c-768)/64; wire=c-768-(chamber-6)*64;}

	  if(chamber>7){logstr << MSG::DEBUG << "Wrong MWPC chamber number : "<< chamber   <<endmsg;}
	  else{
	    mwpcraw[chamber]->addCwireno(wire);
	    mwpcraw[chamber]->addNwires(w);
	    mwpcraw[chamber]->setXchambers(isX[chamber]);
	    logstr << MSG::DEBUG << " MWPC chamber="<< chamber<< "  wire=" << wire << " (w="<<w<<")" << "  c="<<c<< endmsg;
	  }

	  w = (int) (word2>>12); c = (int) ((0xfff & word2) - w/2 + !(w&1)); 
	  if( c < 768 ) {chamber = c/128; wire=c-chamber*128;} 
	  else {chamber=6+(c-768)/64; wire=c-768-(chamber-6)*64;}


	  if(chamber>7){logstr << MSG::INFO << "Wrong MWPC chamber number : "<< chamber  <<endmsg;}
	  else{
	    mwpcraw[chamber]->addCwireno(wire);
	    mwpcraw[chamber]->addNwires(w);
	    mwpcraw[chamber]->setXchambers(isX[chamber]);
	    logstr << MSG::DEBUG << " MWPC chamber="<< chamber<< "  wire=" << wire << " (w="<<w<<")" << "  c="<<c<< endmsg;
	  }

	}

	for( int i=0;i<8;i++){
	  m_mwpcrawCont->push_back(mwpcraw[i]);
	}
	
	logstr << MSG::DEBUG << " End of MWPC subfragment " << endmsg;	
        //if(unrec_code == 7 ) gotobject=true;

      }
      break;
    case 0xf1: // Run header -----------------------------------------
      {
	int pos=m_subfrag_firstdata;
	char *strw;
	int nword = m_subfrag_size - 2;
	if(nword%16){
	  logstr << MSG::ERROR << "bad number of lines. nwords=" << nword <<endmsg;
	  break;
	}
	int nline= nword / 16;
	strw = (char*)&m_rodBlock[pos];
	for(int i=0;i<nline;i++){
	  //       strw = (char*) &m_rodBlock[pos+i*64];
	  std::string sline="";
	  std::string skey="";
	  for(int j=0;j<63;j++) {
	    if(strw[i*64+j]==0) break;
	    sline+=strw[i*64+j];
	  }
	  //logstr << MSG::INFO  << sline << endmsg;
	  std::istringstream iss (sline);
	  iss >> skey;	  
	  if(skey=="RunNumber") {
	    MSG::dec(logstr) << MSG::DEBUG  << sline << endmsg;
	    iss >> m_run_num;
	  }
	  if(skey=="BeamMomentum") {
	    MSG::dec(logstr) << MSG::DEBUG  << sline << endmsg;
	    iss >> m_beam_moment;
	  }
	  if(skey=="BeamParticle") {
	    MSG::dec(logstr) << MSG::DEBUG  << sline << endmsg;
	    iss >> m_beam_part;
	  }
	  if(skey=="CryoX") {
	    MSG::dec(logstr) << MSG::DEBUG  << sline << endmsg;
	    iss >> m_cryoX;
	  }
	  if(skey=="CryoAngle") {
	    MSG::dec(logstr) << MSG::DEBUG  << sline << endmsg;
	    iss >> m_cryoAngle;
	  }
	  if(skey=="TableY") {
	    MSG::dec(logstr) << MSG::DEBUG  << sline << endmsg;
	    iss >> m_tableY;
	  }
	  if(skey=="miniROD")  {
	    logstr << MSG::DEBUG  << sline << endmsg;
	    m_boards.clear();
	    int board;
	    //	    iss>>board; 
	    while(iss.good()) {iss>>board;m_boards.push_back(board);}
	    logstr << MSG::DEBUG  << "Found nboards="<< m_boards.size() << endmsg;
	    for(unsigned i=0;i<m_boards.size();i++)logstr << MSG::DEBUG << m_boards[i]<<" ";
            logstr << MSG::DEBUG<<endmsg;
	  }
	  if(skey=="FebSamples") {
	    MSG::dec(logstr) << MSG::DEBUG  << sline << endmsg;
	    m_samples.clear();
	    int sample;
	    // iss>>sample;
	    while(iss.good()) {iss>>sample;m_samples.push_back(sample);}
	    MSG::dec(logstr) << MSG::DEBUG  << "nsample="<< m_samples.size() << endmsg;
	    for(unsigned i=0;i<m_samples.size();i++)logstr << MSG::DEBUG << m_samples[i]<<" ";
            logstr << MSG::DEBUG<<endmsg;
	  }
	  if(skey=="FebGains")  {
	    MSG::dec(logstr) << MSG::DEBUG  << sline << endmsg;
	    m_gains.clear();
	    int gain;
	    //	    iss>>gain;
	    while(iss.good()) {iss>>gain;m_gains.push_back(gain);}
	    int gs=m_gains.size() ;
	    if(gs>1) if(m_gains[gs-1]==m_gains[gs-2])m_gains.resize(gs-1);
	    MSG::dec(logstr) << MSG::DEBUG  << "numb of gains="<< m_gains.size()  <<  endmsg;
	    for(unsigned i=0;i<m_gains.size();i++)logstr << MSG::DEBUG << m_gains[i]<<" ";
            logstr << MSG::DEBUG<<endmsg;
	  }
	  if(skey=="FebAuto")  {
	    MSG::dec(logstr) << MSG::DEBUG  << sline << endmsg;
	    m_febgain.clear();
	    int gain;
	    //	    iss>>gain;
	    while(iss.good()) {iss>>gain;m_febgain.push_back(gain);}
	    MSG::dec(logstr) << MSG::DEBUG  << "n febauto="<< m_febgain.size()  << " " <<  endmsg;
	    for(unsigned i=0;i<m_febgain.size();i++)logstr << MSG::DEBUG << m_febgain[i]<<" ";
            logstr << MSG::DEBUG<<endmsg;
	  }
	  if(skey=="FebFirstSample")  {
	    MSG::dec(logstr) << MSG::DEBUG  << sline << endmsg;
	    m_firstsamples.clear();
	    int firstsample;
	    //	    iss>>firstsample;
	    while(iss.good()) {iss>>firstsample;m_firstsamples.push_back(firstsample);}
	    MSG::dec(logstr) << MSG::DEBUG  << "firstsample="<< m_firstsamples.size()   << endmsg;
	  }
	}

	// If there was no febgain initialise to 0 :
	if(m_febgain.size()==0) m_febgain.resize(8,0);
	// now initialize every vector : 
	for(int i=0;i<128;i++) {for(int j=0;j<4;j++) {m_arrayofsample[i][j].clear();m_arrayofsample[i][j].resize(32,0);}}

	//     m_summary_path = m_summary_path+"/run";
//     m_summary_path = m_summary_path+m_run_num;
//     m_summary_path = m_summary_path+".summary";
	std::ofstream outfile(m_summary_path.c_str());
	
	outfile << "Run number " << m_run_num << std::endl;
	outfile << std::endl;
	outfile << "Beam Type \t " << m_beam_part << std::endl;
	outfile << "Beam Momentum \t " << m_beam_moment << std::endl;
	outfile << std::endl;
	outfile << "Cryostat X pos \t " << m_cryoX << std::endl;
	outfile << "Cryostat Angle \t " << m_cryoAngle << std::endl;
	outfile << "Table Y pos \t " << m_tableY << std::endl;



      }
      break;
    case 0xff : // Calib_ID ------------------------------------
      {
	int pos=m_subfrag_firstdata;
	int nword = m_subfrag_size - 2;
	logstr << MSG::DEBUG << " Found Calib_ID subfragment. Nword= "<<nword << endmsg;
	if(nword!=6) { // expect 6 words (to hold 22 bytes)
	  logstr<<MSG::ERROR<< "Calibration subfrag (0xff) has not expected size" <<endmsg;
	  sc=StatusCode::FAILURE;
	  break;
	}
	
	m_isCalib = true;

	for(int i=0;i<4;i++){ // get channel pattern
	  int first16 = firstword(m_rodBlock[pos+i]);
	  int second16 = secondword(m_rodBlock[pos+i]);

	  m_calib_pattern[4*i] = 255 & first16;
	  m_calib_pattern[4*i+1] = first16 >> 8;
	  m_calib_pattern[4*i+2] = 255 & second16;
	  m_calib_pattern[4*i+3] = second16 >> 8;
	}

        logstr << MSG::DEBUG << "Found calibration header information " << endmsg;
        logstr << MSG::DEBUG << " Calib pattern: " << endmsg;
        for(int byte=0; byte<16; byte++) {
          logstr << MSG::DEBUG << " byte " << byte << "  : " ;
          for(int bit=0; bit<8; bit++) {
            // bool onoff = 1<<bit & m_calib_pattern[byte];
            bool onoff = 128>>bit & m_calib_pattern[byte];
            logstr << MSG::DEBUG << onoff << "/" ;
          }
          logstr << MSG::DEBUG << endmsg;
        }
        
	char dac[4];
	unsigned long* tmp;
	
	int first16 = firstword(m_rodBlock[pos+4]);
	int second16 = secondword(m_rodBlock[pos+4]);
	
	dac[0] = 255 & first16;
	dac[1] = first16 >> 8;
	dac[2] = 255 & second16;
	dac[3] = second16 >> 8;

	// this code is copied from Petr Gorbunov H6 doc.
	tmp = (unsigned long*) dac;
	m_calib_dac = (uint16_t) *tmp;

	first16 = firstword(m_rodBlock[pos+5]);
	m_calib_delay = 255 & first16;
	m_calib_error = ((first16 >> 8) != 0);

        // logstr << MSG::DEBUG << "       Pattern 0/1/2/3 = "  << m_calib_pattern[0]
        //                                                       << "/" <<  m_calib_pattern[1]
        //                                                       << "/" <<  m_calib_pattern[2]
        //                                                       << "/" <<  m_calib_pattern[3] << endmsg;
        logstr << MSG::DEBUG << "                   DAC = "  <<  m_calib_dac        << endmsg;
        logstr << MSG::DEBUG << "                 DELAY = "  <<  m_calib_delay      << endmsg;
      }
      break;
    default : // -----------------------------------------
      logstr << MSG::DEBUG << "Found undefined subfragment id= "<< m_subfrag_id << endmsg;
      break;
    }
  }

  sc = H6RecordObjects(unrec_code);
  return sc;
 

}

/*---------------------------------------------------------------------------*/

StatusCode TBByteStreamCnvTool::H8BuildObjects(int unrec_code)

/*---------------------------------------------------------------------------*/
{
  /** Build all beam detectors raw objects from m_rodBlock using H8 convention.
      Objects are recorded in SG if they do not correspond to unrec_code
      Will return faillure if : -one of the object could not be recorded in SG
      -the requested object (unrec_code) was not found

  **/


  MsgStream logstr(msgSvc(), name());
  logstr << MSG::DEBUG << "H8BuildObject invoked for code " << unrec_code<< endmsg;
  
  StatusCode sc=StatusCode::FAILURE;
  bool gotobject=false;
  bool recordfailure=false;  

  bool goodRodBlock=true;  // This is a hack to avoid runtime exceptions
  
  std::string name;


  // Get ROD block.
  sc=GetRODBlock(m_subdet_id,eformat::TDAQ_BEAM_CRATE); //Possible improvement: Check if vector is already valid. 
  //sc=GetRODBlock(EventFormat::TDAQ_BEAM_CRATE);
  
  if (sc!=StatusCode::SUCCESS) {
    // return sc;          
    logstr << MSG::ERROR << "ReadRodBlock: RodBlock error!  Will try to fill SG with empty objects." << endmsg;
    goodRodBlock=false;
  }
  if (m_rodBlock.size()<2)  {
    logstr << MSG::ERROR << "ReadRodBlock: RodBlock too small!  Will try to fill SG with empty objects." << endmsg;
    // return StatusCode::FAILURE;
    goodRodBlock=false;
  }
  
  m_tdcrawCont = new TBTDCRawCont();
  m_adcrawCont = new TBADCRawCont();

  // Define some dummy ADC & TDC :
  TBADCRaw* dummyadc = new TBADCRaw("dummy",true,0);
  TBTDCRaw* dummytdc = new TBTDCRaw("dummy",true,0,true);
  m_tdcrawCont->push_back(dummytdc);
  m_adcrawCont->push_back(dummyadc);


  // Scintillator init :
  TBScintillatorRaw * S0 = new TBScintillatorRaw("S0");
  TBScintillatorRaw * S1 = new TBScintillatorRaw("S1");
  TBScintillatorRaw * S2up = new TBScintillatorRaw("S2-Up");
  TBScintillatorRaw * S2down = new TBScintillatorRaw("S2-Down");
  TBScintillatorRaw * S3left = new TBScintillatorRaw("S3-Left");
  TBScintillatorRaw * S3right = new TBScintillatorRaw("S3-Right");
  TBScintillatorRaw * C1 = new TBScintillatorRaw("C1");
  TBScintillatorRaw * C2 = new TBScintillatorRaw("C2");
  TBScintillatorRaw * muTag = new TBScintillatorRaw("muTag");
  TBScintillatorRaw * muHalo = new TBScintillatorRaw("muHalo");
  TBScintillatorRaw * muVeto = new TBScintillatorRaw("muVeto");
  TBScintillatorRaw * sTRT = new TBScintillatorRaw("TRTSci");

  // S0 has only TDC
  TBTDCRaw* S0tdc =dummytdc;
  // S1,S2,S3  have 1 TDC +1 ADC
  TBADCRaw * S1adc=dummyadc; 
  TBTDCRaw * S1tdc=dummytdc;
  
  TBADCRaw * S2upadc=dummyadc;
  TBTDCRaw * S2uptdc=dummytdc;
  TBADCRaw * S2downadc=dummyadc;
  TBTDCRaw * S2downtdc=dummytdc;

  TBADCRaw * S3leftadc=dummyadc;
  TBTDCRaw * S3lefttdc=dummytdc;
  TBADCRaw * S3rightadc=dummyadc;
  TBTDCRaw * S3righttdc=dummytdc;
  //Clock phase
  TBTDCRaw * ClockPhasetdc=dummytdc;

  //The following Scintillators have only ADC
  TBADCRaw * muTagadc=dummyadc;
  TBADCRaw * C1adc=dummyadc;
  TBADCRaw * C2adc=dummyadc;
  TBADCRaw * muHaloadc=dummyadc;
  TBADCRaw * muVetoadc=dummyadc;
  TBADCRaw * sTRTadc=dummyadc;
  
  // Define BPCs 
  // Some data words for tdc channels may be absent and if not they arrive in a 
  // different order than the one we need to store in TBBPCRaw. So we need all this stuff to get
  // everything done :
  int BPCNum=5;
  TBBPCRaw* bpcraw[5];
  TBTDCRaw * BPCtdc[5][4];
  short tdc_order[4] = {2,3,0,1}; // bytestream : udlr - TBBPCRaw lrud
  for(int i=0;i<BPCNum;i++){
    bpcraw[i] = new TBBPCRaw("BPC");
    for(int j=0;j<4;j++)  BPCtdc[i][j]=0;
  }
  bpcraw[0]->setDetectorName("BPC-2");
  bpcraw[1]->setDetectorName("BPC-1");
  bpcraw[2]->setDetectorName("BPC0");
  bpcraw[3]->setDetectorName("BPC1");
  bpcraw[4]->setDetectorName("BPC2");

  // for string conversion
  std::ostringstream os;

  // Now loop through fragment
  
  // with this initialisation, first call of NextSubFrag will initialise correctly the index :
  m_subfrag_id=0;
  m_subfrag_size=2;   
  m_subfrag_firstdata=0;
  // while(NextSubFrag()){
  while(NextSubFrag() and goodRodBlock){  // Hack for bad ROD block
    switch(m_subfrag_id){
    case 0x10: // ADC1 block --------------------------------------
      {
	name="frag0x10chan";
	logstr << MSG::DEBUG << "Got SubFragment " << MSG::hex << m_subfrag_id << " name=" << name   
	       << " Data words = " << MSG::dec << m_subfrag_size-2<<endmsg;
	
	int pos=m_subfrag_firstdata;
	
	// Overflows ??

	S1adc = new TBADCRaw(name+"0",false,m_rodBlock[pos] & 0xfff);
	// do not set signal here cause we also need TDC (in 0x11)
	logstr << MSG::DEBUG << "S1adc: 0x" << MSG::hex << (int)(m_rodBlock[pos] & 0xfff) << " [0x" << m_rodBlock[pos] << "]" 
	       << MSG::dec << endmsg;
	m_adcrawCont->push_back(S1adc);

	pos++; // 2
	S2upadc = new TBADCRaw(name+"1",false,m_rodBlock[pos] & 0xfff);
	logstr << MSG::DEBUG << "S2 :" << MSG::hex << (m_rodBlock[pos] & 0xfff) << " [0x" << m_rodBlock[pos] << "]" 
	       << MSG::dec << endmsg;
	m_adcrawCont->push_back(S2upadc);


	pos++; // 2
	S3rightadc = new TBADCRaw(name+"2",false,m_rodBlock[pos] & 0xfff);
	logstr << MSG::DEBUG << "S3 :" << MSG::hex << (m_rodBlock[pos] & 0xfff) << " [0x" << m_rodBlock[pos] << "]"
	       << MSG::dec << endmsg;
	m_adcrawCont->push_back(S3rightadc);


	pos++; // 2
	muTagadc = new TBADCRaw(name+"3",false,m_rodBlock[pos] & 0xfff);
	logstr << MSG::DEBUG << "muTag :" << MSG::hex << (m_rodBlock[pos] & 0xfff) << " [0x" << m_rodBlock[pos] << "]" 
	      << MSG::dec << endmsg;
	m_adcrawCont->push_back(muTagadc);
	//muTag->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,muTagadc);  

	pos++; // 2
	C1adc = new TBADCRaw(name+"4",false,m_rodBlock[pos] & 0xfff);
	logstr << MSG::DEBUG << "C1 :" << MSG::hex << (m_rodBlock[pos] & 0xfff) << " [0x" << m_rodBlock[pos] << "]"
	       << MSG::dec << endmsg;
	m_adcrawCont->push_back(C1adc);
	//C1->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,C1adc);  

	pos++; // 2
	C2adc = new TBADCRaw(name+"5",false,m_rodBlock[pos] & 0xfff);
	logstr << MSG::DEBUG << "C2 :" << MSG::hex << (m_rodBlock[pos] & 0xfff) << " [0x" << m_rodBlock[pos] << "]"
	       << MSG::dec << endmsg;
	m_adcrawCont->push_back(C2adc);
	//C2->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,C2adc);  


	pos++; // 2
	muHaloadc = new TBADCRaw(name+"6",false,m_rodBlock[pos] & 0xfff);
	logstr << MSG::DEBUG << "muHalo :" << MSG::hex << (m_rodBlock[pos] & 0xfff) << " [0x" << m_rodBlock[pos] << "]"
	       << MSG::dec << endmsg;
	m_adcrawCont->push_back(muHaloadc);
	//muHalo->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,muHaloadc);  

	pos++; // 2
	muVetoadc = new TBADCRaw(name+"7",false,m_rodBlock[pos] & 0xfff);
	logstr << MSG::DEBUG << "muVeto :" << MSG::hex << (m_rodBlock[pos] & 0xfff) << " [0x" << m_rodBlock[pos] << "]"
	       << MSG::dec << endmsg;
	m_adcrawCont->push_back(muVetoadc);
	//muVeto->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,muVetoadc);  
	
	if(m_dump){
	  logstr << MSG::INFO << " DUMP : ";
	  logstr << name << " "<< m_subfrag_size-2 <<" words -  " <<  S1adc->getADC() << " " <<  S2upadc->getADC();
	  logstr << " " << S3rightadc->getADC()<< " " << muTagadc->getADC() << " " << C1adc->getADC();
	  logstr << " " << C2adc->getADC() << " " << muHaloadc->getADC() << " "<< muVetoadc->getADC() <<endmsg ;

	}

      }
      break;
    case 0x11: // ADC2 segment ----------------------------------
      {
	name="frag0x11chan";
	logstr << MSG::DEBUG << "Got SubFragment " << MSG::hex << m_subfrag_id << " name=" << name   
	       << " Data words = " << MSG::dec << m_subfrag_size-2<<endmsg;
	
	int pos=m_subfrag_firstdata;

	// channel 0 not used 
	// Modification: 2005/01/27 W.L. channel 0 used by TRT Scintillator
	sTRTadc = new TBADCRaw(name+"0",false,m_rodBlock[pos] & 0xfff);
	logstr << MSG::DEBUG << "sTRT:" << MSG::hex << (m_rodBlock[pos] & 0xfff) << " [0x" << m_rodBlock[pos] << "]"
	       << MSG::dec << endmsg;
	pos++; //1
	S2downadc = new TBADCRaw(name+"1",false,m_rodBlock[pos] & 0xfff);
	logstr << MSG::DEBUG << "S2 :" << MSG::hex << (m_rodBlock[pos] & 0xfff) << " [0x" << m_rodBlock[pos] << "]"
	       << MSG::dec << endmsg;
	m_adcrawCont->push_back(S2downadc);

	pos++; // 2
	S3leftadc = new TBADCRaw(name+"2",false,m_rodBlock[pos] & 0xfff);
	logstr << MSG::DEBUG << "S3 :" << MSG::hex << (m_rodBlock[pos] & 0xfff) << " [0x" << m_rodBlock[pos] << "]"
	       << MSG::dec << endmsg;
	m_adcrawCont->push_back(S3leftadc);
	
	if(m_dump){
	  logstr << MSG::INFO << " DUMP : ";
	  logstr << name << " "<< m_subfrag_size-2 <<" words -  " << S2downadc->getADC() << " " << S3leftadc->getADC()<<endmsg;
	}

      }
      break;
    case 0x12: // ps TDC  --------------------------------------
      {
	name="frag0x12chan";
	logstr << MSG::DEBUG << "Got SubFragment " << MSG::hex << m_subfrag_id << " name=" << name  
	       << " Data words = " << MSG::dec << m_subfrag_size-2<<endmsg;
	int pos=m_subfrag_firstdata;
	if(m_dump) logstr << MSG::INFO << " DUMP : "<< name<< " "<< m_subfrag_size-2 <<" words - " ;

	for( int i=0;i<m_subfrag_size-2;i++){
	  pos=m_subfrag_firstdata+i;
	  // get channel number
	  const int chan= (m_rodBlock[pos] >> 16) & 0x1F;
	  const unsigned int tdc = m_rodBlock[pos] & 0xfff; // 12 first bits (0-11)
	  const int corrup = (m_rodBlock[pos]>>21)&0x3f;    // test bits 21-26
	  const bool endofdata=(((m_rodBlock[pos] >> 24)&0x7)==0x4); 
	  // from doc it appears that  endofdata <=> corrup==32
	  const bool overflow=(m_rodBlock[pos]>>12)&1;
	  const bool underthresh=(m_rodBlock[pos]>>13)&1;
	  logstr << MSG::DEBUG << MSG::dec << "Pos=" << pos <<" Chan "<< chan << "=" << tdc << ", OV=" << overflow 
		 << ", UN=" <<underthresh << " EOD="<< endofdata << ", corrupt=" << corrup 
		 << " [0x" << MSG::hex << m_rodBlock[pos] << "]" << MSG::dec <<endmsg; 
	  if(m_dump)
 	    MSG::dec(logstr) << " "<< (m_rodBlock[pos] & 0xfff);
	  

	  if (!endofdata && !corrup) {
	    os.str("");
	    os << name << chan;
	    switch(chan){
	    case 0:
	      ClockPhasetdc = new TBTDCRaw(os.str(),overflow,tdc,underthresh);
	      m_tdcrawCont->push_back(ClockPhasetdc);
	      break;
	    case 1:
	      S1tdc = new TBTDCRaw(os.str(),overflow,tdc,underthresh);
	      m_tdcrawCont->push_back(S1tdc);
	      break;
	    case 2:
	      S2downtdc = new TBTDCRaw(os.str(),overflow,tdc,underthresh);
	      m_tdcrawCont->push_back(S2downtdc);
	      break;
	    case 3:
	      S3lefttdc = new TBTDCRaw(os.str(),overflow,tdc,underthresh);
	      m_tdcrawCont->push_back(S3lefttdc);
	      break;
	    case 4:
	      S2uptdc = new TBTDCRaw(os.str(),overflow,tdc,underthresh);
	      m_tdcrawCont->push_back(S2uptdc);
	      break;
	    case 5:
	      S3righttdc = new TBTDCRaw(os.str(),overflow,tdc,underthresh);
	      m_tdcrawCont->push_back(S3righttdc);
	      break;
	    case 6:
	      S0tdc = new TBTDCRaw(os.str(),overflow,tdc,underthresh);
	      m_tdcrawCont->push_back(S0tdc);
	      break;
	    default:
	      break;
	    } // end switch
	  }//end if not corrupted and not end of data
	  else 

	    if (endofdata) {
	      if (i==m_subfrag_size-3)
		logstr << MSG::DEBUG << " End of data word found at pos " << pos <<endmsg;
	      else
		logstr << MSG::ERROR << " Unexpected end-of-data word found at pos " << pos <<endmsg;
	    }
	    else // corrupt
	      logstr<< MSG::ERROR << "Corrupted data in SubFragment 0x12 pos=" << pos << "channel=" <<chan << " [0x" <<
		MSG::hex << m_rodBlock[pos] << "]" << MSG::dec<<endmsg;
	} //end loop over subfrags
 	if(m_dump) logstr << endmsg;
      }
      break;
    case 0x13:  // ns TDC (for BPC)  --------------------------------------
      {
	name="frag0x13chan";
	logstr << MSG::DEBUG << "Got SubFragment " << MSG::hex << m_subfrag_id << " name=" << name 
	       << " Data words = " << MSG::dec << m_subfrag_size-2<<endmsg;
	if(m_dump) logstr << MSG::INFO << " DUMP : "<< name<< " "<< m_subfrag_size-2 <<" words - " << endmsg;
	for( int i=0;i<m_subfrag_size-2;i++){
	  int pos=m_subfrag_firstdata+i;
	  // get channel number
	  const int chan= (m_rodBlock[pos] >> 17) & 0xF;     // bits 17-20
	  const unsigned int tdc = m_rodBlock[pos] & 0xffff; // 16 first bits (0-15)
	  const bool corrup = (m_rodBlock[pos]>>21)&0x1;     // test bit 21
	  const bool endofdata=!((m_rodBlock[pos] >> 23)&0x1); // test bit 23
	  logstr << MSG::DEBUG << MSG::dec <<"Pos=" << pos << " Chan "<< chan << "=" << tdc 
		 << " EOD="<< endofdata << ", corrupt=" << corrup 
		 << " [0x" << MSG::hex << m_rodBlock[pos] << "]" << MSG::dec <<endmsg; 
	  if (!endofdata && !corrup) {
	    os.str("");
	    os << name << chan;
	    TBTDCRaw * tbtdc = new TBTDCRaw(os.str(),false,tdc,false);
	    m_tdcrawCont->push_back(tbtdc);
	    const short bpc_ind = chan / 4 ;
	    if(bpc_ind>4) {
	      logstr<< MSG::DEBUG << "Error in 0x13 : found chan="<<chan<<" corresponding to bpc "<<bpc_ind<<endmsg;
	      continue;
	    }
	    const short signaltype = tdc_order[chan % 4];
	    if (BPCtdc[bpc_ind][signaltype]==NULL)
	      BPCtdc[bpc_ind][signaltype] = tbtdc;
	  }
	  else 
	    if (endofdata) {
	      if (i==m_subfrag_size-3)
		logstr << MSG::DEBUG << " End of data word found at pos " << pos <<endmsg;
	      else
		logstr << MSG::ERROR << " Unexpected end-of-data word found at pos " << pos <<endmsg;
	    }
	    else // corrupt
	      logstr<< MSG::ERROR << "Corrupted data in SubFragment 0x13 pos=" << pos << "channel=" <<chan << " [0x" <<
		MSG::hex << m_rodBlock[pos] << "]" << MSG::dec<<endmsg;
	} // end for loop
	
      }
      break;
    case 0x14:  // ns TDC (for BPC) --------------------------------------
      {
	name="frag0x14chan";
	logstr << MSG::DEBUG << "Got SubFragment " << MSG::hex << m_subfrag_id << " name=" << name 
	       << " Data words = " << MSG::dec << m_subfrag_size-2<<endmsg;


	if(m_dump) logstr << MSG::INFO << " DUMP : "<< name<< " "<< m_subfrag_size-2 <<" words - " << endmsg;


	for( int i=0;i<m_subfrag_size-2;i++){
	  int pos=m_subfrag_firstdata+i;

	   // get channel number
	  const int chan= (m_rodBlock[pos] >> 17) & 0xF;     // bits 17-20
	  const unsigned int tdc = m_rodBlock[pos] & 0xffff; // 16 first bits (0-15)
	  const bool corrup = (m_rodBlock[pos]>>21)&0x1;     // test bit 21
	  const bool endofdata=!((m_rodBlock[pos] >> 23)&0x1); // test bit 23
	  //const bool overflow=(m_rodBlock[pos]>>12)&1;
	  //const bool underthresh=(m_rodBlock[pos]>>13)&1;
	  logstr << MSG::DEBUG << MSG::dec << "Pos=" << pos << " Chan "<< chan << "=" << tdc 
		 << " EOD="<< endofdata << ", corrupt=" << corrup 
		 << " [0x" << MSG::hex << m_rodBlock[pos] << "]" << MSG::dec <<endmsg; 

	  if (!endofdata && !corrup) {
	    os.str("");
	    os << name << chan;
	    TBTDCRaw * tbtdc = new TBTDCRaw(os.str(),false,tdc,false);
	    m_tdcrawCont->push_back(tbtdc);
	    const short bpc_ind = chan / 4 + 3;
	    if(bpc_ind>4) { logstr<< MSG::DEBUG << "Error in 0x14 : found chan="<<chan<<" corresponding to bpc "<<bpc_ind<<endmsg;continue;}
	    const short signaltype = tdc_order[chan % 4];
	    if (BPCtdc[bpc_ind][signaltype]==NULL)
	      BPCtdc[bpc_ind][signaltype] = tbtdc;
	  }
	  else 
	    if (endofdata) {
	      if (i==m_subfrag_size-3)
		logstr << MSG::DEBUG << " End of data word found at pos " << pos <<endmsg;
	      else
		logstr << MSG::ERROR << " Unexpected end-of-data word found at pos " << pos <<endmsg;
	    }
	    else // corrupt
	      logstr<< MSG::ERROR << "Corrupted data in SubFragment 0x14 pos=" << pos << "channel=" <<chan << " [0x" <<
		MSG::hex << m_rodBlock[pos] << "]" << MSG::dec<<endmsg;
	} // end for loop
      }
      break;
    case 0x15:  // trigger word --------------------------------------
      {
	logstr << MSG::INFO << "Found trigger word fragment" << endmsg;
      }
      break;
    default :
      logstr << MSG::DEBUG << "Found undefined subfragment id= "<< m_subfrag_id << endmsg;
      break;
    
    }
  }
  


  logstr << MSG::DEBUG << "Filling Scint" << endmsg;

  // fill scint container
  m_scintrawCont = new TBScintillatorRawCont();
  //m_scintrawCont->push_back(S0);
  //logstr << MSG::DEBUG << "S1 " << endmsg

  // Here some tdc maybe missing, so test for them :
  if(S0tdc) S0->setSignals(m_tdcrawCont,S0tdc,m_adcrawCont,dummyadc); //Scintillator 0 has only TDC
  else S0->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,dummyadc);  
  m_scintrawCont->push_back(S0);

  if(S1tdc && S1adc) S1->setSignals(m_tdcrawCont,S1tdc,m_adcrawCont,S1adc);  
  else S1->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,S1adc);  
  m_scintrawCont->push_back(S1);

  if(S2uptdc) S2up->setSignals(m_tdcrawCont,S2uptdc,m_adcrawCont,S2upadc);  
  else S2up->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,S2upadc);  
  m_scintrawCont->push_back(S2up);

  if(S2downtdc) S2down->setSignals(m_tdcrawCont,S2downtdc,m_adcrawCont,S2downadc);  
  else S2down->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,S2downadc);  
  m_scintrawCont->push_back(S2down);

  if(S3lefttdc) S3left->setSignals(m_tdcrawCont,S3lefttdc,m_adcrawCont,S3leftadc);  
  else S3left->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,S3leftadc);  
  m_scintrawCont->push_back(S3left);

  if(S3righttdc)  S3right->setSignals(m_tdcrawCont,S3righttdc,m_adcrawCont,S3rightadc);
  else S3right->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,S3rightadc);  
  m_scintrawCont->push_back(S3right); 

  C1->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,C1adc);  
  m_scintrawCont->push_back(C1); 
  C2->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,C2adc); 
  m_scintrawCont->push_back(C2);
  muTag->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,muTagadc); 
  m_scintrawCont->push_back(muTag);
  muHalo->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,muHaloadc);
  m_scintrawCont->push_back(muHalo);
  muVeto->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,muVetoadc);
  m_scintrawCont->push_back(muVeto);
  sTRT->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,sTRTadc);
  m_scintrawCont->push_back(sTRT);


  // fill bpc container
  logstr << MSG::DEBUG << "Filling BPC" << endmsg;
  m_bpcrawCont = new TBBPCRawCont();

  for(int i=0;i<BPCNum;i++){
    std::list<const TBTDCRaw*> listtdc;
    std::list<const TBADCRaw*> listadc;
    for(int j=0;j<4;j++){
      if(BPCtdc[i][j]==0) listtdc.push_back(dummytdc);
      else listtdc.push_back(BPCtdc[i][j]);
    }
    listadc.push_back(dummyadc);  listadc.push_back(dummyadc);
    bpcraw[i]->setSignals(m_tdcrawCont,listtdc,m_adcrawCont,listadc);
    m_bpcrawCont->push_back(bpcraw[i]);   
  }



  // -----------------------------------------------------------------------------------------------

  // Now Get info from TileAux RodBlock subdet ID = 0x70
  //sc=GetRODBlock((EventFormat::SubDetector) 0x70); //Possible improvement: Check if vector is already valid. 
  sc=GetRODBlock(m_subdet_id,eformat::TILECAL_LASER_CRATE);
  if (sc!=StatusCode::SUCCESS) {
    // return sc;
    goodRodBlock = false;
    logstr << MSG::ERROR << "Tile laser crate ROD block error!  Nothing valid in SG." << endmsg;
  }
  if (m_rodBlock.size()<2) {
      logstr << MSG::ERROR << "Tile laser crate ROD block too small!  Nothing valid in SG" << endmsg;
      // return StatusCode::FAILURE;
      goodRodBlock = false;
   }  

  // with this initialisation, first call of NextSubFrag will initialise correctly the index :
  m_subfrag_id=0;
  m_subfrag_size=2;   
  m_subfrag_firstdata=0;
  // while(NextSubFrag()){
  while(NextSubFrag() and goodRodBlock ){
    switch(m_subfrag_id){
    case 0x1: // ADC1 block --------------------------------------
      {
	name="frag0x1chan";
	int pos=m_subfrag_firstdata;
	TBScintillatorRaw * SC1 = new TBScintillatorRaw("SC1");
	TBADCRaw* SC1adc = new TBADCRaw(name+"0",false,m_rodBlock[pos] & 0xfff);
	m_adcrawCont->push_back(SC1adc);
	SC1->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,SC1adc);  
	m_scintrawCont->push_back(SC1);

	pos++;
	TBScintillatorRaw * SC2 = new TBScintillatorRaw("SC2");
	TBADCRaw* SC2adc = new TBADCRaw(name+"1",false,m_rodBlock[pos] & 0xfff);
	m_adcrawCont->push_back(SC2adc);
	SC2->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,SC2adc);  
	m_scintrawCont->push_back(SC2);
	logstr << MSG::DEBUG << "Found Scintillator SC1 and SC2" << endmsg;
	break;
      }
    case 0x2: // ADC2 block --------------------------------------
      {

	int pos=m_subfrag_firstdata;
	for(int i=0;i<8;i++){
	  pos=pos+i;
	  std::stringstream RawName;
	  RawName << "frag0x1chan"<<i;
	  TBADCRaw* MuWalladc = new TBADCRaw(RawName.str(),false,m_rodBlock[pos] & 0xfff);
	  m_adcrawCont->push_back(MuWalladc);

	  std::stringstream MuWallName;
	  MuWallName << "MuWall"<<i;
	  TBScintillatorRaw* MuWallScint = new TBScintillatorRaw(MuWallName.str());
	  MuWallScint->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,MuWalladc);
	  m_scintrawCont->push_back(MuWallScint);
	}	
	logstr << MSG::DEBUG << "Found first muWall block" << endmsg; 
	break;
      }
    case 0x3: // ADC3 block --------------------------------------
      {

	int pos=m_subfrag_firstdata;
	
	for(int i=0;i<4;i++){
	  pos=pos+i;
	  std::stringstream RawName;
	  RawName << "frag0x1chan"<<(i+8);
	  TBADCRaw* MuWalladc = new TBADCRaw(name,false,m_rodBlock[pos] & 0xfff);
	  m_adcrawCont->push_back(MuWalladc);

	  std::stringstream MuWallName;
	  MuWallName << "MuWall"<<i+8;
	  TBScintillatorRaw* MuWallScint = new TBScintillatorRaw(MuWallName.str());
	  MuWallScint->setSignals(m_tdcrawCont,dummytdc,m_adcrawCont,MuWalladc);
	  m_scintrawCont->push_back(MuWallScint);
	}
	logstr << MSG::DEBUG << "Found second muWall block" << endmsg; 
	break;
      }
    default :
      if (m_subfrag_id!=3 && m_subfrag_id!=5 && m_subfrag_id!=7) 
	logstr << MSG::DEBUG << "Found undefined subfragment id= "<< m_subfrag_id << endmsg;
      break;

    }
  }




  // -----------------------------------------------------------------------------------------------

  // Now record object if they have not been invoked from converter
  
//   if(unrec_code!=1){ //  If object was not requested by the converter, store it in SG
//     sc = m_storeGate->record(m_trigpat,m_keys[1]);
//     if ( sc.isFailure( ) ) {recordfailure=true;
//       logstr << MSG::FATAL << "Cannot record TBTrigPat " << endmsg;
//     }
//   }else {sc=StatusCode::SUCCESS; gotobject=true;} // else return success to converter


  if(unrec_code!=5){ //  If object was not requested by the converter, store it in SG
    sc = evtStore()->record(m_bpcrawCont,m_keys[5]);
    if ( sc.isFailure( ) ) {recordfailure=true;
      logstr << MSG::FATAL << "Cannot record BPCRawCont" << endmsg;
    }
  }else {sc=StatusCode::SUCCESS; gotobject=true;} // else return success to converter


  if(unrec_code!=6){ //  If object was not requested by the converter, store it in SG
    sc = evtStore()->record(m_scintrawCont,m_keys[6]);
    if ( sc.isFailure( ) ) {recordfailure=true;
      logstr << MSG::FATAL << "Cannot record ScintRawCont " << endmsg;
    }
  } else {sc=StatusCode::SUCCESS; gotobject=true;} // else return success to converter


  if(unrec_code!=2){ //  If object was not requested by the converter, store it in SG
    sc = evtStore()->record(m_tdcrawCont,m_keys[2]);
    if ( sc.isFailure( ) ) {recordfailure=true;
      logstr << MSG::FATAL << "Cannot record TDCCont " << endmsg;
    }  
  }else {sc=StatusCode::SUCCESS; gotobject=true;} // else return success to converter
  
  if(unrec_code!=3){ //  If object was not requested by the converter, store it in SG
    sc = evtStore()->record(m_adcrawCont,m_keys[3]);
    if ( sc.isFailure( ) ) {recordfailure=true;
      logstr << MSG::FATAL << "Cannot record ADCCont " << endmsg;
    }  
  }else {sc=StatusCode::SUCCESS; gotobject=true;} // else return success to converter


  m_trigpat      = new TBTriggerPatternUnit();
  m_trigpat->setTriggerWord(m_h8_triggword);
  if(unrec_code!=1) { //  If object was not requested by the converter, store it in SG
    logstr << MSG::DEBUG << "Recording TBTriggerPatternUnit with key " << m_keys[1] << endmsg;
    //    if(! evtStore()->contains<TBTriggerPatternUnit>(m_keys[1])) {
    sc = evtStore()->record(m_trigpat,m_keys[1]);
    if ( sc.isFailure( ) ) {
      logstr << MSG::ERROR << "Cannot record TBTrigPat " << endmsg;
    }
  } else {
    sc=StatusCode::SUCCESS;
    gotobject=true;
  } // else return success to converter


  logstr << MSG::DEBUG << " End of H8 Build " << endmsg;
  if(!gotobject) {logstr<< MSG::ERROR<< " Could not find object of type "<<unrec_code << endmsg; sc=StatusCode::FAILURE;}
  if(recordfailure) {logstr<< MSG::ERROR<< " One object could not be recorded "<< endmsg; sc=StatusCode::FAILURE;}


  return sc;
}

StatusCode TBByteStreamCnvTool::H6RecordObjects(int unrec_code)
{

  StatusCode sc;
  MsgStream logstr(msgSvc(), name());
  logstr << MSG::DEBUG << "About to try recording.  unrec_code = " << unrec_code << endmsg;

  if( !m_tailcatchraw) {
    logstr << MSG::DEBUG << " WARNING! Did not find TailCatcher. Returning an empty one : this may crash every attempt to use it" << endmsg;
    m_tailcatchraw = new TBTailCatcherRaw();
  }
  
  if(unrec_code!=1) { //  If object was not requested by the converter, store it in SG
    logstr << MSG::DEBUG << "Recording TBTriggerPatternUnit with key " << m_keys[1] << endmsg;
    sc = evtStore()->record(m_trigpat,m_keys[1]);
    if ( sc.isFailure( ) ) {
      logstr << MSG::ERROR << "Cannot record TBTrigPat " << endmsg;
    }
  }

  if(unrec_code!=4){ //  If object was not requested by the converter, store it in SG
    logstr << MSG::DEBUG << "Recording TBTailCatcherRaw with key " << m_keys[4] << endmsg;
    sc = evtStore()->record(m_tailcatchraw,m_keys[4]);
    if ( sc.isFailure( ) ) {
      logstr << MSG::ERROR << "Cannot record TailCatcherRaw " << endmsg;
    }
  }
    
  if(unrec_code!=5) { //  If object was not requested by the converter, store it in SG
    logstr << MSG::DEBUG << "Recording TBBPCRawCont with key " << m_keys[5] << endmsg;
    //    if(! evtStore()->contains<TBBPCRawCont>(m_keys[5])) {
      sc = evtStore()->record(m_bpcrawCont,m_keys[5]);
      if ( sc.isFailure( ) ) {
        logstr << MSG::ERROR << "Cannot record BPCRawCont" << endmsg;
      }
      //    } else {
      //      logstr << MSG::ERROR << " Object TBBPCRawCont already in SG (memory leak!)" << endmsg;
      //    }
  }
  
  if(unrec_code!=6){ //  If object was not requested by the converter, store it in SG
    logstr << MSG::DEBUG << "Recording TBScintillatorRawCont with key " << m_keys[6] << endmsg;
    //    if(! evtStore()->contains<TBScintillatorRawCont>(m_keys[6])) {
      sc = evtStore()->record(m_scintrawCont,m_keys[6]);
      if ( sc.isFailure( ) ) {
        logstr << MSG::ERROR << "Cannot record ScintRawCont " << endmsg;
      }
      //    } else {
      //      logstr << MSG::ERROR << " Object TBScintillatorRawCont already in SG (memory leak!)" << endmsg;
      //    }
  }
  
  if(unrec_code!=7){ //  If object was not requested by the converter, store it in SG
    //    if(! evtStore()->contains<TBMWPCRawCont>(m_keys[7])) {
      logstr << MSG::DEBUG << "record TBMWPCRawCont with key " << m_keys[7] <<endmsg;
      sc = evtStore()->record(m_mwpcrawCont,m_keys[7]);
      if ( sc.isFailure( ) ) {
        logstr << MSG::ERROR << "Cannot record MWPCRawCont " << endmsg;
      }
      //    } else {
      //      logstr << MSG::ERROR << " Object TBMWPCRawCont already in SG (memory leak!)" << endmsg;
      //    }
  }

  // TDCRaw and ADCRaw are special since they are build from all
  // subfragments in H6.  Record them slightly differently.
  
  if(unrec_code!=2){ //  If object was not requested by the converter, store it in SG
    logstr << MSG::DEBUG << "record TDC cont with key " << m_keys[2] << endmsg;
    //    if(! evtStore()->contains<TBTDCRawCont>(m_keys[2])) {
      sc = evtStore()->record(m_tdcrawCont,m_keys[2]);
      if ( sc.isFailure( ) ) {
        logstr << MSG::ERROR << "Cannot record TDCCont " << endmsg;
      }
  }
      //    } else {
      //      logstr << MSG::ERROR << " Object TBTDCRawCont already in SG (memory leak!)" << endmsg;
      //    }

  
  if(unrec_code!=3){ //  If object was not requested by the converter, store it in SG
    logstr << MSG::DEBUG << "record ADC cont with key " << m_keys[3] << endmsg;
    //    if(! evtStore()->contains<TBADCRawCont>(m_keys[3])) {
      sc = evtStore()->record(m_adcrawCont,m_keys[3]);
      if ( sc.isFailure( ) ) {
        logstr << MSG::FATAL << "Cannot record ADCCont " << endmsg;
      }
  }
  //    } else {
      //      logstr << MSG::ERROR << " Object TBADCRawCont already in SG (memory leak!)" << endmsg;
      //    }

  for(int c=10;c<14;c++){
    if(unrec_code!=c){ //  If object was not requested by the converter, store it in SG
      
      logstr << MSG::DEBUG << "record TBLArDigitContainer with key " << m_keys[c] << " and size " << m_tblardigitcont[c-10]->size() << endmsg;
      sc = evtStore()->record(m_tblardigitcont[c-10],m_keys[c]);
      if ( sc.isFailure( ) ) {
        logstr << MSG::FATAL << "Cannot record  " <<m_keys[c-10]<< endmsg;
      }
    }
  }

  for(int c=10;c<14;c++){
    if(unrec_code!=c){ //  If object was not requested by the converter, store it in SG
      
      logstr << MSG::DEBUG << "record TBLArCalibDigitContainer with key " << m_keys[c] << " and size " << m_tblarcalibdigitcont[c-10]->size() << endmsg;
      sc = evtStore()->record(m_tblarcalibdigitcont[c-10],m_keys[c]);
      if ( sc.isFailure( ) ) {
        logstr << MSG::FATAL << "Cannot record  " <<m_keys[c-10]<< endmsg;
      }
    }
  }
  
  if(unrec_code!=30){ //  If object was not requested by the converter, store it in SG
    sc = evtStore()->record(m_eventinfo,"TBEventInfo");
    if ( sc.isFailure( ) ) {
      logstr << MSG::FATAL << "Cannot record TBEventInfo "<< endmsg;
    }
  }
  //    } else {
  //      logstr << MSG::ERROR << " Object TBADCRawCont already in SG (memory leak!)" << endmsg;
  //    }

  logstr << MSG::DEBUG << " End of H6 Record " << endmsg;

  // Print run summary in text file

  return sc;

}

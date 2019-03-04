/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArADC2MeVTool.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "LArCabling/LArCablingLegacyService.h"
//#include "LArCabling/LArSuperCellCablingTool.h"

#include "LArElecCalib/LArConditionsException.h"

#include "LArRawConditions/LArConditionsContainer.h"

LArADC2MeVTool::LArADC2MeVTool(const std::string& type, 
			       const std::string& name, 
			       const IInterface* parent) 
  : 
    AthAlgTool(type, name, parent) ,
    m_lar_on_id(nullptr),
    m_cablingService(nullptr),
    m_larmcsym("LArMCSymTool"),
    m_IOVDbSvc("IOVDbSvc",name),
    m_keyADC2DAC("LArRamp"),	
    m_keyDAC2uA("LArDAC2uA"), 
    m_keyuA2MeV("LAruA2MeV"),
    m_keyMphysMcal("LArMphysOverMcal"),
    m_keyHVScaleCorr("LArHVScaleCorr"),
    m_MCSym(false), 
    m_useMphysOverMcal(true),
    m_useHVScaleCorr(false),
    m_useFEBGainThresholds(false),
    m_cacheValid(false),
    m_isSC(false)

{
  declareInterface<ILArADC2MeVTool>(this);
  declareProperty("keyADC2DAC",m_keyADC2DAC);
  declareProperty("keyDAC2uA",m_keyDAC2uA);
  declareProperty("keyuA2MeV",m_keyuA2MeV);
  declareProperty("keyMphysMcal",m_keyMphysMcal);
  declareProperty("keyHVScaleCorr",m_keyHVScaleCorr);
  declareProperty("MCSym",m_MCSym);
  declareProperty("UseMphysOverMcal",m_useMphysOverMcal);
  declareProperty("UseHVScaleCorr",m_useHVScaleCorr);
  declareProperty("LoadAtBegin",m_loadAtBegin=true);
  declareProperty("UseFEBGainTresholds",m_useFEBGainThresholds);
  declareProperty("IsSC",m_isSC);
  m_ADC2MeV=NULL;
}


LArADC2MeVTool::~LArADC2MeVTool() {
  delete m_ADC2MeV;
  if ( m_isSC ) m_ADC2MeV_vec.clear();
  
}

StatusCode LArADC2MeVTool::initialize() {
  ATH_MSG_DEBUG( "LArADC2MeVTool initialize() begin" );
  StatusCode sc;
  
  if ( !m_isSC ) {
    const LArOnlineID* laron;
    sc = detStore()->retrieve(laron,"LArOnlineID");
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve  LArOnlineID from DetectorStore");
      return StatusCode::FAILURE;
    } else m_lar_on_id = (LArOnlineID_Base*) laron;
    
    ToolHandle<LArCablingLegacyService> larcab("LArCablingLegacyService");
    if(larcab.retrieve().isFailure()){
      ATH_MSG_ERROR("Unable to get CablingLegacyService");
      return StatusCode::FAILURE;
    } else m_cablingService = (LArCablingBase*) &(*larcab);
    
    ATH_CHECK( m_larmcsym.retrieve( DisableTool{ !m_MCSym } ));

  } else { // isSC

    m_larmcsym.disable();    

    const LArOnline_SuperCellID* laron;
    sc = detStore()->retrieve(laron,"LArOnline_SuperCellID");
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve  LArOnlineID from DetectorStore");
      return StatusCode::FAILURE;
    } else m_lar_on_id = (LArOnlineID_Base*) laron;
    
    //ToolHandle<LArSuperCellCablingTool> larcab("LArSuperCellCablingTool");
    //if(larcab.retrieve().isFailure()){
      ATH_MSG_ERROR("Not working for SC, use the conditions Alg");
      return StatusCode::FAILURE;
    //} else m_cablingService = (LArCablingBase*) &(*larcab);
    
  }

  if (m_IOVDbSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to regrieve IOVDbSvc");
    return StatusCode::FAILURE;
  }


  // CallBack functions

  // for ramps
  sc=detStore()->regFcn(&ILArADC2MeVTool::LoadCalibration,dynamic_cast<ILArADC2MeVTool*>(this),
			m_dd_ADC2DAC,m_keyADC2DAC);
  if(sc.isSuccess()){
    ATH_MSG_INFO("Registered callback for key: " << m_keyADC2DAC);
  } else {
    ATH_MSG_ERROR("Cannot register testCallback function for key " << m_keyADC2DAC);
  }

  // for DAC2uA
  sc=detStore()->regFcn(&ILArADC2MeVTool::LoadCalibration,dynamic_cast<ILArADC2MeVTool*>(this),
			m_dd_DAC2uA,m_keyDAC2uA);
  if(sc.isSuccess()){
    ATH_MSG_INFO("Registered callback for key: " << m_keyDAC2uA);
  } else {
    ATH_MSG_ERROR("Cannot register testCallback function for key " << m_keyDAC2uA);
  }
  
  // for uA2MeV
  sc=detStore()->regFcn(&ILArADC2MeVTool::LoadCalibration,dynamic_cast<ILArADC2MeVTool*>(this),
			m_dd_uA2MeV,m_keyuA2MeV);
  if(sc.isSuccess()){  
    ATH_MSG_INFO("Registered callback for key: " << m_keyuA2MeV);
  } else {
    ATH_MSG_ERROR("Cannot register testCallback function for key " << m_keyuA2MeV);
  }

  if(m_useMphysOverMcal){
  
    // for MphysOverMcal
    sc=detStore()->regFcn(&ILArADC2MeVTool::LoadCalibration,dynamic_cast<ILArADC2MeVTool*>(this),
			  m_dd_MphysMcal,m_keyMphysMcal);
    if(sc.isSuccess()){  
      ATH_MSG_INFO("Registered callback for key: " << m_keyMphysMcal);
    } else {
      ATH_MSG_WARNING("Cannot register testCallback function for key " << m_keyMphysMcal);
      ATH_MSG_WARNING(" Will set useMPhysOverMcal to false. This is OK for MC done before release 14 ");
      m_useMphysOverMcal = false;
    }
  }

  if (m_useHVScaleCorr) {
    sc = detStore()->regFcn(&ILArADC2MeVTool::LoadCalibration,dynamic_cast<ILArADC2MeVTool*>(this),
                            m_dd_HVScaleCorr,m_keyHVScaleCorr);  
    if(sc.isSuccess()){  
      ATH_MSG_INFO("Registered callback for key: " << m_keyHVScaleCorr);
    } else {
      ATH_MSG_ERROR("Cannot register testCallback function for key " << m_keyHVScaleCorr);
    }
  }

  if(m_useFEBGainThresholds) ATH_CHECK(m_configKey.initialize());

  if (m_loadAtBegin) {
    ATH_MSG_DEBUG( "Setting callback function to load calibration at begin of run");
    // Incident Service: 
    IIncidentSvc* incSvc;
    sc = service("IncidentSvc", incSvc);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve pointer to IncidentSvc");
      return sc;
    }
   
    //start listening to "BeginRun". The incident should be fired AFTER the IOV callbacks and only once.
    const long priority=std::numeric_limits<long>::min(); //Very low priority
    incSvc->addListener(this, "BeginRun", priority ,true,true); //single-shot incident
  }
    

  //
  ATH_MSG_DEBUG( "LArADC2MeVTool initialize() end");
  return StatusCode::SUCCESS;
}

StatusCode LArADC2MeVTool::finalize()
{
  return StatusCode::SUCCESS;
}


StatusCode LArADC2MeVTool::LoadCalibration(IOVSVC_CALLBACK_ARGS) {
  ATH_MSG_DEBUG( "In LoadCalibration");
  m_cacheValid=false;
  return StatusCode::SUCCESS;
}

void LArADC2MeVTool::handle(const Incident&) {
  ATH_MSG_DEBUG( "In Incident-handle");
  if (m_cacheValid) {
    ATH_MSG_DEBUG( "Cached data already computed.");
    return;
  }
  
  StatusCode sc=getADC2MeV();
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Failed to compute ADC2MeV factor at begin of run.");
    m_cacheValid=false;
  }
}



// *** compute global ADC2MeV factor from subfactors *** 
StatusCode LArADC2MeVTool::getADC2MeV() const {
  ATH_MSG_DEBUG( "in getADC2MeV");
  StatusCode sc;

  //Retrieve all ingeredients from Detector Store
  sc=detStore()->retrieve(m_dd_ADC2DAC,m_keyADC2DAC);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Cannot load LArADC2DAC object with key " << m_keyADC2DAC);
    ATH_MSG_ERROR("Check jobOption IOVDbSvc.Folders!"); 
    return StatusCode::FAILURE;
  }
 
  sc=detStore()->retrieve(m_dd_DAC2uA,m_keyDAC2uA);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Cannot load LArDAC2uA object with key " <<   m_keyDAC2uA);
    ATH_MSG_ERROR("Check jobOption IOVDbSvc.Folders!"); 
    return StatusCode::FAILURE;
  }
  
  sc=detStore()->retrieve(m_dd_uA2MeV,m_keyuA2MeV);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Cannot load LAruA2MeV object with key " << m_keyuA2MeV);
    ATH_MSG_ERROR("Check jobOption IOVDbSvc.Folders!"); 
    return StatusCode::FAILURE;
  }

  if (m_useMphysOverMcal) {
    sc=detStore()->retrieve(m_dd_MphysMcal,m_keyMphysMcal);
    if (sc.isFailure()) {
      ATH_MSG_WARNING("Cannot load LArMphysMcal object with key "<< m_keyMphysMcal << " but jobOption 'useMphysOverMcal' set.");
      ATH_MSG_WARNING("This is OK for MC generated before release 14"); 
    }
  }

  if (m_useHVScaleCorr) {
    sc=detStore()->retrieve(m_dd_HVScaleCorr,m_keyHVScaleCorr);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Cannot load LArHVScaleCorr object with key " << m_keyHVScaleCorr);
      ATH_MSG_ERROR("Check jobOption IOVDbSvc.Folders!"); 
      return StatusCode::FAILURE;
    }
  }

  if ( !m_isSC ){
  delete m_ADC2MeV; //Delete previous instance (if there is any)
  m_ADC2MeV=new LArConditionsContainer< std::vector<float> >();
  m_ADC2MeV->setGroupingType(LArConditionsContainerBase::SingleGroup); 
  CHECK( m_ADC2MeV->initialize());
  } else {
  for(unsigned int i=0;i<m_ADC2MeV_vec.size();i++) (m_ADC2MeV_vec[i]).clear();
  m_ADC2MeV_vec.clear();
  m_ADC2MeV_vec.assign( m_lar_on_id->channelHashMax(), std::vector<float>(0) ) ;
  }

  float DAC2uA;
  float uA2MeV;
  float MphysMcal = 0.0;
  float HVScaleCorr = 1.0;
  float ADC2MeV_temp;

  int count     = 0;       //Total number of channels
  int count2    = 0;       //Connected number of channels
  int count3[3] = {0,0,0}; //Channels without ramp (per gain)
  int count4    = 0;       //channels without DAC2uA
  int count5    = 0 ;      //channels without uA2DAC
  int count6    = 0 ;      //channels without MphysOverMcal
  int count7    = 0 ;      //channels without HVScaleCorr
  int count8    = 0;       //Channels withouth HIGH gain (med gain threshold=0)

 // get HWIdentifier iterator
  std::vector<HWIdentifier>::const_iterator it  = m_lar_on_id->channel_begin();
  std::vector<HWIdentifier>::const_iterator it_e= m_lar_on_id->channel_end();

  // loop over em Identifiers
  ATH_MSG_DEBUG( "start loop over cells in getADC2MeV");
  unsigned int ngains(3);
  if ( m_isSC ) ngains=1;
  
  // retrieve LArFebConfig if needed
  const LArFebConfig *febConfig=nullptr;
  if(m_useFEBGainThresholds) {
     SG::ReadCondHandle<LArFebConfig> configHdl{m_configKey};
     febConfig = *configHdl;
     if (febConfig==nullptr) {
        ATH_MSG_ERROR( "Unable to retrieve LArFebConfig with key " << m_configKey.key());
        return StatusCode::FAILURE;
     }
  }
  for(;it!=it_e;++it) {
    count ++;
    const HWIdentifier id  = *it;
    const IdentifierHash idhash = m_lar_on_id->channel_Hash( *it );
    bool flagBadValue = false;
    ATH_MSG_VERBOSE("*** Channel 0x" << MSG::hex << id.get_identifier32().get_compact() << MSG::dec);
    
    if(m_cablingService->isOnlineConnected(id)) {
      if(m_MCSym) {
	HWIdentifier id2 = m_larmcsym->symOnline(id);
	if (id2 != id) continue;
      }    
      count2++;
      // get subfactors for this channel
      DAC2uA = m_dd_DAC2uA->DAC2UA(id);
      uA2MeV = m_dd_uA2MeV->UA2MEV(id);

      if (DAC2uA == (float)ILArDAC2uA::ERRORCODE) {
	count4++;
	flagBadValue = true;
      }
      if (uA2MeV == (float)ILAruA2MeV::ERRORCODE) {
	count5++;
	flagBadValue = true;
      }    

      if ( !m_isSC ) {
      if (m_useHVScaleCorr && m_dd_HVScaleCorr) {
        HVScaleCorr = m_dd_HVScaleCorr->HVScaleCorr(id);
        if (HVScaleCorr == (float)ILAruA2MeV::ERRORCODE) {
          HVScaleCorr = 1.;
          count7++;
        }
      }
      }

      // the ADC2DAC subfactor is a function of gain
      for(unsigned int igain=0;igain<ngains;igain++) {
	// ADC2DAC is a vector (polynomial fit of ramps)
	const ILArRamp::RampRef_t ADC2DAC = m_dd_ADC2DAC->ADC2DAC(id,igain);

	// MphysOverMcal is a float and is gain-dependant
	if(m_useMphysOverMcal && m_dd_MphysMcal) {
	    MphysMcal = m_dd_MphysMcal->MphysOverMcal(id,igain);
	    if(MphysMcal == (float)ILArMphysOverMcal::ERRORCODE) {
	      //W.L 6.Feb 08: If no MphysOverMcal still use ramp (don't flag bad) and assume 1 for MphysOverMcal
	      MphysMcal = 1.0;
	      count6++;
	    }
	    else if (MphysMcal<=0) {
	      flagBadValue = true;
	      ATH_MSG_DEBUG( "Expect positive MphysOverMcal, found " << MphysMcal);
	      count6++;
	    } //end MphysMCal<0
	} //end if useMphysOverMcal
	
        // ###
	 if (msgLvl(MSG::VERBOSE)) {
	   msg( MSG::VERBOSE) << " Gain = " << igain << endmsg ;
	   if ( ADC2DAC.size() > 0 ) {
	     msg(MSG::VERBOSE) << " DAC2uA   = " << DAC2uA << endmsg ;
	     msg(MSG::VERBOSE) << " uA2MeV   = " << uA2MeV << endmsg ;
	   } 
	   else 
	     msg(MSG::VERBOSE) << " has empty ramps" << endmsg ;
	 }//end if VERBOSE
	 // ###	
      
	// multiply the subfactors for each element of the vector
	std::vector<float>  vADC2MeV;
        const std::size_t iMax = ADC2DAC.size(); 
        vADC2MeV.reserve(iMax); 
  	for(unsigned int i=0;i<iMax;i++) {
	  if (febConfig && igain==1 && i==0 && febConfig->lowerGainThreshold(id)<5) {
	    //Don't use ramp offset in MED gain offset if HIGH gain is not used
	    ++count8;
	    vADC2MeV.push_back(0.);
	  }
	  else {
	    ADC2MeV_temp = ADC2DAC[i]*DAC2uA*uA2MeV;
	    if(m_useMphysOverMcal && MphysMcal>0) ADC2MeV_temp/= MphysMcal;
	    if (m_useHVScaleCorr) ADC2MeV_temp = ADC2MeV_temp * HVScaleCorr;
	    vADC2MeV.push_back(ADC2MeV_temp);
	    // ###
	    //ATH_MSG_VERBOSE(" ADC2DAC[" << i << "] = " << ADC2DAC[i]);
	    // ###
	  }//end else
	}//End loop over ramp-vector	  	  
	// finally, fill the global factor ADC2MeV with the result
	if(flagBadValue == false){ // if no bad value was found, fill with result
	  if ( m_isSC ) m_ADC2MeV_vec[idhash] = vADC2MeV;
	  else m_ADC2MeV->setPdata(id,vADC2MeV,igain);
	}
	if(vADC2MeV.size() == 0 ) count3[igain]++;
      }//end loop over gains  
    }// end if connected
    else//unconnected   
      for(unsigned int igain=0;igain<ngains;igain++) 
	if ( m_isSC ) m_ADC2MeV_vec[idhash] = std::vector<float>(0);
	else m_ADC2MeV->setPdata(id,std::vector<float>(0),igain);
   //   
//        std::vector<float>  vADC2MeV;
//        vADC2MeV.push_back(0);
//        //std::cout << "setting disconnected Feb id "<< feb.get_compact() <<" channel " << chan << std::endl;
//        m_ADC2MeV->setPdata(id,vADC2MeV,igain);
//     }
    
  }//End loop over channels

  ATH_MSG_INFO("Ntotal = " << count);
  ATH_MSG_INFO("Nconnected = " << count2);
  ATH_MSG_INFO("Channels without DAC2uA = " << count4);
  ATH_MSG_INFO("Channels without uA2MeV = " << count5);
  ATH_MSG_INFO("Channels with medium gain threshold set to zero = " << count8);
  if(m_useMphysOverMcal) ATH_MSG_INFO("Channels without MphysMcal = " << count6);
  if (m_useHVScaleCorr)  ATH_MSG_INFO("Channels without HVScaleCorr = " << count7);
  ATH_MSG_INFO("High Gain: Channels without Ramp = " << count3[0]);
  ATH_MSG_INFO("Med Gain: Channels without Ramp = " << count3[1]);
  ATH_MSG_INFO("Low Gain: Channels without Ramp = " << count3[2]);
  ATH_MSG_DEBUG( "end of loop over cells ");

  m_cacheValid=true;

  return StatusCode::SUCCESS;  
}


// *** retrieve ADC2MEV for a given cell ***
const std::vector<float>& LArADC2MeVTool::ADC2MEV(const HWIdentifier& CellID, 
						  int gain ) const
{
  HWIdentifier id;
  if (m_MCSym) 
    id = m_larmcsym->symOnline(CellID);
  else
    id = CellID;
  if (!m_cacheValid) {
    StatusCode sc=getADC2MeV();
    if (sc.isFailure()) throw LArConditionsException("Could not compute ADC2MeV factors");
  }
  //Note that the const and the non-cost get method of the 
  //LArConditionsContainer behave differently. Here we want he const version.
  if ( m_isSC ) {
    IdentifierHash idhash = m_lar_on_id -> channel_Hash( CellID );
    return (m_ADC2MeV_vec[idhash] );
  } else 
  return const_cast<const LArConditionsContainer< std::vector<float> >* >(m_ADC2MeV)->get(id,gain);//feb,chan);
}

// *** retrieve ADC2MEV for a given cell ***
const std::vector<float>& LArADC2MeVTool::ADC2MEV(const Identifier& CellID, 
						  int gain) const
{
  HWIdentifier id;
  if (m_MCSym)
    id = m_larmcsym->symOnline(CellID);
  else 
    id = m_cablingService->createSignalChannelID(CellID); 
  if (!m_cacheValid) {
    StatusCode sc=getADC2MeV();
    if (sc.isFailure()) throw LArConditionsException("Could not compute ADC2MeV factors");
  }
  if ( m_isSC ) {
    IdentifierHash idhash = m_lar_on_id -> channel_Hash( id );
    return (m_ADC2MeV_vec[idhash] );
  } else 
  return ADC2MEV(id, gain);  
}

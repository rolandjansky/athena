/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArOnlDbPrep/LArOnlDbPrepAlg.h"

#include "LArRawConditions/LArOFCComplete.h" 
#include "LArRawConditions/LArPedestalComplete.h" 
#include "LArElecCalib/ILArRamp.h"
#include "LArElecCalib/ILAruA2MeV.h"
#include "LArElecCalib/ILArDAC2uA.h"
#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArMphysOverMcal.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "LArRawConditions/LArDSPThresholdsComplete.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include "StoreGate/StoreGateSvc.h"

#include <fstream>
#include <time.h>

LArOnlDbPrepAlg::LArOnlDbPrepAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name,pSvcLocator),
  m_detStore(0),
  m_lar_on_id(0),
  m_cablingService(0),
  m_ttService(0),
  m_calibCont(0),
  m_energyCont(0),
  m_keyShape("LArShape"), m_keyOFC("LArOFC"), 
  m_keyRamp("LArRamp"), m_keyPedestal("Pedestal"),
  m_keyuA2MeV("LAruA2MeV"),
  m_keyDAC2uA("LArDAC2uA"),
  m_keyMphysOverMcal("LArMphysOverMcal"),
  m_keyHVScaleCorr("LArHVScaleCorr"), 
  m_outFileName("output.xml")
  //m_defaultScale(1.0,5)
{
  declareProperty("keyShape",m_keyShape);
  declareProperty("keyOFC",m_keyOFC);
  declareProperty("keyRamp",m_keyRamp);
  declareProperty("keyPedestal",m_keyPedestal);
  declareProperty("keyuA2MeV",m_keyuA2MeV);
  declareProperty("keyDAC2uA",m_keyDAC2uA);
  declareProperty("keyMphysOverMcal",m_keyMphysOverMcal);
  declareProperty("keyHVScaleCorr",m_keyHVScaleCorr);
  declareProperty("keyDSPThresholds",m_keyDSPThresholds="LArDSPThresholds");
  declareProperty("febList",m_febList);
  declareProperty("applyRampIntercept",m_applyRampIntercept=0);
  declareProperty("OutputFile",m_outFileName);
}

StatusCode LArOnlDbPrepAlg::execute() {
  std::string out;
  time_t t1=clock();
  std::cout << "Start prepare" << std::endl;
  prepare().ignore();
  time_t t2=clock();
  std::cout << "stat xml" << std::endl;
  getXML(m_febList,out).ignore();
  time_t t3=clock();
  std::ofstream outfile(m_outFileName.c_str());
  outfile << out; 
  outfile.close();
  //std::cout << out << std::endl;
  time_t t4=clock();
  std::cout << "Prepare: " << float(t2-t1)/CLOCKS_PER_SEC << "sec" << std::endl;
  std::cout << "XML: " << float(t3-t2)/CLOCKS_PER_SEC << "sec" << std::endl;
  std::cout << "Print: " << float(t4-t3)/CLOCKS_PER_SEC << "sec" << std::endl;
  return StatusCode::SUCCESS;
}

StatusCode LArOnlDbPrepAlg::finalize()
{
  return StatusCode::SUCCESS; 
}
 
StatusCode LArOnlDbPrepAlg::initialize()
{
  MsgStream log( msgSvc(), name() );
  
  log << MSG::DEBUG << " initialize() begin" << endreq;

  // retrieve DetectorStore
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
    log  << MSG::ERROR << "Unable to retrieve  LArOnlineID from DetectorStore" 
	 << endreq;
    return StatusCode::FAILURE;
  }


  IToolSvc* toolSvc;
  sc = service( "ToolSvc",toolSvc );
  if(sc.isSuccess()) 
  {
    // retrieve LArCablingService
    sc = toolSvc->retrieveTool("LArCablingService",m_cablingService);
    if(sc.isFailure()){
      log << MSG::ERROR << "Unable to get CablingService " << endreq;
      return sc;
    }

    // retrieve CaloTriggerTowerService
    sc = toolSvc->retrieveTool("CaloTriggerTowerService",m_ttService);
    if(sc.isFailure()){
      log << MSG::ERROR << "Unable to get TriggerTowerService " << endreq;
      return sc;
    }
  } 
  else 
  {
     log << MSG::ERROR << "Could not retrieve ToolSvc " << endreq;
     return sc;
  }

  return StatusCode::SUCCESS; 
}


StatusCode LArOnlDbPrepAlg::prepare()
{

  MsgStream log( msgSvc(), name() );

  //std::vector<float> defaultRamp(2,1.0);
  //std::vector<float> defaultOFC(5,1.0);

  const CaloDetDescrManager * theCaloDDM = CaloDetDescrManager::instance();

  const LArPedestalComplete* peds ;	
  StatusCode sc = m_detStore->retrieve(peds,m_keyPedestal); 

  if(!sc.isSuccess())
  {
	log<<MSG::ERROR<<" Failed to retrieve Pedestal" << endreq;
	return sc; 
  } 

  const ILArOFC* ofcs ;	
  sc = m_detStore->retrieve(ofcs,m_keyOFC); 
  if(!sc.isSuccess())
  {
	log<<MSG::ERROR<<" Failed to retrieve OFC" << endreq;
	return sc; 
  } 


  const ILArRamp* ramps ;	
  sc = m_detStore->retrieve(ramps,m_keyRamp); 
  if(!sc.isSuccess())
  {
	log<<MSG::ERROR<<" Failed to retrieve Ramp" << endreq;
	return sc; 
  } 


  const ILAruA2MeV* uA2MeVs ;	
  sc = m_detStore->retrieve(uA2MeVs,m_keyuA2MeV); 
  if(!sc.isSuccess())
  {
	log<<MSG::ERROR<<" Failed to retrieve uA2MeV" << endreq;
	return sc; 
  } 

  const ILArDAC2uA* DAC2uAs ;	
  sc = m_detStore->retrieve(DAC2uAs,m_keyDAC2uA); 
  if(!sc.isSuccess())
  {
	log<<MSG::ERROR<<" Failed to retrieve DAC2uAs" << endreq;
	return sc; 
  } 

  const ILArShape* shapes  ;	
  sc = m_detStore->retrieve(shapes,m_keyShape ); 
  if(!sc.isSuccess())
  {
	log<<MSG::ERROR<<" Failed to retrieve LArShape" << endreq;
	return sc; 
  } 

  const ILArMphysOverMcal* mphysOverMcals ;
  sc = m_detStore->retrieve(mphysOverMcals,m_keyMphysOverMcal); 
  if(!sc.isSuccess())
    {
      log<<MSG::ERROR<<" Failed to retrieve LArMphysOverMcal" << endreq;
      return sc; 
    } 


  const ILArHVScaleCorr* hvScaleCorrs ;
  sc = m_detStore->retrieve(hvScaleCorrs,m_keyHVScaleCorr); 
  if(!sc.isSuccess())
    {
      log<<MSG::ERROR<<" Failed to retrieve LArHVScaleCorr" << endreq;
      return sc; 
    } 


  const LArDSPThresholdsComplete* dspThresh=NULL;
  if (m_keyDSPThresholds.size()>0) {
    sc=m_detStore->retrieve(dspThresh,m_keyDSPThresholds);
    if(!sc.isSuccess()){
      log<<MSG::ERROR<<" Failed to retrieve LArDSPThresholds with key " << m_keyDSPThresholds << endreq;
      return sc; 
    } 
  } else 
    log << MSG::WARNING << "No LArDSPThresholds key specified, no data loaded from DB. Use default values" << endreq;


  // initialize new containers
  m_calibCont = new LArOnlCalibDataContainer(); 
  m_calibCont->initialize();
  m_calibCont->setLArOnlineID(m_lar_on_id); 

  m_energyCont = new LArOnlEnergyDataContainer(); 
  m_energyCont->initialize();
  m_energyCont->setLArOnlineID(m_lar_on_id); 

  // record the data in StoreGate
  sc = m_detStore->record(m_calibCont,"LArOnlCalibData"); 
  if(!sc.isSuccess())
  {
	log<<MSG::ERROR<<" Failed to record LArOnlCalibData" << endreq;
	return sc; 
  } 

  sc = m_detStore->record(m_energyCont,"LArOnlEnergyData"); 
  if(!sc.isSuccess())
  {
	log<<MSG::ERROR<<" Failed to record LArOnlEnergyData" << endreq;
	return sc; 
  } 

  // loop over Pedestals, and get all the other calibration data for
  //  each channel.

  int n = 0; 
  for (unsigned int gain = 0; gain<3;++gain)
  { 

    log << MSG::DEBUG << " Gain: " << gain << endreq;

    LArPedestalComplete::ConstConditionsMapIterator it=peds->begin(gain) ;
    LArPedestalComplete::ConstConditionsMapIterator it_e=peds->end(gain) ;
    for(;it!=it_e;++it)
    {
	log<<MSG::VERBOSE<< " number of entry "<<n << endreq; 
	const LArPedestalComplete::LArCondObj& p = (*it); 
	// LArPedestalP p = (*it); 

	if(p.m_Pedestal <= (1.0+LArElecCalib::ERRORCODE)) {	  
          log<<MSG::ERROR<<" Pedestal is empty    " <<endreq;
          log<<MSG::ERROR<< m_lar_on_id->show_to_string(it.channelId()) <<endreq;	
	  continue; 
	}

	LArOnlCalibData t ; 
	t.m_pedestal = p.m_Pedestal ; 

        HWIdentifier febId( it.febId() ) ; 
	HWIdentifier chanId = it.channelId() ; 
        int chan= it.getChannel(); 

        log<<MSG::DEBUG<<" feb id , chan id, chan num " << febId << " "<<chanId<<" " << chan<<endreq;

	if(m_lar_on_id->isEMBchannel(chanId))
	  log << MSG::DEBUG << "partition: EMB" << endreq;
	else if(m_lar_on_id->isEMECchannel(chanId))
	  log << MSG::DEBUG << "partition: EMEC" << endreq;
	else if(m_lar_on_id->isHECchannel(chanId))
	  log << MSG::DEBUG << "partition: HEC" << endreq;
	else if(m_lar_on_id->isFCALchannel(chanId))
	  log << MSG::DEBUG << "partition: FCAL" << endreq;
	else
	  log << MSG::DEBUG << "partition: none!!" << endreq;


	if(!m_cablingService->isOnlineConnected(chanId)){
	  //log<<MSG::WARNING<<" not connected channel, skip  " << endreq;
	 continue ;
        }

	float uA2MeV = uA2MeVs->UA2MEV(chanId); 
	float DAC2uA = DAC2uAs->DAC2UA(chanId); 
        log<<MSG::DEBUG<<" uA2MeV, DAC2uA  "<< uA2MeV << " " << DAC2uA << endreq;

	const ILArRamp::RampRef_t ramp = ramps->ADC2DAC(chanId,gain);
        if(ramp.size()<2) {
	  if (gain!=2 && !m_lar_on_id->isEMBPS(chanId)){ 
	    //LOW gain PS has no ramp
	    log<<MSG::WARNING<<" Ramp size <2 for gain " << gain << " Channel:"
	     <<  m_lar_on_id->channel_name(chanId) << endreq;
	  }
	  continue;
        }

        ILArOFC::OFCRef_t ofc_a = ofcs->OFC_a(chanId,gain,0) ; 
        log<<MSG::DEBUG<<" OFC_A Size  "<<ofc_a.size()<<endreq;
        ILArOFC::OFCRef_t ofc_b = ofcs->OFC_b(chanId,gain,0) ; 
        log<<MSG::DEBUG<<" OFC_B Size  "<<ofc_b.size()<<endreq;

	if(ofc_a.size()!=ofc_b.size() ){
         log<<MSG::WARNING<<" ofc_a and ofc_b sizes dont match, skip  " <<endreq;
         log<<MSG::WARNING<< m_lar_on_id->show_to_string(it.channelId()) <<endreq;	
         continue;
        }	 


	if(ofc_a.size()==0  ){
	  if (gain!=2 && !m_lar_on_id->isEMBPS(chanId)){ 
	    //LOW gain PS has no OFCs
	    log<<MSG::WARNING<<" ofc size ==0 for gain " << gain << " Channel:"
	       <<  m_lar_on_id->channel_name(chanId) << endreq;
	  }
	  continue ;
        }

        ILArShape::ShapeRef_t shape = shapes->Shape(chanId,gain,0) ; 	
        log<<MSG::DEBUG<<" shape Size  "<<shape.size()<<endreq   ;


	// add check ERRORCODE... 
        if(shape.size()<ofc_a.size()){ 
         log<<MSG::ERROR<< " Shape size less than OFC size for gain " << gain << " Channel:"
	    <<  m_lar_on_id->channel_name(chanId) << endreq;
	 continue ;        
        } 


	if (uA2MeV<0) {
	   log<<MSG::ERROR<< " Invalid uA2MeV for gain " << gain << " Channel:"
	      <<  m_lar_on_id->channel_name(chanId) << endreq;
	   continue ;        
        } 

	if (DAC2uA<0) {
	   log<<MSG::ERROR<< " Invalid DAC2uA for gain " << gain << " Channel:"
	      <<  m_lar_on_id->channel_name(chanId) << endreq;
	   continue ;        
        } 


	float mphysOverMcal = mphysOverMcals->MphysOverMcal(chanId,gain);

	if (mphysOverMcal<0){
	  if (!m_lar_on_id->isFCALchannel(chanId)) {
	    //FCAL has no MphysOverMcal
	    log<<MSG::WARNING<<" negative MphysOverMcal " << mphysOverMcal 
	       << " for gain " << gain << " Channel:" << m_lar_on_id->channel_name(chanId) << endreq;
	    log<<MSG::WARNING<< "Set to 1.0." << endreq;
	  }
	  mphysOverMcal=1.0;
	}


	float hvCorr = hvScaleCorrs->HVScaleCorr(chanId);

	if (hvCorr <= 0){
          log<<MSG::WARNING<<" negative HVScaleCorr " << hvCorr << endreq;
          log<<MSG::WARNING<< m_lar_on_id->show_to_string(it.channelId()) <<endreq;
	  continue ;
	}


	float f = uA2MeV*DAC2uA*ramp[1]/mphysOverMcal*hvCorr ; 
	t.m_escale = f;

	t.m_rampRatio = 0.;

	if(m_applyRampIntercept){
	  
	  float ofc_sum = 0.;
	  for(unsigned int i = 0; i<ofc_a.size();++i){
	    ofc_sum += ofc_a[i];
	  }

	  // HEC only gets rampRatio for low gain
	  if(m_lar_on_id->isHECchannel(chanId) && gain==2 && 
	     fabs(ramp[1])>std::numeric_limits<float>::epsilon() && 
	     fabs(ofc_sum)>std::numeric_limits<float>::epsilon())
	    t.m_rampRatio = ramp[0]/ramp[1]/ofc_sum;
	  else if(gain!=0 && 
	     fabs(ramp[1])>std::numeric_limits<float>::epsilon() && 
	     fabs(ofc_sum)>std::numeric_limits<float>::epsilon())
	    t.m_rampRatio = ramp[0]/ramp[1]/ofc_sum;

	  log << MSG::DEBUG << "ramps (ofcsum,ramp[0],ramp[1],ramp[0]/ramp[1]/ofcsum,ped-ramp[0]/ramp[1]/ofcsum): " << endreq;
	  log << MSG::DEBUG << "       " << ofc_sum << "," << ramp[0] << "," << ramp[1] << "," << t.m_rampRatio << "," << t.m_pedestal-t.m_rampRatio << endreq;
	}

	for(unsigned int i = 0; i<ofc_a.size();++i)
	{
	  t.m_fxofc_a.push_back(f*ofc_a[i]); 
	  t.m_fxofc_b.push_back(f*ofc_b[i]); 
	  t.m_shape.push_back(float(shape[i])/f);
	}

	m_calibCont->setPdata(chanId,t,gain); 

	if (gain==0) {
	  // get information for energy calculation
	  LArOnlEnergyData e;

	  // get geometry info (real eta phi information)
	  Identifier offline_id = m_cablingService->cnvToIdentifier(chanId);
	  const CaloDetDescrElement* elem = theCaloDDM->get_element(offline_id);
	  float eta = elem->eta();
	  float phi = elem->phi();
	  log<<MSG::DEBUG<<" eta phi = "<<eta<< " " <<phi <<std::endl;

	  float v, costheta, sinthetacosphi, sinthetasinphi;
	  v = 1./cosh(eta); // sintheta
	  costheta=tanh(eta);
	  sinthetacosphi=v*cos(phi);
	  sinthetasinphi=v*sin(phi);
	  e.m_geoweight.push_back(costheta);
	  e.m_geoweight.push_back(sinthetacosphi);
	  e.m_geoweight.push_back(sinthetasinphi);
	  
	  // Trigger Tower Indentifier
	  //	  HWIdentifier onttid = m_ttService->whichTTChannelID( chanId ) ; // online TT ID
	  //	  Identifier offttid = m_ttService->cnvToIdentifier(onttid);      // offline TT ID
	  Identifier offttid = m_ttService->whichTTID(offline_id);

	  std::stringstream aux;
	  //	  aux << onttid;
	  aux << offttid;
	  e.m_ttid=aux.str();
	  log << MSG::DEBUG << " ttid = " << aux.str() << std::endl;

	  //default values	  
	  float th_timechi2 = 250.;
	  float th_out5sample = 1000.;
	  if (dspThresh) {
	    th_timechi2=dspThresh->tQThr(chanId);
	    th_out5sample=dspThresh->samplesThr(chanId);
	  }
	  e.m_threshold.push_back(th_timechi2);
	  e.m_threshold.push_back(th_out5sample);

	  m_energyCont->setPdata(chanId,e); 
	  //std::cout << "FEB: " << std::hex << febId << std::dec << ", Chan:" << chan <<" ths=" << th_timechi2 << " " << th_out5sample << std::endl;
	}
	
	++n ;
    }
  }

  // Test iterator in LArConditionsContainer
  /*
  int barrel_ec = 0;
  int pos_neg = 0;
  int feedthrough = 20;
  int slot = 4;
  HWIdentifier febHWId = m_calibCont->onlineHelper()->feb_Id(barrel_ec, pos_neg, feedthrough, slot);

  //  std::vector<LArOnlCalibDataContainer::FebId> febIds;
  //  febIds.push_back(febHWId.get_compact());
  
  //  slot = 3;
  //  febHWId = m_calibCont->onlineHelper()->feb_Id(barrel_ec, pos_neg, feedthrough, slot);
  //  febIds.push_back(febHWId.get_compact());

  for (unsigned int gain = 0; gain<3;++gain) {
    log<<MSG::INFO<< " gain = "<< gain << endreq;
    LArOnlCalibDataContainer::ConstConditionsMapIterator it = m_calibCont->begin(gain, febHWId);
    //    LArOnlCalibDataContainer::ConstConditionsMapIterator it = m_calibCont->begin(gain, febIds);
    //    LArOnlCalibDataContainer::ConstConditionsMapIterator it = m_calibCont->begin(gain);
    LArOnlCalibDataContainer::ConstConditionsMapIterator it_end = m_calibCont->end(gain);
    
    int n = 0;
    for(; it != it_end; it++) {
      log<<MSG::INFO<< " number of entry "<< (n+1) << endreq;
      LArOnlCalibData data = (*it);
      
      if(!data.isEmpty()) {
	log << MSG::INFO << "Ped = " << data.m_pedestal << endreq;
	log << MSG::INFO << "OfcA = " << data.m_fxofc_a[0] << endreq;
	log << MSG::INFO << "OfcB = " << data.m_fxofc_b[0] << endreq;
	log << MSG::INFO << "Shape = " << data.m_shape[0] << endreq;
      }
      n++;
    }
  }
  */
  log << MSG::INFO << " Number of payload created " << n << endreq;
  return StatusCode::SUCCESS; 
}

StatusCode LArOnlDbPrepAlg::getXML(const std::vector<unsigned int>& febIds, std::string& XMLoutputString){

  MsgStream log( msgSvc(), name() );

  std::stringstream XMLstream;

  std::vector<unsigned int>::const_iterator febit = febIds.begin();
  std::vector<unsigned int>::const_iterator febit_end = febIds.end();
  for(; febit != febit_end; febit++) {
    unsigned int febId = (*febit);
    log << MSG::DEBUG << " febId = "<< febId << endreq;

    XMLstream << "          <feb febId='" << febId 
	      << "' barrel_ec='" <<  m_calibCont->onlineHelper()->barrel_ec(HWIdentifier(febId)) 
	      << "' pos_neg='" << m_calibCont->onlineHelper()->pos_neg(HWIdentifier(febId)) 
	      << "' feedthrough='" << m_calibCont->onlineHelper()->feedthrough(HWIdentifier(febId)) 
	      << "' slot='" << m_calibCont->onlineHelper()->slot(HWIdentifier(febId)) << "'>" 
	      << std::endl;

    XMLstream << "          <LArOnlCalibDataContainer>" << std::endl;

    for (unsigned int gain = 0; gain<3;++gain) {
      log << MSG::DEBUG << " gain = "<< gain << endreq;
      XMLstream << "            <data gain='" << gain << "'>" << std::endl;

      LArOnlCalibDataContainer::ConstConditionsMapIterator it = m_calibCont->begin(gain, HWIdentifier(febId));
      LArOnlCalibDataContainer::ConstConditionsMapIterator it_end = m_calibCont->end(gain);
    
      int n = 0;
      for(; it != it_end; it++) {
	log << MSG::DEBUG << " number of entries = "<< (n+1) << endreq;
	LArOnlCalibData data = (*it);
	
	if(!data.isEmpty()) {
	  log << MSG::DEBUG << "Chan = " << it.getChannel() << endreq;
	  log << MSG::DEBUG << "Ped = " << data.m_pedestal << endreq;
	  log << MSG::DEBUG << "Escale = " << data.m_escale << endreq;
	  log << MSG::DEBUG << "OfcA = " << data.m_fxofc_a[0] << endreq;
	  log << MSG::DEBUG << "OfcB = " << data.m_fxofc_b[0] << endreq;
	  log << MSG::DEBUG << "Shape = " << data.m_shape[0] << endreq;
	  log << MSG::DEBUG << "RampRatio = " << data.m_rampRatio << endreq;

	  XMLstream << "              <condition channelID='" << it.getChannel()
		    << std::fixed << "' vPed='" << data.m_pedestal << " "
		    << "' vEscale='" << data.m_escale << " "
		    << "' vOfcA='";

	  std::vector<float>::const_iterator vecit = data.m_fxofc_a.begin();
	  std::vector<float>::const_iterator vecit_end = data.m_fxofc_a.end();
	  for(; vecit != vecit_end; vecit++) {
	    float OfcA = (*vecit);
	    XMLstream << std::fixed << OfcA << " "; 
	  }

	  XMLstream << "' vOfcB='";

	  vecit = data.m_fxofc_b.begin();
	  vecit_end = data.m_fxofc_b.end();
	  for(; vecit != vecit_end; vecit++) {
	    float OfcB = (*vecit);
	    XMLstream << std::fixed << OfcB << " "; 
	  }

	  XMLstream << "' vShape='";

	  vecit = data.m_shape.begin();
	  vecit_end = data.m_shape.end();
	  for(; vecit != vecit_end; vecit++) {
	    float sh = (*vecit);
	    XMLstream << std::fixed << sh << " "; 
	  }

	  XMLstream << "' vRampRatio='";

	  XMLstream << std::fixed << data.m_rampRatio << " "; 
	  
	  XMLstream << "' />" << std::endl; 

	}

	n++;
      }

      XMLstream << "            </data>" << std::endl;
    }

    XMLstream << "          </LArOnlCalibDataContainer>" << std::endl;
    XMLstream << "          <LArOnlEnergyDataContainer>" << std::endl;

    XMLstream << "            <data gain='0'>" << std::endl;

    LArOnlEnergyDataContainer::ConstConditionsMapIterator it = m_energyCont->begin(0, HWIdentifier(febId));
    LArOnlEnergyDataContainer::ConstConditionsMapIterator it_end = m_energyCont->end(0);

    int n = 0;
    for(; it != it_end; it++) {
      log << MSG::DEBUG << " number of entries = "<< (n+1) << endreq;
      LArOnlEnergyData data = (*it);
	
      if(!data.isEmpty()) {

	log << MSG::DEBUG << "Chan = " << it.getChannel() << endreq;
	log << MSG::DEBUG << "Threshold = " << data.m_threshold[0] << endreq;
	log << MSG::DEBUG << "GeoWeight = " << data.m_geoweight[0] << endreq;
	log << MSG::DEBUG << "TTId = " << data.m_ttid << endreq;

	XMLstream << "              <geometry channelID='" << it.getChannel()
		  << "' vThreshold='";

	  std::vector<float>::const_iterator vecit = data.m_threshold.begin();
	  std::vector<float>::const_iterator vecit_end = data.m_threshold.end();
	  for(; vecit != vecit_end; vecit++) {
	    float thresh = (*vecit);
	    XMLstream << std::fixed << thresh << " "; 
	  }

	  XMLstream << "' vGeoWeight='";

	  vecit = data.m_geoweight.begin();
	  vecit_end = data.m_geoweight.end();
	  for(; vecit != vecit_end; vecit++) {
	    float geoW = (*vecit);
	    XMLstream << std::fixed << geoW << " "; 
	  }

	  XMLstream << "' vTTId='" << data.m_ttid << "' />" << std::endl;

      }

      n++;
    }
    XMLstream << "            </data>" << std::endl
	      << "          </LArOnlEnergyDataContainer>" << std::endl;
    XMLstream << "          </feb>" << std::endl;
  }

  //std::cout << XMLstream.str() << std::endl;

  XMLoutputString = XMLstream.str();

  return StatusCode::SUCCESS;

}

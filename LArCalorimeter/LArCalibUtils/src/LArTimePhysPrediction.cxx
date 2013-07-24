/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArTimePhysPrediction.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArRawConditions/LArCaliWaveContainer.h"
#include "LArRawConditions/LArPhysCaliTdiffComplete.h"
#include "LArRawConditions/LArWaveHelper.h"
#include "CaloDetDescr/CaloDepthTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/IToolSvc.h"
#include <TMath.h>

typedef LArCaliWaveContainer::ConstConditionsMapIterator CaliCellIt;
typedef LArCaliWaveContainer::LArCaliWaves::const_iterator CaliWaveIt;

LArTimePhysPrediction::LArTimePhysPrediction (const std::string& name, ISvcLocator* pSvcLocator) : 
  Algorithm(name, pSvcLocator),
  m_thistSvc(0),
  m_CaloDepthTool(0),
  m_nt(0)  
{
  declareProperty("KeyInput", m_keyinput="LArCaliWave");
  declareProperty("KeyOutput", m_keyoutput="LArPhysCaliTdiff");
  declareProperty("vLCalib_EMB",m_vLCalib_EMB);
  declareProperty("vEtaMin_EMB",m_vEtaMin_EMB);
  declareProperty("vEtaMax_EMB",m_vEtaMax_EMB);
  declareProperty("vLCalib_EMEC",m_vLCalib_EMEC = 0);
  declareProperty("vLCalib_HEC",m_vLCalib_HEC = 0);
  declareProperty("vLSignal_EMB",m_vLSignal_EMB);
  declareProperty("vLSignal_EMEC",m_vLSignal_EMEC = 0);
  declareProperty("vLSignal_HEC",m_vLSignal_HEC = 0);
  declareProperty("vLSignal_FCAL",m_vLSignal_FCAL);
  declareProperty("vDeltaTTC_EMB",m_vDeltaTTC_EMB);
  declareProperty("vDeltaTTC_EC",m_vDeltaTTC_EC);
  declareProperty("vDeltaTTC_ECC_SPEC",m_vDeltaTTC_ECC_SPEC);
  declareProperty("vDeltaTTC_ECA_SPEC",m_vDeltaTTC_ECA_SPEC);
  declareProperty("sCalib",m_sCalib=5.5);
  declareProperty("sSignal",m_sSignal=5.5);
  declareProperty("sPig",m_sPig=5.5);
  declareProperty("sLTP",m_sLTP=5.5);
  declareProperty("nchannels_max",m_nchannels_max=1000000);
  declareProperty("GroupingType",m_groupingType="SubDetector");
}

LArTimePhysPrediction::~LArTimePhysPrediction() 
{}

StatusCode LArTimePhysPrediction::initialize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "LArTimePhysPrediction in initialize()" << endreq ;
  StatusCode sc;  
  
  //Initialize histograms
  sc = service("THistSvc", m_thistSvc);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to retrieve pointer to THistSvc" << endreq;
    return sc;
  }
  
  //Initialize ntuples
  NTupleFilePtr file1(ntupleSvc(),"/NTUPLES/FILE1");
  if (!file1){
    log  << MSG::ERROR << "Could not get NTupleFilePtr: failed" << endreq;
    return StatusCode::FAILURE;
  }
  NTuplePtr nt(ntupleSvc(),"/NTUPLES/FILE1/MyNtuple");
  if (!nt) {
    nt=ntupleSvc()->book("/NTUPLES/FILE1/MyNtuple",CLID_ColumnWiseTuple,"Timing ntuple");
  }
  if (!nt){
    log  << MSG::ERROR << "Booking of NTuple failed" << endreq;
    return StatusCode::FAILURE; 
  }
  
  m_nt=nt;
    
  //Book leaves 
  sc=nt->addItem("Chid",m_Chid);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem Chid failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("Channel",m_Channel);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem Channel failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("CalibLine",m_CalibLine);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem CalibLine" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("is_lar_em",m_is_lar_em);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem is_lar_em failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("is_lar_hec",m_is_lar_hec);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem is_lar_hec failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("is_lar_fcal",m_is_lar_fcal);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem is_lar_fcal failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("pos_neg",m_pos_neg);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem pos_neg failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("barrel_ec",m_barrel_ec);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem barrel_ec failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("FT",m_FT);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem FT failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("FEBid",m_FEBid);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem FEBid failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("slot",m_slot);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem slot failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("eta",m_eta);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem eta failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("phi",m_phi);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem phi failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("layer",m_layer);
 if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem layer failed" << endreq;
    return StatusCode::FAILURE;
  }
 sc=nt->addItem("real_eta",m_real_eta);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem real_eta failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("real_phi",m_real_phi);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem real_phi failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("t0",m_t0);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem t0 failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("tcali",m_tcali);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem tcali failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("tCalibPredicted",m_tCalibPredicted);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem tCalibPredicted failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("CalibCables",m_CalibCables);
  if (sc!=StatusCode::SUCCESS) {
   log << MSG::ERROR << "addItem CalibCables failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("SignalCables",m_SignalCables);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem SignalCables failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("TOF",m_TOF);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem TOF failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("DeltaTTC",m_DeltaTTC);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem DeltaTTC failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("tPhysPredicted",m_tPhysPredicted);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "addItem tPhysPredicted failed" << endreq;
    return StatusCode::FAILURE;
  }
  
 return StatusCode::SUCCESS;
}

StatusCode LArTimePhysPrediction::stop() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "LArTimePhysPrediction in stop()" << endreq ;
  StatusCode sc;
  //Intermediate variables declaration (should be removed in an updated version)
  const double meter2ns = 3.33564095;
  int Channel;
  int CalibLine;
  double tphys;
  double tcali;
  double dt;
  double TOF;
  double CalibCables=0;
  double LSignalFCAL=0;
  double DeltaTTC;
  double t0;
  int barrel_ec;
  int layer;
  int eta;
  int phi;  
  int FT;  
  int slot;
  int pos_neg;
  double real_eta;
  double real_phi;
  double radius=0;
  
  //DetectorStore
  StoreGateSvc* detStore; 
  sc = service("DetectorStore",detStore);
  if (sc != StatusCode::SUCCESS) {
    log << MSG::ERROR << "Cannot get DetectorStore!" << endreq;
    return sc;
  }

  //Retrieve the LArCaliwaveContainer
  const LArCaliWaveContainer* larCaliWaveContainer;    
  sc = detStore->retrieve(larCaliWaveContainer, m_keyinput);
  if (sc.isFailure()) {
    log << MSG::WARNING << "Cannot read LArCaliWaveContainer from StoreGate with key = " << m_keyinput << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Loaded LArCaliWaveContainer with key = " << m_keyinput << endreq;
  }

  //Create the LArPhysCaliTdiffComplete object
  LArPhysCaliTdiffComplete *larPhysCaliTdiffComplete = new LArPhysCaliTdiffComplete();
  sc=larPhysCaliTdiffComplete->setGroupingType(m_groupingType,log);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Failed to set groupingType for LArPhysCaliTdiffComplete object" << endreq;
    return sc;
  }
  
  sc=larPhysCaliTdiffComplete->initialize(); 
  if (sc.isFailure()) {
    log << MSG::ERROR << "Failed to initialize LArPhysCaliTdiffComplete object" << endreq;
    return sc;
  }

  //Pointer to Tool Service
  IToolSvc* p_toolSvc = 0;
  sc = service("ToolSvc", p_toolSvc);
  if (sc.isFailure()) {
    log << MSG::FATAL << " Tool Service not found " << endreq;
    return StatusCode::FAILURE;
  } else {
    
    //Create CaloDepthTool
    IAlgTool* algTool;
    sc = p_toolSvc->retrieveTool("CaloDepthTool", algTool, this);
    
    if (sc.isFailure()) {
      log << MSG::FATAL
	  << "Unable to find tool for CaloDepthTool"
	  << endreq; 
      return sc;
    }
    else 
      {
	m_CaloDepthTool=dynamic_cast<CaloDepthTool*>(algTool);
	log << MSG::INFO << "CaloDepthTool retrieved with name " << m_CaloDepth << endreq;
      }
  }
  
  //LArCablingSvc
  ToolHandle<LArCablingService> larCablingSvc("LArCablingService");
  sc = larCablingSvc.retrieve();
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << " Can't get LArCablingSvc " << endreq;
    return sc;
  }
  
  //Define helpers
  LArWaveHelper larWaveHelper;
  
  const LArOnlineID* m_onlineHelper;
  sc = detStore->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Can't get LArOnlineID helper " << endreq;
    return StatusCode::FAILURE;
  }
  
  //Calo DDM gives "detector description"
  //including real positions of cells
  const CaloDetDescrManager* caloDDM = CaloDetDescrManager::instance() ;
  if ( ! caloDDM->isInitialized() )
    {
      log << MSG::ERROR
	  << "CaloDetDescrManager is not initialized, module unusable!"
	  << endreq;
      return false;
    }
    
  //const CaloCell_ID* m_caloCID = m_caloDDM->getCaloCell_ID();
  const CaloIdManager *m_caloIdMgr=CaloIdManager::instance() ;
  
  //Get identifiers
  const LArEM_ID* m_emId;
  m_emId=m_caloIdMgr->getEM_ID();
  const LArHEC_ID* m_hecId;
  m_hecId=m_caloIdMgr->getHEC_ID();
  const LArFCAL_ID* m_fcalId; 
  m_fcalId=m_caloIdMgr->getFCAL_ID();
  
  //------------------------------------------------------------------------------------------------------------------------------
  //--------------Start to loop on the LArCaliWaveContainer------------------------------------------------------------------------
    for ( unsigned gain_it = CaloGain::LARHIGHGAIN ; gain_it < CaloGain::LARNGAIN ; ++gain_it ) { // Gains
      
      CaliCellIt cell_it = larCaliWaveContainer->begin(gain_it) ;
      CaliCellIt cell_it_e = larCaliWaveContainer->end(gain_it) ;
      
      if ( cell_it == cell_it_e ) {
	log << MSG::DEBUG << "LArCaliWaveContainer (key = " << m_keyinput << ") has no wave with gain = " << gain_it << endreq;
	continue;
      } else {
	log << MSG::INFO << "Processing LArCaliWaveContainer (key = " << m_keyinput << ") in gain = " << gain_it << endreq;
      }
      
      //counters for channels and waves
      int nchannels = 0;
      unsigned nwaves    = 0;
      
      for ( ; cell_it != cell_it_e; ++cell_it) { // Channels
	
	if(nchannels==m_nchannels_max){continue;}
	std::cout << "nchannels " << nchannels << std::endl;
	nchannels++;

	CaliWaveIt wave_it = cell_it->begin();
	CaliWaveIt wave_it_e = cell_it->end();
	if ( wave_it == wave_it_e ) {
	  log << MSG::DEBUG << "Empty channel found..." << endreq ;
	  continue; // skip empty channels
	} 
      
	//use HWIdentifier
	HWIdentifier chid = cell_it.channelId();

	Identifier id;
	// Calo DDM gives "detector description"
	// including real positions of cells
	caloDDM = CaloDetDescrManager::instance() ;
	const CaloCell_ID* m_caloCID = caloDDM->getCaloCell_ID();
	
	try {
	  id = larCablingSvc->cnvToIdentifier(chid);   
	} catch ( LArID_Exception ) {
	  log << MSG::ERROR << "LArCablingSvc exception caught for channel " << MSG::hex << chid << MSG::dec << endreq ;
	  continue;
	}

	Channel = m_onlineHelper->channel(chid);

	const std::vector<HWIdentifier>& calibLineV = larCablingSvc->calibSlotLine(chid);
        std::vector<HWIdentifier>::const_iterator calibLineIt = calibLineV.begin();   
	CalibLine = m_onlineHelper->channel(*calibLineIt) ;
	
	//indexes eta/phi/layer
	if (m_emId->is_lar_em(id)) {
	  eta=m_emId->eta(id); 
	  phi=m_emId->phi(id);
	  layer=m_emId->sampling(id);}
	else if (m_hecId->is_lar_hec(id)) {
	  eta=m_hecId->eta(id); 
	  phi=m_hecId->phi(id);
	  layer=m_hecId->sampling(id);}
	else if (m_fcalId->is_lar_fcal(id)) {
	  eta=m_fcalId->eta(id); 
	  phi=m_fcalId->phi(id);
	  layer=m_fcalId->module(id);}
	else { log << MSG::INFO << "cell not in the calorimeters " << endreq; continue;}
	  
	//identification using the online helper (common to all LAr calorimeters)
	pos_neg = m_onlineHelper->pos_neg(chid);
	barrel_ec = m_onlineHelper->barrel_ec(chid);
	FT   = m_onlineHelper->feedthrough(chid);
	slot = m_onlineHelper->slot(chid);

	//get the FT online identifier
	HWIdentifier febid = m_onlineHelper->feb_Id(chid);
	m_FEBid = febid.get_identifier32().get_compact();
	
	//real eta and phi: need the hash identifier
	IdentifierHash theHash = m_caloCID->calo_cell_hash(id) ;
	const CaloDetDescrElement* theDDE = caloDDM->get_element(theHash) ;
	
	if(theDDE==0) {
	  log << MSG::INFO << "CellIndex =  " << theHash << " has a DDE pointer NULL " << endreq;
	  continue;
	}
	
	real_eta = theDDE->eta();
	real_phi = theDDE->phi();
	CaloCell_ID::CaloSample sample = theDDE->getSampling();//ok for EMB and EMEC
	//retrieve/compute the shower depth
	//use the "best" available parametrisation of the shower depth at the time of development
	//WARNING: use the CaloDepthTool's convention radius=r(barrel), radius=z(end-cap)
	//for HEC and FCAL: lengths could be moved in the job options
	if(m_emId->is_lar_em(id)){
	  radius = m_CaloDepthTool->cscopt2_parametrized(sample,real_eta,real_phi);
	}
	else if(m_hecId->is_lar_hec(id)){//assumption: "arrival point" = middle of the compartment 
	  if(layer==0) radius=4398.;
	  if(layer==1) radius=4806.;
	  if(layer==2) radius=5359.;
	  if(layer==3) radius=5840.;
	}
	else if(m_fcalId->is_lar_fcal(id)){//assumption: "arrival point" = middle of the compartment 
	  if(layer==1) {radius=4916.;LSignalFCAL=m_vLSignal_FCAL[0];}
	  if(layer==2) {radius=5366.;LSignalFCAL=m_vLSignal_FCAL[1];}
	  if(layer==3) {radius=5816.;LSignalFCAL=m_vLSignal_FCAL[2];}
	}
	
	nwaves += cell_it->size();
	for ( ; wave_it != wave_it_e; ++wave_it) { // DACs <==> iterator = the caliwave
	  
	  //initialize tphys
	  tphys=-999.;

	  //----step 1: get the calibration time
	  dt = wave_it->getDt();
	  tcali  = dt * larWaveHelper.getMax(*wave_it);
	  //modified version of getT0
	  const unsigned Nbase=5;
	  double rT0=0.;
	  double asamp1=0.;
	  double asamp2=0.;
	  double base = larWaveHelper.getBaseline(*wave_it,Nbase);
	  double amax = larWaveHelper.getMaxAmp(*wave_it);
	  unsigned int imax=larWaveHelper.getMax(*wave_it);
	  
	  for (unsigned int i=1; i<imax ; i++) {
	    asamp1=wave_it->getSample(i);
	    asamp2=wave_it->getSample(i-1);
	    if((asamp1-base)>amax*0.10 && (asamp2-base)<amax*0.10)
	      rT0 = i-1+(0.01*amax-(asamp2-base))/(asamp1-asamp2);
	  }
	  t0=rT0;
	  	  
	  //----step 2: compute the TOF
	  if(m_emId->is_lar_em(id) && barrel_ec==0) TOF = fabs(radius)*TMath::CosH(real_eta)* meter2ns/1000;//EMB
	  else TOF = fabs(radius)/TMath::TanH(fabs(real_eta))* meter2ns/1000;//EC
	  
	  //----step 3: deduction of the propagation time due to the propagation of the calibration pulse
	  //from the pulse until the calibration board
	  if(m_emId->is_lar_em(id) && barrel_ec==0){//EMB
	    for(int ieta=0;ieta<16;ieta++){
	      if(fabs(real_eta)>ieta*0.1 && fabs(real_eta)<(ieta+1)*0.1) CalibCables=0.7*m_sPig+m_vLCalib_EMB[layer][ieta]*m_sCalib; 
	    }
	  }
	  else if(m_emId->is_lar_em(id) && abs(barrel_ec)==1) CalibCables=0.9*m_sPig+m_vLCalib_EMEC*m_sCalib;//EMEC
	  else if(m_hecId->is_lar_hec(id)) CalibCables=m_vLCalib_HEC*m_sCalib;//HEC
	  else if(m_fcalId->is_lar_fcal(id)) CalibCables=-LSignalFCAL*m_sSignal;//FCAL
	  	  	  
	  //----step 4: deduction of the propagation times due to the optical fibers from USA15 to FEC
	  if(m_emId->is_lar_em(id) && barrel_ec==0){
	    if(pos_neg==1) DeltaTTC=m_vDeltaTTC_EMB[1][FT];//EMBA
	    else DeltaTTC=m_vDeltaTTC_EMB[0][FT];//EMBC 
	  }
	  else {
	    if(real_eta>0) DeltaTTC=m_vDeltaTTC_EC[1][FT];//ECA
	    else DeltaTTC=m_vDeltaTTC_EC[0][FT];//ECC
	    //correction for special crates
	    //A FTs 2/3
	    if(m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==2 && real_eta>0 && slot<11) DeltaTTC=m_vDeltaTTC_ECA_SPEC[0][0];
	    if((m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==2 && real_eta>0 && slot>10) || (m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==3 && real_eta>0 && slot<3)) DeltaTTC=m_vDeltaTTC_ECA_SPEC[0][1];
	    if(m_hecId->is_lar_hec(id) && abs(barrel_ec)==1 && FT==3 && real_eta>0) DeltaTTC=m_vDeltaTTC_ECA_SPEC[0][2];
	    //A FTs 9/10
	    if(m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==9 && real_eta>0 && slot<11) DeltaTTC=m_vDeltaTTC_ECA_SPEC[1][0];
	    if((m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==9 && real_eta>0 && slot>10) || (m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==10 && real_eta>0 && slot<3)) DeltaTTC=m_vDeltaTTC_ECA_SPEC[1][1];
	    if(m_hecId->is_lar_hec(id) && abs(barrel_ec)==1 && FT==10 && real_eta>0) DeltaTTC=m_vDeltaTTC_ECA_SPEC[1][2];
	    //A FTs 15/16
	    if(m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==15 && real_eta>0 && slot<11) DeltaTTC=m_vDeltaTTC_ECA_SPEC[2][0];
	    if((m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==15 && real_eta>0 && slot>10) || (m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==16 && real_eta>0 && slot<3)) DeltaTTC=m_vDeltaTTC_ECA_SPEC[2][1];
	    if(m_hecId->is_lar_hec(id) && abs(barrel_ec)==1 && FT==16 && real_eta>0) DeltaTTC=m_vDeltaTTC_ECA_SPEC[2][2];
	    //A FTs 21/22
	    if(m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==21 && real_eta>0 && slot<11) DeltaTTC=m_vDeltaTTC_ECA_SPEC[3][0];;
	    if((m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==21 && real_eta>0 && slot>10) || (m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==21 && real_eta>0 && slot<3)) DeltaTTC=m_vDeltaTTC_ECA_SPEC[3][1];
	    if(m_hecId->is_lar_hec(id) && abs(barrel_ec)==1 && FT==22 && real_eta>0) DeltaTTC=m_vDeltaTTC_ECA_SPEC[3][2];
	    //C FTs 2/3
	    if(m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==2 && real_eta<0 && slot<11) DeltaTTC=m_vDeltaTTC_ECC_SPEC[0][0];
	    if((m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==2 && real_eta<0 && slot>10) || (m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==3 && real_eta<0 && slot<3)) DeltaTTC=m_vDeltaTTC_ECC_SPEC[0][1];
	    if(m_hecId->is_lar_hec(id) && abs(barrel_ec)==1 && FT==3 && real_eta>0) DeltaTTC=m_vDeltaTTC_ECC_SPEC[0][2];
	    //C FTs 9/10
	    if(m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==9 && real_eta<0 && slot<11) DeltaTTC=m_vDeltaTTC_ECC_SPEC[1][0];
	    if((m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==9 && real_eta<0 && slot>10) || (m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==10 && real_eta<0 && slot<3)) DeltaTTC=m_vDeltaTTC_ECC_SPEC[1][1];
	    if(m_hecId->is_lar_hec(id) && abs(barrel_ec)==1 && FT==10 && real_eta<0) DeltaTTC=m_vDeltaTTC_ECC_SPEC[1][2];
	    //C FTs 15/16
	    if(m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==15 && real_eta<0 && slot<11) DeltaTTC=m_vDeltaTTC_ECC_SPEC[2][0];
	    if((m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==15 && real_eta<0 && slot>10) || (m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==16 && real_eta<0 && slot<3)) DeltaTTC=m_vDeltaTTC_ECC_SPEC[2][1];
	    if(m_hecId->is_lar_hec(id) && abs(barrel_ec)==1 && FT==16 && real_eta<0) DeltaTTC=m_vDeltaTTC_ECC_SPEC[2][2];
	    //C FTs 21/22
	    if(m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==21 && real_eta<0 && slot<11) DeltaTTC=m_vDeltaTTC_ECC_SPEC[3][0];
	    if((m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==21 && real_eta<0 && slot>10) || (m_emId->is_lar_em(id) && abs(barrel_ec)==1 && FT==21 && real_eta<0 && slot<3)) DeltaTTC=m_vDeltaTTC_ECC_SPEC[3][1];
	    if(m_hecId->is_lar_hec(id) && abs(barrel_ec)==1 && FT==22 && real_eta<0) DeltaTTC=m_vDeltaTTC_ECC_SPEC[3][2];
	  }				     
	  
  	  //----step 5: computation of tphys
	  //tphys=tcali-CalibCables+TOF-DeltaTTC;
	  //add the effect from LTP cables: LTPI->LTP and LTP->LTP (hard coded)
	  //for A-C sides difference: a 8 ns is set (derived from September 2008 splash events)
	  if(pos_neg==1 && m_emId->is_lar_em(id) && abs(barrel_ec)==0) tphys=tcali-CalibCables+TOF+DeltaTTC+8+0.3*m_sLTP;//EMBA
	  else if(pos_neg==0 && m_emId->is_lar_em(id) && abs(barrel_ec)==0) tphys=tcali-CalibCables+TOF+DeltaTTC+0.3*m_sLTP;//EMBC
	  else if(pos_neg==1 && m_emId->is_lar_em(id) && abs(barrel_ec)==1) tphys=tcali-CalibCables+TOF+DeltaTTC+8;//EMECA
	  else if(real_eta>0 && (m_emId->is_lar_hec(id)||m_emId->is_lar_fcal(id))) tphys=tcali-CalibCables+TOF+DeltaTTC+8+0.3*m_sLTP;//HEC,FCAL A
	  else if(real_eta<0 && (m_emId->is_lar_hec(id)||m_emId->is_lar_fcal(id))) tphys=tcali-CalibCables+TOF+DeltaTTC+0.3*m_sLTP;//HEC,FCAL C 
	  else tphys=tcali-CalibCables+TOF+DeltaTTC;//EMECC
	  
	  //prediction of the expected calibration time
	  //not finalized (EMEC signal cables lengths not implemented)
	  if(m_emId->is_lar_em(id) && abs(barrel_ec)==0){//EMB
	    for(int ieta=0;ieta<16;ieta++){
	      if(fabs(real_eta)>ieta*0.1 && fabs(real_eta)<(ieta+1)*0.1 && (layer==0 || (layer==1 && fabs(real_eta)<0.6))) m_SignalCables=0.9*m_sPig+m_vLSignal_EMB[layer][ieta]*m_sSignal;
	      else if(fabs(real_eta)>ieta*0.1 && fabs(real_eta)<(ieta+1)*0.1) m_SignalCables=0.7*m_sPig+m_vLSignal_EMB[layer][ieta]*m_sSignal;
	    }
	  }
	  else if(m_emId->is_lar_em(id) && abs(barrel_ec)==1) m_SignalCables=0.9*m_sPig+m_vLSignal_EMEC*m_sSignal;//EMEC
	  else if(m_hecId->is_lar_hec(id)) m_SignalCables=m_vLSignal_HEC*m_sSignal;//HEC
	  else if(m_fcalId->is_lar_fcal(id)) m_SignalCables=0.;//FCAL
	  
	  m_tCalibPredicted=m_SignalCables+CalibCables+tcali-t0;
	  if(m_fcalId->is_lar_fcal(id))m_tCalibPredicted=tcali-t0;
	  
	  //fill larPhysCaliTdiffComplete (needed to fill the DB)
	  //larPhysCaliTdiffComplete->set(chid,gain_it,tphys);
	  
	  //for debugging purposes: check the computed times
	  /**std::cout << "for a given (GAIN-CHANNEL-DAC) " << std::endl;
	  std::cout << "chid " << chid << std::endl;
	  std::cout << "Channel " << Channel << std::endl;
	  std::cout << "CalibLine " << CalibLine << std::endl;
	  std::cout << "channel_eta " << eta << std::endl;
	  std::cout << "real_eta " << real_eta << std::endl;
	  std::cout << "channel_phi " << phi << std::endl;
	  std::cout << "real_phi " << real_phi << std::endl;
	  std::cout << "sample " << sample << std::endl;
	  std::cout << "layer " << layer << std::endl;
	  std::cout << "barrel_ec " << barrel_ec << std::endl;	  
	  std::cout << "pos_neg " << pos_neg << std::endl;	  
	  std::cout << "FT " << FT << std::endl;
	  std::cout << "FEBid " << m_FEBid << std::endl;
	  std::cout << "Slot " << slot << std::endl;
	  std::cout << "radius " << radius << std::endl;
	  std::cout << "tcali " << tcali << std::endl;
	  std::cout << "tcalipredicted " << m_tCalibPredicted << std::endl;
	  std::cout << "CalibCables " << CalibCables << std::endl;
	  std::cout << "Signal " << m_SignalCables << std::endl;
	  std::cout << "TOF " << TOF << std::endl;
	  std::cout << "DeltaTTC " << DeltaTTC << std::endl;
	  std::cout << "tphys " << tphys << std::endl;
	  std::cout << "t0 " << t0 << std::endl;
	  //time difference between calibration and physics can be easily computed:
	  //tdiff=tphys-tcali;
	  */
	  
	  //write the ntuple
	  if(m_emId->is_lar_em(id)) m_is_lar_em=1;
	  else m_is_lar_em=0;
	  if(m_emId->is_lar_hec(id)) m_is_lar_hec=1;
	  else m_is_lar_hec=0;
	  if(m_emId->is_lar_fcal(id)) m_is_lar_fcal=1;
	  else m_is_lar_fcal=0;
	  m_Chid=chid.get_identifier32().get_compact();
	  m_Channel=Channel;
	  m_CalibLine=CalibLine;
	  m_pos_neg=pos_neg;
	  m_barrel_ec=barrel_ec;
	  m_FT=FT;
	  m_slot=slot;
	  m_eta=eta;
	  m_phi=phi;
	  m_layer=layer;
	  m_real_eta=real_eta;
	  m_real_phi=real_phi;
	  m_t0=t0;
	  m_tcali=tcali;
	  m_CalibCables=CalibCables;
	  m_TOF=TOF;
	  m_DeltaTTC=DeltaTTC;
	  m_tPhysPredicted=tphys;
	  //m_SignalCables: already filled
	  //m_tCalibPredicted: already filled (computation to be finalized) 
	  
	  log << MSG::VERBOSE << "Try to write to ntuple " << endreq ;
	  sc=ntupleService()->writeRecord(m_nt);
	  if (sc!=StatusCode::SUCCESS) {
	    log << MSG::ERROR << "writeRecord failed" << endreq;
	    return StatusCode::FAILURE;
	  } else {
	    log << MSG::VERBOSE << "wave written to ntuple" << endreq ;
	  }

	  
	} // end of loop over DACs 	
	
      }// end of loop over Channels
      
    } // end of loop over Gains
    
    // Record LArPhysCaliTdiffComplete (needed to fill the DB)
    /**sc = detStore->record(larPhysCaliTdiffComplete,m_keyoutput);
       if (sc != StatusCode::SUCCESS) 
       {
       log << MSG::ERROR	
       << " Cannot store LArPhysCaliTdiffComplete in TDS " 
       << endreq;
       return sc;
       }
       // Make symlink
       sc = detStore->symLink(larPhysCaliTdiffComplete,(ILArPhysCaliTdiff*)larPhysCaliTdiffComplete);
       if (sc != StatusCode::SUCCESS) 
       {
       log << MSG::ERROR 
       << " Cannot make link for Data Object "
       << endreq;
       return sc;
       }*/ 
    
    //-----------------------------------end of the main loop-------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------
    
    log << MSG::INFO << "end of stop" << endreq;
    return StatusCode::SUCCESS ;
}

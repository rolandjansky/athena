/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCellRec/LArCellBuilderFromLArHitTool.h"

#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"

#include "LArRecEvent/LArCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloGain.h"
//#include "CaloUtils/CaloCellCorrection.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "xAODEventInfo/EventInfo.h"
//#include "EventInfo/EventID.h"


#include "AthenaKernel/IAtRndmGenSvc.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

#include "StoreGate/StoreGateSvc.h" 
#include "CLHEP/Units/SystemOfUnits.h"


using CLHEP::HepRandomEngine;
using CLHEP::RandGauss;
using CLHEP::GeV;


/////////////////////////////////////////////////////////////////// 

LArCellBuilderFromLArHitTool::LArCellBuilderFromLArHitTool(
				      const std::string& type, 
				      const std::string& name, 
				      const IInterface* parent)
  :AlgTool(type, name, parent),
    m_priority(1000), 
    m_eHitThreshold(-999999.), 
    m_ThresholdOnE(-999999.), 
    m_ThresholdOnAbsE(-999999.),
    m_ThresholdOnEinSigma(-999999.), 
    m_ThresholdOnAbsEinSigma(-999999.), 
    m_Windows(false), 
    m_WindowsEtaSize(0.4),  
    m_WindowsPhiSize(0.5),
    m_WithNoise(true),
    m_WithElecNoise(true),
    m_WithPileUpNoise(false),
    m_NoiseToolName("CaloNoiseTool/calonoisetool"),
    m_mcEventName("GEN_EVENT")
 
{
  declareInterface<ICaloCellMakerTool>(this); 
  // Priority of the IncidentListener (for "BeginRun")
  declareProperty("BeginRunPriority",m_priority); 


  // LAr Region of interest:
  declareProperty("LArRegion",m_LArRegion);

  // LAr Region of interest:
  declareProperty("LArHitContainers",m_HitContainers);

  //threshold on energy of the Hits
  declareProperty("eHitThreshold", m_eHitThreshold);

  // Energy thresholds (in MeV)
  declareProperty("ThresholdOnE",          m_ThresholdOnE);
  declareProperty("ThresholdOnAbsE",       m_ThresholdOnAbsE);
  // Energy thresholds in sigma electronic noise 
  declareProperty("ThresholdOnEinSigma",   m_ThresholdOnEinSigma);
  declareProperty("ThresholdOnAbsEinSigma",m_ThresholdOnAbsEinSigma);  

  // Windows 
  declareProperty("Windows",m_Windows);
  declareProperty("WindowsEtaSize",m_WindowsEtaSize);
  declareProperty("WindowsPhiSize",m_WindowsPhiSize);

  // switch on the noise (true by default)
  declareProperty("WithNoise",m_WithNoise);
  declareProperty("WithElecNoise",m_WithElecNoise);
  declareProperty("WithPileUpNoise",m_WithPileUpNoise);//only for checks
 
  // NoiseTool
  declareProperty("NoiseTool",m_NoiseToolName);

  // McEventCollection
  declareProperty("McEventName", m_mcEventName);

}


/////////////////////////////////////////////////////////////////// 

StatusCode LArCellBuilderFromLArHitTool::initialize()
{
  MsgStream log(msgSvc(), name());

  StoreGateSvc* detStore;
  if (service("DetectorStore", detStore).isFailure()) {
    log << MSG::ERROR   << "Unable to access DetectoreStore" << endreq ;
    return StatusCode::FAILURE;
  }

  const IGeoModelSvc *geoModel=0;
  StatusCode sc = service("GeoModelSvc", geoModel);
  if(sc.isFailure())
  {
    log << MSG::ERROR << "Could not locate GeoModelSvc" << endreq;
    return sc;
  }

  // dummy parameters for the callback:
  int dummyInt=0;
  std::list<std::string> dummyList;

  if (geoModel->geoInitialized())
  {
    return geoInit(dummyInt,dummyList);
  }
  else
  {
    sc = detStore->regFcn(&IGeoModelSvc::geoInit,
			  geoModel,
			  &LArCellBuilderFromLArHitTool::geoInit,this);
    if(sc.isFailure())
    {
      log << MSG::ERROR << "Could not register geoInit callback" << endreq;
      return sc;
    }
  }
  return sc;
}

StatusCode
LArCellBuilderFromLArHitTool::geoInit(IOVSVC_CALLBACK_ARGS)
{
  
  MsgStream log(msgSvc(), name());

  //
  m_WithMap = false ;
  //
  //m_WithMap = true;
  
  
  // initialize detector description pointers:

  // pointer to detector manager:
  // m_lar_dd_man = LArDetDescrManager::instance(); 

  m_calo_dd_man = CaloDetDescrManager::instance(); 

  m_caloCID = m_calo_dd_man->getCaloCell_ID();
  const CaloIdManager* caloCIM = m_calo_dd_man->getCalo_Mgr();
  m_emID = caloCIM->getEM_ID();
  m_hecID = caloCIM->getHEC_ID();
  m_fcalID = caloCIM->getFCAL_ID();


  if (m_LArRegion == "LAr_EM" || 
      m_LArRegion == "LAr_EM_Barrel" || 
      m_LArRegion == "LAr_EM_Endcap" ) 
    {     
      m_caloNum = CaloCell_ID::LAREM ;
      m_atlas_id = m_emID;
      log << MSG::INFO << " LAr Calo is em " << endreq ;
    }
  else if (m_LArRegion == "LAr_HEC")
    {
      m_caloNum = CaloCell_ID::LARHEC ;
      m_atlas_id = m_hecID;
      log << MSG::INFO << " LAr Calo is hec " << endreq ;
    }
  else if (m_LArRegion == "LAr_FCal") 
    {
      m_caloNum = CaloCell_ID::LARFCAL ;
      m_atlas_id = m_fcalID;
      log << MSG::INFO << " LAr Calo is FCAL " << endreq ;
    }  
  else
    {
      log << MSG::ERROR
	  << " cannot use region "  << m_LArRegion <<  endreq ;    
      m_atlas_id = 0;
      return StatusCode::FAILURE;
    }


  
// threshold on hit energy
  if (m_eHitThreshold==-999999.) {
    m_applyHitEnergyThreshold = false ;    
    log << MSG:: INFO << " No threshold on hit energy " << endreq ;
  } else
    {
      m_applyHitEnergyThreshold = false ;    
      log << MSG:: INFO << " Threshold on hit energy " << m_eHitThreshold << endreq ;
    }
  
  

//---- check the thresholds on E ----------------

  //(check there is only one selected threshold; else set none)
  
  int iThreshold[4]; 
  for(int i=0;i<4;++i) iThreshold[i]=0;  
  m_ThresholdSelectedNotInSigma=0;
  if(m_ThresholdOnE!=-999999.)          {iThreshold[0]=1;
                                         m_ThresholdSelectedNotInSigma=1;}  
  if(m_ThresholdOnAbsE!=-999999.)       {iThreshold[1]=1;
                                         m_ThresholdSelectedNotInSigma=1;} 
  if(m_ThresholdOnEinSigma!=-999999.)    iThreshold[2]=1; 
  if(m_ThresholdOnAbsEinSigma!=-999999.) iThreshold[3]=1; 
  m_ThresholdSelected=0; 
 
  for(int i=0;i<4;++i) 
  {  
    m_ThresholdSelected+=iThreshold[i];
  } 

  if(m_ThresholdSelected>=2) 
  {     
    m_ThresholdOnE=-999999.; 
    m_ThresholdOnAbsE=-999999.; 
    m_ThresholdOnEinSigma=-999999.; 
    m_ThresholdOnAbsEinSigma=-999999.; 
    m_ThresholdSelected=0;
    m_ThresholdSelectedNotInSigma=0;
 
    log << MSG::WARNING
	<<"More than one threshold was selected => none selected !" 
	<< endreq; 
  } 
  else 
  {     
    if(iThreshold[0]==1) 
      log<<MSG::INFO<<"THRESHOLD CHOICE : ThresholdOnE="
	 <<m_ThresholdOnE<<endreq;
    else 
    if(iThreshold[1]==1) 
      log<<MSG::INFO<<"THRESHOLD CHOICE : ThresholdOnAbsE="
	 <<m_ThresholdOnAbsE<<endreq;
    else 
    if(iThreshold[2]==1) 
      log<<MSG::INFO<<"THRESHOLD CHOICE : ThresholdOnEinSigma="
	 <<m_ThresholdOnEinSigma<<endreq;
    else
    if(iThreshold[3]==1) 
      log<<MSG::INFO<<"THRESHOLD CHOICE : ThresholdOnAbsEinSigma="
	 <<m_ThresholdOnAbsEinSigma<<endreq;
    else   
      log<<MSG::INFO<<"THRESHOLD CHOICE : No Threshold !"<<endreq;
  }

  if (m_ThresholdSelected==1) {
    // thresholds on cell are only implemented when using internal map
    log << MSG::INFO << " One threshold selected: use internal map" << endreq;
    
    m_WithMap = true ;
  }
  


 
//---- initialize the HitContainers ----------------

  //int nPool = 0 ;  
 
  if (m_LArRegion == "LAr_EM") 
     {       
        m_HitContainers.push_back("LArHitEMB");	
        m_HitContainers.push_back("LArHitEMEC");
        //nPool=175000;
	
     }
  else if (m_LArRegion == "LAr_EM_Barrel")
     {
        m_HitContainers.push_back("LArHitEMB");
        //nPool=150000;
     }
  else if (m_LArRegion == "LAr_EM_Endcap")
     {
        m_HitContainers.push_back("LArHitEMEC");
        //nPool=150000;
     }
  else if (m_LArRegion == "LAr_HEC")
     {
        m_HitContainers.push_back("LArHitHEC");
        //nPool=6000;
     }
  else if (m_LArRegion == "LAr_FCal")  
    {      
        m_HitContainers.push_back("LArHitFCAL");
        //nPool=3600;
    }  
  else if (m_LArRegion == "UserDefined" && (m_HitContainers.size()>0) )
     {
      log << MSG::INFO
	<< "Using user defined Region: hitContainers are "  <<  endreq;    
      log << MSG::INFO << " cannot simulate noise " << endreq ;
      
      m_WithNoise = false;
      m_WithMap = false;
      
      std::string names ; 
      for(unsigned int i=0; i < m_HitContainers.size(); ++i)
	names+=m_HitContainers[i]+" ";
      log << MSG::INFO<< names  <<  endreq;    
      //nPool=10000;
     }
  else
  {
      log << MSG::ERROR
	<< " Unknown Region: "  << m_LArRegion <<  endreq;    
      return StatusCode::FAILURE;
  }

//---- initialize the StoreGateSvc ptr ----------------

  StatusCode sg= service("StoreGateSvc", m_storeGate);

//---- initialize the DetectorStore ptr ----------------

  StatusCode scDS = service("DetectorStore", m_detStore);
  if (scDS.isFailure()) 
  {
    log << MSG::ERROR
	<< "Unable to retrieve pointer to DetectorStore "
	<< endreq;
    return scDS;
  }



  // retrieve the noisetool  
  //always needed even without noise to get the gain estimate

  IToolSvc* p_toolSvc = 0;// Pointer to Tool Service
  StatusCode sc = service("ToolSvc", p_toolSvc);
  if (sc.isFailure()) {
    log << MSG::FATAL
	<< " Tool Service not found "
	<< endreq;
    return sc;
  }
  IAlgTool* algtool;
  ListItem corr(m_NoiseToolName);	  
  sc = p_toolSvc->retrieveTool(corr.type(), corr.name(), algtool);
  if (sc.isFailure()) 
    log << MSG::INFO
	<< "Unable to find tool for " << m_NoiseToolName
	<< endreq;
  else  log << MSG::INFO
	    << m_NoiseToolName << " successfully retrieved" 
	    << endreq;

  m_noisetool=dynamic_cast<ICaloNoiseTool*>(algtool);


// IF NOISE
  if( m_WithNoise )
  {

    // mandatory to use map
    m_WithMap = true ;
    log << MSG::INFO << " Noise selected ("
	<<m_WithElecNoise<<" "<<m_WithPileUpNoise<<")" << endreq ; 


    //access random noise number service
    static const bool CREATEIFNOTTHERE(true);
    sc = service("AtRndmGenSvc", m_AtRndmGenSvc, CREATEIFNOTTHERE);
    if (!sc.isSuccess() || 0 == m_AtRndmGenSvc)
      {
	log << MSG::ERROR << " Could not initialize Random Number Service" 
	    << endreq;
	return sc;
      }	
  }
  else log << MSG::INFO << " no noise selected " << endreq ;
   

// Incident Service: 
  IIncidentSvc* incSvc;
  sc = service("IncidentSvc", incSvc);
  if (sc.isFailure()) 
  {
    log << MSG::ERROR
	<< "Unable to retrieve pointer to DetectorStore "
	<< endreq;
    return sc;
  }
  //start listening to "BeginRun"
  incSvc->addListener(this, "BeginRun", m_priority);


  if( m_WithMap )
  {
    log << MSG::INFO << " Internal map will be used " << endreq ; 
  }
  
  else {
      log << MSG::INFO << " Internal map will not be used " << endreq ;
    }
  

  return sg; //sg from "initialize the StoreGateSvc ptr"  
}

/////////////////////////////////////////////////////////////////// 

void 
LArCellBuilderFromLArHitTool::handle(const Incident& /* inc*/ )
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG
      << "LArCellBuilderFromLArHit handle()" 
      << endreq;

  StatusCode sc = m_detStore->retrieve(m_dd_fSampl) ;
  if (sc != StatusCode::SUCCESS) 
    {
      log << MSG::FATAL 
	  << "Error in retrieving sampling fraction from Detector Store !" 
	  << endreq;
      return;
    }
  else log << MSG::INFO << " -- ILArfSampl retrieved" << endreq;
  
  if (m_WithMap) 
  {    
    log << MSG::INFO << " initialize internal cell collection " 
	<< endreq ;
    //
    if (this->initializeCellPermamentCollection()!=SUCCESS)
    {
      log << MSG::FATAL<<"Making of cell permament collection failed"<< endreq;
      return;
    }    
  }
  return;
}

/////////////////////////////////////////////////////////////////// 

StatusCode LArCellBuilderFromLArHitTool::finalize()
{
  MsgStream log(msgSvc(), name());  
  log << MSG::INFO << " finalize: delete cellPermamentCollection " << endreq;
  //clean all LarHitInfo*
  for (unsigned int i=0;i<m_cellPermanentCollection.size();++i){
    if (m_cellPermanentCollection[i]!=0) {
      delete m_cellPermanentCollection[i];
    }
  }


  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 

StatusCode LArCellBuilderFromLArHitTool::process( CaloCellContainer * theCellContainer )
{
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "Executing LArCellBuilderFromLArHitTool" << endreq;

  unsigned int nCells0 = theCellContainer->size();
  

//---- re-initialize the HitMap ----------------
  // WITH NOISE 
  if( m_WithMap )
  {      
    StatusCode sc;
    sc = this->resetCellPermanentCollection();
    if (!sc.isSuccess()) return sc;
    if (m_Windows){
      log <<MSG::WARNING << " windows not reimplemented yet" << endreq ;
      StatusCode sc = this->buildWindowOnPermanentCollection();
      if (!sc.isSuccess()) return sc;      
    }
  }
  

//---- Get the HitContainer ----------------

  std::vector<std::string>::const_iterator hit_itr = m_HitContainers.begin(); 
  std::vector<std::string>::const_iterator hit_itr_end = m_HitContainers.end();

  for(; hit_itr!=hit_itr_end; hit_itr++) 
  { 
    //  const DataHandle < LArHitContainer >     hitcoll ;
    const LArHitContainer* hitcoll;
    StatusCode sc = m_storeGate->retrieve(hitcoll,*hit_itr); 
  
    if(sc!=StatusCode::SUCCESS || !hitcoll){ 
      log<<MSG::ERROR<<" Can not retrieve LArHitContainer: "<< *hit_itr 
	 <<endreq; 
      return StatusCode::FAILURE;      
    }    

    log << MSG::DEBUG << "LArHit container: " << *hit_itr 
	<< " Size=" << hitcoll->size() <<endreq; 
      
    LArHitContainer::const_iterator f_cell = hitcoll->begin();
    LArHitContainer::const_iterator l_cell = hitcoll->end();
    
    for ( ; f_cell!=l_cell; ++f_cell)
    {	          
      LArHit* hit = (*f_cell) ; 
      // hits and cells are now in MeV

      double e  = hit->energy() ;
      if (m_applyHitEnergyThreshold) {
	if (e>m_eHitThreshold) continue;
	
      }
      
      Identifier id  = hit->cellID();

      //FIXME
      //log << MSG::DEBUG << " Hit : " << m_atlas_id->show_to_string(id)
      //	  << " energy " << e << endreq ;
      
      // WITH MAP
      if( m_WithMap )
	{	
	  IdentifierHash idHash;
	  
	  if (m_caloNum==CaloCell_ID::LAREM) {
	    idHash=m_emID->channel_hash(id); 
	  }
	  
	  else if (m_caloNum==CaloCell_ID::LARHEC) {
	    idHash=m_hecID->channel_hash(id); 
	  }
	  
	  else if (m_caloNum==CaloCell_ID::LARFCAL) {
	    idHash=m_fcalID->channel_hash(id); 
	  }
	  

	  // set the hits of hitmap
	  if (m_cellPermanentCollection[idHash]!=0) {
	    m_cellPermanentCollection[idHash]->setHit(hit);
	   }	
	  else {
	     log << MSG::WARNING << " Trying to fill hit with no DDE " 
		 << m_atlas_id->show_to_string(id);
	   }
	}	
      // NO MAP
      else
	{	
	  // directly make the cells and add them to theCellContainer
	  e/= m_dd_fSampl->FSAMPL(id);	  

	  //FIXME
	  //  log << MSG::DEBUG << " .. new e " << e << endreq ;

	  const double t  = hit->time(); 
	  const double q  = 1. ;	  
	  // note that gain is estimated inside MakeTheCell
	  // because caloDDE is needed
	  MakeTheCell(theCellContainer,id,e,t,q);     
	}	

    }//end of loop on cells   
  }
  

  HepRandomEngine* engine=0; 
  if (m_WithNoise) 
    {
      const xAOD::EventInfo* eventInfo;
      StatusCode sc = m_storeGate->retrieve(eventInfo);

      if (sc.isFailure())
	{
	  log << MSG::ERROR << "Could not retrieve event info" << endreq;
	}

      const int iSeedNumber = eventInfo->runNumber()*10000000
	                     +eventInfo->eventNumber();
      
      engine = m_AtRndmGenSvc->setOnDefinedSeeds(iSeedNumber,this->name());
    }


//now create the cells
  // with map
  if( m_WithMap )
  {      
    CellPermanentCollection::const_iterator it, itEnd;

    if (m_Windows){
      it=m_windowOnPermanentCollection.begin();
      itEnd=m_windowOnPermanentCollection.end();
    }
    else{
      it=m_cellPermanentCollection.begin();
      itEnd=m_cellPermanentCollection.end();
    }    
 
    long index =-1;
    
    for ( ; it!=itEnd; ++it)
    {
      //check
      ++index;
      if (*it==0) {	
	//log << MSG::WARNING << " hole in permanent collection " 
	//    << index << endreq ;
	continue;
      }
      
      const LArHitInfo & info = **it;           

      //if no noise eliminate now cells with zero energy (=cells with no hits)
      //or zero energy cell
      if ( ! m_WithNoise) {
        if (!info.hasBeenHit()) continue ;
      }
      
      double e=info.energy();   

      // scale     
      e/= m_dd_fSampl->FSAMPL( info.caloDDE()->identify() ); 

      // choose the gain   

      const CaloGain::CaloGain g=m_noisetool->estimatedGain(info.caloDDE(),e,
				    ICaloNoiseToolStep::RAWCHANNELS);      
      const int igain = static_cast<int>(g);
      // retrieve the data of the LArHitInfo
      std::vector<float> SIGMANOISE=info.SIGMANOISE();

      // add the noise
      double sigma=SIGMANOISE[igain];  

      if (m_WithNoise) {
	double rndm = RandGauss::shoot(engine);
	e+=sigma*rndm;
	//	double rndm=RandGauss::shoot();
      }
      

      if (m_ThresholdOnE !=-999999){
	if (e < m_ThresholdOnE ) continue ;
      }
      if (m_ThresholdOnAbsE !=-999999){
	if (std::abs(e) < m_ThresholdOnAbsE ) continue ;
      }
      if (m_ThresholdOnEinSigma !=-999999){
	if (e     < m_ThresholdOnEinSigma*sigma ) continue ;
      }
      if (m_ThresholdOnAbsEinSigma !=-999999){
	if (std::abs(e)  < m_ThresholdOnAbsEinSigma*sigma) continue ;
      }
      
      // make the LArCell
      double t=info.time();
      double q=1.;
      const CaloDetDescrElement * caloDDE=info.caloDDE();
      MakeTheCell(theCellContainer,caloDDE,e,t,q,g);
      
    }// end loop on hitmap  
  }
   
//---- cellMin & cellMax ----------------

  if (log.level() <= MSG::DEBUG) {
    CaloCellContainer::const_iterator itrCell =theCellContainer->begin(); 
    CaloCellContainer::const_iterator endCell = theCellContainer->end();

    double Emax = -100000. ;
    double Emin = 100000. ;
    const CaloCell * cellMax = 0;
    const CaloCell * cellMin = 0;

    //dump few first cells
    long index = 0;
    
    for ( ; itrCell!=endCell; ++itrCell)
    {

      if (((*itrCell)->energy()) > Emax) { 
        cellMax = (*itrCell);
        Emax =cellMax->energy(); 
        }
      if (((*itrCell)->energy()) < Emin) { 
        cellMin = (*itrCell);
        Emin =cellMin->energy(); 
        }

      //      if (index <= 10) {
      	
      log << MSG::VERBOSE 
	  << " Cell id " << m_atlas_id->show_to_string((*itrCell)->ID())
	  << " eta " << (*itrCell)->eta()
	  << " 	phi " << (*itrCell)->phi()
	  << " e " << (*itrCell)->energy()
	  << endreq;
     
      
      ++index;
      
    }
  if (cellMax != 0 ) 
    log << MSG::DEBUG << "Cell with max energy " << Emax 
	<<" ID=" << m_atlas_id->show_to_string(cellMax->ID()) 
	<< " eta=" << cellMax->eta() << " phi=" << cellMax-> phi()<< endreq;  
  if (cellMin != 0 ) 
    log << MSG::DEBUG << "Cell with min energy " << Emin 
	<<" ID=" << m_atlas_id->show_to_string(cellMin->ID()) 
	<< " eta=" << cellMin->eta() << " phi=" << cellMin-> phi()<< endreq;  

  }

  //specify that a given calorimeter has been filled
  if (theCellContainer->hasCalo(m_caloNum) )
  {
    log << MSG::WARNING << "CaloCellContainer has already been filled with calo " 
	<< m_caloNum << endreq ;    
  }
      
  theCellContainer->setHasCalo(m_caloNum);
  
  

//---- end ---------------- 
   
  log << MSG::DEBUG << "CaloCellContainer : number of cells added " 
      << theCellContainer->size()-nCells0 
      << "  new size = " << theCellContainer->size()<< endreq;
  
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
 
void 
LArCellBuilderFromLArHitTool::MakeTheCell(CaloCellContainer * & cellcoll, 
				      const CaloDetDescrElement * &caloDDE,
				      const double & e,const double & t,
				      const double & q,
				      const CaloGain::CaloGain & g)
{
CaloCell* theCell;
  
theCell = new LArCell(caloDDE,e,t,q,g);
cellcoll->push_back(theCell);  
}

/////////////////////////////////////////////////////////////////// 

// estimate the gain as well
void LArCellBuilderFromLArHitTool::MakeTheCell(CaloCellContainer* & cellcoll, 
					   Identifier & id,
					   const double & e,const double & t,
                                           const double & q)

{  
  LArCell* newCell;

  const CaloDetDescrElement* caloDDE=m_calo_dd_man->get_element(id);
  

  //estimate the gain
  const CaloGain::CaloGain g=
    m_noisetool->estimatedGain(caloDDE,e,ICaloNoiseToolStep::RAWCHANNELS);  
  
  newCell = new LArCell(caloDDE,e,t,q,g);

  cellcoll->push_back(newCell);

}

/////////////////////////////////////////////////////////////////// 

StatusCode LArCellBuilderFromLArHitTool::initializeCellPermamentCollection()
{

  MsgStream log(msgSvc(), name());
  
  m_cellPermanentCollection.clear();
   

  std::vector <CaloDetDescrElement*>::const_iterator itrDDE, itrEndDDE;
  itrDDE=m_calo_dd_man->element_begin (m_caloNum);
  itrEndDDE=m_calo_dd_man->element_end (m_caloNum);

  //resize to correct size and rest to zero
  IdentifierHash caloCellMin, caloCellMax ;
  m_caloCID->calo_cell_hash_range(m_caloNum,caloCellMin,caloCellMax);
  unsigned int nCellCalo=caloCellMax-caloCellMin;
  m_cellPermanentCollection.resize(nCellCalo,0);


  long index = -1;
  
  long nHole =0 ;
  long nFilled =0 ;
  
  for (; itrDDE!=itrEndDDE;++itrDDE){
    ++index;

    CaloDetDescrElement* caloDDE = (*itrDDE);
    
    //check if no hole
    if (caloDDE==0) {
      //      log << MSG::WARNING << "hole detected calo num " 
      // 	  << m_caloType << " index " << index << endreq ;
      ++nHole;
      continue;
    }
    
    ++nFilled ;
    
    //Identifier id=caloDDE->identify() ;
    IdentifierHash idHash=caloDDE->subcalo_hash() ;
    
    LArHitInfo* theLArHitInfo = new LArHitInfo(caloDDE);
    
    // FIXME: USELESS ! 
    theLArHitInfo->setEpart2Edep(1. );
    theLArHitInfo->setEscale(1.);    
	
    std::vector<float> vNoise;
    if(m_WithElecNoise && !m_WithPileUpNoise)
      vNoise=
	m_noisetool->elecNoiseRMS3gains(caloDDE,-1,
					ICaloNoiseToolStep::RAWCHANNELS);
    //only for checks :
    if(!m_WithElecNoise && m_WithPileUpNoise)
      for(int igain=0;igain<3;++igain)
	vNoise.push_back(m_noisetool->pileupNoiseRMS(caloDDE));
    if(m_WithElecNoise && m_WithPileUpNoise)
      for(int igain=0;igain<3;++igain)
	vNoise.push_back(m_noisetool->
			 totalNoiseRMS(caloDDE,
				       static_cast<CaloGain::CaloGain>(igain)) 
			               );

    theLArHitInfo->setSIGMANOISE(vNoise);

    m_cellPermanentCollection[idHash]=theLArHitInfo;
    
  }

  log << MSG::INFO << "permanent cell collection size = " 
      << m_cellPermanentCollection.size()
      << " Filled : " << nFilled << endreq ;
  
  if (nHole!=0){
    log << MSG::WARNING << nHole << " holes detected in caloDDE " << endreq ;
  }
  

  log << MSG::INFO << " Build permament cell collection with size " 
      << m_cellPermanentCollection.size()<< endreq ;
   
  //check all elements have been created
  int nFailed = 0 ;
  
  for (unsigned int i=0;i<m_cellPermanentCollection.size();++i){
    if (m_cellPermanentCollection[i]==0) {
      //log << MSG::WARNING 
      //<< " one element missing in m_cellPermanentCollection " << i << endreq;
      ++nFailed;      
    }
  }
  
  if (nFailed == 0 ) {
    return StatusCode::SUCCESS;
  }
  else{
    log << MSG::WARNING << " total missing " << nFailed << endreq ;
    return StatusCode::SUCCESS;
  }
  
}

/////////////////////////////////////////////////////////////////// 

StatusCode LArCellBuilderFromLArHitTool::resetCellPermanentCollection()
{
  for (unsigned int i=0;i<m_cellPermanentCollection.size();++i){
    if (m_cellPermanentCollection[i]!=0)
      m_cellPermanentCollection[i]->setHasNotBeenHit();
  }
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 

StatusCode LArCellBuilderFromLArHitTool::buildWindowOnPermanentCollection()
{
  StatusCode sc=this->defineWindow();
  if (!sc.isSuccess()) return sc;
  
  m_windowOnPermanentCollection.clear();
  if (m_etaPart.size()==0) return StatusCode::SUCCESS;

  const float pi=2*asin(1);  
  for (unsigned int i=0;i<m_cellPermanentCollection.size();++i)
  {
    if (m_cellPermanentCollection[i]==0) continue ;
    const CaloDetDescrElement *caloDDE=m_cellPermanentCollection[i]->caloDDE();
    //loop on the particles of the truth, 
    //and select a zone around each one

    for(unsigned int iPart=0;iPart<m_etaPart.size();++iPart)
    { 	     
      float deltaPhi=fmod(m_phiPart[iPart]-caloDDE->phi()+3*pi,2*pi)-pi;
      float deltaEta=std::abs(m_etaPart[iPart]-caloDDE->eta());
      if( std::abs(deltaPhi)<m_WindowsPhiSize/2. &&
	  std::abs(deltaEta)<m_WindowsEtaSize/2. )
      {
	m_windowOnPermanentCollection.push_back(m_cellPermanentCollection[i]);
	break ;
      }
    }	
  }	
  
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 

StatusCode LArCellBuilderFromLArHitTool::defineWindow()
{
//define a window around each selected Geant-particle (eta,phi)

    MsgStream log(msgSvc(), name());     
    m_etaPart.clear();
    m_phiPart.clear();
    
    //get pointer of MC collection
    const McEventCollection * mcCollptr; 
    if ( m_storeGate->retrieve(mcCollptr,m_mcEventName).isFailure() ) {
      log << MSG::ERROR 
	  << "Could not retrieve McEventCollection for HitMap Windows"
	  << endreq;
      return StatusCode::FAILURE;
    }

    McEventCollection::const_iterator itr; 
    for (itr = mcCollptr->begin(); itr!=mcCollptr->end(); ++itr) {
      const HepMC::GenEvent* myEvent=(*itr);
      HepMC::GenEvent::particle_const_iterator itrPart; 
      for (itrPart = myEvent->particles_begin(); 
	   itrPart!=myEvent->particles_end(); ++itrPart ) 
	{
	  HepMC::GenParticle *part=*itrPart;
	  //works only for photons(22) and electrons(11) primary 
	  //particle from Geant (status>1000) with pt>5 GeV      
	  if( (part->pdg_id()==22 || abs(part->pdg_id())==11) 
	      && part->status()>1000 
	      && part->momentum().perp()>5 * GeV )
	    {
	      m_etaPart.push_back(part->momentum().pseudoRapidity()); 
	      m_phiPart.push_back(part->momentum().phi()); 
	    }
	} 
    }

  if (msgSvc()->outputLevel() <= MSG::DEBUG) {
    for(unsigned int iPart=0;iPart<m_etaPart.size();++iPart)
    {      
      log << MSG::DEBUG << " Selected window" 
	  << m_etaPart[iPart] << " phi=" << m_phiPart[iPart] 
	  << " deta=" << m_WindowsEtaSize << " dphi="<< m_WindowsPhiSize 
	  << endreq ;
    }
  }
  
  return StatusCode::SUCCESS; 
}

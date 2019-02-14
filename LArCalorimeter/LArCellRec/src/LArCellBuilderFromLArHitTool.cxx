/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCellBuilderFromLArHitTool.h"

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
#include "StoreGate/ReadHandle.h"

#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"


#include "AthenaKernel/RNGWrapper.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiKernel/ListItem.h"


using CLHEP::HepRandomEngine;
using CLHEP::RandGauss;
using CLHEP::GeV;


/////////////////////////////////////////////////////////////////// 

LArCellBuilderFromLArHitTool::LArCellBuilderFromLArHitTool(
				      const std::string& type, 
				      const std::string& name, 
				      const IInterface* parent)
  :base_class(type, name, parent),
   m_priority(1000), 
   m_caloNum(),
   m_calo_dd_man(nullptr),
   m_caloCID(nullptr),
   m_atlas_id(nullptr),
   m_emID(nullptr),
   m_hecID(nullptr),
   m_fcalID(nullptr),
   m_applyHitEnergyThreshold(false),
   m_eHitThreshold(-999999.), 
   m_ThresholdOnE(-999999.), 
   m_ThresholdOnAbsE(-999999.),
   m_ThresholdOnEinSigma(-999999.), 
   m_ThresholdOnAbsEinSigma(-999999.), 
   m_ThresholdSelected(0),
   m_ThresholdSelectedNotInSigma(0),
   m_Windows(false), 
   m_WindowsEtaSize(0.4),  
   m_WindowsPhiSize(0.5),
   m_WithNoise(true),
   m_WithElecNoise(true),
   m_WithPileUpNoise(false),
   m_NoiseToolName("CaloNoiseTool/calonoisetool"),
   m_noisetool(nullptr),
   m_WithMap(false),
   m_athRNGSvc("AthRNGSvc", name)
{
  // Priority of the IncidentListener (for "BeginRun")
  declareProperty("BeginRunPriority",m_priority); 


  // LAr Region of interest:
  declareProperty("LArRegion",m_LArRegion);

  // LAr Region of interest:
  declareProperty("LArHitContainers",m_HitContainerKeys);

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
  declareProperty("McEventName", m_mcEventKey = "GEN_EVENT");

  declareProperty("AthRNGSvc", m_athRNGSvc);
}


/////////////////////////////////////////////////////////////////// 

StatusCode LArCellBuilderFromLArHitTool::initialize()
{
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
      ATH_MSG_VERBOSE (" LAr Calo is em ");
    }
  else if (m_LArRegion == "LAr_HEC")
    {
      m_caloNum = CaloCell_ID::LARHEC ;
      m_atlas_id = m_hecID;
      ATH_MSG_VERBOSE (" LAr Calo is hec ");
    }
  else if (m_LArRegion == "LAr_FCal") 
    {
      m_caloNum = CaloCell_ID::LARFCAL ;
      m_atlas_id = m_fcalID;
      ATH_MSG_VERBOSE (" LAr Calo is FCAL ");
    }  
  else
    {
      ATH_MSG_ERROR (" cannot use region "  << m_LArRegion);
      m_atlas_id = 0;
      return StatusCode::FAILURE;
    }


  
// threshold on hit energy
  if (m_eHitThreshold==-999999.) {
    m_applyHitEnergyThreshold = false ;    
    ATH_MSG_INFO (" No threshold on hit energy ");
  } else
    {
      m_applyHitEnergyThreshold = false ;    
      ATH_MSG_INFO (" Threshold on hit energy " << m_eHitThreshold);
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

    ATH_MSG_WARNING ("More than one threshold was selected => none selected !");
  } 
  else 
  {     
    if(iThreshold[0]==1) 
      ATH_MSG_INFO ("THRESHOLD CHOICE : ThresholdOnE="<<m_ThresholdOnE);
    else 
    if(iThreshold[1]==1) 
      ATH_MSG_INFO ("THRESHOLD CHOICE : ThresholdOnAbsE="<<m_ThresholdOnAbsE);
    else 
    if(iThreshold[2]==1) 
      ATH_MSG_INFO ("THRESHOLD CHOICE : ThresholdOnEinSigma="<<m_ThresholdOnEinSigma);
    else
    if(iThreshold[3]==1) 
      ATH_MSG_INFO ("THRESHOLD CHOICE : ThresholdOnAbsEinSigma="<<m_ThresholdOnAbsEinSigma);
    else   
      ATH_MSG_INFO ("THRESHOLD CHOICE : No Threshold !");
  }

  if (m_ThresholdSelected==1) {
    // thresholds on cell are only implemented when using internal map
    ATH_MSG_INFO (" One threshold selected: use internal map");
    
    m_WithMap = true ;
  }
  


 
//---- initialize the HitContainers ----------------

  //int nPool = 0 ;  
 
  if (m_LArRegion == "LAr_EM") 
     {       
        m_HitContainerKeys.emplace_back("LArHitEMB");
        m_HitContainerKeys.emplace_back("LArHitEMEC");
        //nPool=175000;
	
     }
  else if (m_LArRegion == "LAr_EM_Barrel")
     {
        m_HitContainerKeys.emplace_back("LArHitEMB");
        //nPool=150000;
     }
  else if (m_LArRegion == "LAr_EM_Endcap")
     {
        m_HitContainerKeys.emplace_back("LArHitEMEC");
        //nPool=150000;
     }
  else if (m_LArRegion == "LAr_HEC")
     {
        m_HitContainerKeys.emplace_back("LArHitHEC");
        //nPool=6000;
     }
  else if (m_LArRegion == "LAr_FCal")  
    {      
        m_HitContainerKeys.emplace_back("LArHitFCAL");
        //nPool=3600;
    }  
  else if (m_LArRegion == "UserDefined" && (m_HitContainerKeys.size()>0) )
     {
      ATH_MSG_INFO ("Using user defined Region: hitContainers are ");
      ATH_MSG_INFO (" cannot simulate noise ");
      
      m_WithNoise = false;
      m_WithMap = false;
      
      std::string names ; 
      for(unsigned int i=0; i < m_HitContainerKeys.size(); ++i)
	names+=m_HitContainerKeys[i].key()+" ";
      ATH_MSG_INFO (names);
      //nPool=10000;
     }
  else
  {
      ATH_MSG_ERROR (" Unknown Region: "  << m_LArRegion);
      return StatusCode::FAILURE;
  }

  ATH_CHECK( m_HitContainerKeys.initialize() );

  // retrieve the noisetool  
  //always needed even without noise to get the gain estimate

  IToolSvc* p_toolSvc = 0;// Pointer to Tool Service
  ATH_CHECK( service("ToolSvc", p_toolSvc) );
  IAlgTool* algtool;
  ListItem corr(m_NoiseToolName);	  
  ATH_CHECK( p_toolSvc->retrieveTool(corr.type(), corr.name(), algtool) );
  m_noisetool=dynamic_cast<ICaloNoiseTool*>(algtool);

// IF NOISE
  if( m_WithNoise )
  {

    // mandatory to use map
    m_WithMap = true ;
    ATH_MSG_INFO (" Noise selected ("
                  <<m_WithElecNoise<<" "<<m_WithPileUpNoise<<")" );


    //access random noise number service
    ATH_CHECK( m_athRNGSvc.retrieve() );
  }
  else ATH_MSG_INFO (" no noise selected ");
   

// Incident Service: 
  IIncidentSvc* incSvc;
  ATH_CHECK( service("IncidentSvc", incSvc) );
  //start listening to "BeginRun"
  incSvc->addListener(this, "BeginRun", m_priority);

  if( m_WithMap ) {
    ATH_MSG_VERBOSE (" Internal map will be used ");
  }
  else {
    ATH_MSG_VERBOSE (" Internal map will not be used ");
  }

  if (m_WithMap && m_Windows) {
    ATH_CHECK( m_mcEventKey.initialize() );
  }
  else {
    m_mcEventKey = "";
  }

  ATH_CHECK(m_cablingKey.initialize());

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 

void 
LArCellBuilderFromLArHitTool::handle(const Incident& /* inc*/ )
{
  ATH_MSG_DEBUG ("LArCellBuilderFromLArHit handle()");

  if (detStore()->retrieve(m_dd_fSampl).isFailure())
    ATH_MSG_ERROR ("Could not retrieve ILArfSampl");
  else
    ATH_MSG_VERBOSE (" -- ILArfSampl retrieved");
  
  if (m_WithMap) 
  {    
    ATH_MSG_VERBOSE (" initialize internal cell collection ");
    if (!this->initializeCellPermanentCollection().isSuccess())
    {
      ATH_MSG_FATAL ("Making of cell permanent collection failed");
      return;
    }    
  }
  return;
}

/////////////////////////////////////////////////////////////////// 

StatusCode LArCellBuilderFromLArHitTool::finalize()
{
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 

StatusCode
LArCellBuilderFromLArHitTool::process ( CaloCellContainer* theCellContainer,
                                        const EventContext& ctx) const
{
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey,ctx};
  const LArOnOffIdMapping* cabling(*cablingHdl);

  ATH_MSG_DEBUG ("Executing LArCellBuilderFromLArHitTool");

  unsigned int nCells0 = theCellContainer->size();
  

//---- Get the HitContainer ----------------

  std::vector<const LArHit*> hits;
  if( m_WithMap ) {
    hits.resize (m_cellPermanentCollection.size());
  }

  for (const SG::ReadHandleKey<LArHitContainer>& k : m_HitContainerKeys)
  {
    SG::ReadHandle<LArHitContainer> hitcoll (k, ctx);
    ATH_MSG_DEBUG ("LArHit container: " << k.key()
                   << " Size=" << hitcoll->size());

    for (const LArHit* hit : *hitcoll)
    {	          
      // hits and cells are now in MeV

      double e  = hit->energy() ;
      if (m_applyHitEnergyThreshold) {
	if (e < m_eHitThreshold) continue;
      }
      
      const Identifier id  = hit->cellID();
      const HWIdentifier hwid=cabling->createSignalChannelID(id);
      
      //FIXME
      //log << MSG::DEBUG << " Hit : " << m_atlas_id->show_to_string(id)
      //	  << " energy " << e << endmsg ;
      
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
	  if (m_cellPermanentCollection[idHash].caloDDE()!=0) {
            hits[idHash] = hit;
          }
	  else {
            ATH_MSG_WARNING (" Trying to fill hit with no DDE " 
                             << m_atlas_id->show_to_string(id));
	   }
	}	
      // NO MAP
      else
	{	
	  // directly make the cells and add them to theCellContainer
	  e/= m_dd_fSampl->FSAMPL(hwid);	  

	  //FIXME
	  //  log << MSG::DEBUG << " .. new e " << e << endmsg ;

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
      ATHRNG::RNGWrapper* wrapper = m_athRNGSvc->getEngine (this, this->name());
      wrapper->setSeed (this->name(), ctx);
      engine = wrapper->getEngine (ctx);
    }


//now create the cells
  // with map
  if( m_WithMap )
  {      
    Window window (m_WindowsEtaSize, m_WindowsPhiSize);
    if (m_Windows){
      ATH_MSG_WARNING (" windows not reimplemented yet");
      ATH_CHECK( this->defineWindow (window, ctx) );
    }
 
    size_t sz = m_cellPermanentCollection.size();
    for (size_t index = 0; index < sz; ++index)
    {
      const LArHitInfo& info = m_cellPermanentCollection[index];
      if (!info.caloDDE()) continue;
      if (m_Windows && !window.isInWindow (*info.caloDDE())) continue;
      const LArHit* hit = hits[index];

      //if no noise eliminate now cells with zero energy (=cells with no hits)
      //or zero energy cell
      if ( ! m_WithNoise) {
        if (!hit) continue ;
      }
      
      double e = hit ? hit->energy() : 0;

      // scale     
      e/= m_dd_fSampl->FSAMPL(cabling->createSignalChannelID(info.caloDDE()->identify()) ); 

      // choose the gain   

      const CaloGain::CaloGain g=m_noisetool->estimatedGain(info.caloDDE(),e,
				    ICaloNoiseToolStep::RAWCHANNELS);      
      const int igain = static_cast<int>(g);
      // retrieve the data of the LArHitInfo
      const std::vector<float>& SIGMANOISE=info.SIGMANOISE();

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
      double t = hit ? hit->time() : 0;
      double q=1.;
      const CaloDetDescrElement * caloDDE=info.caloDDE();
      MakeTheCell(theCellContainer,caloDDE,e,t,q,g);
      
    }// end loop on hitmap  
  }
   
//---- cellMin & cellMax ----------------

  if (msgLvl(MSG::DEBUG)) {
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

      ATH_MSG_VERBOSE (" Cell id " << m_atlas_id->show_to_string((*itrCell)->ID())
                       << " eta " << (*itrCell)->eta()
                       << " 	phi " << (*itrCell)->phi()
                       << " e " << (*itrCell)->energy());
      
      ++index;
      
    }
  if (cellMax != 0 ) 
    ATH_MSG_DEBUG ("Cell with max energy " << Emax 
                   <<" ID=" << m_atlas_id->show_to_string(cellMax->ID()) 
                   << " eta=" << cellMax->eta() << " phi=" << cellMax-> phi());
  if (cellMin != 0 ) 
    ATH_MSG_DEBUG ("Cell with min energy " << Emin 
                   <<" ID=" << m_atlas_id->show_to_string(cellMin->ID()) 
                   << " eta=" << cellMin->eta() << " phi=" << cellMin-> phi());

  }

  //specify that a given calorimeter has been filled
  theCellContainer->resetLookUpTable();
  theCellContainer->setHasCalo(m_caloNum);
  
  

//---- end ---------------- 

  ATH_MSG_DEBUG ("CaloCellContainer : number of cells added " 
                 << theCellContainer->size()-nCells0 
                 << "  new size = " << theCellContainer->size());
  
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
 
void 
LArCellBuilderFromLArHitTool::MakeTheCell(CaloCellContainer* cellcoll, 
                                          const CaloDetDescrElement* caloDDE,
                                          const double e,const double t,
                                          const double q,
                                          const CaloGain::CaloGain g) const
{
  cellcoll->push_back_fast(new LArCell(caloDDE,e,t,q,g));
}

/////////////////////////////////////////////////////////////////// 

// estimate the gain as well
void LArCellBuilderFromLArHitTool::MakeTheCell(CaloCellContainer* cellcoll, 
                                               const Identifier& id,
                                               const double e,const double t,
                                               const double q) const
  
{  
  const CaloDetDescrElement* caloDDE=m_calo_dd_man->get_element(id);

  //estimate the gain
  const CaloGain::CaloGain g=
    m_noisetool->estimatedGain(caloDDE,e,ICaloNoiseToolStep::RAWCHANNELS);  
  
  cellcoll->push_back_fast (new LArCell(caloDDE,e,t,q,g));

}

/////////////////////////////////////////////////////////////////// 

StatusCode LArCellBuilderFromLArHitTool::initializeCellPermanentCollection()
{
  m_cellPermanentCollection.clear();
   
  //resize to correct size and rest to zero
  IdentifierHash caloCellMin, caloCellMax ;
  m_caloCID->calo_cell_hash_range(m_caloNum,caloCellMin,caloCellMax);
  unsigned int nCellCalo=caloCellMax-caloCellMin;
  m_cellPermanentCollection.resize(nCellCalo);


  long index = -1;
  
  long nHole =0 ;
  long nFilled =0 ;

  for (const CaloDetDescrElement* caloDDE :
         m_calo_dd_man->element_range (m_caloNum))
  {
    ++index;
    
    //check if no hole
    if (caloDDE==0) {
      //      log << MSG::WARNING << "hole detected calo num " 
      // 	  << m_caloType << " index " << index << endmsg ;
      ++nHole;
      continue;
    }
    
    ++nFilled ;
    
    //Identifier id=caloDDE->identify() ;
    IdentifierHash idHash=caloDDE->subcalo_hash() ;
    
    LArHitInfo theLArHitInfo (caloDDE);
    
    // FIXME: USELESS ! 
    theLArHitInfo.setEpart2Edep(1. );
    theLArHitInfo.setEscale(1.);    
	
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

    theLArHitInfo.setSIGMANOISE(vNoise);

    m_cellPermanentCollection[idHash]=std::move(theLArHitInfo);
    
  }

  ATH_MSG_INFO ("permanent cell collection size = " 
                << m_cellPermanentCollection.size()
                << " Filled : " << nFilled);
  
  if (nHole!=0){
    ATH_MSG_WARNING (nHole << " holes detected in caloDDE ");
  }
  

  ATH_MSG_INFO (" Build permament cell collection with size " 
                << m_cellPermanentCollection.size());
   
  //check all elements have been created
  int nFailed = 0 ;
  
  for (unsigned int i=0;i<m_cellPermanentCollection.size();++i){
    if (m_cellPermanentCollection[i].caloDDE()==0) {
      //log << MSG::WARNING 
      //<< " one element missing in m_cellPermanentCollection " << i << endmsg;
      ++nFailed;      
    }
  }
  
  if (nFailed == 0 ) {
    return StatusCode::SUCCESS;
  }
  else{
    ATH_MSG_WARNING (" total missing " << nFailed);
    return StatusCode::SUCCESS;
  }
}

/////////////////////////////////////////////////////////////////// 

StatusCode
LArCellBuilderFromLArHitTool::defineWindow (Window& window,
                                            const EventContext& ctx) const
{
//define a window around each selected Geant-particle (eta,phi)

    //get pointer of MC collection
    SG::ReadHandle<McEventCollection> mcCollptr (m_mcEventKey, ctx);

    window.reset (mcCollptr->size());

    for (const HepMC::GenEvent* myEvent : *mcCollptr) {
      HepMC::GenEvent::particle_const_iterator itrPart; 
      for (itrPart = myEvent->particles_begin(); 
	   itrPart!=myEvent->particles_end(); ++itrPart ) 
	{
	  const HepMC::GenParticle *part=*itrPart;
	  //works only for photons(22) and electrons(11) primary 
	  //particle from Geant (status>1000) with pt>5 GeV      
	  if( (part->pdg_id()==22 || abs(part->pdg_id())==11) 
	      && part->status()>1000 
	      && part->momentum().perp()>5 * GeV )
          {
            window.push (part->momentum().pseudoRapidity(),
                         part->momentum().phi());
          }
        }
    }

  if (msgSvc()->outputLevel() <= MSG::DEBUG) {
    for (const std::pair<float, float>& p : window.m_parts) {
      ATH_MSG_DEBUG( " Selected window" 
                     << p.first << " phi=" << p.second
                     << " deta=" << m_WindowsEtaSize << " dphi="<< m_WindowsPhiSize 
                     );
    }
  }
  
  return StatusCode::SUCCESS; 
}


/////////////////////////////////////////////////////////////////// 


LArCellBuilderFromLArHitTool::Window::Window (float etaSize,
                                              float phiSize)
  : m_etaSize (etaSize),
    m_phiSize (phiSize)
{
}


/////////////////////////////////////////////////////////////////// 


void LArCellBuilderFromLArHitTool::Window::reset (size_t n)
{
  m_parts.clear();
  m_parts.reserve (n);
}


/////////////////////////////////////////////////////////////////// 


bool LArCellBuilderFromLArHitTool::Window::isInWindow
 (const CaloDetDescrElement& caloDDE) const
{
  for (const std::pair<float, float>& p : m_parts) {
    float deltaPhi = fmod(p.second - caloDDE.phi()+3*M_PI,2*M_PI)-M_PI;
    float deltaEta = std::abs(p.first - caloDDE.eta());
    if( std::abs(deltaPhi) < m_phiSize/2. &&
        std::abs(deltaEta) < m_etaSize/2. )
    {
      return true;
    }
  }

  return false;
}

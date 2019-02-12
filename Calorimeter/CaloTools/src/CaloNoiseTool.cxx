/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTools/CaloNoiseTool.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileCablingService.h"

// For Gaudi
#include "GaudiKernel/IIncidentSvc.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGauss.h"

// Exceptions
#include "LArElecCalib/LArConditionsException.h"

using CLHEP::GeV;
using CLHEP::RandGauss;
 
//////////////////////////////////////////////////

CaloNoiseTool::CaloNoiseTool(const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent) 
  : AthAlgTool(type, name, parent),
    m_CachedGetNoiseCDDE(nullptr),
    m_CachedGetNoiseCELL(nullptr),
    m_highestGain(),
    m_WithOF(true),
    m_atlas_id(nullptr),
    m_calo_id_man(nullptr),
    m_lar_em_id(nullptr),
    m_lar_hec_id(nullptr),
    m_lar_fcal_id(nullptr),
    m_tile_id(nullptr),
    m_calo_dd_man(nullptr),
    m_calocell_id(nullptr),
    m_cablingService("LArCablingLegacyService"),
    m_tileInfoName("TileInfo"),
    m_tileInfo(nullptr),
    m_Adc2MeVFactor(0),
    m_AdcPerMev(0),
    m_adc2mevTool("LArADC2MeVTool"),
    m_SigmaNoise(0),
    m_CNoise(0),
    m_useRMSpedestal(false),
    m_RMSpedestal(0),
    m_OFCTool("LArOFCTool"),
    m_nsamples(0),
    m_fSampl(0),
    m_MinBiasRMS(0),
    m_Nminbias(-1),
    m_Nminbias_usedForCache(0),
    m_WorkMode(1),
    m_NormalizeAutoCorr(false),
    m_UseLAr(true),m_UseTile(true),m_UseSymmetry(true),
    m_DumpDatabaseHG(false),m_DumpDatabaseMG(false),m_DumpDatabaseLG(false),
    m_isMC(true),
    m_keyNoise("LArNoise"), m_keyPedestal("LArPedestal"),
    m_keyAutoCorr("LArAutoCorr"),
    m_keyShape("LArShape"), m_keyfSampl("LArfSampl"), m_keyMinBias("LArMinBias"),
    m_Nmessages_forTilePileUp(0),
    m_noiseOK(false),
    m_DiagnosticHG(true),m_DiagnosticMG(true),m_DiagnosticLG(true),
    m_cacheValid(false),m_deltaBunch(1),m_firstSample(0)

{
  declareInterface<ICaloNoiseTool>(this);
  declareInterface<ICalorimeterNoiseTool>(this);

  //switch on Optimal Filtering (applied or not)
  declareProperty("WithOF",m_WithOF);
  //name of TileInfo
  declareProperty("TileInfoName",m_tileInfoName);
  declareProperty("LArADC2MeVTool",m_adc2mevTool,"Tool handle for ADC2MeV");
  declareProperty("LArOFCTool",m_OFCTool,"Tool handle for OFC");
  declareProperty("NMinBias",m_Nminbias);
  declareProperty("WorkMode",m_WorkMode);
  //declareProperty("Geant3",m_Geant3);
  declareProperty("UseLAr",m_UseLAr);
  declareProperty("UseTile",m_UseTile);
  declareProperty("UseSymmetry",m_UseSymmetry);
  declareProperty("DumpDatabaseHG",m_DumpDatabaseHG);
  declareProperty("DumpDatabaseMG",m_DumpDatabaseMG);
  declareProperty("DumpDatabaseLG",m_DumpDatabaseLG);
  declareProperty("DiagnosticHG",m_DiagnosticHG);
  declareProperty("DiagnosticMG",m_DiagnosticMG);
  declareProperty("DiagnosticLG",m_DiagnosticLG);
  declareProperty("ReturnNoise",m_ReturnNoiseName="electronicNoise");
  declareProperty("FixGain",m_gain_from_joboption=-1);
  declareProperty("IsMC",m_isMC);
  declareProperty("keyAutoCorr",m_keyAutoCorr); 
  declareProperty("keyPedestal",m_keyPedestal); 
  declareProperty("keyShape",m_keyShape); 
  declareProperty("keyfSampl",m_keyfSampl); 
  declareProperty("keyMinBias",m_keyMinBias); 
  declareProperty("keyNoise",m_keyNoise); 
  declareProperty("LoadAtBegin",m_loadAtBegin=true);
  declareProperty("deltaBunch",m_deltaBunch);
  declareProperty("firstSample",m_firstSample);
}
 
//////////////////////////////////////////////////

StatusCode 
CaloNoiseTool::initialize()
{
  ATH_MSG_INFO( "CaloNoiseTool called " << this->name() << " initialize() begin" );
  
  if ((std::string)this->name()=="ToolSvc.calonoisetool") 
    ATH_MSG_WARNING( "calonoisetool is obsolete. Please use CaloNoiseToolDefault.py"  );

  //diagnostic
  m_diagnostic[CaloGain::LARHIGHGAIN]  =m_DiagnosticHG;
  m_diagnostic[CaloGain::LARMEDIUMGAIN]=m_DiagnosticMG;
  m_diagnostic[CaloGain::LARLOWGAIN]   =m_DiagnosticLG;  

  //dumps
  m_DumpDatabase[CaloGain::LARHIGHGAIN]  =m_DumpDatabaseHG;
  m_DumpDatabase[CaloGain::LARMEDIUMGAIN]=m_DumpDatabaseMG;
  m_DumpDatabase[CaloGain::LARLOWGAIN]   =m_DumpDatabaseLG;

  m_Nmessages_forTilePileUp=0;

  ATH_CHECK( m_cablingService.retrieve() );
  ATH_MSG_DEBUG( "CablingService retrieved"  );
  
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::  
  //retrieves helpers for LArCalorimeter

  m_calo_dd_man = CaloDetDescrManager::instance();
  m_calo_id_man = m_calo_dd_man->getCalo_Mgr();
  
  m_lar_em_id   = m_calo_id_man->getEM_ID();
  m_lar_hec_id  = m_calo_id_man->getHEC_ID();
  m_lar_fcal_id = m_calo_id_man->getFCAL_ID();

  //FIXME
  m_tile_id = m_calo_id_man->getTileID();
  
  if (m_tile_id==nullptr)
  {
    m_UseTile=false;
    ATH_MSG_WARNING(" no tile_id -> noise unavailable for Tiles !" );
  } 
    
  
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //retrieves helpers for Calorimeter
  m_calocell_id = m_calo_dd_man->getCaloCell_ID();

  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //set calohash maximums
  m_LArHashMax =0;
  m_TileHashMax=0;
  if(m_UseLAr)
    m_LArHashMax  =   m_lar_em_id->channel_hash_max()
                    + m_lar_hec_id->channel_hash_max()
                    + m_lar_fcal_id->channel_hash_max();
  if(m_UseTile) 
    m_TileHashMax = m_tile_id->cell_hash_max();
  m_CaloHashMax = m_LArHashMax + m_TileHashMax;

  //set calohash minimum
  if(m_UseLAr) m_CaloHashMin = 0;
  if(m_UseTile && m_UseLAr==false) m_CaloHashMin = m_LArHashMax;

  ATH_MSG_DEBUG( " UseLAr: " <<m_UseLAr<< " UseTile: " <<m_UseTile
                 << " => CaloHashMin= " <<m_CaloHashMin
                 << " CaloHashMax= " <<m_CaloHashMax );

  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //gain-thresholds 
  m_LowGainThresh[CaloCell_ID::LAREM]    = 3900;//ADC counts in MediumGain 
  m_HighGainThresh[CaloCell_ID::LAREM]   = 1300;//ADC counts in MediumGain
  m_LowGainThresh[CaloCell_ID::LARHEC]   = 2500;//ADC counts in MediumGain 
  m_HighGainThresh[CaloCell_ID::LARHEC]  = 0;//-> high-gain never used for HEC
  if(m_WorkMode==0)
  {
    m_LowGainThresh[CaloCell_ID::LARFCAL]  = 60.*GeV;//MeV
    m_HighGainThresh[CaloCell_ID::LARFCAL] =  6.*GeV; //MeV
  }
  else
  {
    m_LowGainThresh[CaloCell_ID::LARFCAL]  = 2000.;//ADC counts
    m_HighGainThresh[CaloCell_ID::LARFCAL] = 1100.;//ADC counts
  }
  m_LowGainThresh[CaloCell_ID::TILE]  = 0.;// unit ?
  m_HighGainThresh[CaloCell_ID::TILE] = 0.;//
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  m_highestGain[CaloCell_ID::LAREM]   = CaloGain::LARHIGHGAIN;
  m_highestGain[CaloCell_ID::LARHEC]  = CaloGain::LARMEDIUMGAIN;
  m_highestGain[CaloCell_ID::LARFCAL] = CaloGain::LARHIGHGAIN;
  m_highestGain[CaloCell_ID::TILE]    = CaloGain::TILEHIGHHIGH;

  
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //retrieves TileInfo
  if(m_UseTile)
  {
    StatusCode sc = detStore()->retrieve(m_tileInfo, m_tileInfoName);
    if (sc.isFailure()) {
      ATH_MSG_WARNING( "Unable to retrieve TileInfo from DetectorStore"
                       <<"  -> noise unavailable for Tiles !"  );
      m_UseTile=false;
    }
  }

  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //retrieve the LAr tools
  if(m_WorkMode==1 && m_UseLAr)
  {    
    //---- retrieve the ADC2MeV tool ----------------
    ATH_CHECK( m_adc2mevTool.retrieve() );
    ATH_MSG_DEBUG( " --ILArADC2MeVTool retrieved"  );

    if(m_WithOF) {      
      //---- retrieve the OFC Tool ----------------
      ATH_CHECK( m_OFCTool.retrieve() );
      ATH_MSG_DEBUG( " -- LArOFCTool retrieved"  );
    }
  }
  
  // Cache, what noise to return
  m_CachedGetNoiseCDDE=nullptr;
  m_CachedGetNoiseCELL=nullptr;
  if(m_ReturnNoiseName=="electronicNoise") {
    ATH_MSG_INFO( "Will cache electronic noise"  );
    m_CachedGetNoiseCDDE=&CaloNoiseTool::elecNoiseRMS;
    m_CachedGetNoiseCELL=&CaloNoiseTool::elecNoiseRMS;
  }
  if(m_ReturnNoiseName=="pileupNoise") {
    ATH_MSG_INFO( "Will cache pileupNoise noise"  );
    m_CachedGetNoiseCDDE=&CaloNoiseTool::pileupNoiseRMS;
    m_CachedGetNoiseCELL=&CaloNoiseTool::pileupNoiseRMS;
  }
  if(m_ReturnNoiseName=="totalNoise") {
    ATH_MSG_INFO( "Will cache totalNoise noise"  );
    m_CachedGetNoiseCDDE=&CaloNoiseTool::totalNoiseRMS;
    m_CachedGetNoiseCELL=&CaloNoiseTool::totalNoiseRMS;
  }
  if(m_CachedGetNoiseCDDE==nullptr || 
     m_CachedGetNoiseCELL==nullptr) {
    ATH_MSG_ERROR( "Unknown noise !"  );
    return StatusCode::FAILURE;
  }


  // callback functions
  if(m_UseLAr)
  {

    if(m_isMC){
      StatusCode sc=detStore()->regFcn(&ICaloNoiseTool::LoadCalibration,
			     dynamic_cast<ICaloNoiseTool*>(this),
			     m_dd_noise,m_keyNoise,true);
      if(sc.isSuccess()){
	ATH_MSG_INFO( "Registered callback for key: " 
                      << m_keyNoise  );
      } else {
	ATH_MSG_ERROR( "Cannot register callback function for key " 
                       << m_keyNoise  );
      }
    }else{
      StatusCode sc=detStore()->regFcn(&ICaloNoiseTool::LoadCalibration,
			     dynamic_cast<ICaloNoiseTool*>(this),
			     m_dd_pedestal,m_keyPedestal,true);
      if(sc.isSuccess()){
	ATH_MSG_INFO( "Registered callback for key: " 
                      << m_keyPedestal  );
      } else {
	ATH_MSG_ERROR( "Cannot register callback function for key " 
                       << m_keyPedestal  );
      }
    }

    StatusCode sc=detStore()->regFcn(&ICaloNoiseTool::LoadCalibration,
			   dynamic_cast<ICaloNoiseTool*>(this),
			   m_dd_acorr,m_keyAutoCorr,true);
    if(sc.isSuccess()){
      ATH_MSG_INFO( "Registered callback for key: " 
                    << m_keyAutoCorr  );
    } else {
      ATH_MSG_ERROR( "Cannot register callback function for key " 
                     << m_keyAutoCorr  );
    }
  
    if(m_WorkMode==1) 
    {

// get pulse shape, m_fSampl and minBiasRMS to compute pileup noise (MC only)
      if(m_isMC){

        StatusCode sc=detStore()->regFcn(&ICaloNoiseTool::LoadCalibration,
			      dynamic_cast<ICaloNoiseTool*>(this),
			      m_dd_shape,m_keyShape,true);
        if(sc.isSuccess()){
	  ATH_MSG_INFO( "Registered callback for key: " 
                        << m_keyShape  );
        } else {
	  ATH_MSG_ERROR( "Cannot register callback function for key " 
                         << m_keyShape  );
        }

        sc=detStore()->regFcn(&ICaloNoiseTool::LoadCalibration,
			       dynamic_cast<ICaloNoiseTool*>(this),
			       m_dd_fsampl,m_keyfSampl,true);
        if(sc.isSuccess()){
	  ATH_MSG_INFO( "Registered callback for key: " 
                        << m_keyfSampl  );
	} else {
	  ATH_MSG_ERROR( "Cannot register callback function for key " 
                         << m_keyfSampl  );
        }
	
        sc=detStore()->regFcn(&ICaloNoiseTool::LoadCalibration,
			       dynamic_cast<ICaloNoiseTool*>(this),
			       m_dd_minbias,m_keyMinBias,true);
        if(sc.isSuccess()){
	  ATH_MSG_INFO( "Registered callback for key: " 
                        << m_keyMinBias  );
         } else {
	  ATH_MSG_ERROR( "Cannot register callback function for key " 
                         << m_keyMinBias  );
         }
      }


      if (StatusCode::SUCCESS==
	  detStore()->regFcn(&ILArADC2MeVTool::LoadCalibration,&(*m_adc2mevTool),
			      &ICaloNoiseTool::LoadCalibration,
			      dynamic_cast<ICaloNoiseTool*>(this),true) ) {
	ATH_MSG_INFO( "Registered callbacks for LArADC2MeVTool -> CaloNoiseTool" 
                      );
      } else {
	ATH_MSG_ERROR( "Cannot register callbacks for LArADC2MeVTool -> CaloNoiseTool" 
                       );
      }

      if(m_WithOF){
	if (StatusCode::SUCCESS==
	    detStore()->regFcn(&ILArOFCTool::LoadCalibration,&(*m_OFCTool),
				&ICaloNoiseTool::LoadCalibration,
				dynamic_cast<ICaloNoiseTool*>(this),true) ) {
	  ATH_MSG_INFO( "Registered callbacks for LArOFCTool -> CaloNoiseTool" );
	} else {
	  ATH_MSG_ERROR( "Cannot register callbacks for LArOFCTool -> CaloNoiseTool"  );
	}
      }

    }

  }//UseLAr

  if(m_UseTile) {
    //currently no database for Tile
    ATH_CHECK( m_tileCellNoise.retrieve() );
    ATH_CHECK( m_tileToolEmscale.retrieve() );
    ATH_CHECK( m_tileToolNoiseSample.retrieve() );
    ATH_CHECK( m_tileIdTransforms.retrieve() );
  } else {
    m_tileCellNoise.disable();
    m_tileToolEmscale.disable();
    m_tileToolNoiseSample.disable();
    m_tileIdTransforms.disable();
  }

  m_CNoise    = 0.456E-3;
  m_AdcPerMev = 5.*GeV;



  if (m_loadAtBegin) {
    ATH_MSG_DEBUG( "Setting callback function to load calibration at begin of run"  );
    // Incident Service: 
    IIncidentSvc* incSvc = nullptr;
    ATH_CHECK( service("IncidentSvc", incSvc) );
    
    //start listening to "BeginRun". The incident should be fired AFTER the IOV callbacks and only once.
    const long priority=std::numeric_limits<long>::min(); //Very low priority
    incSvc->addListener(this, "BeginRun", priority ,false,true); //single-shot incident
  }


  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  ATH_MSG_INFO( "CaloNoiseTool initialize() end"  );

  return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////
 
StatusCode 
CaloNoiseTool::initContainers()
{
  //initialize the maps m_ElecNoiseContainer and m_ScaleContainer 
  //(assuming type of elements of the containers is the same for Tile and LAr)

  ATH_MSG_INFO( "initContainers() begin "  );

  // intialise indices
  this->initIndex(); 

  //::::::::::::::::::::::::::::::::::::::     
  m_elecNoiseRAWContainer.resize(m_idSymmCaloHashContainer.size());
  m_elecNoiseCELLContainer.resize(m_idSymmCaloHashContainer.size());
  if(m_WorkMode==1)
    m_pileupNoiseContainer.resize(m_idSymmCaloHashContainer.size());
  m_adc2mevContainer.resize(m_idSymmCaloHashContainer.size());
  //::::::::::::::::::::::::::::::::::::::
  ATH_MSG_INFO( "initContainers() end : "
      <<" size of containers = "
      <<m_idSymmCaloHashContainer.size() );
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////

void 
CaloNoiseTool::initIndex()

{
  //::::::::::::::::::::::::::::::::::::::
  m_indexContainer.clear();
  m_indexContainer.resize(m_CaloHashMax,
                          static_cast<unsigned int> (-1));

  //maybe the other container sould be reset
  m_idSymmCaloHashContainer.clear();
  m_idSymmCaloHashContainer.reserve(5000);

  
  
  for (unsigned int intIdCaloHash=m_CaloHashMin; intIdCaloHash<m_CaloHashMax;
       ++intIdCaloHash)
  {  
    

    IdentifierHash idCaloHash=static_cast<IdentifierHash>(intIdCaloHash);

    //    std::cout << "DRDEBUG in initIndex loop " << intIdCaloHash << std::endl ;

    
    // initialize the vector of indexes (big vector without symmetry)




    // o idCaloHash -> id -> idSymm (symmetry phi->0 and z->|z|)
    // o idSymm -> idSymmCaloHash
    // o idSymmCaloHash stored in m_idSymmCaloHashContainer
    // o an index is associated to an idSymmCaloHash
    // o index stored in m_indexContainer
    
    CaloCell_ID::SUBCALO iCalo = this->caloNum(idCaloHash); 
    Identifier     id    = m_calocell_id->cell_id(idCaloHash);
    Identifier     regId;
    Identifier     idSymm;
    IdentifierHash idSymmCaloHash;

    if(m_UseSymmetry)
    {
      if(iCalo==CaloCell_ID::LAREM) 
      {
        int barrel_ec = m_lar_em_id->barrel_ec(id);
        int sampling  = m_lar_em_id->sampling(id);
        int region    = m_lar_em_id->region(id);
        int eta       = m_lar_em_id->eta(id);
        regId         = m_lar_em_id->region_id(abs(barrel_ec),sampling,region);
        idSymm        = m_lar_em_id->channel_id(regId,
                                                eta,
                                                m_calocell_id->phi_min(regId));
        idSymmCaloHash= m_calocell_id->calo_cell_hash(idSymm);
      }
      else if(iCalo==CaloCell_ID::LARHEC) 
      {
        int pos_neg  = m_lar_hec_id->pos_neg(id);
        int sampling = m_lar_hec_id->sampling(id);
        int region   = m_lar_hec_id->region(id);
        int eta      = m_lar_hec_id->eta(id);
        regId        = m_lar_hec_id->region_id(abs(pos_neg),sampling,region);
        idSymm       = m_lar_hec_id->channel_id(regId,
                                                eta,
                                                m_calocell_id->phi_min(regId));
        idSymmCaloHash= m_calocell_id->calo_cell_hash(idSymm);
      }
      else if(iCalo==CaloCell_ID::LARFCAL) 
      {
        int pos_neg = m_lar_fcal_id->pos_neg(id);
        int module  = m_lar_fcal_id->module(id);
        int eta     = m_lar_fcal_id->eta(id);
        int phi     = m_lar_fcal_id->phi(id);
        if(phi>7) phi = phi-8; //as in LArMCSymTool
        regId       = m_lar_fcal_id->module_id(abs(pos_neg),module);
        idSymm      = m_lar_fcal_id->channel_id(regId,
                                                eta,
                                                phi);
        idSymmCaloHash= m_calocell_id->calo_cell_hash(idSymm);
      }
      else if(iCalo==CaloCell_ID::TILE) 
      {
        idSymm         = id;
        idSymmCaloHash = m_calocell_id->calo_cell_hash(idSymm);
      }
      else
      {
        ATH_MSG_WARNING("CaloNoiseTool::chooseIndex  wrong id ! " 
                        << m_lar_em_id->show_to_string(id)  );
        continue ;
      }

      assert (idSymmCaloHash < m_CaloHashMax);
      if (m_indexContainer[idSymmCaloHash] != static_cast<unsigned int>(-1)) {
        m_indexContainer[idCaloHash] = m_indexContainer[idSymmCaloHash];
        continue;
      }
    }
    else idSymmCaloHash=idCaloHash;// no symmetry


    if(iCalo!=CaloCell_ID::TILE) {
      if(this->checkIfConnected(id)==false) {
        std::cout << "DRDEBUG ... NOT connected " << std::endl ;
        continue; 
      }
      // else { std::cout << "DRDEBUG ... connected " << std::endl ;}
    }

    //  std::cout << "DRDEBUG ... check connected OK " << intIdCaloHash << std::endl ;

    /* cabling eta= 0 -> 0.8  (for private debug)
       int samp  = m_lar_em_id->sampling(id);
       int region= m_lar_em_id->region(id);
       int eta   = m_lar_em_id->eta(id);
       int phi   = m_lar_em_id->phi(id);
       if(samp==1 && eta>=256) return;
       if(samp==1 && eta==0) return;
       if(samp==2 && eta>=32) return;
       if(samp==3 && eta>=16) return;
       if(region>0) return;
    */
  
    // std::cout << "DRDEBUG ... register under  " << m_new_index << std::endl ;

    //we come here if idSymmHash is not yet indexed (and is connected)   
    m_indexContainer[idCaloHash] = 
      m_indexContainer[idSymmCaloHash] =
      m_idSymmCaloHashContainer.size();
  
    m_idSymmCaloHashContainer.push_back(idSymmCaloHash);
  }// loop on all cells
}

//////////////////////////////////////////////////

bool 
CaloNoiseTool::checkIfConnected(const Identifier &id)
{
  try
  {
    HWIdentifier hwid = m_cablingService->createSignalChannelID(id);
    if(!m_cablingService->isOnlineConnected(hwid)) 
    {
      //std::cout<<m_lar_em_id->show_to_string(id)
      //	       <<" not connected !!"<<std::endl;
      return false;
    }
  }
  catch(LArID_Exception & except) 
    {return false;}
  //std::cout<<m_lar_em_id->show_to_string(id)
  //	       <<" connected !!"<<std::endl;  
  return true;
}

//////////////////////////////////////////////////

int 
CaloNoiseTool::index(const IdentifierHash &idCaloHash)
{
  return m_indexContainer[idCaloHash];
}

//////////////////////////////////////////////////

StatusCode CaloNoiseTool::LoadCalibration(IOVSVC_CALLBACK_ARGS_K(keys))
{

  ATH_MSG_INFO( "Callback invoked for " << keys.size() << " keys "  );

  // invalidates the cache
  m_cacheValid = false; 

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////
 
StatusCode 
CaloNoiseTool::initData()
{
  StatusCode sc ;
  sc = this->initContainers();
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "initContainers failed"  );
    return sc;
  }
 
  // reset diagnostics
  for(int igain=0;igain<CaloGain::LARNGAIN;++igain)
  {
    m_nCellsWithProblem[igain]=0;
    for(int i=0;i<5000;++i) m_nReason[i][igain]=0;
    for(int i=0;i<10;++i)   m_itReason[i][igain]=0;
  }

 
  if(m_UseLAr)
  {
    //stores the Adc2MeV factors 
    sc = this->initAdc2MeV();
    if (!sc.isSuccess())  
      ATH_MSG_ERROR( "initData(): error with initAdc2MeV() "  );
  }

  //calculates and stores the electronic noise
  sc = this->initElecNoise(); 
  if (!sc.isSuccess())  
    ATH_MSG_ERROR( "initData(): error with initElecNoise() "  );

  //calculates and stores the pileup noise
  if(m_WorkMode==1) {
    sc = this->initPileUpNoise();
    if (!sc.isSuccess())  
      ATH_MSG_ERROR( "initData(): error with initPileUpNoise() " );
    }
  

  m_cacheValid=true; 
  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

StatusCode 
CaloNoiseTool::initAdc2MeV() 
{
  ATH_MSG_INFO( "initAdc2MeV() begin "  );
  for (unsigned int it=0; it<m_adc2mevContainer.size(); ++it)
  { 
    CaloCell_ID::SUBCALO iCalo = this->caloNum(m_idSymmCaloHashContainer[it]);
    Identifier id=m_calocell_id->cell_id(m_idSymmCaloHashContainer[it]);
    //::::::::::::::::::::::::::::::::::::::
    if(m_WorkMode==1)  
    {
      if(iCalo!=CaloCell_ID::TILE) 
      {
	std::vector<float>& adc2mevVector = m_adc2mevContainer[it];
        adc2mevVector.reserve (CaloGain::LARNGAIN);
	for(unsigned int igain=0;igain<CaloGain::LARNGAIN;++igain)
        {
	  const std::vector<float> *
	    polynom_adc2mev = &(m_adc2mevTool->ADC2MEV(id,igain));
	  if(polynom_adc2mev->size()==0)
	    adc2mevVector.push_back(0.);
	  else 
	    adc2mevVector.push_back((*polynom_adc2mev)[1]);
	}
      }
    } 
    //::::::::::::::::::::::::::::::::::::::
  } 
  ATH_MSG_INFO( "initAdc2MeV() end "  );
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////

StatusCode 
CaloNoiseTool::initElecNoise()
{
  // initialize the parameters (the same for each event for each Identifier) 
  // for the calculation of the electronic noise


  ATH_MSG_DEBUG( "initElecNoise() begin "  );
 
  for (unsigned int it=0; it<m_elecNoiseRAWContainer.size(); ++it)
  {
    CaloCell_ID::SUBCALO iCalo = this->caloNum(m_idSymmCaloHashContainer[it]);
    //::::::::::::::::::::::::::::::::::::::        
    if(iCalo==CaloCell_ID::TILE) 
      m_elecNoiseRAWContainer[it] =
        this->calculateElecNoiseForTILE(m_idSymmCaloHashContainer[it]);
    //::::::::::::::::::::::::::::::::::::::
    else
      m_elecNoiseRAWContainer[it] =
	this->calculateElecNoiseForLAR(m_idSymmCaloHashContainer[it],iCalo,
                                       m_Nminbias);
    //::::::::::::::::::::::::::::::::::::::
    m_elecNoiseCELLContainer[it] = m_elecNoiseRAWContainer[it];
  }

  //print diagnostic
  if(m_UseLAr)
    for(int igain=0;igain<CaloGain::LARNGAIN;++igain)
      if(m_diagnostic[igain])
      {
	ATH_MSG_INFO("" );
	ATH_MSG_INFO("===== Diagnostic for  gain "<<igain<<" =====" );
	ATH_MSG_INFO("" );
	for(int i=0;i<m_nCellsWithProblem[igain];++i)
        {
	  Identifier id = m_calocell_id->cell_id(m_idHash[i][igain]);
	  msg(MSG::DEBUG)<<m_idHash[i][igain]<<" "
	     <<m_lar_em_id->show_to_string(id)
	     <<" "<<m_nReason[i][igain]<<" : ";
	  for(int j=0;j<m_nReason[i][igain];++j)
	    msg(MSG::DEBUG)<<m_reasonName[m_reason[i][j][igain]]<<" ";
	  msg(MSG::DEBUG)<<endmsg;
	}
	msg(MSG::DEBUG)<<endmsg;
	ATH_MSG_INFO("N cells with problem(s) = "
                     <<m_nCellsWithProblem[igain] );
	for(int i=0;i<10;++i)
          if(m_itReason[i][igain]>0) 
            ATH_MSG_INFO(i<<" "<<m_reasonName[i] 
                         <<": for "<<m_itReason[i][igain]<<" cells" );
      }

  ATH_MSG_DEBUG( "initElecNoise() end "  );
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////

StatusCode 
CaloNoiseTool::initPileUpNoise()
{
  // initialize the parameters (the same for each event for each Identifier) 
  // for the calculation of the PileUp noise

  ATH_MSG_DEBUG( "initPileUpNoise() begin "  );
  ATH_MSG_INFO( "N events of Minimum Bias per bunch crossing =  " 
                << m_Nminbias );
  //::::::::::::::::::::::::::::::::::::::
  for (unsigned int it=0; it<m_pileupNoiseContainer.size(); ++it)  
    m_pileupNoiseContainer[it]
      =this->calculatePileUpNoise(m_idSymmCaloHashContainer[it],m_Nminbias);
  //::::::::::::::::::::::::::::::::::::::
  m_Nminbias_usedForCache=m_Nminbias;
  ATH_MSG_DEBUG( "initPileUpNoise() end "  );
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////

std::vector<float> 
CaloNoiseTool::calculateElecNoiseForLAR(const IdentifierHash & idCaloHash,
					const CaloCell_ID::SUBCALO iCalo, 
					const float &Nminbias)
{ 
 /*  

E=SUMi { OFCi * (short[ (PulseShapei*Ehit/Adc2MeV(gain) + Noisei(gain)  
                       + pedestal) ]  
                 - pedestal) * Adc2Mev(gain) ] } 
   with Noisei =SUMj { cij*Rndm } * m_SigmaNoise

   NB:  without short and with cij=identity (no autocorrelation) 
        E=SUMi { NOISEi(gain)*Rndm } 
        with  NOISEi(gain) = Adc2MeV(gain) * OFCi * m_SigmaNoise(gain)        

   => Sigma^2=SUMi{NOISEi(gain)*NOISEj(gain)*cij} + quantification part
             =        NOISE(gain)                 +   REST
      Sigma  = sqrt( NOISE(gain) + REST) 


========= for FCAL on WorkMode=0 (NOVA DB) ==================== 

E=SUMi { OFCi * (short[ ( (PulseShapei*Ehit+Noisei)*gain + CNoisei ) 
                         * AdcPerGev + pedestal] 
                 - pedestal) } / gain
   with Noisei =SUMj { cij*Rndm } * m_SigmaNoise
        CNoisei=SUMj { cij*Rndm } * m_CNoise

   NB:  without short and with cij=identity (no autocorrelation) 
        E=SUMi { Ai*Rndm + Bi*Rndm } 
        with  Ai=(AdcPerGev)     *OFCi*m_SigmaNoise
              Bi=(AdcPerGev/gain)*OFCi*m_CNoise

   => Sigma^2=SUMi{Ai*Aj*cij + Bi*Bj*cij } + quantification part
             =        A      + (  B        +      C) / gain^2 
 
*/

  std::vector<float> sigmaVector (CaloGain::LARNGAIN,BADVALUE);
  float sigma;

  Identifier id = m_calocell_id->cell_id(idCaloHash);

  for(int igain=0;igain<CaloGain::LARNGAIN;++igain) 
  {
    m_noiseOK=true;
    //::::::::::::::::::::::::::::::::::::::::::::::::::
    //==== retrieve the database ====
    //::::::::::::::::::::::::::::::::::::::::::::::::::

    for(int i=0;i<nDATABASE;++i) m_retrieve[i]=false; 
    m_retrieve[iADC2MEV]=true;
    m_retrieve[iSIGMANOISE]=true;
    m_retrieve[iAUTOCORR]=true;
    if(m_WithOF) m_retrieve[iOFC]=true;
    //m_retrieve[iSHAPE]=true;
    StatusCode sc=this->retrieveCellDatabase(idCaloHash,id,igain,Nminbias,
					     "calculateElecNoiseForLAR");    
    //if(sc.isFailure()) continue;
      //NOTE: if an element of the database is empty, 
      //      leave the iteration (on gains) => value will be BADVALUE
      //the interfaces take care of that !   

    //::::::::::::::::::::::::::::::::::::::::::::::::::
    //==== calculations ====
    //::::::::::::::::::::::::::::::::::::::::::::::::::

    if(sc.isFailure()) {
      sigma=float(BADVALUE_TO_RETURN);
    }
    else    
    {
      float OFC_AC_OFC,OFC_OFC;
      this->commonCalculations(OFC_AC_OFC,OFC_OFC,1);
      //::::::::::::::::::::::::::::::::::::::
      if(iCalo==CaloCell_ID::LARFCAL && m_WorkMode==0)
      {
        float A=OFC_AC_OFC*m_SigmaNoise*m_SigmaNoise*m_AdcPerMev*m_AdcPerMev; 
          // A is the part with m_SigmaNoise 
        float B=OFC_AC_OFC*m_CNoise*m_CNoise*m_AdcPerMev*m_AdcPerMev;     
          // B is the part with the constant noise (m_CNoise) 
        float C=OFC_OFC*(1./12.);				 
          // C is the quantification part (effect of the truncation  short(..) )
          //   12.=sqrt(12)*sqrt(12)
        float gain[3]={1.,10.,100.};  
        sigma=A + (B+C)/(gain[igain]*gain[igain]) ;     
      }
      else// USUAL CASE
      {
        float NOISE= OFC_AC_OFC*m_SigmaNoise*m_SigmaNoise ;
        float REST = OFC_OFC*(1./12.);// 12.=sqrt(12)*sqrt(12)
        sigma=(NOISE+REST) * m_Adc2MeVFactor*m_Adc2MeVFactor;   
      }
      //::::::::::::::::::::::::::::::::::::::
      if(sigma>0) sigma=sqrt(sigma);
      else  
      {
        sigma=-sqrt(-sigma);
        //:::::::::::::::::
        //      MsgStream log(msgSvc(), name());
        //      if(igain==0) log << MSG::ERROR 
        //	  <<m_lar_em_id->show_to_string(id)<<" gain "<<igain
        //	  <<" : negative root square => WRONG noise "
        //	  <<"(please check if OFC or AutoCorr are correct for this cell)"
        //	  <<endmsg;
      }

      //diagnostic    
      if(m_diagnostic[igain]) 
      {
        if(m_noiseOK && sigma<0)       
  	  this->updateDiagnostic(9,"sigma<0",igain);         
        if(m_noiseOK==false) 
        {
	  m_idHash[m_nCellsWithProblem[igain]][igain]=idCaloHash;
	  ++m_nCellsWithProblem[igain];
	  m_nReason[m_nCellsWithProblem[igain]][igain]=0;
        }      
      }
      //::::::::::::::::::::::::::::::::::::::
      if(m_noiseOK==false || sigma<0) sigma=float(BADVALUE_TO_RETURN);
    }
    sigmaVector[igain]=sigma;

  }//loop on gains
  
  return sigmaVector;  
}

//////////////////////////////////////////////////

std::vector<float> 
CaloNoiseTool::calculateElecNoiseForTILE(const IdentifierHash & idCaloHash)
{
  std::vector<float> sigmaVector (CaloGain::LARNGAIN,BADVALUE);

  Identifier id = m_calocell_id->cell_id(idCaloHash);
  //std::cout<<"TILE elecnoise " <<m_lar_em_id->show_to_string(id);
  for(int igain=0;igain<CaloGain::LARNGAIN;++igain)
  {
    CaloGain::CaloGain gain = static_cast<CaloGain::CaloGain>(igain);
    float sigma = m_tileCellNoise->getCellNoise(id, gain);
    // Conversion from ADC sigma noise to OF sigma noise
    sigma *= m_tileInfo->getNoiseScaleFactor();

    //the LAr gain is internally (in CellNoiseSigma) converted into Tile gain
    sigmaVector[igain]= sigma;
    //::::::::::::::::::::::::::::::::::::::
    //std::cout<<" "<<sigma;
  }
  //std::cout<<" "<<std::endl;
  
  return sigmaVector;
}

//////////////////////////////////////////////////

float 
CaloNoiseTool::calculatePileUpNoise(const IdentifierHash & idCaloHash, 
				    const float &Nminbias)
{
  if(Nminbias<=0.000001) return 0.;
    //only on WorkMode==1
  if(this->caloNum(idCaloHash)==CaloCell_ID::TILE) return 0.;
    //no pile-up for tiles, for the moment ...

  /*
    SigmaPileUp^2 = ( SigmaE * sqrt(Nmb) )^2 * Ipileup/Tc
    where:  
      - Ipileup = Tc * SUM(k=1->Nb) g(tk)^2 
      - Tc is the time between bunch crossings
      - Nb is the number of bunch crossings 
        (over which the response function is non-zero)
      - g is the shape
      - SigmaE is the RMS of the energy in 1 minimum bias event
      - Nmb is the number of minimum bias events (depending on the luminosity)
   */ 
  
  Identifier id = m_calocell_id->cell_id(idCaloHash);

  //::::::::::::::::::::::::::::::::::::::

  for(int i=0;i<nDATABASE;++i) m_retrieve[i]=false;   
  m_retrieve[iAUTOCORR]=true;
  if(m_WithOF) m_retrieve[iOFC]=true;
  m_retrieve[iSHAPE]=true;
  m_retrieve[iMINBIASRMS]=true;
  m_retrieve[iFSAMPL]=true;
  StatusCode sc=this->retrieveCellDatabase(idCaloHash,id,
					   CaloGain::LARHIGHGAIN,Nminbias,
					   "calculatePileUpNoise");    
  if(sc.isFailure()) return 0.;

  //::::::::::::::::::::::::::::::::::::::

  //in the database, RMS is at the scale of the Hits, 
  // so we need to scale it at the e.m scale using the sampling fraction ...
  m_MinBiasRMS /= m_fSampl;

  //::::::::::::::::::::::::::::::::::::::

// overall normalization factor
  float  PileUp=m_MinBiasRMS*sqrt(Nminbias);
 
  //::::::::::::::::::::::::::::::::::::::

  float OFC_AC_OFC,OFC_OFC; 
  unsigned int firstSample=m_firstSample; 
  // for HEC, always use firstSample=1 when the number of samples is 4 
  if (m_lar_hec_id->is_lar_hec(id) && m_nsamples==4 && m_firstSample==0) firstSample=1; 
  this->commonCalculations(OFC_AC_OFC,OFC_OFC,2,firstSample); 
 
  //::::::::::::::::::::::::::::::::::::::

  PileUp*=sqrt(OFC_AC_OFC);

  //std::cout<<"PILEUP "<<m_lar_em_id->show_to_string(id)<<" "
  //	   <<MinBiasRMS<<" "<<OFC_AC_OFC<<" "<<PileUp<<std::endl;

  return PileUp; 
}

//////////////////////////////////////////////////

void
CaloNoiseTool::commonCalculations(float & OFC_AC_OFC,float & OFC_OFC,int icase, unsigned int firstSample) 
{

  // case 1 electronic noise
  if (icase==1) {
     //calculate the matrix of autocorrelation
     for(int i=0;i<m_nsamples;++i) 
       for(int j=0;j<m_nsamples;++j)  
       { 
         if(i==j)               m_c[i][j] = 1.; 
         for(int k=1;k<m_nsamples;++k) 
	   if(i==j-k || i==j+k)	
	     m_c[i][j] = m_AutoCorr[k-1];
       }
  }
// other case: pileup noise
  else {
     for (int i=0;i<m_nsamples;i++) {
      for (int j=0;j<m_nsamples;j++)
      {
        m_c[i][j]=0.;
        int nsize = m_Shape.size();
        for (int k=0;k<nsize;k++) {
           if ((j-i+k)>=0 && (j-i+k)<nsize) {
             int ibunch=0;
             if ((i+firstSample-k)%m_deltaBunch == 0 ) ibunch=1;
             m_c[i][j] += ((double) (ibunch)) * (m_Shape[k]) * (m_Shape[j-i+k]);
           }
        }
      }
     }
  }

  //::::::::::::::::::::::::::::::::::::::
  OFC_AC_OFC=0;
  OFC_OFC=0;
  
  if(m_WithOF)     
  {
    float tmp;
    for(int i=0;i<m_nsamples;++i) 
    {   
      tmp=0.; 
      for(int j=0;j<m_nsamples;++j)  	
	tmp+=m_c[i][j]*m_OFC[j]; 
      tmp*=m_OFC[i]; 
      OFC_AC_OFC+=tmp;  
      OFC_OFC+= m_OFC[i] * m_OFC[i]; 
      //std::cout<<"    "<<i<<" "<<OFC_AC_OFC<<" "<<OFC_OFC<<std::endl;
    }
  }
  else //equivalent to have only the third sample (peak)
  {
    // <=> (*m_OFC)[2]==1, others are null
    OFC_AC_OFC=m_c[2][2];
    OFC_OFC=1;
  } 
  //::::::::::::::::::::::::::::::::::::::
}

//////////////////////////////////////////////////

StatusCode
CaloNoiseTool::retrieveCellDatabase(const IdentifierHash & idCaloHash,
				    const Identifier & id, 
				    int igain,
				    const float &Nminbias,
				    std::string function_name)
{
  bool PRINT=false;
  if(m_DumpDatabase[igain]) PRINT=true;

  if(PRINT) std::cout<<"============ "<<m_lar_em_id->show_to_string(id)
		     <<" hash="<<idCaloHash
		     <<" gain="<<igain
		     <<" (HG="<<CaloGain::LARHIGHGAIN
		     << ",MG="<<CaloGain::LARMEDIUMGAIN
		     << ",LG="<<CaloGain::LARLOWGAIN<<")"<<std::endl;

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //ADC2MEV

  HWIdentifier hwid=m_cablingService->createSignalChannelID(id);

  if(m_retrieve[iADC2MEV])
  {
    int index=this->index(idCaloHash);
    m_Adc2MeVFactor = (m_adc2mevContainer[index])[igain];       
    if(PRINT) std::cout<<"m_Adc2MeVFactor="<<m_Adc2MeVFactor<<std::endl;
  }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //SIGMANOISE
  if(m_retrieve[iSIGMANOISE])
  {
    if(m_isMC)
      m_SigmaNoise = m_dd_noise->noise(hwid,igain);
    else          
    {
      m_RMSpedestal = m_dd_pedestal->pedestalRMS(hwid,igain);
      if(m_RMSpedestal>(1.0+LArElecCalib::ERRORCODE)) 
	m_SigmaNoise = m_RMSpedestal;
      else
      {     
	//	MsgStream log(msgSvc(), name());
	//	log << MSG::WARNING << function_name
	//	    <<" PedestalRMS vector empty for "
	//	    <<m_lar_em_id->show_to_string(id)<<" at gain "<<igain<<endmsg;     
        m_SigmaNoise = 0.;
      }
    }
    if(PRINT) std::cout<<"m_SigmaNoise(inADC)="<<m_SigmaNoise<<std::endl;
  }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //AUTOCORR
  if(m_retrieve[iAUTOCORR])
  {
    m_AutoCorr = m_dd_acorr->autoCorr(hwid,igain);
    ////////// 
    if(PRINT) {
      std::cout<<"AutoCorr= ";
      for(float i : m_AutoCorr)                
	std::cout<<i<<" ";
      std::cout<<std::endl;
    }
  }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //OFC
  if(m_retrieve[iOFC])
  {
    m_OFC = m_OFCTool->OFC_a(hwid, igain) ;
    /////////
    if(PRINT) {
      std::cout<<"OFC= ";
      for(float i : m_OFC) 
	std::cout<<i<<" ";
      std::cout<<"  Nminbias="<<Nminbias<<std::endl;
    }
  }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //SHAPE
  if(m_retrieve[iSHAPE])
  {
    m_Shape = m_dd_shape->Shape(hwid,0);
    //////////
    if(PRINT) {
      std::cout<<"Shape= ";
      for(float i : m_Shape) 
	std::cout<<i<<" ";
      std::cout<<std::endl;
    }
  }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //MinimumBias RMS 
  if(m_retrieve[iMINBIASRMS])
  {
    m_MinBiasRMS = m_dd_minbias->minBiasRMS(hwid);
    if(PRINT) std::cout<<"MinBiasRMS="<<m_MinBiasRMS<<std::endl;
  }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //SAMPLING FRACTION
  if(m_retrieve[iFSAMPL])
  {
    m_fSampl = m_dd_fsampl->FSAMPL(hwid);
    if(PRINT) std::cout<<"fSampl="<<m_fSampl<<std::endl;
  }

  return this->checkCellDatabase(id,igain,function_name);
}

//////////////////////////////////////////////////

StatusCode
CaloNoiseTool::checkCellDatabase(const Identifier & id, int igain,
				 std::string function_name)
{
  StatusCode StatusDatabase=StatusCode::SUCCESS;

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //ADC2MEV
  if(m_retrieve[iADC2MEV]) {
    if(fabs(m_Adc2MeVFactor)<0.000001) {
      StatusDatabase=StatusCode::FAILURE;
      if(m_diagnostic[igain]) 
	this->updateDiagnostic(0,"m_Adc2MeVFactor=0",igain);
    }
  }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //SIGMANOISE
  if(m_retrieve[iSIGMANOISE]) {
    if(fabs(m_SigmaNoise)<0.000001) {
      StatusDatabase=StatusCode::FAILURE;
      if(m_diagnostic[igain]) 
	this->updateDiagnostic(1,"m_SigmaNoise=0",igain);	
    }
  }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //AUTOCORR
  if(m_retrieve[iAUTOCORR])
  {
    if (!m_AutoCorr.valid()) 
    {
      ATH_MSG_WARNING( function_name
                       <<" AutoCorr invalid for "
                       <<m_lar_em_id->show_to_string(id)<<" at gain "<<igain );
      StatusDatabase=StatusCode::FAILURE;
    }
    if (m_AutoCorr.size()==0) 
    {
      //      MsgStream log(msgSvc(), name());
      //      log << MSG::WARNING << function_name
      //	  <<"  AutoCorr vector empty for "
      //	  <<m_lar_em_id->show_to_string(id)<<" at gain "<<igain<<endmsg;
      StatusDatabase=StatusCode::FAILURE;
      if(m_diagnostic[igain]) this->updateDiagnostic(2,"AC empty",igain);
    }
    /* // autocorr can be null (and it is for low-gain !), so allow it !
    else            
      if(m_diagnostic[igain])
      {
	unsigned int n_ACnull=0;
	for(unsigned int i=0;i<(*m_AutoCorr).size();++i)
	  if(fabs((*m_AutoCorr)[i])<0.000001) ++n_ACnull;
	if(n_ACnull==(*m_AutoCorr).size()) 
	  this->updateDiagnostic(3,"AC=0",igain);
      }
    */
    m_nsamples=m_AutoCorr.size()+1;
  }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //OFC
  if(m_retrieve[iOFC])
  {
    if (!m_OFC.valid()) 
    {
      ATH_MSG_WARNING( function_name
                       <<"  OFC pointer null for "
                       <<m_lar_em_id->show_to_string(id)<<" at gain "<<igain );
      StatusDatabase=StatusCode::FAILURE;
    }
    if (m_OFC.size()==0) 
    {
      //      MsgStream log(msgSvc(), name());
      //      log<<MSG::WARNING<< function_name
      //	 <<"  OFC vector empty for "
      //	 <<m_lar_em_id->show_to_string(id)<<" at gain "<<igain<<endmsg;
      StatusDatabase=StatusCode::FAILURE;
      if(m_diagnostic[igain]) this->updateDiagnostic(4,"OFC empty",igain);
    }
    else
      if(m_diagnostic[igain]) 
      {
	unsigned int n_OFCnull=0;
	for(float i : m_OFC)
	  if(fabs(i)<0.000001) ++n_OFCnull;
	if(n_OFCnull==m_OFC.size()) this->updateDiagnostic(5,"OFC=0",igain);
      }
    m_nsamples=m_OFC.size();
  }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //SHAPE
  if(m_retrieve[iSHAPE])
  {
    if (!m_Shape.valid()) 
    {
      ATH_MSG_WARNING( function_name
                       <<"  Shape pointer null -> PileUp will be 0 for "
                       <<m_lar_em_id->show_to_string(id) );
      StatusDatabase=StatusCode::FAILURE;
    }
    if (m_Shape.size()==0) 
    {      
      //      MsgStream log(msgSvc(), name());
      //      log<<MSG::WARNING<< function_name
      //       <<"  Shape vector empty -> PileUp will be 0 for "
      //       <<m_lar_em_id->show_to_string(id)<<endmsg;
      StatusDatabase=StatusCode::FAILURE;
      if(m_diagnostic[igain]) this->updateDiagnostic(6,"Shape empty",igain);
    }
    else
      if(m_diagnostic[igain]) 
      {
	unsigned int n_SHAPEnull=0;
	for(float i : m_Shape)
	  if(fabs(i)<0.000001) ++n_SHAPEnull;
	if(n_SHAPEnull==m_Shape.size()) 	  
	  this->updateDiagnostic(7,"Shape=0",igain);
      }	
  }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //NSAMPLES
  if(m_retrieve[iOFC] && m_retrieve[iAUTOCORR] 
     && m_OFC.size()!=m_AutoCorr.size()+1)
  {
    m_nsamples=std::min(m_OFC.size(),m_AutoCorr.size()+1);
    ATH_MSG_DEBUG("AutoCorr and OFC vectors have not the same "
                  <<"number of elements"
                  <<" ("<<m_AutoCorr.size()<<"/"<<m_OFC.size()
                  <<" ) => will take into account only " << m_nsamples << " samples !"
                  );
  }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  if(m_retrieve[iOFC] && m_retrieve[iSHAPE] && m_OFC.size()==m_Shape.size())
  {
    float scalar=0;
    for(unsigned int i=0;i<m_Shape.size();++i)
      scalar+=m_Shape[i]*m_OFC[i];
    if((scalar-1)>0.05)
      this->updateDiagnostic(8,"[Shape].[OFC] not 1",igain);
  }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //SAMPLING FRACTION
  if(m_retrieve[iFSAMPL])
  {
    if (m_fSampl<0.000001) 
    {
      ATH_MSG_WARNING( function_name
                       <<"  fSampl null -> PileUp will be 0 for "
                       <<m_lar_em_id->show_to_string(id) );
      StatusDatabase=StatusCode::FAILURE;
    }
  }

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  return StatusDatabase;
}

//////////////////////////////////////////////////
// functions for the getNoise interface
float
CaloNoiseTool::getNoise(const CaloDetDescrElement* caloDDE, CalorimeterNoiseType type)
{
  float Nminbias=-1;
  if( m_Nminbias>0 )
    Nminbias=m_Nminbias;
  
  CaloGain::CaloGain igain = m_highestGain[caloDDE->getSubCalo()];
  if( m_gain_from_joboption>=0 &&
      (type == ICalorimeterNoiseTool::ELECTRONICNOISE ||
       type == ICalorimeterNoiseTool::PILEUPNOISE ||
       type == ICalorimeterNoiseTool::TOTALNOISE ))
    igain = static_cast<CaloGain::CaloGain>(m_gain_from_joboption);
  
  switch(type) {
  case ICalorimeterNoiseTool::JOBOPTION:
    return (this->*m_CachedGetNoiseCDDE)(caloDDE,igain,Nminbias,
					 ICaloNoiseToolStep::CELLS);
    break;
  case ICalorimeterNoiseTool::ELECTRONICNOISE_HIGHESTGAIN:
  case ICalorimeterNoiseTool::ELECTRONICNOISE:
    return elecNoiseRMS(caloDDE,igain,Nminbias,
			ICaloNoiseToolStep::CELLS);
    break;
  case ICalorimeterNoiseTool::PILEUPNOISE_HIGHESTGAIN:
  case ICalorimeterNoiseTool::PILEUPNOISE:
    return pileupNoiseRMS(caloDDE,igain,Nminbias,
			  ICaloNoiseToolStep::CELLS);
    break;
  case ICalorimeterNoiseTool::TOTALNOISE_HIGHESTGAIN:
  case ICalorimeterNoiseTool::TOTALNOISE:
    return totalNoiseRMS(caloDDE,igain,Nminbias,
			 ICaloNoiseToolStep::CELLS);
    break;
    
  default:
    return 0.;
    break;
  }
}

float
CaloNoiseTool::getNoise(const CaloCell* caloCell, CalorimeterNoiseType type)
{
  const CaloDetDescrElement* caloDDE = caloCell->caloDDE();
  float Nminbias=-1;
  if( m_Nminbias>0 )
    Nminbias=m_Nminbias;
  
  CaloGain::CaloGain igain = caloCell->gain();
  
  if( m_gain_from_joboption>=0 &&
      (type == ICalorimeterNoiseTool::ELECTRONICNOISE ||
       type == ICalorimeterNoiseTool::PILEUPNOISE ||
       type == ICalorimeterNoiseTool::TOTALNOISE ))
    igain = static_cast<CaloGain::CaloGain>(m_gain_from_joboption);
  
  switch(type) {
  case ICalorimeterNoiseTool::JOBOPTION:
    return (this->*m_CachedGetNoiseCELL)(caloCell,igain,Nminbias,
					 ICaloNoiseToolStep::CELLS);
    break;
  case ICalorimeterNoiseTool::ELECTRONICNOISE_HIGHESTGAIN:
    // overwrite iGain with highest gain for that detector and
    // continue into normal code (NO break !!)
    igain=m_highestGain[caloDDE->getSubCalo()];
    /* FALLTHROUGH */
  case ICalorimeterNoiseTool::ELECTRONICNOISE:
    return elecNoiseRMS(caloDDE,igain,Nminbias,
			ICaloNoiseToolStep::CELLS);
    break;
    
  case ICalorimeterNoiseTool::PILEUPNOISE_HIGHESTGAIN:
    // overwrite iGain with highest gain for that detector and
    // continue into normal code (NO break !!)
    igain=m_highestGain[caloDDE->getSubCalo()];
    //  NOTE: igain is not used at the moment for pileupnoise
    /* FALLTHROUGH */
  case ICalorimeterNoiseTool::PILEUPNOISE:
    return pileupNoiseRMS(caloDDE,igain,Nminbias,
			  ICaloNoiseToolStep::CELLS);
    break;
    
  case ICalorimeterNoiseTool::TOTALNOISE_HIGHESTGAIN:
    // overwrite iGain with highest gain for that detector and
    // continue into normal code (NO break !!)
    igain=m_highestGain[caloDDE->getSubCalo()];
    //  NOTE: igain is only used for the electronics noise at the moment
    /* FALLTHROUGH */
  case ICalorimeterNoiseTool::TOTALNOISE:
    return totalNoiseRMS(caloDDE,igain,Nminbias,
			 ICaloNoiseToolStep::CELLS);
    break;
    
  default:
    return 0.;
    break;
  }
}

//////////////////////////////////////////////////

void
CaloNoiseTool::updateDiagnostic(int ireason,std::string nameReason,int igain)
{
  int nTmp=m_nCellsWithProblem[igain];
  if (nTmp >= 5000) return;
  int nr = m_nReason[nTmp][igain];
  if (nr >= 10) return;
  m_reason[nTmp][nr][igain]=ireason;
  m_reasonName[ireason]=nameReason;
  ++m_nReason[nTmp][igain];
  ++m_itReason[ireason][igain]; 
  m_noiseOK=false;
  //std::cout<<nTmp<<" "<<n_reason[nTmp][igain]<<" "
  //         <<it_reason[ireason][igain]<<std::endl;
}

//////////////////////////////////////////////////


//========================  USER INTERFACES ===================================


//////////////////////////////////////////////////

float 
CaloNoiseTool::elecNoiseRMS(const CaloCell* caloCell, 
		 	    const int step)
{
  const CaloDetDescrElement* caloDDE = caloCell->caloDDE();
  //CaloGain::CaloGain igain = this->estimatedGain(caloCell,caloDDE,step);
  CaloGain::CaloGain igain = caloCell->gain();
  return this->elecNoiseRMS(caloDDE,igain,-1,step);  
}

//////////////////////////////////////////////////

float 
CaloNoiseTool::elecNoiseRMS(const CaloCell* caloCell, 
			    const float Nminbias, const int step)
{
  const CaloDetDescrElement* caloDDE = caloCell->caloDDE();
  //CaloGain::CaloGain igain = this->estimatedGain(caloCell,caloDDE,step);
  CaloGain::CaloGain igain = caloCell->gain();
  return this->elecNoiseRMS(caloDDE,igain,Nminbias,step);  
}

//////////////////////////////////////////////////

float
CaloNoiseTool::elecNoiseRMSHighestGain(const CaloCell* caloCell, 
				       const int step)
{    
  return this->elecNoiseRMSHighestGain(caloCell->caloDDE(),step);
}

//////////////////////////////////////////////////

float
CaloNoiseTool::elecNoiseRMSHighestGain(const CaloCell* caloCell, 
				       const float Nminbias, const int step)
{   
  //WARNING: this function is almost 200 times slower than the previous one
  return this->elecNoiseRMSHighestGain(caloCell->caloDDE(),Nminbias,step);
}

//////////////////////////////////////////////////

float
CaloNoiseTool::elecNoiseRMSHighestGain(const CaloDetDescrElement* caloDDE, 
				       const int step)
{    
  CaloCell_ID::SUBCALO iCalo = caloDDE->getSubCalo();
  CaloGain::CaloGain highestGain=m_highestGain[iCalo];
  return this->elecNoiseRMS(caloDDE,highestGain,-1,step);    
}

//////////////////////////////////////////////////

float
CaloNoiseTool::elecNoiseRMSHighestGain(const CaloDetDescrElement* caloDDE, 
				       const float Nminbias, const int step)
{    
  //CaloCell_ID::SUBCALO iCalo = this->caloNum(caloDDE->calo_hash());
  CaloCell_ID::SUBCALO iCalo = caloDDE->getSubCalo();
  CaloGain::CaloGain highestGain=m_highestGain[iCalo];
  return this->elecNoiseRMS(caloDDE,highestGain,Nminbias,step);    
}

//////////////////////////////////////////////////

float 
CaloNoiseTool::elecNoiseRMS(const CaloDetDescrElement* caloDDE, 
			    const CaloGain::CaloGain gain,
			    const float Nminbias, 
			    const int step)
// << base class >>
{  

  if(!m_cacheValid){
    ATH_MSG_DEBUG( " in elecNoiseRMS, cache not valid yet" );
    StatusCode sc = this->initData();
    if (sc.isFailure()) {
      throw LArConditionsException("Could not compute elecNoiseRMS ");
    }
  }


  float sigma=0.;

  const IdentifierHash idCaloHash = caloDDE->calo_hash();
  //CaloCell_ID::SUBCALO iCalo = this->caloNum(idCaloHash);
  CaloCell_ID::SUBCALO iCalo = caloDDE->getSubCalo();
  int index=this->index(idCaloHash);

  
  int igain=static_cast<int>(gain);//for LAr  
  if(iCalo==CaloCell_ID::TILE)     //for Tile 
  {
    CaloGain::CaloGain convertedGain;
    switch(gain)
    {
  //convert Tile gain into LAr gain (the one used to store the noise in arrays)
    case CaloGain::TILEHIGHHIGH : convertedGain=CaloGain::LARHIGHGAIN;   break;
    case CaloGain::TILEHIGHLOW :  convertedGain=CaloGain::LARMEDIUMGAIN; break;
    case CaloGain::TILELOWHIGH :  convertedGain=CaloGain::LARMEDIUMGAIN; break;
    case CaloGain::TILELOWLOW :   convertedGain=CaloGain::LARLOWGAIN;    break;
    case CaloGain::TILEONEHIGH :  convertedGain=CaloGain::LARHIGHGAIN;   break;
    case CaloGain::TILEONELOW :   convertedGain=CaloGain::LARLOWGAIN;    break;
    default: convertedGain = CaloGain::INVALIDGAIN;
    }
    igain=static_cast<int>(convertedGain);
  } 

  if (gain==CaloGain::INVALIDGAIN || gain==CaloGain::UNKNOWNGAIN) {
    ATH_MSG_WARNING( " ask noise for invalid/unknown gain, will return noise for high gain "  );
    igain=static_cast<int>(CaloGain::LARHIGHGAIN);
  }

  if (iCalo<0 || index<0)
  {
    ATH_MSG_WARNING( "CaloNoiseTool::elecNoiseRMS  wrong id ! " 
                     << "iCalo="<<iCalo
                     << "index="<<index
                     << "id:" << m_lar_em_id->show_to_string(caloDDE->identify())
                     );
    return 0.;
  } 
  else 
  {         
    const std::vector<float>* sigmaVector = nullptr;
    if(step==ICaloNoiseToolStep::CELLS)	
      sigmaVector = &m_elecNoiseCELLContainer[index];
    else if(step==ICaloNoiseToolStep::RAWCHANNELS)    
      sigmaVector = &m_elecNoiseRAWContainer[index];
    else 
    {
      ATH_MSG_WARNING( "CaloNoiseTool::elecNoiseRMS  wrong step !!!  "
                       <<step );
      return 0.;
    }

    bool retry=true;
    int shift_gain=0;
    int gain_wanted=igain;    
    if(m_WorkMode==0) gain_wanted=2-igain;
    int gain_shifted=gain_wanted;

    while(retry)
    {      
      //:::::::::::::::::
      retry=false;
      //:::::::::::::::::
      if(m_WorkMode==0)      gain_shifted=gain_wanted+shift_gain;
      else if(m_WorkMode==1) gain_shifted=gain_wanted-shift_gain;
      //:::::::::::::::::
      sigma = (*sigmaVector)[gain_shifted];
      //:::::::::::::::::
      if(m_WorkMode==1 && Nminbias>=0 && Nminbias!=m_Nminbias_usedForCache &&
	 iCalo != CaloCell_ID::TILE )
      {
	sigma = this->calculateElecNoiseForLAR(idCaloHash,
                                               this->caloNum(idCaloHash),
                                               Nminbias) [gain_shifted];
// 	if(m_Geant3 && step==ICaloNoiseToolStep::CELLS) 
// 	  sigma = sigma*m_scaleContainer[index];
      }  
      //:::::::::::::::::
      if(this->isBadValue(sigma)) 
      {
	++shift_gain;
	if(shift_gain<=igain) retry=true;
	ATH_MSG_WARNING( "noise is missing for this cell " << m_lar_em_id->show_to_string(caloDDE->identify())
                         << " at this gain ("
                         <<gain_wanted<<"), return the noise at next gain ("
                         <<gain_shifted<<")" );
      }
      //:::::::::::::::::
    }
    return sigma;   
  }  
}

//////////////////////////////////////////////////

std::vector<float> 
CaloNoiseTool::elecNoiseRMS3gains(const CaloDetDescrElement* caloDDE, 
				  const int step)
{  
  std::vector<float> sigma;
  sigma.reserve (CaloGain::LARNGAIN);

  for(int igain=0;igain<CaloGain::LARNGAIN;++igain)
    sigma.push_back(this->elecNoiseRMS(caloDDE,
				       static_cast<CaloGain::CaloGain>(igain),
				       -1,
				       step));
  return sigma;
}

//////////////////////////////////////////////////

std::vector<float> 
CaloNoiseTool::elecNoiseRMS3gains(const CaloDetDescrElement* caloDDE, 
				  const float NMinBias,const int step)
{  
  std::vector<float> sigma;
  sigma.reserve (CaloGain::LARNGAIN);
  for(int igain=0;igain<CaloGain::LARNGAIN;++igain)
    sigma.push_back(this->elecNoiseRMS(caloDDE,
				       static_cast<CaloGain::CaloGain>(igain),
				       NMinBias,
				       step));
  for(int igain=0;igain<CaloGain::LARNGAIN;++igain)
    if(this->isBadValue(sigma[igain]) && 
       igain!=CaloGain::LARHIGHGAIN) 
      sigma[igain]=sigma[igain-1];//take the next gain (low->medium->high)
  return sigma;
}

//////////////////////////////////////////////////

VectorContainer* 
CaloNoiseTool::elecNoiseRMSContainer(const int &iCalo) 
{
  ATH_MSG_WARNING( " elecNoiseRMSContainer not implemented " << iCalo  );
  //return &m_elecNoiseContainer_old[iCalo];
  return nullptr;
}

//////////////////////////////////////////////////
// new interfaces to enable caching of the pointer to member

float 
CaloNoiseTool::elecNoiseRMS(const CaloCell* theCell,
			    const CaloGain::CaloGain /*gain*/,
			    const float Nminbias, 
			    const int step) {
  return this->elecNoiseRMS(theCell,Nminbias,step);
}

float 
CaloNoiseTool::pileupNoiseRMS(const CaloCell* theCell,
			      const CaloGain::CaloGain /*gain*/,
			      const float Nminbias, 
			      const int /*step*/) {
  return this->pileupNoiseRMS(theCell,Nminbias);
}

float 
CaloNoiseTool::totalNoiseRMS(const CaloCell* theCell,
			     const CaloGain::CaloGain gain,
			     const float Nminbias, 
			     const int step) {
  float elecNoiseRMS_tmp   = this->elecNoiseRMS(theCell,gain,Nminbias,step);
  float pileupNoiseRMS_tmp = this->pileupNoiseRMS(theCell,gain,Nminbias,step);
  
  if(elecNoiseRMS_tmp>=0)
    return sqrt((elecNoiseRMS_tmp*elecNoiseRMS_tmp) + (pileupNoiseRMS_tmp*pileupNoiseRMS_tmp));
  return(-1);
}

float 
CaloNoiseTool::pileupNoiseRMS(const CaloDetDescrElement* caloDDE, 
			      const CaloGain::CaloGain /*gain*/,
			      const float Nminbias, 
			      const int /*step*/) {
  return this->pileupNoiseRMS(caloDDE,Nminbias);
}

float 
CaloNoiseTool::totalNoiseRMS(const CaloDetDescrElement* caloDDE, 
			     const CaloGain::CaloGain gain,
			     const float Nminbias, 
			     const int step) {
  float elecNoiseRMS_tmp   = this->elecNoiseRMS(caloDDE,gain,Nminbias,step);
  float pileupNoiseRMS_tmp = this->pileupNoiseRMS(caloDDE,gain,Nminbias,step);
  
  return sqrt((elecNoiseRMS_tmp*elecNoiseRMS_tmp) + (pileupNoiseRMS_tmp*pileupNoiseRMS_tmp) );
}
//////////////////////////////////////////////////

float 
CaloNoiseTool::pileupNoiseRMS(const CaloCell* caloCell, const float Nminbias)
{
  const CaloDetDescrElement* caloDDE = caloCell->caloDDE();
  return this->pileupNoiseRMS(caloDDE,Nminbias);
}

//////////////////////////////////////////////////

float 
CaloNoiseTool::pileupNoiseRMS(const CaloDetDescrElement* caloDDE, 
			      const float Nminbias)
// << base class >>
{


  if(!m_cacheValid){
    StatusCode sc = this->initData();
    if (sc.isFailure()) {
      throw LArConditionsException("Could not compute pileupNoiseRMS ");
    }
  }

  const IdentifierHash idCaloHash = caloDDE->calo_hash();
  CaloCell_ID::SUBCALO iCalo = this->caloNum(idCaloHash);
  
  if(iCalo!=CaloCell_ID::TILE)
  {
    int index=this->index(idCaloHash);
    float PileUp;
    // check if noise stored in container was calcualted with this Nminbias
    if ((Nminbias==m_Nminbias_usedForCache) || (Nminbias<=0)) //default 
      PileUp=m_pileupNoiseContainer[index];
    else
      PileUp=this->calculatePileUpNoise(idCaloHash,Nminbias);//slower !!
    return PileUp;
  }
  else//TILE
  {
    if(m_Nmessages_forTilePileUp<10)// to not have thousands of messages
    {
      ATH_MSG_INFO("CaloNoiseTool::pileupNoiseRMS : NOT IMPLEMENTED "
                   <<"for TILEs ! (-> returns 0. for the moment)" );
      ++m_Nmessages_forTilePileUp;
    }
    return 0.;   
  }
}

//////////////////////////////////////////////////

float 
CaloNoiseTool::totalNoiseRMS(const CaloCell* caloCell, const float Nminbias) 
{
  CaloGain::CaloGain igain = caloCell->gain();
  return this->totalNoiseRMS(caloCell->caloDDE(),igain,Nminbias);
}

//////////////////////////////////////////////////

float 
CaloNoiseTool::totalNoiseRMS(const CaloDetDescrElement* caloDDE, 
			     const CaloGain::CaloGain gain, 
			     const float Nminbias)
{
  float elecNoiseRMS_tmp   = this->elecNoiseRMS(caloDDE,gain,Nminbias,
						ICaloNoiseToolStep::CELLS);
  float pileupNoiseRMS_tmp = this->pileupNoiseRMS(caloDDE,Nminbias);
  
  float totalNoiseRMS = -1;
  
  // checks that elecNoise is valid
  if(elecNoiseRMS_tmp>0) 
    totalNoiseRMS= sqrt((elecNoiseRMS_tmp*elecNoiseRMS_tmp) + (pileupNoiseRMS_tmp*pileupNoiseRMS_tmp) );
  
  return totalNoiseRMS;
}

//////////////////////////////////////////////////

float 
CaloNoiseTool::totalNoiseRMSHighestGain(const CaloCell* caloCell, 
					const float Nminbias)
{
  return this->totalNoiseRMSHighestGain(caloCell->caloDDE(),Nminbias);
}

//////////////////////////////////////////////////

float 
CaloNoiseTool::totalNoiseRMSHighestGain(const CaloDetDescrElement* caloDDE, 
					const float Nminbias)
{
  //CaloCell_ID::SUBCALO iCalo = caloNum(caloDDE->calo_hash());
  CaloCell_ID::SUBCALO iCalo = caloDDE->getSubCalo();
  CaloGain::CaloGain highestGain=m_highestGain[iCalo];
  return this->totalNoiseRMS(caloDDE,highestGain,Nminbias);
}


//////////////////////////////////////////////////
//////////////////////////////////////////////////

CaloGain::CaloGain 
CaloNoiseTool::estimatedGain(const CaloCell* caloCell,
			     const int &step)
{
  return this->estimatedGain(caloCell,caloCell->caloDDE(),step);
}

//////////////////////////////////////////////////

CaloGain::CaloGain
CaloNoiseTool::estimatedGain(const CaloCell* caloCell, 
			     const CaloDetDescrElement* caloDDE,
			     const int &step)
{
  //CaloCell_ID::SUBCALO iCalo = caloNum(caloDDE->calo_hash());
  CaloCell_ID::SUBCALO iCalo = caloDDE->getSubCalo();

  if(iCalo==CaloCell_ID::LAREM || 
     iCalo==CaloCell_ID::LARHEC || 
     iCalo==CaloCell_ID::LARFCAL) 
    return this->estimatedLArGain(iCalo,caloDDE,caloCell->energy(),step);
  else if(iCalo==CaloCell_ID::TILE)
    return this->estimatedTileGain(caloCell,caloDDE,step);
  else
  {
    ATH_MSG_WARNING("CaloNoiseTool::estimatedGain  wrong id ! " 
                    <<m_lar_em_id->show_to_string(caloDDE->identify()) );
    return CaloGain::INVALIDGAIN;
  } 
}

//////////////////////////////////////////////////

CaloGain::CaloGain 
CaloNoiseTool::estimatedGain(const CaloDetDescrElement* caloDDE,
			     const float &energy, 			     
			     const int &step)
{
  //CaloCell_ID::SUBCALO iCalo = caloNum(caloDDE->calo_hash());
  CaloCell_ID::SUBCALO iCalo = caloDDE->getSubCalo();

  if(iCalo==CaloCell_ID::LAREM || 
     iCalo==CaloCell_ID::LARHEC || 
     iCalo==CaloCell_ID::LARFCAL) 
    return this->estimatedLArGain(iCalo,caloDDE,energy,step);
  else if(iCalo==CaloCell_ID::TILE)
  {
    ATH_MSG_WARNING("CaloNoiseTool::estimatedGain  NOT IMPLEMENTED FOR TILE "
                    <<"with these arguments! " 
                    << m_lar_em_id->show_to_string(caloDDE->identify()) );
    return CaloGain::INVALIDGAIN;
  } 
  else
  {
    ATH_MSG_WARNING("CaloNoiseTool::estimatedGain  wrong id ! " 
                    << m_lar_em_id->show_to_string(caloDDE->identify()) );
    return CaloGain::INVALIDGAIN;
  } 
}

//////////////////////////////////////////////////

CaloGain::CaloGain 
CaloNoiseTool::estimatedLArGain(const CaloCell_ID::SUBCALO &iCalo,
                                const CaloDetDescrElement* caloDDE,
				const float &energy, 
				const int &step)
{
  //returns the gain of a cell according to the energy 
  //(converts in ADC counts in MediumGain and uses thresholds in ADC counts 
  //to determine the gain)

  //  the function could be called before (step=0, RawChannels) 
  //  or after  (step=1, Cells) LArG3Escale

  CaloGain::CaloGain igain=CaloGain::INVALIDGAIN; 

  //--EM/HEC--
  if( iCalo==CaloCell_ID::LAREM || iCalo==CaloCell_ID::LARHEC ||
     (iCalo==CaloCell_ID::LARFCAL && m_WorkMode==1) )
  {     
    //We choose the gain in applying thresholds on the energy 
    //converted in ADC counts in MediumGain (index "1" of adc2mev(id,1)).
    //Indeed, thresholds in ADC counts are defined with respect 
    //to the MediumGain.
    //
    //              1300              3900 
    // ---------------|----------------|-------------> ADC counts in MediumGain
    //    HighGain  <---  MediumGain  --->  LowGain

    float adc=0.;
    
    //Cells (with E scale and weights from LArG3Escale)
    if(step==ICaloNoiseToolStep::CELLS) 
      adc=energy/(this->adc2mev(caloDDE,CaloGain::LARMEDIUMGAIN)) + 1000 ;
    //RawChannels    
    else if(step==ICaloNoiseToolStep::RAWCHANNELS) 
      adc=energy/(this->adc2mev(caloDDE,CaloGain::LARMEDIUMGAIN)) + 1000 ;
    else 
    {
      ATH_MSG_WARNING( "CaloNoiseTool::estimatedGain   wrong step" );
    }  
    if(adc<m_HighGainThresh[iCalo])     igain=CaloGain::LARHIGHGAIN;  
    else if(adc>m_LowGainThresh[iCalo]) igain=CaloGain::LARLOWGAIN; 
    else                                igain=CaloGain::LARMEDIUMGAIN;  
  }
  //--FCAL--
  else if(iCalo==CaloCell_ID::LARFCAL && m_WorkMode==0)
  { 
    if(energy<m_HighGainThresh[iCalo])     igain=CaloGain::LARHIGHGAIN; 
    else if(energy>m_LowGainThresh[iCalo]) igain=CaloGain::LARLOWGAIN; 
    else                                       igain=CaloGain::LARMEDIUMGAIN;
  }  
  
  return igain;
}

//////////////////////////////////////////////////

CaloGain::CaloGain
CaloNoiseTool::estimatedTileGain(const CaloCell* caloCell,
				 const CaloDetDescrElement* caloDDE,
				 const int &/*step*/)
{
  const TileCell * tileCell = (TileCell *)caloCell;
  //double eneTot = tileCell->energy();

    // threshold (1023 counts) is the same for all channels
  double threshold = m_tileInfo->ADCmax();

  static const TileHWID * tileHWID = TileCablingService::getInstance()->getTileHWID();
  static const IdContext chContext = tileHWID->channel_context();
  HWIdentifier hwid1;
  
  tileHWID->get_id(caloDDE->onl1(), hwid1, &chContext ); // pmt id
  hwid1 = tileHWID->adc_id(hwid1, TileHWID::HIGHGAIN); // high gain ADC id
  
  unsigned int drawerIdx1(0), channel1(0), adc1(0);
  m_tileIdTransforms->getIndices(hwid1, drawerIdx1, channel1, adc1);

  // first PMT, convert energy to ADC counts
  double amplitude1 = tileCell->ene1();
  amplitude1 /= m_tileToolEmscale->channelCalib(drawerIdx1, channel1, adc1, 1.0,
                                          TileRawChannelUnit::ADCcounts,
                                          TileRawChannelUnit::MegaElectronVolts);

  double pedestal1 = m_tileToolNoiseSample->getPed(drawerIdx1, channel1, adc1);

  int igain1;

  if (amplitude1 + pedestal1 < threshold ) {
    igain1 = TileID::HIGHGAIN;
  } else {
    igain1 = TileID::LOWGAIN;
  }

  // second PMT, if it exists
  if (caloDDE->onl2() !=  TileID::NOT_VALID_HASH ) {

    HWIdentifier hwid2;
    tileHWID->get_id(caloDDE->onl2(), hwid2, &chContext ); // pmt id
    hwid2 = tileHWID->adc_id(hwid2, TileHWID::HIGHGAIN); // high gain ADC id

    unsigned int drawerIdx2(0), channel2(0), adc2(0);
    m_tileIdTransforms->getIndices(hwid2, drawerIdx2, channel2, adc2);

    // first PMT, convert energy to ADC counts
    double amplitude2 = tileCell->ene2();
    amplitude2 /= m_tileToolEmscale->channelCalib(drawerIdx2, channel2, adc2, 1.0,
                                            TileRawChannelUnit::ADCcounts,
                                            TileRawChannelUnit::MegaElectronVolts);

    double pedestal2 = m_tileToolNoiseSample->getPed(drawerIdx2, channel2, adc2);

    if (amplitude2 + pedestal2 < threshold) {
      // igain2 high
      return igain1 == TileID::LOWGAIN ? CaloGain::TILEHIGHLOW : CaloGain::TILEHIGHHIGH;
    } else {
      // igain2 low
      return igain1 == TileID::LOWGAIN ? CaloGain::TILELOWLOW : CaloGain::TILEHIGHLOW;
    }
    // nb. Having HIGHLOW in two places seems wrong, but that's what the
    // original code was doing.
  } 

  // igain2 doesn't exist.
  return igain1 == TileID::LOWGAIN ? CaloGain::TILEONELOW : CaloGain::TILEONEHIGH;
}

//////////////////////////////////////////////////

float 
CaloNoiseTool::adc2mev(const CaloDetDescrElement* caloDDE,
		       const CaloGain::CaloGain gain)
{

  if(!m_cacheValid){
    StatusCode sc = this->initData();
    if (sc.isFailure()) {
      throw LArConditionsException("Could not compute adc2mev ");
    }
  }


  float factor=1.;
  IdentifierHash idCaloHash = caloDDE->calo_hash();
  CaloCell_ID::SUBCALO iCalo = this->caloNum(idCaloHash);
 
  if(iCalo==CaloCell_ID::LAREM || iCalo==CaloCell_ID::LARHEC)
  {      
    int index=this->index(idCaloHash);
    if(m_WorkMode==0)
      factor=(m_adc2mevContainer[index])[2-gain];
    else if(m_WorkMode==1)
      factor=(m_adc2mevContainer[index])[gain];
  }
  else if(iCalo==CaloCell_ID::LARFCAL)
  {
    if(m_WorkMode==0)
    {
      ATH_MSG_WARNING("CaloNoiseTool::adc2mev(id,gain) : NOT IMPLEMENTED !"
                      <<"for FCAL (-> returns 5000. for the moment)" );
      //NOT CLEAR what we should return 
      //(for the moment, returns AdcPerGev from LArDigitMaker)
      factor=m_AdcPerMev;
    }
    else if(m_WorkMode==1)
    {
      int index=this->index(idCaloHash);
      factor=(m_adc2mevContainer[index])[gain];
    }
  }  
  else if(iCalo==CaloCell_ID::TILE)
  {
    //TILE_PART
    ATH_MSG_WARNING("CaloNoiseTool::adc2mev(id,gain) : NOT IMPLEMENTED !"
                    <<"for TILE (-> returns 1. for the moment)" );
    factor=1.; 
  }  
  else
  {
    ATH_MSG_WARNING("CaloNoiseTool::adc2mev(id,gain)  wrong id ! " 
                    <<m_lar_em_id->show_to_string(caloDDE->identify()) );
    factor=0.;
  }  
  return factor;
}

//////////////////////////////////////////////////

float 
CaloNoiseTool::adc2mev(const Identifier& id,const CaloGain::CaloGain gain)
{
  return adc2mev(m_calo_dd_man->get_element(id),gain);
}



void CaloNoiseTool::handle(const Incident&) {
  ATH_MSG_DEBUG( "In Incident-handle"  );
  if (m_cacheValid) {
    ATH_MSG_DEBUG( "Cached data already computed."  );
    return;
  }
  
  StatusCode sc = this->initData();

  if (sc.isFailure()) {
    ATH_MSG_ERROR( "handle:  error with initData() "  );
    ATH_MSG_ERROR( "Failed to update CaloNoiseTool Cache at the begin of run."  );
    m_cacheValid=false;
  }
}

/////////////////////////////////////////////////////////////////////////

bool CaloNoiseTool::isEOverNSigma(const CaloCell* caloCell, float sigmaCut ,
                     CalorimeterNoiseSymmetryHandling symmetryHandling,
                     CalorimeterNoiseType type )
{
  float noise_cut = fabs(sigmaCut*this->getEffectiveSigma(caloCell, symmetryHandling, type));
  float energy = fabs(caloCell->e());
  return (energy>noise_cut);

}

////////////////////////////////////////////////////////////////////

float CaloNoiseTool::getRandomisedE(const CaloCell* caloCell , CLHEP::HepRandomEngine* engine, CalorimeterNoiseType type)
{
  double rms = this->getNoise(caloCell,type);
  return RandGauss::shoot(engine,0.,rms);
}

///////////////////////////////////////////////////////////////////////////

float CaloNoiseTool::getEffectiveSigma(const CaloCell* caloCell,
                     CalorimeterNoiseSymmetryHandling /*symmetryHandling*/,
                     CalorimeterNoiseType type )
{
     // simple implementation for gaussian noise
     return this->getNoise(caloCell,type);
}



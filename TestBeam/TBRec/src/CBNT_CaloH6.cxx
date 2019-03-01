/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################
//#                                                   #
//# File   : CBNT_Calo_H6.cxx                         #
//# Package: TBRec                                    #
//#                                                   #
//# Author : Marco Bieri <mbieri@sfu.ca>              #
//# Adopted for CBNTAA Pavol Strizenec                #
//#                                                   #
//#####################################################

#include "CBNT_CaloH6.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloInterface/ICaloNoiseTool.h"

#include "Identifier/Identifier.h"


CBNT_CaloH6::CBNT_CaloH6(const std::string & name, ISvcLocator * pSvcLocator) :
  CBNT_TBRecBase(name, pSvcLocator),
  m_emecID_help(0),
  m_hecID_help(0),
  m_fcalID_help(0),
  m_noiseTool("CaloNoiseTool/CaloNoiseToolDefault")
{
  declareProperty("Det_Type", m_det_type);
  declareProperty("NoiseToolName", m_noiseTool);
  declareProperty("Max_Chan", m_numchan);
  m_noiseSigma = 0;

  m_NoiseRMS = 0;
  m_energy = 0;
  m_eta = 0;
  m_ieta = 0;
  m_iphi = 0;
  m_phi = 0;
  m_region = 0;
  m_sampling = 0;
}

CBNT_CaloH6::~CBNT_CaloH6(){}

StatusCode CBNT_CaloH6::CBNT_initialize()
{ //initialize
  ATH_MSG_DEBUG( "initialize()" );
  
  m_hecID_help = NULL;
  m_emecID_help = NULL;
  m_fcalID_help = NULL;
  
  // // retrieve detector description manager for LAr subsystem
  // const DataHandle<LArDetDescrManager> m_larMgr;
  // sc = m_detStore->retrieve(m_larMgr);
  // if (sc.isFailure()) {
  //   log << MSG::ERROR << "unable to retrieve LArDetDescrManager from detecto\r store"<< endmsg;
  //   return sc;
  // }
  
  // Identifier helpers
  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );
  m_hecID_help = idHelper->hec_idHelper();
  if (!m_hecID_help) {
    ATH_MSG_ERROR ( "unable to obtain hec id " );
    return StatusCode::FAILURE;
  }

  m_emecID_help = idHelper->em_idHelper();
  if (!m_emecID_help) {
    ATH_MSG_ERROR ( "unable to obtain emec id " );
    return StatusCode::FAILURE;
  }
  
  m_fcalID_help =  idHelper->fcal_idHelper();
  if (!m_fcalID_help) {
    ATH_MSG_ERROR ( "unable to obtain fcal id " );
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Got to before p_toolSvc");
  
  ATH_CHECK( m_noiseTool.retrieve() );

  ATH_MSG_DEBUG ( "Before Booking Ntuple" );
  
  //hec, 241 channels
  //emec, 113 channels
  //fcal, 347 channels
  
  addBranch(m_det_type+"_energy",m_energy);
  addBranch(m_det_type+"_noiseRMS",m_NoiseRMS);
  addBranch(m_det_type+"_ieta",m_ieta);
  addBranch(m_det_type+"_iphi",m_iphi);
  addBranch(m_det_type+"_eta",m_eta);
  addBranch(m_det_type+"_phi",m_phi);
  addBranch(m_det_type+"_sampling",m_sampling);
  
  if(m_det_type=="HEC") {
    addBranch(m_det_type+"_region",m_region);
  } else m_region = 0;
  
  return StatusCode::SUCCESS;
  
} //initialize

/*-----------------------------------Execute-----------------------------*/

StatusCode CBNT_CaloH6::CBNT_execute()
{ //execute
  ATH_MSG_DEBUG ( "execute()");
  
  //Accesing the CaloCellContainer
  const CaloCellContainer* cellContainer;
  
  ATH_CHECK( evtStore()->retrieve(cellContainer) );
  
  CaloCellContainer::const_iterator ifirst = cellContainer->begin();
  CaloCellContainer::const_iterator ilast = cellContainer->end();
  
  for (; ifirst!=ilast; ifirst++){ //iterating
    
    //obtaining cell pointer
    const CaloCell* cell_ptr = *ifirst;
    
    //obtaining dde pointer
    const CaloDetDescrElement* caloDDE_ptr = cell_ptr->caloDDE();
    
    //obtaining SubCalo
    const CaloCell_ID::SUBCALO subcalo = caloDDE_ptr->getSubCalo();
    
    //obtaining sampling (layer)
    const CaloCell_ID::CaloSample sampling = caloDDE_ptr->getSampling();                                                
    if (m_det_type=="HEC" && subcalo==CaloCell_ID::LARHEC) { 
      
      float noiseSigma = m_noiseTool->elecNoiseRMS(cell_ptr);

      ATH_MSG_DEBUG("noiseRMS HEC"<<noiseSigma);
      
      const Identifier hecid=cell_ptr->ID();
      
      m_energy->push_back(cell_ptr->energy());
      m_NoiseRMS->push_back(m_noiseTool->elecNoiseRMS(cell_ptr));
      m_eta->push_back(caloDDE_ptr->eta());
      m_phi->push_back(caloDDE_ptr->phi());
      m_ieta->push_back(m_hecID_help->eta(hecid));
      m_iphi->push_back(m_hecID_help->phi(hecid));
      m_sampling->push_back(sampling);
      m_region->push_back(m_hecID_help->region(hecid));
    } 
    
    if (m_det_type=="EMEC" && subcalo==CaloCell_ID::LAREM) {
      
      float noiseSigma = m_noiseTool->elecNoiseRMS(cell_ptr);
      
      ATH_MSG_DEBUG("noiseRMS EMEC"<<noiseSigma);
      
      const Identifier emecid=cell_ptr->ID();
      
      m_energy->push_back(cell_ptr->energy());
      m_NoiseRMS->push_back(m_noiseTool->elecNoiseRMS(cell_ptr));
      m_eta->push_back(caloDDE_ptr->eta());
      m_phi->push_back(caloDDE_ptr->phi());
      m_ieta->push_back(m_emecID_help->eta(emecid));
      m_iphi->push_back(m_emecID_help->phi(emecid));
      m_sampling->push_back(sampling);
    }
    if (m_det_type=="FCAL" && subcalo==CaloCell_ID::LARFCAL) {
      
      float noiseSigma = m_noiseTool->elecNoiseRMS(cell_ptr);
      
      ATH_MSG_DEBUG("noiseRMS FCAL"<<noiseSigma);
      
      const Identifier fcalid=cell_ptr->ID();
      
      m_energy->push_back(cell_ptr->energy());
      m_NoiseRMS->push_back(m_noiseTool->elecNoiseRMS(cell_ptr));
      m_eta->push_back(caloDDE_ptr->eta());
      m_phi->push_back(caloDDE_ptr->phi());
      m_ieta->push_back(m_fcalID_help->eta(fcalid));
      m_iphi->push_back(m_fcalID_help->phi(fcalid));
      m_sampling->push_back(sampling);
    }
  }
  
  return StatusCode::SUCCESS;
  
} //execute

/*-----------------------------------Finalize-----------------------------*/

StatusCode CBNT_CaloH6::CBNT_finalize()
{ //finalize
  ATH_MSG_INFO ( "finalize()" );
  return StatusCode::SUCCESS;   
  
} //finalize

StatusCode CBNT_CaloH6::CBNT_clear()
{
 if(m_energy) m_energy->clear();
 if(m_NoiseRMS) m_NoiseRMS->clear();
 if(m_eta) m_eta->clear();
 if(m_phi) m_phi->clear();
 if(m_ieta) m_ieta->clear();
 if(m_iphi) m_iphi->clear();
 if(m_sampling) m_sampling->clear();
 if(m_region) m_region->clear();

 return StatusCode::SUCCESS;
}

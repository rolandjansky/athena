/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// algorithm to merge LArHit from different subevents from pileup service to
//   a single event, assuming all subevents are at the same time and neglecting the
//   individual hit times
//  This is aimed to run at first stage for very large luminosity pileup

#include "LArDigitization/LArHitFilter.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArSimEvent/LArHit.h"
#include "CaloIdentifier/LArID.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArDigitization/LArHitEMap.h"

// Pile up
#include "PileUpTools/PileUpMergeSvc.h"

LArHitFilter::LArHitFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator), m_larem_id(NULL),m_larhec_id(NULL),m_larfcal_id(NULL)
{
  m_SubDetectors      = "LAr_All"; 
  m_EmBarrelHitContainerName = "LArHitEMB";
  m_EmEndCapHitContainerName = "LArHitEMEC";
  m_HecHitContainerName      = "LArHitHEC";
  m_ForWardHitContainerName  = "LArHitFCAL";
  //
  // ........ declare the private data as properties
  //
  declareProperty("SubDetectors",m_SubDetectors,"subdetector selection");
  declareProperty("EmBarrelHitContainerName",m_EmBarrelHitContainerName,"Hit container name for EMB");
  declareProperty("EmEndCapHitContainerName",m_EmEndCapHitContainerName,"Hit container name for EMEC");
  declareProperty("HecHitContainerName",m_HecHitContainerName,"Hit container name for HEC");
  declareProperty("ForWardHitContainerName",m_ForWardHitContainerName,"Hit container name for FCAL");

  m_ecut[0][0]=0.05;
  m_ecut[0][1]=0.2;
  m_ecut[0][2]=0.1;
  m_ecut[0][3]=0.1;
  m_ecut[1][0]=0.05;
  m_ecut[1][1]=0.1;
  m_ecut[1][2]=0.05;
  m_ecut[1][3]=0.05;
  m_ecut[2][0]=0.05;
  m_ecut[2][1]=0.05;
  m_ecut[2][2]=0.05;
  m_ecut[2][3]=0.05;
  m_ecut[3][0]=0.05;
  m_ecut[3][1]=0.05;
  m_ecut[3][2]=0.05;
  m_ecut[3][3]=0.05;


  return;
}


LArHitFilter::~LArHitFilter()
{  
return;
}


StatusCode LArHitFilter::initialize()
{
//
// ......... make the Sub-detector flag vector
//

  for (int i=0; i < LArHitEMap::NUMDET ; i++)
  {
   m_SubDetFlag.push_back(false);
  }

//
// ......... make the digit container name list
//

  if ( m_SubDetectors == "LAr_All" ) 
  {
    m_HitContainer.push_back(m_EmBarrelHitContainerName);
    m_CaloType.push_back(LArHitEMap::EMBARREL_INDEX);
    m_SubDetFlag[LArHitEMap::EMBARREL_INDEX] = true;
  
    m_HitContainer.push_back(m_EmEndCapHitContainerName);
    m_CaloType.push_back(LArHitEMap::EMENDCAP_INDEX);
    m_SubDetFlag[LArHitEMap::EMENDCAP_INDEX] = true;

    m_HitContainer.push_back(m_HecHitContainerName);
    m_CaloType.push_back(LArHitEMap::HADENDCAP_INDEX);
    m_SubDetFlag[LArHitEMap::HADENDCAP_INDEX] = true;
    
    m_HitContainer.push_back(m_ForWardHitContainerName);
    m_CaloType.push_back(LArHitEMap::FORWARD_INDEX);
    m_SubDetFlag[LArHitEMap::FORWARD_INDEX] = true;
  }  
  else if ( m_SubDetectors == "LAr_Em" )
  { 
    m_HitContainer.push_back(m_EmBarrelHitContainerName); 
    m_CaloType.push_back(LArHitEMap::EMBARREL_INDEX);
    m_SubDetFlag[LArHitEMap::EMBARREL_INDEX] = true;
  
    m_HitContainer.push_back(m_EmEndCapHitContainerName);
    m_CaloType.push_back(LArHitEMap::EMENDCAP_INDEX);
    m_SubDetFlag[LArHitEMap::EMENDCAP_INDEX] = true;
  }
  else if ( m_SubDetectors == "LAr_EmBarrel" )
  {
    m_HitContainer.push_back(m_EmBarrelHitContainerName);
    m_CaloType.push_back(LArHitEMap::EMBARREL_INDEX);
    m_SubDetFlag[LArHitEMap::EMBARREL_INDEX] = true;
  }
  else if ( m_SubDetectors == "LAr_EmEndCap" )
  {
    m_HitContainer.push_back(m_EmEndCapHitContainerName);
    m_CaloType.push_back(LArHitEMap::EMBARREL_INDEX);
    m_SubDetFlag[LArHitEMap::EMENDCAP_INDEX] = true;
  }
  else if ( m_SubDetectors == "LAr_HEC" )
  {
    m_HitContainer.push_back(m_HecHitContainerName);
    m_CaloType.push_back(LArHitEMap::HADENDCAP_INDEX);
    m_SubDetFlag[LArHitEMap::HADENDCAP_INDEX] = true;
  }
  else if ( m_SubDetectors == "LAr_Fcal" )
  {  
    m_HitContainer.push_back(m_ForWardHitContainerName);
    m_CaloType.push_back(LArHitEMap::FORWARD_INDEX);
    m_SubDetFlag[LArHitEMap::FORWARD_INDEX] = true;
  }
  else if (m_SubDetectors == "LAr_EndCap")
  {
    m_HitContainer.push_back(m_EmEndCapHitContainerName);
    m_CaloType.push_back(LArHitEMap::EMBARREL_INDEX);
    m_SubDetFlag[LArHitEMap::EMENDCAP_INDEX] = true;

    m_HitContainer.push_back(m_HecHitContainerName);
    m_CaloType.push_back(LArHitEMap::HADENDCAP_INDEX);
    m_SubDetFlag[LArHitEMap::HADENDCAP_INDEX] = true;

    m_HitContainer.push_back(m_ForWardHitContainerName);
    m_CaloType.push_back(LArHitEMap::FORWARD_INDEX);
    m_SubDetFlag[LArHitEMap::FORWARD_INDEX] = true;
  }
  else  
  { 
    ATH_MSG_ERROR("Invalid SubDetector properties");
    return(StatusCode::FAILURE); 
  } 
  

 //retrieve ID helpers
  const DataHandle<CaloIdManager> caloIdMgr;
  StatusCode sc = detStore()->retrieve(caloIdMgr);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve CaloIdManager from DetectoreStore");
    return StatusCode::FAILURE;
  }   
  m_larem_id   = caloIdMgr->getEM_ID();
  m_larhec_id  = caloIdMgr->getHEC_ID();
  m_larfcal_id  = caloIdMgr->getFCAL_ID();


  return StatusCode::SUCCESS;

}

StatusCode LArHitFilter::execute()
{

  StatusCode sc;

//
// ............ loop over the wanted hit containers
//
  int nhit_tot=0;
  int nhit_out=0;

  for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++)
  {

    int ical=0;
    if (m_CaloType[iHitContainer] == LArHitEMap::EMBARREL_INDEX)       ical=0;
    else if( m_CaloType[iHitContainer] == LArHitEMap::EMENDCAP_INDEX)  ical=1;
    else if (m_CaloType[iHitContainer] == LArHitEMap::HADENDCAP_INDEX) ical=2; 
    else if (m_CaloType[iHitContainer] == LArHitEMap::FORWARD_INDEX)   ical=3;
    else
    {
     ATH_MSG_ERROR("unknown calo type ! ");
     return StatusCode::FAILURE;
    }

    const LArHitContainer* hitcont;
    sc = evtStore()->retrieve(hitcont,m_HitContainer[iHitContainer]);
    if (sc.isFailure()) {
       ATH_MSG_ERROR(" cannot retrieve LarHit container " << m_HitContainer[iHitContainer] );
       return sc;
    }

    ATH_MSG_DEBUG("Found " <<  m_HitContainer[iHitContainer] << " ical: " << ical << " with " << hitcont->size() << " hits ");
    LArHitContainer* hitcont2 = const_cast<LArHitContainer*>(hitcont);

// Loop over cells in this LArHitContainer
    LArHitContainer::const_iterator f_cell=hitcont2->begin();
    LArHitContainer::const_iterator l_cell=hitcont2->end();

    std::vector<LArHit*> tmphit;
    unsigned int guess_size = hitcont2->size()/2 + 1;
    tmphit.reserve(guess_size);
       
    while (f_cell != l_cell) {
        double energy = (*f_cell)->energy();
        nhit_tot++;
        int ilayer=0;
        Identifier cellId = (*f_cell)->cellID();
        if (ical==0 || ical==1) ilayer = m_larem_id->sampling(cellId);
        if (ical==2 ) ilayer = m_larhec_id->sampling(cellId);
        if (ical==3 ) ilayer = m_larfcal_id->module(cellId);
        if (energy>m_ecut[ical][ilayer]) {
           nhit_out++;
           double time = (*f_cell)->time();
           LArHit* newhit = new LArHit(cellId,energy,time);        
           newhit->finalize();
           tmphit.push_back(newhit);
        }
       ++f_cell;
     }              //  loop over  hits

     hitcont2->Clear();
     hitcont2->reserve(nhit_out);
    
     f_cell=tmphit.begin();
     l_cell=tmphit.end();
     while (f_cell != l_cell) {
        LArHit* hit = (*f_cell);
        hitcont2->push_back(hit);
        ++f_cell;
     }
     ATH_MSG_DEBUG("  -- > size after filtering " << hitcont2->size());
  } // .... end of loop over containers

  ATH_MSG_INFO(" total number of hits found "  << nhit_tot << " " << nhit_out);

  return StatusCode::SUCCESS;

}

StatusCode LArHitFilter::finalize()
{

//
  ATH_MSG_DEBUG(" LArHitFilter finalize completed successfully");

//
  return StatusCode::SUCCESS;

}

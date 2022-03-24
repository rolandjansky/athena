/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//
// algorithm to merge LArHit from different subevents from pileup service to
//   a single event, assuming all subevents are at the same time and neglecting the
//   individual hit times
//  This is aimed to run at first stage for very large luminosity pileup

#include "LArHitMerger.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArSimEvent/LArHit.h"
#include "CaloIdentifier/LArID.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/MsgStream.h"
#include "LArDigitization/LArHitEMap.h"
#include "CaloIdentifier/CaloIdManager.h"

// Pile up
#include "PileUpTools/PileUpMergeSvc.h"

LArHitMerger::LArHitMerger(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    p_mergeSvc(NULL),
    m_SubDetectors ("LAr_All"),
    m_EmBarrelHitContainerName ("LArHitEMB"),
    m_EmEndCapHitContainerName ("LArHitEMEC"),
    m_HecHitContainerName ("LArHitHEC"),
    m_ForWardHitContainerName ("LArHitFCAL"),
    m_larem_id(NULL),m_larhec_id(NULL),m_larfcal_id(NULL)
{
  //
  // ........ declare the private data as properties
  //
  declareProperty("SubDetectors",m_SubDetectors,"subdetector selection");
  declareProperty("EmBarrelHitContainerName",m_EmBarrelHitContainerName,"Hit container name for EMB");
  declareProperty("EmEndCapHitContainerName",m_EmEndCapHitContainerName,"Hit container name for EMEC");
  declareProperty("HecHitContainerName",m_HecHitContainerName,"Hit container name for HEC");
  declareProperty("ForWardHitContainerName",m_ForWardHitContainerName,"Hit container name for FCAL");

//
return;
}


LArHitMerger::~LArHitMerger()
{  
return;
}


StatusCode LArHitMerger::initialize()
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
    ATH_MSG_ERROR("Invalid SubDetector propertie");
    return(StatusCode::FAILURE); 
  } 
  

//
// locate the PileUpMergeSvc and initialize our local ptr
//
  if (!(service("PileUpMergeSvc", p_mergeSvc)).isSuccess() ||
       0 == p_mergeSvc) {
     ATH_MSG_ERROR("Could not find PileUpMergeSvc");
     return StatusCode::FAILURE;
  }
  else
  {
    ATH_MSG_INFO("PileUpMergeSvc successfully initialized");
  }


  //retrieve ID helpers
  const CaloIdManager* caloIdMgr = nullptr;
  StatusCode sc = detStore()->retrieve(caloIdMgr);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve CaloIdManager from DetectoreStore");
    return StatusCode::FAILURE;
  }   
  m_larem_id   = caloIdMgr->getEM_ID();
  m_larhec_id  = caloIdMgr->getHEC_ID();
  m_larfcal_id = caloIdMgr->getFCAL_ID();

  return StatusCode::SUCCESS;

}

StatusCode LArHitMerger::execute()
{

  std::vector<double> eCells_EM;
  std::vector<double> eCells_HEC;
  std::vector<double> eCells_FCAL;

  unsigned int ncells_em = m_larem_id->channel_hash_max();
  unsigned int ncells_hec = m_larhec_id->channel_hash_max();
  unsigned int ncells_fcal = m_larfcal_id->channel_hash_max();

  ATH_MSG_INFO(" ncells " << ncells_em << " " << ncells_hec << " " << ncells_fcal);

  if (m_SubDetFlag[LArHitEMap::EMBARREL_INDEX] || m_SubDetFlag[LArHitEMap::EMENDCAP_INDEX] )
     eCells_EM.resize(ncells_em,0.);
  if (m_SubDetFlag[LArHitEMap::HADENDCAP_INDEX]  )
     eCells_HEC.resize(ncells_hec,0.);
  if (m_SubDetFlag[LArHitEMap::FORWARD_INDEX]  )
     eCells_FCAL.resize(ncells_fcal,0.);

//
// ....... create the new LAr Hit containers
//
  LArHitContainer* larhit_emb=0;
  LArHitContainer* larhit_emec=0;
  LArHitContainer* larhit_hec=0;
  LArHitContainer* larhit_fcal=0;

  StatusCode sc;

  if (m_SubDetFlag[LArHitEMap::EMBARREL_INDEX]) {
     larhit_emb = new LArHitContainer();
     sc = evtStore()->record(larhit_emb,m_EmBarrelHitContainerName);
     if (sc.isFailure()) {
       ATH_MSG_ERROR(" cannot record LArHitEMB container ");
       return sc;
     }
  }
  if (m_SubDetFlag[LArHitEMap::EMENDCAP_INDEX]) {
     larhit_emec = new LArHitContainer();
     sc = evtStore()->record(larhit_emec,m_EmEndCapHitContainerName);
     if (sc.isFailure()) {
       ATH_MSG_ERROR(" cannot record LArHitEMEC container ");
       return sc;
     }
  }

  if (m_SubDetFlag[LArHitEMap::HADENDCAP_INDEX]) {
     larhit_hec = new LArHitContainer();
     sc = evtStore()->record(larhit_hec,m_HecHitContainerName);
     if (sc.isFailure()) {
       ATH_MSG_ERROR("cannot record LArHitHEC container ");
       return sc;
     }
  }
  if (m_SubDetFlag[LArHitEMap::FORWARD_INDEX]) {
     larhit_fcal = new LArHitContainer();
     sc = evtStore()->record(larhit_fcal,m_ForWardHitContainerName);
     if (sc.isFailure()) {
       ATH_MSG_ERROR("cannot record LArHitEMEC container ");
       return sc;
     }
  }

//
// ............ loop over the wanted hit containers
//
  int nhit_tot=0;

  for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++)
  {

    ATH_MSG_DEBUG(" asking for: " << m_HitContainer[iHitContainer]);

    int ical=0;
    if (m_CaloType[iHitContainer] == LArHitEMap::EMBARREL_INDEX ||
        m_CaloType[iHitContainer] == LArHitEMap::EMENDCAP_INDEX) 
    {
      ical=1;
    }
    else if (m_CaloType[iHitContainer] == LArHitEMap::HADENDCAP_INDEX)
    {
      ical=2;
    }
    else if (m_CaloType[iHitContainer] == LArHitEMap::FORWARD_INDEX)
    {
      ical=3;
    }
    else
    {
     ATH_MSG_ERROR("unknown calo type ! " );
     return StatusCode::FAILURE;
    }

    typedef PileUpMergeSvc::TimedList<LArHitContainer>::type TimedHitContList;
    TimedHitContList hitContList;
//
// retrieve list of pairs (time,container) from PileUp service

    if (!(p_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer]
          ,hitContList).isSuccess()) && hitContList.size()==0) {
       ATH_MSG_ERROR(" Could not fill TimedHitContList" );
       return StatusCode::FAILURE;
    }

// loop over this list
    TimedHitContList::iterator iFirstCont(hitContList.begin());
    TimedHitContList::iterator iEndCont(hitContList.end());
    while (iFirstCont != iEndCont) {
// get LArHitContainer for this subevent
         const LArHitContainer& firstCont = *(iFirstCont->second);
// Loop over cells in this LArHitContainer
         LArHitContainer::const_iterator f_cell=firstCont.begin();
         LArHitContainer::const_iterator l_cell=firstCont.end();
       
         while (f_cell != l_cell) {
            double energy = (*f_cell)->energy();
            Identifier cellId = (*f_cell)->cellID();
            ++f_cell;
            nhit_tot++;
            IdentifierHash idHash;
            if (ical==1) {
                idHash=m_larem_id->channel_hash(cellId);
                if (idHash<ncells_em) eCells_EM[idHash] += energy;
            }
            else if(ical==2) {
                idHash=m_larhec_id->channel_hash(cellId);
                if (idHash<ncells_hec) eCells_HEC[idHash] += energy;
            }
            else if(ical==3) {
                idHash=m_larfcal_id->channel_hash(cellId);
                if (idHash<ncells_fcal) eCells_FCAL[idHash] += energy;
            }
         }              //  loop over  hits
         ++iFirstCont;
      }                 // loop over subevent list

    } // .... end of loop over containers

    ATH_MSG_INFO(" total number of hits found "  << nhit_tot);


    double time=0.;

    if (m_SubDetFlag[LArHitEMap::EMBARREL_INDEX] || m_SubDetFlag[LArHitEMap::EMENDCAP_INDEX]) {
       int nhit=0;
       for (unsigned int i=0;i<ncells_em;i++) {
          IdentifierHash idHash = i;
          Identifier cellId = m_larem_id->channel_id(idHash);
          double energy = eCells_EM[i];
          if (energy>1e-6) {
            if ( m_SubDetFlag[LArHitEMap::EMBARREL_INDEX] && m_larem_id->is_em_barrel(cellId) ) {
              LArHit* hit = new LArHit(cellId,energy,time);             
              hit->finalize();
              larhit_emb->push_back(hit);
              nhit++;
            }
            if ( m_SubDetFlag[LArHitEMap::EMENDCAP_INDEX] && m_larem_id->is_em_endcap(cellId) ) {
              LArHit* hit = new LArHit(cellId,energy,time);
              hit->finalize();
              larhit_emec->push_back(hit);
              nhit++;
            }
          }
       }
       ATH_MSG_INFO(" Number of hits filled in LArHitEM containers " << nhit);
    }

    if (m_SubDetFlag[LArHitEMap::HADENDCAP_INDEX]) {
       int nhit=0;
       for (unsigned int i=0;i<ncells_hec;i++) {
          IdentifierHash idHash = i;
          Identifier cellId = m_larhec_id->channel_id(idHash);
          double energy = eCells_HEC[i];
          if (energy>1e-6) {
            LArHit* hit = new LArHit(cellId,energy,time);
            hit->finalize();
            larhit_hec->push_back(hit);
            nhit++;
          }
       } 
       ATH_MSG_INFO(" Number of hits filled in LArHitHEC container " << nhit);
    }

    if (m_SubDetFlag[LArHitEMap::FORWARD_INDEX]) {
       int nhit=0;
       for (unsigned int i=0;i<ncells_fcal;i++) {
          IdentifierHash idHash = i;
          Identifier cellId = m_larfcal_id->channel_id(idHash);
          double energy = eCells_FCAL[i];
          if (energy>1e-6) {
            LArHit* hit = new LArHit(cellId,energy,time);
            hit->finalize();
            larhit_fcal->push_back(hit);
            nhit++;
          }
       }
       ATH_MSG_INFO(" Number of hits filled in LArHitFCAL container " << nhit);
    }



  // lock container in StoreGate
  if (larhit_emb) {
     sc = evtStore()->setConst(larhit_emb);
     if (sc.isFailure()) {
       ATH_MSG_ERROR(" Cannot lock LArHitContainer ");
       return(StatusCode::FAILURE);
     }
  }

  if (larhit_emec) {
     sc = evtStore()->setConst(larhit_emec);
     if (sc.isFailure()) {
       ATH_MSG_ERROR(" Cannot lock LArHitContainer ");
       return(StatusCode::FAILURE);
     }
  }

  if (larhit_hec) {
     sc = evtStore()->setConst(larhit_hec);
     if (sc.isFailure()) {
       ATH_MSG_ERROR(" Cannot lock LArHitContainer ");
       return(StatusCode::FAILURE);
     }
  }

  if (larhit_fcal) {
     sc = evtStore()->setConst(larhit_fcal);
     if (sc.isFailure()) {
       ATH_MSG_ERROR(" Cannot lock LArHitContainer ");
       return(StatusCode::FAILURE);
     }
  }

 
  return StatusCode::SUCCESS;

}

StatusCode LArHitMerger::finalize()
{
//
  ATH_MSG_DEBUG(" LArHitMerger finalize completed successfully");

//
  return StatusCode::SUCCESS;

}

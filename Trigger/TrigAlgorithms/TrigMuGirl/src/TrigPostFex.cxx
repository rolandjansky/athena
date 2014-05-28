/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    Authors: E. Reinherz-Aronis, A. Kreisel 
             This Fex is able to run after both TrigMuGirl and TrigMooHLT
             for now the code assum it's coming in the same sequqance 
             as TrigMuGirl or TrigMooHLT 
***************************************************************************/
#include "TrigMuGirl/TrigPostFex.h" 

#include "TrigMuonEvent/TrigMuonEFContainer.h"

//for the event info
#include "StoreGate/StoreGateSvc.h"
 
#include "GaudiKernel/NTuple.h"
#include "TrigMuGirl/TrigPostFexNtuple.h"
#include "GeneratorObjects/McEventCollection.h"
// for second potput object 
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"



TrigPostFex::TrigPostFex(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::FexAlgo(name, pSvcLocator),
    m_pStoreGate        (NULL),
    m_pTrigPostFexNtuple (NULL)
{

    declareProperty("MuMuMassMin", m_MassMin = 2800.);
    declareProperty("MuMuMassMax", m_MassMax = 4000.);
    declareMonitoredVariable("pT", muon_pT);
    declareProperty("NtupleName",           m_ntupleName    = "/NTUPLES/POSTFEX/POSTFEX");
    declareProperty("NtupleTitle",          m_ntupleTitle   = "PostFex");
    declareProperty("doNTuple",             m_doNTuple      = false);
    declareProperty("doTruth",              m_doTruth       = false);
}

TrigPostFex::~TrigPostFex()
{}

HLT::ErrorCode TrigPostFex::hltInitialize()
{
    msg() << MSG::INFO << "Initializing TrigPostFex hello Erez" << endreq;
    //   StatusCode SCstatus;

    // Initialize NTuple
    if (m_doNTuple)
    {
        m_pTrigPostFexNtuple = new TrigPostFexNtuple(this, ntupleSvc());
        StatusCode SCstatus = m_pTrigPostFexNtuple->book(m_ntupleName, m_ntupleTitle);
        if (SCstatus.isFailure() || m_pTrigPostFexNtuple == NULL)
        {
            msg()<< MSG::ERROR << "Could not book NTuple " << m_ntupleName << endreq;
            return HLT::BAD_JOB_SETUP;
        }
    }

    return HLT::OK;
}

HLT::ErrorCode TrigPostFex::hltFinalize()
{
    msg() << MSG::INFO << "Finalizing TrigPostFex" << endreq;

    return HLT::OK;
}

HLT::ErrorCode TrigPostFex::hltExecute(const HLT::TriggerElement* /*inputTE*/, HLT::TriggerElement* outputTE)
{
  msg()<<MSG::DEBUG<<"Executing TrigPostFex <--*-->"<<endreq;
    
  //  const EventInfo* pEventInfo;
  eRunNumber=0;
  eEventNumber=0;

  m_pt.clear();
  m_cotTh.clear();
  m_phi.clear(); 
  m_m.clear();
  m_charge.clear();

  m_pStoreGate = store();

  if ( m_pStoreGate->retrieve(pEventInfo).isFailure() ) {
    msg()  << MSG::DEBUG << "Failed to get EventInfo " << endreq;
  } else {
    eRunNumber   = pEventInfo->event_ID()->run_number();
    eEventNumber = pEventInfo->event_ID()->event_number();
    msg()<<MSG::DEBUG<<"Run "<< eRunNumber<<" Event "<< eEventNumber<<endreq;
  }
    
  HLT::ErrorCode status;
  // StatusCode   statusfill;
  whichOutput=0;

  const TrigMuonEFInfoContainer* trackInfoCont = 0;
  std::vector<const TrigMuonEFInfoContainer*>  vectorOfTrigMuonEFInfo;
  status=getFeatures(outputTE, vectorOfTrigMuonEFInfo, "eMuonEFInfo");
  if (status != HLT::OK || vectorOfTrigMuonEFInfo.size()==0) 
    {
      msg() << MSG::DEBUG << "Could not retieve vector of TrigMuonEFInfoConatiner "<< endreq;
      return HLT::MISSING_FEATURE;
    }
  else
    {
      whichOutput=1;

      if (status != HLT::OK || vectorOfTrigMuonEFInfo.size()==0) 
	{
	  msg() << MSG::DEBUG << "Could not retieve vector of TrigMuonEFInfoConatiner "<< endreq;
	  return HLT::MISSING_FEATURE;
	}
  
      for (unsigned int i=0; i<vectorOfTrigMuonEFInfo.size(); i++)
	{
	  msg() << MSG::DEBUG << "Element "<<i<<" of vector of TrigMuonEFInfo containers  "<<endreq;
	  // Get first (and only) RoI:
	  trackInfoCont = vectorOfTrigMuonEFInfo[i];
	  if(!trackInfoCont)
	    {
	      msg() << MSG::ERROR
		    << "Retrieval of TrigMuonEFInfo container from vector failed" << endreq;
	      return  HLT::NAV_ERROR;
	    }  
	  else msg() << MSG::DEBUG << "container OK with size "<<trackInfoCont->size()<< endreq;
	}
    }

  // Get vector of pointers to all TrigMuonEF objects linked to the outputTE
  // if the code isn't in the same sequance as TriMuGirl or TrigMooHLT 
  // outputTE --> inputTE
  const TrigMuonEFContainer* trackCont=0;
  std::vector<const TrigMuonEFContainer*> vectorOfTrigMuonEF;  
  
  status=getFeatures(outputTE, vectorOfTrigMuonEF);

  if (status != HLT::OK || vectorOfTrigMuonEF.size()==0) 
    {
      msg() << MSG::DEBUG << "Could not retieve vector of TrigMuonEFConatiner "<< endreq;
      return HLT::MISSING_FEATURE;
    }
  
  for (unsigned int i=0; i<vectorOfTrigMuonEF.size(); i++)
    {
      msg() << MSG::DEBUG << "Element "<<i<<" of vector of TrigMuonEF containers  "<<endreq;
      // Get first (and only) RoI:
      trackCont = vectorOfTrigMuonEF[i];
      if(!trackCont)
	{
	  msg() << MSG::ERROR
		<< "Retrieval of TrigMuonEF  container from vector failed" << endreq;
	  return  HLT::NAV_ERROR;
	}  
      else msg() << MSG::DEBUG << "container OK with size "<<trackCont->size()<< endreq;
    }


  if (m_doNTuple)
    {
      StatusCode statusfill = m_pTrigPostFexNtuple->fillEvent(eEventNumber,eRunNumber);
      if (statusfill.isFailure())
	{
	  msg() << MSG::ERROR << " Cannot fill NTuple event # " << endreq;
	  return HLT::BAD_JOB_SETUP;
	}
    }

  if (m_doTruth)
    {
      if (doMuTruth() != HLT::OK)
	msg() << MSG::ERROR  << "The doMuTruth status is not Ok" << endreq;
    }
 
  nMuons=0;


  if (whichOutput)
    {  
  for (TrigMuonEFInfoContainer::const_iterator tr = trackInfoCont->begin();tr != trackInfoCont->end(); tr++)
    {
      TrigMuonEFInfo* eInfo = (*tr);
      TrigMuonEFCbTrack* trC = eInfo->CombinedTrack();
	m_pt.push_back( trC->pt());
	m_cotTh.push_back( trC->cotTh());
	m_phi.push_back( trC->phi());
	m_m.push_back( trC->m());
	m_charge.push_back( trC->charge());
	if(m_doNTuple)
	  {
	    ++nMuons;
	    // tan(x/2)=sqrt(1+cot**2)-cot
	    double eta=-log(sqrt(1+(trC->cotTh())*(trC->cotTh()))-(trC->cotTh())); 
	    StatusCode statusfill = m_pTrigPostFexNtuple->fillReco(nMuons,trC->phi(),trC->cotTh(),eta,trC->pt(), (int)trC->charge());
	    
	    if (statusfill.isFailure())
	      {
		msg() << MSG::ERROR << " Cannot fill NTuple Reco " << endreq;
		return HLT::BAD_JOB_SETUP;
	      }
	    
	  } 
    }
    }
  else
    {
  for (TrigMuonEFContainer::const_iterator tr = trackCont->begin();tr != trackCont->end(); tr++)
    {
      if ((*tr)->MuonCode()<100 && (*tr)->MuonCode() != 5) continue;
      {
	m_pt.push_back( (*tr)->pt());
	m_cotTh.push_back( (*tr)->cotTh());
	m_phi.push_back( (*tr)->phi());
	m_m.push_back( (*tr)->m());
	m_charge.push_back( (*tr)->charge());

	//        IdTracks.push_back((*tr)->indetTrack());

	if(m_doNTuple)
	  {
	    ++nMuons;
	    // tan(x/2)=sqrt(1+cot**2)-cot
	    double eta=-log(sqrt(1+((*tr)->cotTh())*((*tr)->cotTh()))-((*tr)->cotTh())); 
	    StatusCode statusfill = m_pTrigPostFexNtuple->fillReco(nMuons,(*tr)->phi(),(*tr)->cotTh(),eta,(*tr)->pt(), (int)(*tr)->charge());
	    
	    if (statusfill.isFailure())
	      {
		msg() << MSG::ERROR << " Cannot fill NTuple Reco " << endreq;
		return HLT::BAD_JOB_SETUP;
	      }
	    
	  } 
      }
    }
    }

  muon_pT=-99;
  NumberOfMuons=-99;
  NumberOfMuons=m_pt.size();
  
  //  StatusCode statusNT; 
  if (m_doNTuple)
    {
      msg() << MSG::DEBUG << " Write Ntuple " << endreq;
      StatusCode statusNT = m_pTrigPostFexNtuple->writeRecord();
      if (statusNT.isFailure() || m_pTrigPostFexNtuple == NULL)
	{
	  msg() << MSG::ERROR << "TrigPostFex::execute Cannot write NTuple" << endreq;
	  //delete di_muon;
	  return HLT::BAD_JOB_SETUP;
	}
    }


  //  status= attachFeature(outputTE, pOut, "EFMuPairs");
  
  m_pt.clear();
  m_cotTh.clear();
  m_phi.clear();
  m_m.clear();
  m_charge.clear();
  
  return HLT::OK;
}
HLT::ErrorCode TrigPostFex::doMuTruth()
{
    int nTruthMu = 0;
    const McEventCollection* pMcEventCollection = NULL;
    StatusCode status = m_pStoreGate->retrieve(pMcEventCollection, "TruthEvent");
    if (status.isFailure() || pMcEventCollection == NULL)
    {
        msg() << MSG::WARNING << " Cannot retrieve McEventCollection " << endreq;
		return HLT::SG_ERROR;
    }

	McEventCollection::const_iterator
		itMc = pMcEventCollection->begin(),
		itMc_e = pMcEventCollection->end();
    for (; itMc != itMc_e; ++itMc)
    {
        const HepMC::GenEvent* pGenEvent = *itMc;

		HepMC::GenEvent::particle_const_iterator
			itPart = pGenEvent->particles_begin(),
			itPart_e = pGenEvent->particles_end();
        for (; itPart != itPart_e; ++itPart)
        {
            const HepMC::GenParticle* pPart = *itPart;

            if (pPart->pdg_id() == -13 || pPart->pdg_id() == 13)
            {
                int parent_pdg = 0;
                if (pPart->production_vertex() != NULL)
                {
					HepMC::GenVertex::particle_iterator
						itVxPart = pPart->production_vertex()->particles_begin(HepMC::parents),
						itVxPart_e = pPart->production_vertex()->particles_end(HepMC::parents);
                    for (; itVxPart != itVxPart_e; ++itVxPart)
                    {
                        parent_pdg = (*itVxPart)->pdg_id();
                    }
                }

                ++nTruthMu;
                if (m_doNTuple)
                {
		  double cot=1/tan(2*atan(exp(-pPart->momentum().eta())));

                    status = m_pTrigPostFexNtuple->fillTruth(nTruthMu,
                                                        pPart->momentum().phi(),
                                                        cot,
                                                        pPart->momentum().eta(),
                                                        pPart->momentum().perp(),
                                                        pPart->pdg_id(),
							    parent_pdg);
                    if (status.isFailure())
                    {
                        msg() << MSG::ERROR << " Cannot fill NTuple TRUTH " << endreq;
                        //delete di_muon; 
                        return HLT::BAD_JOB_SETUP;
                    }
                }
            }
        }
    }
    //}

	return HLT::OK;
}



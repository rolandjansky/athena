/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTests/AnalysisCal.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"

#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"




namespace MyAnalysisCal {

  //Constructor
  AnalysisCal::AnalysisCal(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator)
    //m_nevt(0)
  {
  }
  
  //__________________________________________________________________________
  //Destructor
  AnalysisCal::~AnalysisCal()
  {
    MsgStream log( messageService(), name() ) ;
    log << MSG::DEBUG << "AnalysisCal destructor called" << endreq;
  }
  //__________________________________________________________________________
  StatusCode AnalysisCal::initialize()
  {
    
    MsgStream log( messageService(), name() );
    log << MSG::DEBUG <<"Analysiscal initialize()" << endreq;

    return StatusCode::SUCCESS; 
  }
  //__________________________________________________________________________
  StatusCode AnalysisCal::finalize()
  {
    MsgStream log( messageService(), name() );
    log << MSG::DEBUG <<"AnalysisCal finalize()" << endreq;
    return StatusCode::SUCCESS; 
  }
  
  //__________________________________________________________________________
  StatusCode AnalysisCal::execute()
  {
    //.............................................
    
    MsgStream log( messageService(), name() );
    log << MSG::DEBUG << "AnalysisCal execute()" << endreq;

// loop over generated particles

      const DataHandle<McEventCollection> mcCollptr;
      double e_true=0.;
      double eta_true=-999.;
      double phi_true=-999.;
      if ( evtStore()->retrieve(mcCollptr,"GEN_EVENT").isFailure() ) {
             log << MSG::WARNING 
             << "cannot retrieve McEventCollection  with key GEN_EVENT"
             << endreq;
      } 
      else
      {
       McEventCollection::const_iterator itr;
       for (itr = mcCollptr->begin(); itr!=mcCollptr->end(); ++itr) {
        HepMC::GenEvent::particle_const_iterator itrPart;
        for (itrPart = (*itr)->particles_begin(); itrPart!=(*itr)->particles_end(); ++itrPart )
       {
        HepMC::GenParticle *part=*itrPart;
         if(  (abs(part->pdg_id())==11 || part->pdg_id()==22)
              && part->momentum().e()> 5000.)
         {
          e_true = part->momentum().e();
          eta_true = part->momentum().pseudoRapidity();
          phi_true = part->momentum().phi();
          log << MSG::INFO << " true particle found " << part->pdg_id() << " " <<
            e_true << " " << eta_true << " " << phi_true << endreq;
  
         } // e or gamma found
        }  // loop over particle
       }   // loop over mcCollptr
      }

// Loop over LAr hits

  std::vector <std::string> m_HitContainer;

  m_HitContainer.push_back("LArHitEMB");
  m_HitContainer.push_back("LArHitEMEC");
  m_HitContainer.push_back("LArHitHEC");
  m_HitContainer.push_back("LArHitFCAL");
  unsigned int iHitContainer;
  int nhit_tot=0;
  double etot_hit=0.;
  log << MSG::DEBUG << "m_HitContainer.size " << m_HitContainer.size() << endreq;
  for (iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++)
  {
    const LArHitContainer* hit_container ;
    if(evtStore()->retrieve(hit_container,m_HitContainer[iHitContainer])
      .isFailure()) {
        log << MSG::INFO << " cannot retrieve hit container " << endreq;
    }  else
    {
       LArHitContainer::const_iterator hititer;
       for(hititer=hit_container->begin();
           hititer != hit_container->end();hititer++)
       {
          LArHit* hit = (*hititer);
          etot_hit += hit->energy();
          nhit_tot++;
       }
    }
  }
  log << MSG::INFO << " Total number of LAr hits " << nhit_tot << endreq;
  log << MSG::INFO << " Etot LAr hits            " << etot_hit << endreq;


// loop over Calibration Hits

  double etot_cal=0;
  nhit_tot=0;
  std::vector <std::string> m_CalibrationHitContainer;
  m_CalibrationHitContainer.push_back("LArCalibrationHitActive");
  m_CalibrationHitContainer.push_back("LArCalibrationHitDeadMaterial");
  m_CalibrationHitContainer.push_back("LArCalibrationHitInactive");
  log << MSG::DEBUG << "m_CalibrationHitContainer.size " << m_CalibrationHitContainer.size() << endreq;
  for (iHitContainer=0;iHitContainer<m_CalibrationHitContainer.size();iHitContainer++)
  {
    const CaloCalibrationHitContainer* calocalibrationhit_container ;
    if(evtStore()->retrieve(calocalibrationhit_container,m_CalibrationHitContainer[iHitContainer])
      .isFailure()) {
        log << MSG::INFO << " cannot retrieve calo calibration hit container " << endreq;
    }  else
    {
       CaloCalibrationHitContainer::const_iterator calibhititer;
       for(calibhititer=calocalibrationhit_container->begin();
           calibhititer != calocalibrationhit_container->end();calibhititer++)
       {
          CaloCalibrationHit* calibhit = (*calibhititer);
          nhit_tot++;
          etot_cal = etot_cal + calibhit->energyTotal();
       }
    }
  }
  log << MSG::INFO << " Total number of calibration hits " << nhit_tot << endreq;
  log << MSG::INFO << " Etot calibration hits            " << etot_cal << endreq;

  return StatusCode::SUCCESS;
 }
 
} // end of namespace bracket

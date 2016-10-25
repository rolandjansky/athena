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
    ATH_MSG_DEBUG( "AnalysisCal destructor called"  );
  }
  //__________________________________________________________________________
  StatusCode AnalysisCal::initialize()
  {
    ATH_MSG_DEBUG("Analysiscal initialize()"  );
    return StatusCode::SUCCESS; 
  }
  //__________________________________________________________________________
  StatusCode AnalysisCal::finalize()
  {
    ATH_MSG_DEBUG("AnalysisCal finalize()"  );
    return StatusCode::SUCCESS; 
  }
  
  //__________________________________________________________________________
  StatusCode AnalysisCal::execute()
  {
    //.............................................
    
    ATH_MSG_DEBUG( "AnalysisCal execute()"  );

// loop over generated particles

      const DataHandle<McEventCollection> mcCollptr;
      double e_true=0.;
      double eta_true=-999.;
      double phi_true=-999.;
      if ( evtStore()->retrieve(mcCollptr,"GEN_EVENT").isFailure() ) {
        ATH_MSG_WARNING( "cannot retrieve McEventCollection  with key GEN_EVENT" );
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
          ATH_MSG_INFO( " true particle found " << part->pdg_id() << " " <<
                        e_true << " " << eta_true << " " << phi_true  );
  
         } // e or gamma found
        }  // loop over particle
       }   // loop over mcCollptr
      }

// Loop over LAr hits

  std::vector <std::string> HitContainer;

  HitContainer.push_back("LArHitEMB");
  HitContainer.push_back("LArHitEMEC");
  HitContainer.push_back("LArHitHEC");
  HitContainer.push_back("LArHitFCAL");
  unsigned int iHitContainer;
  int nhit_tot=0;
  double etot_hit=0.;
  ATH_MSG_DEBUG( "HitContainer.size " << HitContainer.size()  );
  for (iHitContainer=0;iHitContainer<HitContainer.size();iHitContainer++)
 {
    const LArHitContainer* hit_container ;
    if(evtStore()->retrieve(hit_container,HitContainer[iHitContainer])
      .isFailure()) {
      ATH_MSG_INFO( " cannot retrieve hit container "  );
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
  ATH_MSG_INFO( " Total number of LAr hits " << nhit_tot  );
  ATH_MSG_INFO( " Etot LAr hits            " << etot_hit  );


// loop over Calibration Hits

  double etot_cal=0;
  nhit_tot=0;
  std::vector <std::string> CalibrationHitContainer;
  CalibrationHitContainer.push_back("LArCalibrationHitActive");
  CalibrationHitContainer.push_back("LArCalibrationHitDeadMaterial");
  CalibrationHitContainer.push_back("LArCalibrationHitInactive");
  ATH_MSG_DEBUG( "CalibrationHitContainer.size " << CalibrationHitContainer.size()  );
  for (iHitContainer=0;iHitContainer<CalibrationHitContainer.size();iHitContainer++)
  {
    const CaloCalibrationHitContainer* calocalibrationhit_container ;
    if(evtStore()->retrieve(calocalibrationhit_container,CalibrationHitContainer[iHitContainer])
      .isFailure()) {
      ATH_MSG_INFO( " cannot retrieve calo calibration hit container "  );
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
  ATH_MSG_INFO( " Total number of calibration hits " << nhit_tot  );
  ATH_MSG_INFO( " Etot calibration hits            " << etot_cal  );

  return StatusCode::SUCCESS;
 }
 
} // end of namespace bracket

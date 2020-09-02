/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTests/AnalysisCal.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"

#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "GeneratorObjects/McEventCollection.h"
#include "StoreGate/ReadHandle.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"




namespace MyAnalysisCal {

  //Constructor
  AnalysisCal::AnalysisCal(const std::string& name, ISvcLocator* pSvcLocator):
    AthReentrantAlgorithm(name,pSvcLocator)
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
    ATH_CHECK( m_mcCollName.initialize() );
    ATH_CHECK( m_hitContainerNames.initialize() );
    ATH_CHECK( m_calibHitContainerNames.initialize() );
    return StatusCode::SUCCESS; 
  }
  //__________________________________________________________________________
  StatusCode AnalysisCal::finalize()
  {
    ATH_MSG_DEBUG("AnalysisCal finalize()"  );
    return StatusCode::SUCCESS; 
  }
  
  //__________________________________________________________________________
  StatusCode AnalysisCal::execute (const EventContext& ctx) const
  {
    //.............................................
    
    ATH_MSG_DEBUG( "AnalysisCal execute()"  );

// loop over generated particles

      SG::ReadHandle<McEventCollection> mcCollptr (m_mcCollName, ctx);
      double e_true=0.;
      double eta_true=-999.;
      double phi_true=-999.;
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

  int nhit_tot=0;
  double etot_hit=0.;
  for (const SG::ReadHandleKey<LArHitContainer>& k : m_hitContainerNames) {
    SG::ReadHandle<LArHitContainer> hit_container (k, ctx);
    for (const LArHit* hit : *hit_container) {
      etot_hit += hit->energy();
      nhit_tot++;
    }
  }
  ATH_MSG_INFO( " Total number of LAr hits " << nhit_tot  );
  ATH_MSG_INFO( " Etot LAr hits            " << etot_hit  );


// loop over Calibration Hits

  double etot_cal=0;
  nhit_tot=0;
  for (const SG::ReadHandleKey<CaloCalibrationHitContainer>& k : m_calibHitContainerNames)
  {
    SG::ReadHandle<CaloCalibrationHitContainer> calocalibrationhit_container (k, ctx);
    for (const CaloCalibrationHit* calibhit : *calocalibrationhit_container) {
      nhit_tot++;
      etot_cal = etot_cal + calibhit->energyTotal();
    }
  }
  ATH_MSG_INFO( " Total number of calibration hits " << nhit_tot  );
  ATH_MSG_INFO( " Etot calibration hits            " << etot_cal  );

  return StatusCode::SUCCESS;
 }
 
} // end of namespace bracket

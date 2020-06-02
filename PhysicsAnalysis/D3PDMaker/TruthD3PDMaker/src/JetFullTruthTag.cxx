/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TruthD3PDMaker/src/JetFullTruthTag.cxx
 * @author Zach Marshall <zach.marshall@cern.ch>
 * @date June, 2013
 * @brief Truth tagging info for truth jets
 */

#include "JetFullTruthTag.h"
#include "AthenaKernel/errorcheck.h"
#include "JetEvent/Jet.h"
#include "GeneratorObjects/McEventCollection.h"
#include "AtlasHepMC/GenParticle.h"
#include "HepPID/ParticleIDMethods.hh"
#include "GaudiKernel/SystemOfUnits.h"

#include <iostream>
#include <cmath>

namespace D3PD {

using Gaudi::Units::GeV;

JetFullTruthTag::JetFullTruthTag (const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent)
  : BlockFillerTool<Jet> (type, name, parent)
{
  // Avoid coverity warnings
  book().ignore();

  declareProperty ("HadronMatchDR" ,      m_hadronMatch_dr    = 0.3 );
  declareProperty ("PartonMatchDR" ,      m_partonMatch_dr    = 0.4 );
  declareProperty ("TruthCollectionName" ,  m_truthCollectionName = "GEN_EVENT" );
  declareProperty ("MinPartonPt" , m_min_parton_pt = 5*GeV );
  declareProperty ("MinHadronPt" , m_min_hadron_pt = 5*GeV );
}

StatusCode JetFullTruthTag::book()
{
  CHECK( addVariable ("partonDR",     m_partonDR)  );
  CHECK( addVariable ("partonFlavor", m_partonFlavor)  );
  CHECK( addVariable ("hadronFlavor", m_hadronFlavor)  );
  CHECK( addVariable ("hadronPDGID",  m_hadronPDGID)  );
  return StatusCode::SUCCESS;
}

StatusCode JetFullTruthTag::fill (const Jet& p)
{
  *m_partonDR = -1.;
  *m_partonFlavor = 0;
  *m_hadronFlavor = 0;
  *m_hadronPDGID = 0; 
  double Emax = 0.;
  int pdgid=0;
  double dR2=0.;

  const DataHandle<McEventCollection> mcCollection;
  if (evtStore()->retrieve(mcCollection,m_truthCollectionName).isSuccess()) {
    // We have an McEventCollection
    for (McEventCollection::const_iterator currentGenEventIter = mcCollection->begin();
         currentGenEventIter!=mcCollection->end(); ++currentGenEventIter) {
      for (HepMC::GenEvent::particle_const_iterator currentGenParticleIter= (*currentGenEventIter)->particles_begin(); 
           currentGenParticleIter!= (*currentGenEventIter)->particles_end(); ++currentGenParticleIter) {

        // Grab the PDGID, used both for partons and hadrons
        pdgid = (*currentGenParticleIter)->pdg_id();

        // Parton labeling section...
        if((*currentGenParticleIter)->momentum().e()>=Emax && (*currentGenParticleIter)->momentum().perp()>m_min_parton_pt){
          if( abs(pdgid)<=21 && // Should be a parton
              abs(pdgid)!=6 && // Should not be a top
              (abs(pdgid)==15 || abs(pdgid)<=10 || abs(pdgid)>16) && // Not a lepton
              abs(pdgid)!=0){ // not an unrecognized thingy
 
            dR2 = std::pow( std::acos( std::cos( p.phi() - (*currentGenParticleIter)->momentum().phi() ) ) , 2 );
            dR2 += std::pow( p.eta()-(*currentGenParticleIter)->momentum().eta() , 2 );
        
            if(dR2<=m_partonMatch_dr*m_partonMatch_dr){ // We have a winner
              Emax=(*currentGenParticleIter)->momentum().e();
              *m_partonFlavor = (*currentGenParticleIter)->pdg_id();
              *m_partonDR = static_cast<float> (dR2);
            } // Outside of dR
          } // Wrong PDG ID
        } // Low energy

        // Hadron labeling section
        if ((HepPID::isHadron (pdgid) || abs(pdgid)==15) && ((*currentGenParticleIter)->momentum().perp()>m_min_hadron_pt)){

          // Check on DR match
          dR2 = std::pow( std::acos( std::cos( p.phi() - (*currentGenParticleIter)->momentum().phi() ) ) , 2 );
          dR2 += std::pow( p.eta()-(*currentGenParticleIter)->momentum().eta() , 2 );

          if( dR2<=m_hadronMatch_dr*m_hadronMatch_dr ){
            // Strict ordering bottom up - 0 -> tau -> c -> b
            if( abs(pdgid)==15 && (*m_hadronPDGID)==0 ){
              *m_hadronPDGID = 15;
              *m_hadronFlavor = 15;
            }
            if ( HepPID::hasCharm (pdgid) && ((*m_hadronPDGID)==0 || (*m_hadronPDGID)==15)){
              *m_hadronPDGID = 4;
              *m_hadronFlavor = pdgid;
            }
            if ( HepPID::hasBottom (pdgid) ){
              *m_hadronPDGID = 5;
              *m_hadronFlavor = pdgid;
            }
          } // Passed dR match
        } // End of was a hadron or tau

      } // Loop over particles
    } // Loop over events
  } // Was an McEventCollection

  if ( ((*m_hadronFlavor)==4 || (*m_hadronFlavor)==5) && // hadron label fired
       ((*m_partonFlavor)==9 || (*m_partonFlavor)==21 ) ) // parton was a gluon
    *m_partonFlavor = *m_hadronFlavor;

  return StatusCode::SUCCESS;
}

} // namespace D3PD


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TruthD3PDMaker/src/TruthLeptonNearbyAssociationTool.cxx
 * @author Zach Marshall <zach.marshall@cern.ch>
 * @date Feb, 2014
 * @brief Isolation and dressing info for leptons
 */

#include "TruthLeptonNearbyAssociationTool.h"
#include "AthenaKernel/errorcheck.h"
#include "GeneratorObjects/McEventCollection.h"
#include "AtlasHepMC/GenParticle.h"
#include "McParticleEvent/TruthParticle.h"
#include "TruthUtils/PIDHelpers.h"

#include <iostream>
#include <cmath>

namespace D3PD {

TruthLeptonNearbyAssociationTool::TruthLeptonNearbyAssociationTool( const std::string& type,
                                                                    const std::string& name,
                                                                    const IInterface* parent)
  : BlockFillerTool<TruthParticle> (type, name, parent)
{
  // Avoid coverity warnings
  book().ignore();

  declareProperty ("DressingDR" ,          m_dressing_dr    = 0.1 );
  declareProperty ("TruthCollectionName" , m_truthCollectionName = "GEN_EVENT" );
}

StatusCode TruthLeptonNearbyAssociationTool::book()
{
  CHECK( addVariable ("dressed_pt", m_dressed_pt)  );
  CHECK( addVariable ("dressed_eta", m_dressed_eta)  );
  CHECK( addVariable ("dressed_phi", m_dressed_phi)  );
  CHECK( addVariable ("dressed_m", m_dressed_m)  );
  CHECK( addVariable ("iso02",      m_iso02)  );
  CHECK( addVariable ("iso03",      m_iso03)  );
  return StatusCode::SUCCESS;
}

StatusCode TruthLeptonNearbyAssociationTool::fill (const TruthParticle& p)
{
  *m_dressed_pt = 0.;
  *m_dressed_eta = 0.;
  *m_dressed_phi = 0.;
  *m_dressed_m = 0.;

  *m_iso02 = 0.;
  *m_iso03 = 0.;

  double dR2=0.;
  int real_parent=0;

  CLHEP::HepLorentzVector dressed_4mom = p.hlv();

  const DataHandle<McEventCollection> mcCollection;
  if (evtStore()->retrieve(mcCollection,m_truthCollectionName).isSuccess()) {
    // We have an McEventCollection
    for (McEventCollection::const_iterator currentGenEventIter = mcCollection->begin();
         currentGenEventIter!=mcCollection->end(); ++currentGenEventIter) {

      for (auto currentGenParticle: *(*currentGenEventIter)) {
        if (!currentGenParticle) continue;
        if (currentGenParticle->status()!=1) continue;
        if (HepMC::barcode(currentGenParticle)==p.barcode()) continue; // The same particle twice!

        dR2 = p.phi() - currentGenParticle->momentum().phi();
        if (dR2>M_PI) dR2-=2.*M_PI;
        else if (dR2<-M_PI) dR2+=2.*M_PI;

        dR2 = std::pow(dR2,2)+std::pow(p.eta()-currentGenParticle->momentum().eta(),2);

        if (dR2>=0.09) continue; // Save a little time

        // Isolation section - exclude neutrinos
        if (!MC::PID::isNeutrino( currentGenParticle->pdg_id() ) ){
          *m_iso03 = (*m_iso03)+currentGenParticle->momentum().perp();
          if (dR2<0.04) *m_iso02 = (*m_iso02)+currentGenParticle->momentum().perp();
        }

        // Dressing section
        if (currentGenParticle->pdg_id()!=22) continue; // Only photons
        if (dR2>=0.01) continue; // Only DR<0.1 //AV this is a magic number.

        real_parent = std::abs(get_real_parent( currentGenParticle ));
        if (real_parent>=26 || real_parent==15) continue; // Veto hadron parents

        dressed_4mom += CLHEP::HepLorentzVector(currentGenParticle->momentum().x(),currentGenParticle->momentum().y(),
                                                currentGenParticle->momentum().z(),currentGenParticle->momentum().t());

      } // Loop over particles
    } // Loop over events
  } // Was an McEventCollection

  *m_dressed_pt  = dressed_4mom.perp();
  *m_dressed_eta = dressed_4mom.eta();
  *m_dressed_phi = dressed_4mom.phi();
  *m_dressed_m   = dressed_4mom.m();

  return StatusCode::SUCCESS;
}

int TruthLeptonNearbyAssociationTool::get_real_parent( HepMC::ConstGenParticlePtr p , int depth ) const
{
  if (depth>10) return 0;

  // Work assuming one parent...
//AV This algorithm is ambiguous as it assumesnot more than one parent per particle.
//AV In HepMC2 this could be wrong more often than expected.

  auto prodvertex = p->production_vertex();
  if ( !prodvertex ) return 0;  // parent didn't exist
#ifdef HEPMC3
  if (!prodvertex->particles_in().size()) return 0;
  auto parentparticle=prodvertex->particles_in()[0];
#else
  if (!prodvertex->particles_in_size()) return 0;
  auto parentparticle=*(prodvertex->particles_begin(HepMC::parents));
#endif
  if ( !parentparticle ) return 0;  // parent didn't exist

  // Not a photon - return the parent
  if (parentparticle->pdg_id()!=22) return parentparticle->pdg_id();

  // Photon - iterate
  return get_real_parent( parentparticle , depth+1 );
}

} // namespace D3PD


/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthHelper/GenAccessIO.h"
#include "TruthHelper/GenAll.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"

#include <cmath>
#include <algorithm>


namespace TruthHelper {


  StatusCode GenAccessIO::getMC(MCParticleCollection& mcParticles, const std::string& key) const {
    const GenIMCselector* selector = new GenAll();
    return this->getMC(mcParticles, selector, key);
    /// @todo Memory leak on the undeleted GenAll?
  }


  StatusCode GenAccessIO::getMC(MCParticleCollection& mcParticles, const GenIMCselector* selector, const std::string& key) const {
    MsgStream log(Athena::getMessageSvc(), "GenAccessIO");

    // Retrieve iterators for McEventCollections objects
    SG::ConstIterator<McEventCollection> firstMEC;
    SG::ConstIterator<McEventCollection> lastMEC;
    if ( (m_sgSvc->retrieve(firstMEC, lastMEC)).isFailure() ) {
      log << MSG::ERROR << "Could not retrieve iterators for McEventCollections" << endmsg;
    }
    const McEventCollection& mcColl = *firstMEC;
    int icount = 0;
    for ( ; firstMEC!= lastMEC; ++firstMEC) icount++;
    log << MSG::DEBUG << "Number of McEventCollections=  "<< icount << endmsg;

    // If there is more than one then do the retrieve with the key
    if (icount > 1) {
      log << MSG::DEBUG << "Key = " << key << endmsg;
      const McEventCollection* mcCollptr = nullptr;
      return this->getDH(mcCollptr, key);
    }

    if (icount > 0) {
      // Iterate over all McEvent records
      McEventCollection::const_iterator itr;
      for (itr = mcColl.begin(); itr!=mcColl.end(); ++itr) {
        // Access the HepMC record which is wrapped within McEvent
        const HepMC::GenEvent* genEvt = (*itr);
        if (genEvt == 0) return StatusCode::FAILURE;
        for (auto it: *genEvt) {
          if ((*selector)(it)) mcParticles.push_back(it);
        }
      }
    }
    return StatusCode::SUCCESS;
  }


}

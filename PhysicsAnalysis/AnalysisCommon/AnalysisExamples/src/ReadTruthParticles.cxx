/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// ReadTruthparticles.cxx 
// Implementation file for class ReadTruthparticles
// Author: S.Binet<binet@cern.ch>
// Purpose : Show how one can read the TruthParticleContainer
//           and use the mother/daughter links
/////////////////////////////////////////////////////////////////// 

// STL includes
#include <algorithm>
#include <math.h>
#include <functional>

// Framework includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/DataHandle.h"

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"

#include "McParticleKernel/ITruthParticleCnvTool.h"

#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"

#include "ReadTruthParticles.h"


//////////////////////////////////////////////////////////////////////////////////////
/// Constructor

ReadTruthParticles::ReadTruthParticles(const std::string& name,
                                       ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_cnvTool( )
{
  /// switches to control the analysis through job options

  declareProperty("TruthParticles", m_truthParticlesName = "SpclMC");
  declareProperty("ConvertFromESD", m_fromESD = false );

  declareProperty( "CnvTool",
		   m_cnvTool = CnvTool_t( "TruthParticleCnvTool/CnvTool", 
					  this ),
		   "Handle to the tool converting a McEventCollection into a "
		   "TruthParticleContainer" );
}



/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

ReadTruthParticles::~ReadTruthParticles() {}

////////////////////////////////////////////////////////////////////////////////////
/// Initialize
/// initialize StoreGate

StatusCode ReadTruthParticles::initialize() 
{

  msg() << MSG::INFO
        << "Initializing ReadTruthParticles"
        << endmsg;

  // retrieve the storegate service
  if ( !evtStore().retrieve().isSuccess() ) {
    msg() << MSG::ERROR
          << "Unable to retrieve pointer to StoreGateSvc"
          << endmsg;
     return StatusCode::FAILURE;
  }

  // retrieve the truthparticle converter tool
  if ( !m_cnvTool.retrieve().isSuccess() ) {
    msg() << MSG::ERROR
          << "Could not retrieve the truth particle converter tool !!"
          << endmsg;
    return StatusCode::FAILURE;
  }
 
  return StatusCode::SUCCESS;
}		 

///////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode ReadTruthParticles::finalize() 
{
  return StatusCode::SUCCESS;

}

//////////////////////////////////////////////////////////////////////////////////
/// Execute - on event by event

StatusCode ReadTruthParticles::execute() 
{
  ATH_MSG_DEBUG("execute()");

  /** convert on the fly TruthEvent from the ESD to TruthParticleContainer - Ketevi */
  if ( m_fromESD ) { 
     if ( m_cnvTool->execute().isFailure() ) {
       ATH_MSG_INFO("Could not convert the McEventCollection into "
                    << "a TruthParticleContainer !");
        return StatusCode::SUCCESS;
     }
  }

  /** Read the TruthParticleContainer. Either the one converted from ESD TruthEvent
     as indicated above or the default one provided by the AOD where the conversion
     is done from GEN_AOD. -Ketevi */
  const TruthParticleContainer * mcParts = 0;
  if (!evtStore()->retrieve( mcParts, m_truthParticlesName ).isSuccess() ||
      0 == mcParts ) {
    ATH_MSG_INFO("Could not retrieve TruthParticleContainer at : "
                 << m_truthParticlesName);
    return StatusCode::SUCCESS;
  }

  unsigned int iPart = 0;
  for ( TruthParticleContainer::const_iterator itr = mcParts->begin();
	itr != mcParts->end();
	++itr, ++iPart ) {
    ATH_MSG_INFO("Part " << iPart
                 << " PDG-ID: " << (*itr)->pdgId()
                 << " nChildren: " << (*itr)->nDecay()
                 << " status: " << (*itr)->genParticle()->status()
                 << " bc: " << (*itr)->genParticle()->barcode());
    for ( unsigned int iChild = 0; iChild != (*itr)->nDecay(); ++iChild ){
      const TruthParticle * child = (*itr)->child( iChild );
      if ( 0 != child ) {
        if (msgLvl(MSG::INFO)) {
          msg(MSG::INFO)
            << "\tchild: " << iChild
            << "\tPDGID: " << child->pdgId()
            << " status: " << child->genParticle()->status()
            << " bc: "     << child->genParticle()->barcode()
            << " bc Parents: " << child->nParents() << " [ ";
          for ( unsigned int iMoth = 0; iMoth != child->nParents(); ++iMoth ) {
            msg(MSG::INFO) << child->genMother(iMoth)->barcode() << " ";
          }
          msg(MSG::INFO) << "]" << endmsg;
        }
      } else {
        msg(MSG::WARNING) << "Wrong pointer to child !!" << endmsg;
      }
    }//> loop over children
  }//> end loop over TruthParticles

  return StatusCode::SUCCESS;
}

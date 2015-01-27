/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * TrackIsolationDecorAlg.cxx
 *
 *  Created on: Sep 18, 2014
 *      Author: goblirsc
 */


#include "TrackIsolationDecorAlg.h"
#include "GaudiKernel/Property.h"
#include "xAODMuon/MuonContainer.h"
#include "GaudiKernel/ITHistSvc.h"

using std::string;

//**********************************************************************

TrackIsolationDecorAlg::TrackIsolationDecorAlg(const std::string& name, ISvcLocator* pSvcLocator ) :
  AthAlgorithm( name, pSvcLocator ),
  m_histSvc("THistSvc", name),
  m_iso_tool()
{
  declareProperty("IsolationTool",        m_iso_tool);
  declareProperty("TargetContainer",   m_containerName   = "InDetTrackParticles");
  m_iso_to_run.push_back(xAOD::Iso::ptcone20);
  m_iso_to_run.push_back(xAOD::Iso::ptcone30);
  m_iso_to_run.push_back(xAOD::Iso::ptcone40);
}

//**********************************************************************

TrackIsolationDecorAlg::~TrackIsolationDecorAlg() { }

//**********************************************************************

StatusCode TrackIsolationDecorAlg::initialize() {

  ATH_CHECK(m_histSvc.retrieve());
  ATH_CHECK(m_iso_tool.retrieve());

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode TrackIsolationDecorAlg::finalize() {

	return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode TrackIsolationDecorAlg::execute() {

  // retrieve tag (muon) container
  const xAOD::IParticleContainer* toDecorate = 0;
  if(evtStore()->retrieve(toDecorate, m_containerName).isFailure()) {
    ATH_MSG_FATAL( "Unable to retrieve " << m_containerName );
    return StatusCode::FAILURE;
  }

  xAOD::TrackCorrection corrs;
  corrs.trackbitset = 0x1 << xAOD::Iso::coreTrackPtr;
  xAOD::TrackIsolation result;
  
  for(auto particle : *toDecorate) {
      if (! m_iso_tool->trackIsolation(result, *particle, m_iso_to_run,corrs)){
          ATH_MSG_WARNING("Failed to obrain the isolation for a particle");
      }
      else {
          // the tool sorts the isolation values by descending size
          static SG::AuxElement::Decorator< float > ptcone20("PtCone20");
          static SG::AuxElement::Decorator< float > ptcone30("PtCone30");
          static SG::AuxElement::Decorator< float > ptcone40("PtCone40");
          ptcone20(*particle) = result.ptcones.at(2);
          ptcone30(*particle) = result.ptcones.at(1);
          ptcone40(*particle) = result.ptcones.at(0);
      }

  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

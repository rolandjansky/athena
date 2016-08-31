/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCore/AuxContainerBase.h"

// Local include(s):
#include "HighPtTrackPicker.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"

HighPtTrackPicker::HighPtTrackPicker( const std::string& name, ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc )
{
      declareProperty( "Input", m_inputKey = "InDetTrackParticles" );
      declareProperty( "Output", m_outputKey = "HighPtMCPTracks"); 
      declareProperty( "PtCut", m_pt_cut = 10000.); 
      declareProperty( "IsolationDecorator", m_trk_iso_tool);


}

StatusCode HighPtTrackPicker::initialize() {
    ATH_CHECK(m_trk_iso_tool.retrieve());
    return StatusCode::SUCCESS;
}

StatusCode HighPtTrackPicker::execute() {

   xAOD::TrackParticleContainer* accepted = new xAOD::TrackParticleContainer();
   xAOD::AuxContainerBase* accAux = new xAOD::AuxContainerBase();
   accepted->setStore( accAux ); //< Connect the
   
   const xAOD::TrackParticleContainer* tracks = 0;
   CHECK( evtStore()->retrieve( tracks, m_inputKey ) );
   for(auto iParticle : *tracks) {
      	if (iParticle->pt() > m_pt_cut){
            ATH_CHECK(m_trk_iso_tool->decorate(iParticle));
            xAOD::TrackParticle* trk = new xAOD::TrackParticle();
            trk->makePrivateStore( *iParticle );
            accepted->push_back( trk );
       }
   }
      //record to storegate 
   CHECK( evtStore()->record( accepted, m_outputKey ) );
   CHECK( evtStore()->record( accAux, m_outputKey+"Aux.") );
     // Return gracefully:
   return StatusCode::SUCCESS;

} // namespace CP

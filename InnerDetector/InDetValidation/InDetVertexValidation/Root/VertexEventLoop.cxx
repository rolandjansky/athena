/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#ifdef ROOTCORE
#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
//#endif

#include <InDetVertexValidation/VertexEventLoop.h>

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

#include "InDetVertexValidation/VertexAnalysis.h"
#include "InDetTruthVertexValidation/InDetVertexTruthMatchTool.h"

#include <vector>

// this is needed to distribute the algorithm to the workers
ClassImp(VertexEventLoop)



VertexEventLoop :: VertexEventLoop ()
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().
}



EL::StatusCode VertexEventLoop :: setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.

  job.useXAOD();

  xAOD::Init( "VertexEventLoop" ).ignore();

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode VertexEventLoop :: histInitialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.

  //setup analyzer
  m_analyzer = new VertexAnalysis();

  //use analyzer to create all the histograms
  float m_MaxLayoutEta = 3.0;
  std::vector<std::pair<TH1*,std::string> > vhists = m_analyzer->bookHistograms(m_MaxLayoutEta);
  //add all pointers to the output
  for(auto & hit : vhists )
    wk()->addOutput( hit.first );

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode VertexEventLoop :: fileExecute ()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode VertexEventLoop :: changeInput (bool firstFile)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode VertexEventLoop :: initialize ()
{
  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the output if you have no
  // input events.

  m_event = wk()->xaodEvent();

  Info("initialize()","Number of events = %lli", m_event->getEntries() );

  m_entryNumber = 0;


  m_matchTool = new InDetVertexTruthMatchTool("VertexTruthMatchTool");
  m_matchTool->msg().setLevel( MSG::DEBUG );
  m_matchTool->setProperty( "trackMatchProb", 0.7 );
  m_matchTool->setProperty( "vertexMatchWeight", 0.7 );
  m_matchTool->initialize();




  return EL::StatusCode::SUCCESS;
}



EL::StatusCode VertexEventLoop :: execute ()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.

  m_entryNumber++;

  const xAOD::EventInfo * ei = 0;
  bool result = m_event->retrieve( ei, "EventInfo" );
  if(!result) {
    Error( "execute()", "Failed to retrieve EventInfo on entry %i", static_cast<int>(m_entryNumber));
    return EL::StatusCode::FAILURE;
  }

  if( m_entryNumber % 100 == 0 ) {
    Info( "execute()", 
          "===Start event %i, run %i; %i events processed so far===",
          static_cast<int>( ei->eventNumber() ),
          static_cast<int>( ei->runNumber() ),
          static_cast<int>( m_entryNumber ) );
  }

  //get vertices
  const xAOD::VertexContainer *vxContainer = 0;
  const xAOD::TrackParticleContainer *trackContainer = 0;
  const xAOD::TruthEventContainer *truthEventContainer = 0;
  const xAOD::TruthParticleContainer *truthParticleContainer = 0;

  result = m_event->retrieve( vxContainer, "PrimaryVertices" );
  if(!result) {
    Error( "execute()", "Failed to retrieve PrimaryVertices on entry %i", static_cast<int>(m_entryNumber));
    return EL::StatusCode::FAILURE;
  }

  //---
  const char * trackCollName = "InDetTrackParticles";
  // Work with either the DC14 or MC15 names
  const char * truthEventCollName = m_event->contains<xAOD::TruthEventContainer>("TruthEvents") ? "TruthEvents" : "TruthEvent";
  const char * truthParticleCollName = m_event->contains<xAOD::TruthParticleContainer>("TruthParticles") ? "TruthParticles" : "TruthParticle";

  result = m_event->retrieve( trackContainer, trackCollName );
  if(!result) {
    Error( "execute()", "Failed to retrieve TrackParticles on entry %i", static_cast<int>(m_entryNumber));
    return EL::StatusCode::FAILURE;
  }

  result = m_event->retrieve( truthEventContainer, truthEventCollName );
  if(!result) {
    Error( "execute()", "Failed to retrieve TruthEvents on entry %i", static_cast<int>(m_entryNumber));
    return EL::StatusCode::FAILURE;
  }

  result = m_event->retrieve( truthParticleContainer, truthParticleCollName );
  if(!result) {
    Error( "execute()", "Failed to retrieve TruthParticle on entry %i", static_cast<int>(m_entryNumber));
    return EL::StatusCode::FAILURE;
  }
  //---

  int status = m_matchTool->matchVertices( *vxContainer );
  //status code type -- so 0 is bad
  if(!status) {
    Error( "execute()", "Bad status from matching tool on entry %i", static_cast<int>(m_entryNumber));
    //return success so we keep looping
    return EL::StatusCode::SUCCESS;
  }

  //regular status type so 0 is good
  double m_muCorrected = 1.0;
  status = m_analyzer->execute( *vxContainer, *ei, m_muCorrected, truthEventContainer, trackContainer);
  if( status ) {
    Error( "execute()", "Failure in histogram filling on entry %i", static_cast<int>(m_entryNumber));
    return EL::StatusCode::FAILURE;
  }

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode VertexEventLoop :: postExecute ()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode VertexEventLoop :: finalize ()
{
  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.  This is different from histFinalize() in that it only
  // gets called on worker nodes that processed input events.

  if(m_matchTool) {
    delete m_matchTool;
    m_matchTool = 0;
  }

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode VertexEventLoop :: histFinalize ()
{
  // This method is the mirror image of histInitialize(), meaning it
  // gets called after the last event has been processed on the worker
  // node and allows you to finish up any objects you created in
  // histInitialize() before they are written to disk.  This is
  // actually fairly rare, since this happens separately for each
  // worker node.  Most of the time you want to do your
  // post-processing on the submission node after all your histogram
  // outputs have been merged.  This is different from finalize() in
  // that it gets called on all worker nodes regardless of whether
  // they processed input events.
  return EL::StatusCode::SUCCESS;
}
#endif

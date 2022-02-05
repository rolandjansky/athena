/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexTools/DummyVertexSelectionTool.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandFlat.h"
#include <vector> 

namespace Trk{
 
   //constructor
  DummyVertexSelectionTool::DummyVertexSelectionTool ( const std::string& t, const std::string& n, const IInterface*  p )
    : base_class ( t,n,p ),m_randomEngineName("VertexRnd")
  {
     declareProperty("RandomStreamName",           m_randomEngineName,     "Name of the random number stream");
  }
  
  //destructor
  DummyVertexSelectionTool::~DummyVertexSelectionTool(){}

//initialize
  StatusCode DummyVertexSelectionTool::initialize()
  {
    ATH_CHECK( m_randomSvc.retrieve() );
    return StatusCode::SUCCESS;
   }///EndOfInitialize

   StatusCode DummyVertexSelectionTool::finalize()
   {
     return StatusCode::SUCCESS;
   }

  unsigned int DummyVertexSelectionTool::findVertexInContainer(const xAOD::VertexContainer * vertexContainer ) const
  {
   if(vertexContainer->size()>2)
   {

     // Get the RNG engine.  Reseed if a new event.
     const EventContext& ctx = Gaudi::Hive::currentContext();
     ATHRNG::RNGWrapper* wrapper = m_randomSvc->getEngine(this, m_randomEngineName);
     if (wrapper->evtSeeded (ctx) != ctx.evt()) {
       wrapper->setSeed (name(), ctx);
     }
     CLHEP::HepRandomEngine* engine = wrapper->getEngine (ctx);
   
//only do anything if >1 real vertex is actually present   
     if( CLHEP::RandFlat::shoot(engine)>0.9)
     {
//and only in 10% of cases   let us randomize the vertex choice
     unsigned int ivtx = (unsigned int)( CLHEP::RandFlat::shoot(engine) *  vertexContainer->size());

      ATH_MSG_VERBOSE("Vertex Selection changed to " << ivtx << "!");
      return ivtx;
     
     }//end of random selection
     ATH_MSG_VERBOSE("Vertex Selection unchanged!");
     
   }//end of VertexContainer size check
  
//nothing happened, return the default primary vertex position  
   return 0;
  }

}///End!!!

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexTools/DummyVertexSelectionTool.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "CLHEP/Random/RandFlat.h"
#include <vector> 

namespace Trk{
 
   //constructor
  DummyVertexSelectionTool::DummyVertexSelectionTool ( const std::string& t, const std::string& n, const IInterface*  p )
          : AthAlgTool ( t,n,p ),m_randomSvc("AtRndmGenSvc", n),  m_randomEngineName("VertexRnd"), m_randomEngine(nullptr)
  {
    declareInterface<IVertexSelectionTool> ( this );
     declareProperty("RandomStreamName",           m_randomEngineName,     "Name of the random number stream");
   
  }
  
  //destructor
  DummyVertexSelectionTool::~DummyVertexSelectionTool(){}

//initialize
  StatusCode DummyVertexSelectionTool::initialize()
  {
     msg(MSG::INFO) << "Initialization successful" << endmsg;
   
// Random number service
    if ( m_randomSvc.retrieve().isFailure() ) 
    {
      ATH_MSG_ERROR( "Could not retrieve " << m_randomSvc );
      return StatusCode::FAILURE;
    }

//Get own engine with own seeds:
    m_randomEngine = m_randomSvc->GetEngine(m_randomEngineName);
    if (!m_randomEngine) 
    {
      ATH_MSG_ERROR( "Could not get random engine '" << m_randomEngineName << "'" );
      return StatusCode::FAILURE;
    }

     
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
   
//only do anything if >1 real vertex is actually present   
     if( CLHEP::RandFlat::shoot(m_randomEngine)>0.9)
     {
//and only in 10% of cases   let us randomize the vertex choice
     unsigned int ivtx = (unsigned int)( CLHEP::RandFlat::shoot(m_randomEngine) *  vertexContainer->size());

      ATH_MSG_VERBOSE("Vertex Selection changed to " << ivtx << "!");
      return ivtx;
     
     }//end of random selection
     ATH_MSG_VERBOSE("Vertex Selection unchanged!");
     
   }//end of VertexContainer size check
  
//nothing happened, return the default primary vertex position  
   return 0;
  }

}///End!!!

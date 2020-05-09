/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexTools/TruthVertexSelectionTool.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "CLHEP/Random/RandFlat.h"
#include "AtlasHepMC/SimpleVector.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include <vector> 

namespace Trk{
 
   //constructor
  TruthVertexSelectionTool::TruthVertexSelectionTool ( const std::string& t, const std::string& n, const IInterface*  p )
          : AthAlgTool ( t,n,p )
  {
    declareInterface<IVertexSelectionTool> ( this );
  }
  
  //destructor
  TruthVertexSelectionTool::~TruthVertexSelectionTool(){}

//initialize
  StatusCode TruthVertexSelectionTool::initialize()
  {
    ATH_CHECK( m_mcEventCollectionKey.initialize() );
    msg(MSG::INFO) << "Initialization successful" << endmsg;
     
    return StatusCode::SUCCESS;
  }///EndOfInitialize

  StatusCode TruthVertexSelectionTool::finalize()
  {
     return StatusCode::SUCCESS;
  }

  unsigned int TruthVertexSelectionTool::findVertexInContainer(const xAOD::VertexContainer * vertexContainer ) const
  {
    SG::ReadHandle<McEventCollection> mcCollptr( m_mcEventCollectionKey );

    if ( !mcCollptr.isValid() ) 
    {
      ATH_MSG_WARNING( "Could not retrieve McEventCollection" );
      return 0;
    }    
    ATH_MSG_DEBUG( "HepMC info loaded" );
  
// taking the first event in the truth collection as the signal one
    McEventCollection::const_iterator itr = mcCollptr->begin();   
  
//and the first vertex in its respective collection
    const HepMC::GenEvent *evt = (*itr);

//protection   
    if(!evt) return 0;
   
    HepMC::GenEvent::vertex_const_iterator vitr = evt->vertices_begin();
  
//another one  
    if(!*vitr) return 0;
  
//and its position
    HepMC::FourVector vxGenPos =  (*vitr)->position(); 

//selecting the container vertex closest in Z to the truth one
    unsigned int res_pos = 0;
  
    if(vertexContainer)
    {
      unsigned int cont_size = vertexContainer->size(); 
      if(cont_size>1)
      {
        float z_sim = vxGenPos.z();
        
        float significance = fabs(z_sim - (*vertexContainer)[0]->position().z());//start with the pv value
        for(unsigned int i = 1; i < cont_size; ++i)
        {
          const xAOD::Vertex * vrt = (*vertexContainer)[i];
          float loc_significance =  fabs(z_sim - vrt->position().z())/Amg::error(vrt->covariancePosition(), Trk::z);
          if(loc_significance < significance)
          {
             significance = loc_significance;
             res_pos = i; 
          }//end of comparison
        } //end of the vertex container loop
   
    }//end of the container size check
   }//end of container check
//nothing happened, return the default primary vertex position  
   return res_pos;
  }

}///End!!!

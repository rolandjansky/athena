/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexTools/VertexStoringTool.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "CxxUtils/make_unique.h"

namespace Trk{
 
   //constructor
  VertexStoringTool::VertexStoringTool ( const std::string& t, const std::string& n, const IInterface*  p )
          : AthAlgTool ( t,n,p ),
	    m_key("Unspecified"),
	    m_container("Unspecified"),
	    m_aux("Unspecified")
  {
    declareInterface<IVertexStoringTool> ( this );
    declareProperty("Key", m_key);
  }
  
  //destructor
  VertexStoringTool::~VertexStoringTool(){}

//initialize
  StatusCode VertexStoringTool::initialize()
  {
    // finalize the names of the two StoreGate entries
    m_container = SG::WriteHandle<xAOD::VertexContainer>(m_key);
    m_aux = SG::WriteHandle<xAOD::VertexAuxContainer>(m_key + "Aux.");

    ATH_MSG_DEBUG("Vertex storing tool initialization successful");
    return StatusCode::SUCCESS;
   }///EndOfInitialize

   StatusCode VertexStoringTool::finalize()
   {
     return StatusCode::SUCCESS;
   }

  xAOD::Vertex* VertexStoringTool::storeVertex()
  {
    if (registerContainer().isFailure()) return NULL;
    xAOD::Vertex* s = new xAOD::Vertex();
    m_container->push_back(s);
    return s;
  }

  xAOD::Vertex* VertexStoringTool::storeVertex(const xAOD::Vertex& myVertex)
  {
    if (registerContainer().isFailure()) return NULL;
    xAOD::Vertex* s = new xAOD::Vertex(myVertex);
    m_container->push_back(s);
    return s;
  }

  xAOD::Vertex* VertexStoringTool::storeVertex(const Amg::Vector3D& myVector)
  {
    xAOD::Vertex* s = NULL;
    if (!(s = storeVertex())) return NULL;
    s->setPosition(myVector);
    return s;
  }

  xAOD::Vertex* VertexStoringTool::storeVertex(const Amg::Vector3D& myVector, const AmgSymMatrix(3)& myMatrix)
  {
    xAOD::Vertex* s = NULL;
    if (!(s = storeVertex(myVector))) return NULL;
    s->setCovariancePosition(myMatrix);
    return s;
  }

  StatusCode VertexStoringTool::registerContainer()
  {
    if (m_container.isValid() && m_aux.isValid()) return StatusCode::SUCCESS;
    m_container = CxxUtils::make_unique<xAOD::VertexContainer>();
    m_aux = CxxUtils::make_unique<xAOD::VertexAuxContainer>();
    if (!(m_container.isValid() && m_aux.isValid()))
    {
      ATH_MSG_ERROR("Unable to initialize " << m_container.key() << " and " << m_aux.key());
      return StatusCode::FAILURE;
    }
    m_container->setNonConstStore(&(*m_aux));
    ATH_MSG_VERBOSE( m_container.key() << " and " << m_aux.key() << " initialized and linked");

    return StatusCode::SUCCESS;
  }

}///End trk namespace  

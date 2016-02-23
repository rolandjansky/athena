/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkVertexTools_VertexStoringTool_H
#define TrkVertexTools_VertexStoringTool_H
 
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkVertexFitterInterfaces/IVertexStoringTool.h"

//xAOD include
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/VertexAuxContainer.h"

 /**
  * @class Trk::VertexStoringTool 
  *
  * @author Dave Casper, February 2016
  */
 
 namespace Trk
 {

  class VertexStoringTool : public AthAlgTool, virtual public IVertexStoringTool
  {
  public:
    StatusCode initialize();
    StatusCode finalize();
    
    /**
     * constructor
     */
    VertexStoringTool(const std::string& t, const std::string& n, const IInterface*  p);
    
    /**
     * destructor
     */
    virtual ~VertexStoringTool();
    
    /**
     *Storing
     */
    virtual xAOD::Vertex* storeVertex();
    virtual xAOD::Vertex* storeVertex(const xAOD::Vertex& myVertex);
    virtual xAOD::Vertex* storeVertex(const Amg::Vector3D& myVector);
    virtual xAOD::Vertex* storeVertex(const Amg::Vector3D& myVector, const AmgSymMatrix(3)& myMatrix);

        
  private:
    StatusCode registerContainer();

    std::string m_key;
    SG::WriteHandle<xAOD::VertexContainer> m_container;
    SG::WriteHandle<xAOD::VertexAuxContainer> m_aux;
     
  }; //end of class description
 }//end of namespace definition
 
 #endif

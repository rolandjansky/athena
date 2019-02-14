/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//Author: Lianyou Shan <lianyou.shan@cern.ch>

 #ifndef TrkVertexTools_SecVertexMergingTool_H
 #define TrkVertexTools_SecVertexMergingTool_H
 
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexMergingTool.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"


//xAOD include
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

 /**
  * @class Trk::SecVertexMergingTool 
  *
  * @author Maria Ilaria Besana, June 2014
  *
  * --------------------------------------
  * Changes:
  *
  * David Shope <david.richard.shope@cern.ch> (2016-04-26)
  *
  *   EDM Migration to xAOD - remove method using VxCandidate
  *
  */
 
 namespace Trk
 {
  class IVertexFitter; 

  class SecVertexMergingTool : public AthAlgTool, virtual public IVertexMergingTool
  {
  public:
    StatusCode initialize();
    StatusCode finalize();
    
    /**
     * constructor
     */
    SecVertexMergingTool(const std::string& t, const std::string& n, const IInterface*  p);
    
    /**
     * destructor
     */
    virtual ~SecVertexMergingTool();
    
    /**
     *Merging  
     */
    virtual std::pair<xAOD::VertexContainer*,xAOD::VertexAuxContainer*> mergeVertexContainer(const xAOD::VertexContainer& MyVxCont);

        
  private:
    
    int m_Compatidime ;  // Lianyou added
    float m_minDist ;
    ToolHandle< Trk::IVertexFitter > m_iVertexFitter;

    bool checkCompatibility( const xAOD::Vertex * vx1, const xAOD::Vertex * vx2 );
     
  }; //end of class description
 }//end of namespace definition
 
 #endif

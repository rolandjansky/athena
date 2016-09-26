/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 #ifndef TrkVertexTools_VertexMergingTool_H
 #define TrkVertexTools_VertexMergingTool_H
 
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
  * @class Trk::VertexMergingTool 
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
 
 class IBeamCondSvc;
 
 namespace Trk
 {
  class IVertexFitter; 

  class VertexMergingTool : public AthAlgTool, virtual public IVertexMergingTool
  {
  public:
    StatusCode initialize();
    StatusCode finalize();
    
    /**
     * constructor
     */
    VertexMergingTool(const std::string& t, const std::string& n, const IInterface*  p);
    
    /**
     * destructor
     */
    virtual ~VertexMergingTool();
    
    /**
     *Merging  
     */
    virtual std::pair<xAOD::VertexContainer*,xAOD::VertexAuxContainer*> mergeVertexContainer(const xAOD::VertexContainer& MyVxCont);

        
  private:
    
    ServiceHandle<IBeamCondSvc> m_iBeamCondSvc; //!< pointer to the beam condition service
    ToolHandle< Trk::IVertexFitter > m_iVertexFitter;
    bool m_useBeamConstraint;

    bool checkCompatibility( const xAOD::Vertex * vx1, const xAOD::Vertex * vx2 );
     
  }; //end of class description
 }//end of namespace definition
 
 #endif

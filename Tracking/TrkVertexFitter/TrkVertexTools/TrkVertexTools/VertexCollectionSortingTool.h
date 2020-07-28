/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 #ifndef TrkVertexTools_VertexCollectionSortingTool_H
 #define TrkVertexTools_VertexCollectionSortingTool_H
 
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexCollectionSortingTool.h"

//xAOD include
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

 /**
  * @class Trk::VertexCollectionSortingTool 
  *
  * @author Johanna Bronner, April 2009
  *
  * --------------------------------------
  * Changes:
  *
  * David Shope <david.richard.shope@cern.ch> (2016-04-26)
  *
  *   EDM Migration to xAOD - remove method using VxCandidate
  */
 
 namespace Trk
 {
   class IVertexWeightCalculator;
    
  
  class VertexCollectionSortingTool : public AthAlgTool, virtual public IVertexCollectionSortingTool
  {
  public:
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    
    /**
     * constructor
     */
    
    VertexCollectionSortingTool(const std::string& t, const std::string& n, const IInterface*  p);
    
    /**
     * destructor
     */
     
    virtual ~VertexCollectionSortingTool();
    
    /** Sort **/
    virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
    sortVertexContainer(const xAOD::VertexContainer& MyVxCont) const override;

  private:
    ToolHandle < Trk::IVertexWeightCalculator> m_iVertexWeightCalculator; 
      

 
  }; //end of class description
 }//end of namespace definition
 
 #endif

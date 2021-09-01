/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PropagationEngine.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXENGINE_PROPAGATIONENGINE_H
#define TRKEXENGINE_PROPAGATIONENGINE_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// Trk
#include "TrkExInterfaces/IPropagationEngine.h"
#include "TrkExInterfaces/ExtrapolationMacros.h"
#include "TrkExUtils/ExtrapolationCell.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"
 
namespace Trk {
  
  class Surface;
  class IPropagator;
    
  /** @class PropagationEngine 
  
      Wrapper around the IPropagator interface,
      the neutral propagation is solved by using the surface::straightLineIntersection
      mehtod.
  
      @author Andreas Salzburger -at - cern.ch 
  */
  class PropagationEngine : public AthAlgTool, virtual public IPropagationEngine {
    public:

      /** Constructor */
      PropagationEngine(const std::string&,const std::string&,const IInterface*);
      
      /** Destructor */
      ~PropagationEngine();

      /** AlgTool initialize method */
      StatusCode initialize();
      
      /** AlgTool finalize method */
      StatusCode finalize();

      /** Avoid shaddowing */
      using IPropagationEngine::propagate;

      /** resolve the boundary situation - for charged particles */
      virtual ExtrapolationCode propagate(ExCellCharged& ecCell,
                                          const Surface& sf,
                                          PropDirection dir=alongMomentum,
                                          BoundaryCheck bcheck = true,
                                          bool returnCurvilinear = true) const;  

      /** resolve the boundary situation - for neutral particles */
      virtual ExtrapolationCode propagate(ExCellNeutral& enCell,
                                          const Surface& sf,
                                          PropDirection dir=alongMomentum,
                                          BoundaryCheck bcheck = true,
                                          bool returnCurvilinear = true) const;
       
    protected:
      ToolHandle<IPropagator>     m_propagator{this, "Propagator", "Trk::RungeKuttaPropagator/AtlasRungeKuttaPropagator"};
      double                              m_pathLimitTolerance;


  };
      

} // end of namespace

#endif // TRKEXINTERFACES_INAVIGATIONENGINE_H


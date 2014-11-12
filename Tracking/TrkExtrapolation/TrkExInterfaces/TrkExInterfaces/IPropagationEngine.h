/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPropagationEngine.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXINTERFACES_IPROPAGATIONENGINE_H
#define TRKEXINTERFACES_IPROPAGATIONENGINE_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
#include "TrkExUtils/ExtrapolationCell.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"

namespace Trk {
  
  static const InterfaceID IID_IPropagationEngine("IPropagationEngine", 1, 0);
  
  typedef ExtrapolationCell<TrackParameters>   ExCellCharged;
  typedef ExtrapolationCell<NeutralParameters> ExCellNeutral;
  
  /** @class IPropagationEngine 
  
      A propagation engine wrapping the propagator algtool it respects the path limit
      to stop particles if needed.

      If the propagation is successful to the surface it will return SuccessfulDestination,
      the parameters will be attached to the ExtrapolationCell as leadParameters,
      such that the engine can chose.
  
      It also wraps the MultiTrackParameters
  
      @author Andreas Salzburger -at - cern.ch 
  */
  
  class IPropagationEngine : virtual public IAlgTool {

    public:
     
      /** Virtual destructor */
      virtual ~IPropagationEngine(){}

      /** AlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_IPropagationEngine; }

      /** resolve the boundary situation - for charged particles 
          Possible return codes :
           - SuccessPathLimit (path limit reached)
           - SucessDestination (surface hit, only when finalPropagation == true)
           - InProgress (surface hit, when finalPropagation == false)
           - Recovered (surface not hit, leadParameters stay untouched)
      */
      virtual ExtrapolationCode propagate(ExCellCharged& ecCell,
                                          const Surface& sf,
                                          PropDirection dir=alongMomentum,
                                          BoundaryCheck bcheck = true,
                                          bool returnCurvilinear = true) const = 0;                                                                                         

      /** resolve the boundary situation - for neutral particles
          Possible return codes :
           - SuccessPathLimit (path limit reached)
           - SucessDestination (surface hit, only when finalPropagation == true)
           - InProgress (surface hit, when finalPropagation == false)
           - Recovered (surface not hit, leadParameters stay untouched)
      */
      virtual ExtrapolationCode propagate(ExCellNeutral& enCell,
                                          const Surface& sf,
                                          PropDirection dir=alongMomentum,
                                          BoundaryCheck bcheck = true,
                                          bool returnCurvilinear = true) const = 0;
       
    protected:
      //!< SCREEN output formatting  (SOP) - unify amongst extrapolation engines
      std::string m_sopPrefix;            //!< prefix for screen output
      std::string m_sopPostfix;           //!< prefix for screen output

  };
      

} // end of namespace

#endif // TRKEXINTERFACES_INAVIGATIONENGINE_H


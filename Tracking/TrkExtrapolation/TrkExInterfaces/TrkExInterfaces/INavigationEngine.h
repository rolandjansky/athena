/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// INavigationEngine.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXINTERFACES_INAVIGATIONENGINE_H
#define TRKEXINTERFACES_INAVIGATIONENGINE_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
#include "TrkExUtils/ExtrapolationCell.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"
 
namespace Trk {
  
  static const InterfaceID IID_INavigationEngine("INavigationEngine", 1, 0);
  
  typedef ExtrapolationCell<TrackParameters>   ExCellCharged;
  typedef ExtrapolationCell<NeutralParameters> ExCellNeutral;
  
  /** @class INavigationEngine 
  
      Extrapolation engine interface for Charged and Neutral parameters,
      it serves as the Master extrapolation interface but also as the specialized
      extrapolation engine ones.
  
      @author Andreas Salzburger -at - cern.ch 
  */
  
  class INavigationEngine : virtual public IAlgTool {

    public:
     
      /** Virtual destructor */
      virtual ~INavigationEngine(){}

      /** AlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_INavigationEngine; }

      /** resolve the boundary situation - for charged particles */
      virtual ExtrapolationCode resolveBoundary(ExCellCharged& ecCell, PropDirection dir=alongMomentum) const = 0;                                                                                         

      /** resolve the boundary situation - for neutral particles */
      virtual ExtrapolationCode resolveBoundary(ExCellNeutral& enCell, PropDirection dir=alongMomentum) const = 0;
       
    protected:
      //!< SCREEN output formatting  (SOP) - unify amongst extrapolation engines
      std::string m_sopPrefix;            //!< prefix for screen output
      std::string m_sopPostfix;           //!< prefix for screen output

  };
      

} // end of namespace

#endif // TRKEXINTERFACES_INAVIGATIONENGINE_H


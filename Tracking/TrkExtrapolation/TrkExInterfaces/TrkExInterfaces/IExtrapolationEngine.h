/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IExtrapolationEngine.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXINTERFACES_IEXTRAPOLATIONENGINE_H
#define TRKEXINTERFACES_IEXTRAPOLATIONENGINE_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
#include "TrkDetDescrUtils/GeometrySignature.h"
#include "TrkExUtils/ExtrapolationCell.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"
 
namespace Trk {
  
  static const InterfaceID IID_IExtrapolationEngine("IExtrapolationEngine", 1, 0);
  
  typedef ExtrapolationCell<TrackParameters>   ExCellCharged;
  typedef ExtrapolationCell<NeutralParameters> ExCellNeutral;
  
  /** @class IExtrapolationEngine 
  
      Extrapolation engine interface for Charged and Neutral parameters,
      it serves as the Master extrapolation interface but also as the specialized
      extrapolation engine ones.
  
      @author Andreas Salzburger -at - cern.ch 
  */
  
  class IExtrapolationEngine : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~IExtrapolationEngine(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IExtrapolationEngine; }

       /** charged extrapolation */
       virtual ExtrapolationCode extrapolate(ExCellCharged& ecCharged,
                                             const Surface* sf = 0,
                                             BoundaryCheck bcheck = true) const = 0;


       /** neutral extrapolation */
       virtual ExtrapolationCode extrapolate(ExCellNeutral& ecNeutral,
                                             const Surface* sf = 0,
                                             BoundaryCheck bcheck = true) const = 0;
                                
                                
      /** define for which GeometrySignature this extrapolator is valid */
      virtual GeometryType geometryType() const = 0;                            
      
    protected:
        
      //!< SCREEN output formatting  (SOP) - unify amongst extrapolation engines
      std::string                                         m_sopPrefix;            //!< prefix for screen output
      std::string                                         m_sopPostfix;           //!< prefix for screen output

  };
      

} // end of namespace

#endif // TRKEXINTERFACES_IEXTRAPOLATIONENGINE_H


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMaterialEffectsEngine.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXINTERFACES_IMATERIAKEFFECTSENGINE_H
#define TRKEXINTERFACES_IMATERIAKEFFECTSENGINE_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
#include "TrkExUtils/ExtrapolationCell.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"

namespace Trk {
  
  static const InterfaceID IID_IMaterialEffectsEngine("IMaterialEffectsEngine", 1, 0);
  
  typedef ExtrapolationCell<TrackParameters>   ExCellCharged;
  typedef ExtrapolationCell<NeutralParameters> ExCellNeutral;
  
  
  /** @class IMaterialEffectsEngine 
  
      Material effects engine interface for charged and neutral (fast track simulation) ,
      the update is alwyas on the:
       - eCell.leadParmaeters && eCell.leadLayer 
       - if eCell.leadPameters == eCell.startParamters clone to new parameters
         else : update the new parameters
  
      @author Andreas Salzburger -at - cern.ch 
  */
  class IMaterialEffectsEngine : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~IMaterialEffectsEngine(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IMaterialEffectsEngine; }

       /** charged extrapolation */
       virtual ExtrapolationCode handleMaterial(ExCellCharged& ecCharged,
                                                PropDirection dir=alongMomentum,
                                                MaterialUpdateStage matupstage=fullUpdate ) const = 0;


       /** neutral extrapolation */
       virtual ExtrapolationCode handleMaterial(ExCellNeutral& ecNeutral,
                                                PropDirection dir=alongMomentum,
                                                MaterialUpdateStage matupstage=fullUpdate) const = 0;
      
    protected:
      std::string                               m_sopPrefix;            //!< prefix for screen output
      std::string                               m_sopPostfix;           //!< prefix for screen output

  };
      

} // end of namespace

#endif // TRKEXINTERFACES_IMATERIAKEFFECTSENGINE_H


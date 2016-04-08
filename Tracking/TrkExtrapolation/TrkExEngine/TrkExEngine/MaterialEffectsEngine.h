/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialEffectsEngine.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXINTERFACES_MATERIAKEFFECTSENGINE_H
#define TRKEXINTERFACES_MATERIAKEFFECTSENGINE_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
// Trk
#include "TrkExInterfaces/IMaterialEffectsEngine.h"
#include "TrkExInterfaces/ExtrapolationMacros.h"
#include "TrkExUtils/ExtrapolationCell.h"
#include "TrkExUtils/MaterialInteraction.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"
 
namespace Trk {
  
  class Layer;
    
  /** @class MaterialEffectsEngine 
  
      Material effects engine interface for charged and neutral (fast track simulation) ,
      the update is alwyas on the:
       - eCell.leadParmaeters && eCell.leadLayer 
       - if eCell.leadPameters == eCell.startParamters clone to new parameters
         else : update the new parameters
  
      @author Andreas Salzburger -at - cern.ch 
  */
  class MaterialEffectsEngine : public AthAlgTool, virtual public IMaterialEffectsEngine {
    public:

      /** Constructor */
      MaterialEffectsEngine(const std::string&,const std::string&,const IInterface*);
      
      /** Destructor */
      ~MaterialEffectsEngine();

      /** AlgTool initialize method */
      StatusCode initialize();
      
      /** AlgTool finalize method */
      StatusCode finalize();
      
      /** charged extrapolation */
      virtual ExtrapolationCode handleMaterial(ExCellCharged& ecCharged,
                                               PropDirection dir=alongMomentum,
                                               MaterialUpdateStage matupstage=fullUpdate) const;

      /** neutral extrapolation - only for Fatras, dummy implementation here */
      virtual ExtrapolationCode handleMaterial(ExCellNeutral& ecNeutral,
                                               PropDirection dir=alongMomentum,
                                               MaterialUpdateStage matupstage=fullUpdate) const;
      
    protected:
      /** charged extrapolation */
      const TrackParameters* updateTrackParameters(const Trk::TrackParameters& parameters,
                                                   Trk::ExCellCharged& eCell,
                                                   Trk::PropDirection dir,
                                                   Trk::MaterialUpdateStage matupstage) const; 
        
      MaterialInteraction                          m_interactionFormulae;     //!< the formulas concentrated
      ParticleMasses                               m_particleMasses;          //!< struct of Particle masses   
      bool                                         m_eLossCorrection;         //!< apply the energy loss correction
      bool                                         m_eLossMpv;                //!< apply the energy loss correction as most probable value
      bool                                         m_mscCorrection;           //!< apply the multiple (coulomb) scattering correction

  };
      

} // end of namespace

#endif // TRKEXINTERFACES_MATERIAKEFFECTSENGINE_H


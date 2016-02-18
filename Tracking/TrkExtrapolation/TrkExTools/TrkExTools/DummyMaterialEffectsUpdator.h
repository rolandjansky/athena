/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DummyMaterialEffectsUpdator.h, c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXTOOLS_DUMMYMATERIALEFFECTSUPDATOR_H
#define TRKEXTOOLS_DUMMYMATERIALEFFECTSUPDATOR_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"
// Trk
#include "TrkExInterfaces/IMaterialEffectsUpdator.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {

  class Layer;
  //class TrackParameters;
  class MaterialProperties;
  class IMaterialMapper;
  
  /** @class DummyMaterialEffectsUpdator
    
    Debug tool for navigation tests

    @author Andreas.Salzburger@cern.ch
    
    */
  class DummyMaterialEffectsUpdator : public AthAlgTool,
                                 virtual public IMaterialEffectsUpdator {
    public:
           
      /** AlgTool like constructor */
      DummyMaterialEffectsUpdator(const std::string&,const std::string&,const IInterface*);

      /**Virtual destructor*/
      virtual ~DummyMaterialEffectsUpdator();
       
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
                                          
      /** Updator interface (full update for a layer): Dummy full update
        */ 
      const TrackParameters*  update( const TrackParameters* parm,
                                      const Layer& sf,
                                      PropDirection dir=alongMomentum,
                                      ParticleHypothesis particle=pion,
                                      MaterialUpdateMode matupmode=addNoise) const;
      
      /** Updator interface (pre-update for a layer): Dummy pre update
        */ 
      const TrackParameters*   preUpdate( const TrackParameters* parm,
                                          const Layer& sf,
                                          PropDirection dir=alongMomentum,
                                          ParticleHypothesis particle=pion,
                                          MaterialUpdateMode matupmode=addNoise) const;
       
      /** Updator interface (post-update for a layer): Dummy post update
        */ 
      const TrackParameters*   postUpdate(const TrackParameters& parm,
                                          const Layer& sf,
                                          PropDirection dir=alongMomentum,
                                          ParticleHypothesis particle=pion,
                                          MaterialUpdateMode matupmode=addNoise) const;
                                                                             
       /** Updator interface:
         The parmeters are given as a reference, 
         MaterialProperties based material update
         */
       const TrackParameters*      update( const TrackParameters& parm,
                                           const MaterialProperties& mprop,
                                           double pathcorrection,
                                           PropDirection dir=alongMomentum,
                                           ParticleHypothesis particle=pion,
                                           MaterialUpdateMode matupmode=addNoise) const;


       /** User updator interface (full update for a layer):
         The parmeters are given as a pointer, they are deleted inside the update method.
         Update occurs on the place where the parameters parm are according to the specified MaterialEffectsOnTrack
         */
       virtual const TrackParameters*      update( const TrackParameters* parm,
                                                   const MaterialEffectsOnTrack& ,
                                                   ParticleHypothesis,
                                                   MaterialUpdateMode) const { return parm; }


      /** Validation Action:
          Can be implemented optionally, outside access to internal validation steps */
      virtual void validationAction() const;
                  
   protected:
      // ---------------- validation mode ------------------
      bool                         m_considerPrePostMapping;    //!< take pre/post mapping into account
      bool                         m_validationMode;            //!< boolean switch for the validation mode
      int                          m_validationDirectionSwitch; //!< jO switch for PropDirection
      PropDirection                m_validationDirection;       //!< distinction between forward and backward validation
      ToolHandle< IMaterialMapper > m_materialMapper;            //!< the material mapper for recording the layer material 
      mutable double               m_etaFinalize;               //!< recored values for the finalizeEvent() call of the IMM
      mutable double               m_phiFinalize;               //!< recorded values for the finalizeEvent() call of the IMM
        
  };

} // end of namespace


#endif // TRKEXTOOLS_DUMMYMATERIALEFFECTSUPDATOR_H


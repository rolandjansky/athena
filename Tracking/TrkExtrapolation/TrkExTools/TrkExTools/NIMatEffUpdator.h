/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NIMatEffUpdator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_TRKEXTOOLS_NIMATEFFUPDATOR_H
#define TRK_TRKEXTOOLS_NIMATEFFUPDATOR_H

// GaudiKernel & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Trk
#include "TrkExInterfaces/ITimedMatEffUpdator.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include "TrkDetDescrUtils/GeometrySignature.h" 

class TTree;
  
namespace Trk {

  class IMaterialEffectsUpdator;
      
  /** @class NIMatEffUpdator
 
      Non-interacting timed material effects updator.

      Dummy interaction and dense material update.

      Layer (point-like) update as in MatEffUpdator.
 
      @author sarka.todorova@cern.ch
   */
   
  class NIMatEffUpdator : public AthAlgTool,
                                    virtual public ITimedMatEffUpdator {
    public:      
      /**AlgTool constructor for NIMatEffUpdator*/
      NIMatEffUpdator(const std::string&,const std::string&,const IInterface*);
      /**Destructor*/
      virtual ~NIMatEffUpdator();
       
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
      
      /** Updator interface (full update for a layer): 
         given track parameters are deleted internally,
         if no update has to be done, 
         the pointer is returned
        */ 
      const Trk::TrackParameters*  update(const Trk::TrackParameters* parm,
					  const Trk::Layer& sf,
					  Trk::TimeLimit& time, Trk::PathLimit& path,
                                          Trk::GeometrySignature geoID,
					  Trk::PropDirection dir=Trk::alongMomentum,
					  Trk::ParticleHypothesis particle=Trk::pion) const;
      
      const Trk::TrackParameters*  update( double time,
					   const Trk::TrackParameters* parm,
                                      const Trk::MaterialEffectsOnTrack& meff,
                                      Trk::ParticleHypothesis particle=Trk::pion,
                                      Trk::MaterialUpdateMode matupmode=Trk::addNoise) const;
                                      
     
       /** Updator interface:
         The parmeters are given as a reference, 
         MaterialProperties based material update
         */
       const Trk::TrackParameters*   update( double time,
					   const Trk::TrackParameters& parm,
                                           const Trk::MaterialProperties& mprop,
                                           double pathcorrection,
                                           Trk::PropDirection dir=Trk::alongMomentum,
                                           Trk::ParticleHypothesis particle=Trk::pion,
                                           Trk::MaterialUpdateMode matupmode=Trk::addNoise) const;
    
      /** dummy helper function for a brem photon record */
      void recordBremPhoton(double , double ,double ,
                           const Amg::Vector3D& , Amg::Vector3D& ,
			    Trk::ParticleHypothesis  ) const {};
                          
   private:
     
     /** rely on MaterialEffectsIUpdator */
     ToolHandle<Trk::IMaterialEffectsUpdator>  m_matUpdator;
      
				    };
}

#endif // TRK_TRKEXTOOLS_NIMATEFFUPDATOR_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// StaticEngine.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXENINGE_STATICENGINE_H
#define TRKEXENINGE_STATICENGINE_H

#ifndef TRKEXENINGE_OUTPUTHELPER 
#define TRKEXENINGE_OUTPUTHELPER 
#define OH_CHECKFOUND(object) ( object ? "found" : "not found")
#endif

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// Trk
#include "TrkExInterfaces/IExtrapolationEngine.h"
#include "TrkExInterfaces/ExtrapolationMacros.h"
#include "TrkExUtils/ExtrapolationCell.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"

namespace Trk {

  class IPropagationEngine;    
  class IMaterialEffectsEngine;
  class INavigationEngine;    

    
  /** @class StaticEngine
    
    Extrapolation engine for static layer & volume setup.
    
    This engine relies on the fact that every position in a static layer setup can be uniquely associated to a layer (NavigationLayer or real physical layer),
    and thus to a voume at navigation level. The extrapolation process within a fully static setup is then realised as a step from layer to layer within a volume,
    and from volume to volume at a higher level.

    The entire code is written as a template in either charged or neutral parameters.  
    
    @author Andreas.Salzburger -at- cern.ch
  
  */
  class StaticEngine : public AthAlgTool, virtual public IExtrapolationEngine {

      public:
          
        /** @enum ResolveLayerType 
            - use for code readability
        */
        enum ResolveLayerType {
          StartLayer                = 0,
          NavigationLayer           = 1,
          PassThroughLayer          = 2,
          SubStructureLayer         = 3,
          DestinationLayer          = 4,
          StartAndDestinationLayer  = 6,
          UndefinedLayer            = 5
        };          
        
        /** Constructor */
        StaticEngine(const std::string&,const std::string&,const IInterface*);
        
        /** Destructor */
        ~StaticEngine();

        /** AlgTool initialize method */
        StatusCode initialize();
        
        /** AlgTool finalize method */
        StatusCode finalize();
        
        using IExtrapolationEngine::extrapolate;
        
        /** charged extrapolation - public interface */
        virtual ExtrapolationCode extrapolate(ExCellCharged& ecCharged,
                                              const Surface* sf = 0,
                                              BoundaryCheck bcheck = true) const;


        /** neutral extrapolation - public interface */
        virtual ExtrapolationCode extrapolate(ExCellNeutral& ecNeutral,
                                              const Surface* sf = 0,
                                              BoundaryCheck bcheck = true) const;
                         
        /** define for which GeometrySignature this extrapolator is valid - this is GLOBAL */
        GeometryType geometryType() const;                             
                         
     private:
        /** main loop extrapolation method */
        template <class T> ExtrapolationCode extrapolateT(ExtrapolationCell<T>& eCell,
                                                          const Surface* sf = 0,
                                                          PropDirection dir=alongMomentum,
                                                          BoundaryCheck bcheck = true) const;
                                                                  
        /** init Navigation for static setup */
        template <class T> ExtrapolationCode initNavigationT(ExtrapolationCell<T>& eCell,
                                                             const Surface* sf = 0,
                                                             PropDirection dir=alongMomentum,
                                                             BoundaryCheck bcheck = true) const;
                                                          
        /** main static layer handling */                                                  
        template <class T> ExtrapolationCode handleLayerT(ExtrapolationCell<T>& eCell,
                                                          const Surface* sf = 0,
                                                          PropDirection dir=alongMomentum,
                                                          BoundaryCheck bcheck = true) const;  

        /** main sub structure layer handling */                                                  
        template <class T> ExtrapolationCode resolveLayerT(ExtrapolationCell<T>& eCell,
                                                           const Trk::Surface* sf,
                                                           PropDirection dir=alongMomentum,
                                                           BoundaryCheck bcheck = true,
                                                           bool hasSubStructure = false,
                                                           bool isStartLayer = false,
                                                           bool isDestinationLayer =false) const;  
        /** handle the failure - as configured */
        template <class T> ExtrapolationCode handleReturnT(ExtrapolationCode eCode,
                                                           ExtrapolationCell<T>& eCell,
                                                           const Surface* sf = 0,
                                                           PropDirection dir=alongMomentum,
                                                           BoundaryCheck bcheck = true) const;
             
        ToolHandle<IPropagationEngine>     m_propagationEngine{this, "PropagationEngine", "Trk::PropagationEngine/AtlasStaticPropagation"};        //!< the used propagation engine
        ToolHandle<INavigationEngine>      m_navigationEngine{this, "NavigationEngine", "Trk::StaticNavigationEngine/AtlasStaticNavigation"};         //!< the navigation engine to resolve the boundary
        ToolHandle<IMaterialEffectsEngine> m_materialEffectsEngine{this, "MaterialEffectsEngine", "Trk::MaterialEffectsEngine/AtlasStaticNavigationMaterialEffects"};    //!< the material effects updated
            
    };

  inline GeometryType StaticEngine::geometryType() const 
      { return Trk::Static; }


} // end of namespace

//!< define the templated function    
#include "StaticEngine.icc"  

#endif // TRKEXINTERFACES_IStaticEngine_H


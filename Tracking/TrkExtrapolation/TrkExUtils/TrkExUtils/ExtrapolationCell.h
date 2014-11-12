/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExtrapolationCell.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXUTILS_EXTRAPOLATIONCELL_H
#define TRKEXUTILS_EXTRAPOLATIONCELL_H

#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include "TrkExUtils/TransportJacobian.h"

#ifndef TRKEXUTILS_CHECKPATHMACRO
#define TRKEXUTILS_CHECKPATHMACRO
#define reachedLimit(current, limit, tolerance) ( limit > 0 && (current-limit)*(current-limit)/(limit*limit) < tolerance*tolerance )
#endif

namespace Trk {
    
    class MaterialProperties;
    class TrackingVolume;
    class Layer;
    class Surface;
    
    /** enumeration to decode - for Extrapolation steering 
         - they are used as bits in the configuration integer
      */
    class ExtrapolationMode {
      
      public :
       enum eMode {    
         Direct                        =  1,  // call propagator directly, no navigation 
         Destination                   =  2,  // try to hit the destination, if not other means to stop
         StopWithPathLimit             =  3,  // stop when the path limit is reached
         StopWithMaterialLimitX0       =  4,  // stop when the material limit is reached in X0
         StopWithMaterialLimitL0       =  5,  // stop when the material limit is reached in L0
         StopAtBoundary                =  6,  // stop at the next ID / Calo / MS boundary
         CollectSensitive              =  7,  // collect parameters on sensitive elements
         CollectPassive                =  8,  // collect parameters on passive layers
         CollectBoundary               =  9,  // collect parameters on boundary parameters
         CollectMaterial               = 10,  // collect all material on the way
         CollectJacobians              = 11,  // collect the transport jacobians
         CollectPathSteps              = 12,  // collect the single path steps
         AvoidFallback                 = 13   // don't fallback to propagation
      };
      
    };
    
    
    /** @class ExtrapolationConfig 
      - this is a collection of extrapolation modes and a simple check
    */
    class ExtrapolationConfig {
      public:
        /** Constructor */
        ExtrapolationConfig(unsigned int evalue=0) :
          value(evalue)
        {}
        
        /** Copy Constructor */
        ExtrapolationConfig(const ExtrapolationConfig& eConfig) :
          value(eConfig.value)
        {}
        
        /** add a configuration mode */
        void addMode(ExtrapolationMode::eMode em) {
            // set the bit corresponding to this mode
            value |= (1 << int(em));
        }
        
        /** check the configuration mode */
        bool checkMode(ExtrapolationMode::eMode em) const  {
            // check if the bit is set or not
            return (value & (1 << int(em)));
        }
      private:
        unsigned int value;
        
    };
    
    
    /** @class ExtrapolationCode 
        @author Andreas.Salzburger -at- cern.ch 
    */
    class ExtrapolationCode {
      
      public :  
    
        enum eCode {
              Unset                       = 0,   // no code set yet
              InProgress                  = 1,   // successful : extrapolation in process
              SuccessDestination          = 2,   // successful : destination reached 
              SuccessBoundaryReached      = 3,   // successful : boundary reached & configured to do so
              SuccessPathLimit            = 4,   // successful : path limit reached & configured to do so
              SuccessMaterialLimit        = 5,   // successful : material limit reached & configured to do so
              Recovered                   = 6,   // successful : recovered & configured to do so
              FailureDestination          = 7,   // failure    : could not reach destination
              FailureLoop                 = 8,   // failure    : loop or oscillation between volumes
              FailureNavigation           = 9,   // failure    : general navigation failure
              FailureUpdateKill           = 10,  // failure    : updated track under threshold
              FailureConfiguration        = 11,  // failure    : general configuration failure
              LeftKnownWorld              = 12   // successful ? failure ? if we just knew ...
        };
          
        /** the actual code */
        eCode code;
        
        /* create a simple extrapolation code */
        ExtrapolationCode(eCode c) :
          code(c){}

        /** assigment operator - because we can */
        ExtrapolationCode& operator=(const eCode& ec){
            code = ec;
            return (*this);
        }
          
        /** == operator to eCode */
        bool operator==(const eCode& ec) const { return (ec==code); } 
          
        /** != operator to eCode */
        bool operator!=(const eCode& ec) const { return (ec!=code); }  
    
        /** return inProgress */
        bool inProgress() const { return (code == InProgress); }              
                    
        /** return success */
        bool isSuccess() const { return (code > InProgress && code < Recovered); }  
        
        /** return success or recovered */
        bool isSuccessOrRecovered() const { return (code > InProgress && code <= FailureDestination); }  
        
        /** return failure */
        bool isFailure() const { return (code > Recovered); }
        
        /** return failure or recovered */
        bool isFailureOrRecovered() const { return (code > SuccessMaterialLimit); };

        /** toString */
        const std::string& toString() const { return s_ecodeNames[code]; }
        
     private :
         static std::vector<std::string>  s_ecodeNames; 
        
    };

    /** @class ExtrapolationStep 
    
        templated class to record the different possible entities during extrapolation
    
        */
    template <class T> class ExtrapolationStep {
      public:  
        const T*                    parameters;             //!< the parameters of this step
        const Surface*              surface;                //!< the surface where the step is bound
        const Layer*                layer;                  //!< the associatedLayer() or materialLayer() of the surface
        ExtrapolationConfig         stepConfiguration;      //!< sensitive, passive, boundary to name the parameters
        const MaterialProperties*   material;               //!< the associated material 
        Amg::Vector3D               materialPosition;       //!< position from where the material is taken   
        double                      materialScaling;        //!< scale factor for the material as calculated
        const TransportJacobian*    transportJacobian;      //!< the transport jacobian from the last step
        double                      pathLength;             //!< the path length from the last step
        
        ExtrapolationStep(const T* pars = 0,
                          const Surface* sf = 0,  
                          ExtrapolationConfig eConfig = ExtrapolationConfig(),
                          const MaterialProperties* mprop=0,
                          const TransportJacobian* tjac=0,
                          double pLength = -1.) :
         parameters(pars),
         surface(sf),
         layer(0),        
         stepConfiguration(eConfig),              
         material(mprop),
         materialPosition(Amg::Vector3D(0.,0.,0.)), 
         materialScaling(1.),         
         transportJacobian(tjac),
         pathLength(pLength)
        {}        
        
    };
        
    /** @class ExtrapolationCell
    
        templated class as an input-output object of the extrapolation,
        only public members, since it is a container class
    
        @author Andreas.Salzburger -at- cern.ch 
    */
    
    template <class T> class ExtrapolationCell {
        public :
            const T*                                startParameters;        //!< by reference - need to be defined
            const TrackingVolume*                   startVolume;            //!< the start volume - needed for the volumeToVolume loop
            const Layer*                            startLayer;             //!< the start layer  - needed for layerToLayer loop
                                                                            
            const T*                                endParameters;          //!< by pointer - are newly created and can be optionally 0
            const TrackingVolume*                   endVolume;              //!< the end Volume - can be optionally 0 (needs other trigger to stop)
            const Layer*                            endLayer;               //!< the end Layer  - can be optionally 0 (needs other trigger to stop)
            const Surface*                          endSurface;             //!< keep track of the destination surface - can be optionally 0
                                                                            
            const T*                                leadParameters;         //!< the one last truely valid parameter in the stream     
            const TrackingVolume*                   leadVolume;             //!< the lead Volume - carrying the navigation stream
            const Layer*                            leadLayer;              //!< the lead Layer  - carrying the navigation stream
            const Surface*                          leadLayerSurface;       //!< if the lead layer has sub structure that is the first one to start with
                                                    
            const T*                                lastBoundaryParameters; //!< this is the last boundary surface to prevent loops
            const Surface*                          lastBoundarySurface;    //!< this is the last boundary surface to prevent loops
                                                    
            int                                     navigationStep;         //!< a counter of the navigation Step
            double                                  pathLength;             //!< the path length accumulated
            double                                  pathLimit;              //!< the maximal limit of the extrapolation
            
            double                                  materialX0;             //!< collected material so far in units of X0
            double                                  materialLimitX0;        //!< given material limit in X0
            double                                  materialL0;             //!< collected material so far in units of L0
            double                                  materialLimitL0;        //!< given material limit in L0
            int                                     materialProcess;        //!< the material process to be generated
                                                                              
            ParticleHypothesis                      pHypothesis;            //!< what particle hypothesis to be used, default : pion
            MagneticFieldProperties                 mFieldMode;             //!< what magnetic field mode to be used, default : fullField
            MaterialUpdateMode                      materialUpdateMode;     //!< how to deal with the material effect, default: addNoise
            bool                                    navigationCurvilinear;  //!< stay in curvilinear parameters where possible, default : true
            bool                                    sensitiveCurvilinear;   //!< stay in curvilinear parameters even on the destination surface
            bool                                    destinationCurvilinear; //!< return curvilinear parameters even on destination

            std::vector< ExtrapolationStep<T> >     extrapolationSteps;     //!< parameters on sensitive detector elements
                
            ExtrapolationConfig                     extrapolationConfiguration; //!< overall global configuration
            
        
          /** start parameters are compulory to be given */  
          ExtrapolationCell(const T& sParameters, unsigned int econfig=1) :
            startParameters(&sParameters),
            startVolume(0),
            startLayer(0),
            endParameters(0),
            endVolume(0),
            endLayer(0),
            endSurface(0),
            leadParameters(&sParameters),
            leadVolume(0),
            leadLayer(0),
            leadLayerSurface(0),
            lastBoundaryParameters(0),
            lastBoundarySurface(0),
            navigationStep(0),
            pathLength(0.),
            pathLimit(-1),
            materialX0(0.),
            materialLimitX0(-1.),
            materialL0(0.),
            materialLimitL0(-1.),
            materialProcess(0),
            pHypothesis(Trk::pion),
            mFieldMode(Trk::MagneticFieldProperties(Trk::FullField)),
            materialUpdateMode(Trk::addNoise),
            navigationCurvilinear(true),
            sensitiveCurvilinear(false),
            destinationCurvilinear(false),
            extrapolationConfiguration(econfig)
        {}

          /** add a configuration mode */
          void addConfigurationMode(ExtrapolationMode::eMode em) {
              // set the bit corresponding to this mode
              extrapolationConfiguration.addMode(em);
          }
          
          /** check the configuration mode */
          bool checkConfigurationMode(ExtrapolationMode::eMode em) const  {
              // check if the bit is set or not
              return extrapolationConfiguration.checkMode(em);
          }
          
          /** check if you are still at the last boundary surface */
          bool onLastBoundary() const { return (leadParameters == lastBoundaryParameters); }
          
          /** fill or attach the parameters from a step - memory management steered by extrapolationConfiguration */
          void stepParameters(const T* pars, ExtrapolationMode::eMode fillMode);
          
          /** fill transport information - path length and TransportJacobian 
              - jacobians need to be cleared */
          void stepTransport(const Surface& sf, double pathLength=0., const TransportJacobian* tjac=0);
          
          /** fill or attach material, jacobian, step length 
              - material is just a pointer copy */
          void stepMaterial(const Surface& sf, const Layer* lay, const Amg::Vector3D& position, double sfactor, const MaterialProperties* mprop=0);
            
          /** check if this is the initial volume */
          bool initialVolume() const { return (leadVolume==startVolume); }    
                                                 
          /** trigger for final volume */
          bool finalVolumeReached() const { return (leadVolume==endVolume && endVolume); }

          /** the materialLimitReached */
          bool pathLimitReached(double tolerance=0.001) const { 
              return (checkConfigurationMode(Trk::ExtrapolationMode::StopWithPathLimit) && reachedLimit(pathLength,pathLimit,tolerance) );
          }
          
          /** the materialLimitReached */
          bool materialLimitReached(double tolerance=0.001) const { 
              return ( (checkConfigurationMode(Trk::ExtrapolationMode::StopWithMaterialLimitX0) && reachedLimit(materialX0,materialLimitX0,tolerance) ) ||
                       (checkConfigurationMode(Trk::ExtrapolationMode::StopWithMaterialLimitL0) && reachedLimit(materialL0,materialLimitL0,tolerance) ) );
          }
          
          
          /** prepare destination as new start point - optimised for Kalman filtering */
          void restartAtDestination();
          
          /** finalize call - this one is called by the ExtrapoaltionEngine */
          void finalize(const ExtrapolationCode& ec);
          
          /** memory cleanup */
          void emptyGarbageBin(const ExtrapolationCode& ec);

	  /** set ParticleHypothesis */
	  void setParticleHypothesis(const ParticleHypothesis& hypo) {
	    pHypothesis = hypo;
	  }

        private :
           mutable std::vector<const T*>    m_garbageCollection; 
        
    };


    template <class T> void ExtrapolationCell<T>::restartAtDestination() {
        /** set end to start - and reset the rest */
        startParameters             = endParameters;
        startVolume                 = endVolume;
        startLayer                  = endLayer;
        endParameters               = 0;
        endVolume                   = 0;
        endLayer                    = 0;
        endSurface                  = 0;
        leadParameters              = startParameters;
        leadVolume                  = 0;
        leadLayer                   = 0;
        leadLayerSurface            = 0;
        lastBoundaryParameters      = 0;
        lastBoundarySurface         = 0;
        navigationStep              = 0;
        pathLength                  = 0.;
        materialX0                  = 0.;
        materialL0                  = 0.;
        // clear the vector
        extrapolationSteps.clear();        
    }

    template <class T> void ExtrapolationCell<T>::finalize(const ExtrapolationCode& ec) { 
      // set the leadParameters to the endParameters if anything happened here and the code wass succesful
      if (ec.isSuccessOrRecovered() && leadParameters != startParameters) 
          endParameters = leadParameters;
      // now do the cleanup
      emptyGarbageBin(ec); 
    }

    template <class T> void ExtrapolationCell<T>::emptyGarbageBin(const ExtrapolationCode& ec) 
    { for (auto bC : m_garbageCollection) delete bC;
      // in case of failure of the extrapolation stream, clear all the caches
      if (ec.isFailure()) {
          for (auto es : extrapolationSteps) {
              delete es.parameters;
              delete es.transportJacobian;      
         }
      }
    }
    
    template <class T>  void ExtrapolationCell<T>::stepParameters(const T* parameters, Trk::ExtrapolationMode::eMode fillMode)
    {  
       // this is the garbage bin collection
       if (!checkConfigurationMode(fillMode)){
           // only dump them in the garbage bin
           m_garbageCollection.push_back(parameters);
           return;
       }       
       // find out if you want to attach or you need a new one 
       // current step surface
       const Surface* cssf = &(parameters->associatedSurface());
       // get the last step surface - if it is identical with the current one -> attach information
       const Surface* lssf = extrapolationSteps.size() ? extrapolationSteps[extrapolationSteps.size()-1].surface : 0;
       // create a new step 
       if (cssf != lssf)
           extrapolationSteps.push_back(ExtrapolationStep<T>());
       // fill the parameters, the surface and add the mode
       extrapolationSteps[extrapolationSteps.size()-1].parameters = parameters;
       extrapolationSteps[extrapolationSteps.size()-1].surface    = cssf;
       extrapolationSteps[extrapolationSteps.size()-1].stepConfiguration.addMode(fillMode);
    }
    
    template <class T>  void ExtrapolationCell<T>::stepTransport(const Surface& sf,
                                                                 double pLength,
                                                                 const TransportJacobian* tjac)
    {  
       // find out if you want to attach or you need a new one 
       // current step surface
       const Surface* cssf = &sf;
       // get the last step surface - if it is identical with the current one -> attach information
       const Surface* lssf = extrapolationSteps.size() ? extrapolationSteps[extrapolationSteps.size()-1].surface : 0;
       // only create a new step for a transport jacobian
       if (tjac){
           // create a new step 
           if (cssf != lssf)
               extrapolationSteps.push_back(ExtrapolationStep<T>());
           // set the surface
           extrapolationSteps[extrapolationSteps.size()-1].surface    = cssf;
           // set the the transport information 
           extrapolationSteps[extrapolationSteps.size()-1].transportJacobian = tjac;
           extrapolationSteps[extrapolationSteps.size()-1].stepConfiguration.addMode(Trk::ExtrapolationMode::CollectJacobians);
           // fill the step path length
           if (pLength > 0.){
               extrapolationSteps[extrapolationSteps.size()-1].pathLength = pLength;
               extrapolationSteps[extrapolationSteps.size()-1].stepConfiguration.addMode(Trk::ExtrapolationMode::CollectPathSteps);
           }
       } else {
           // let's just fill the pathLength information
           pathLength += pLength;
       }
    }
    
    template <class T>  void ExtrapolationCell<T>::stepMaterial(const Surface& sf,
                                                                const Layer* lay,
                                                                const Amg::Vector3D& mposition,
                                                                double sfactor,
                                                                const MaterialProperties* mprop)
    {  
    
       // find out if you want to attach or you need a new one 
       // current step surface
       const Surface* cssf = &sf;
       // get the last step surface - if it is identical with the current one -> attach information
       const Surface* lssf = extrapolationSteps.size() ? extrapolationSteps[extrapolationSteps.size()-1].surface : 0;
       // create a new step 
       if (cssf != lssf)
           extrapolationSteps.push_back(ExtrapolationStep<T>());
       // set the surface
       extrapolationSteps[extrapolationSteps.size()-1].surface    = cssf;
       extrapolationSteps[extrapolationSteps.size()-1].layer      = lay;
       // fill the material if there
       if (mprop){
           // the overal material
           materialX0 += sfactor * mprop->thicknessInX0();
           materialL0 += sfactor * mprop->thicknessInL0();
           // record the step information
           extrapolationSteps[extrapolationSteps.size()-1].material = mprop;
           extrapolationSteps[extrapolationSteps.size()-1].stepConfiguration.addMode(Trk::ExtrapolationMode::CollectMaterial);
           extrapolationSteps[extrapolationSteps.size()-1].materialPosition = mposition;
           extrapolationSteps[extrapolationSteps.size()-1].materialScaling  = sfactor;
       }
    }
    

        
} // end of namespace

#endif // TRKEXUTILS_SOLUTIONSELECTOR_H


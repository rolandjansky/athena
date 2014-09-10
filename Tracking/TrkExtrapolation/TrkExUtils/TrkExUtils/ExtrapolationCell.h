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
#include "TrkExUtils/MaterialUpdateMode.h"

namespace Trk {
    
    class TrackingVolume;
    class Layer;
    class Surface;
    
    /** enumeration to decode - for Extrapolation steering 
         - they are used as bits in the configuration integer
      */
    class ExtrapolationMode {
      
      public :
       enum eMode {    
         Direct                        =  0,  // call propagator directly, no navigation 
         Destination                   =  1,  // try to hit the destination, if not other means to stop
         StopWithPathLimit             =  2,  // stop when the path limit is reached
         StopAtBoundary                =  3,  // stop at the next ID / Calo / MS boundary
         CollectSensitive              =  4,  // collect parameters on sensitive elements
         CollectPassive                =  5,  // collect parameters on passive layers
         CollectBoundary               =  6,  // collect parameters on boundary parameters
         CollectMaterial               =  7,  // collect all material on the way
         CollectJacobians              =  8,  // collect the transport jacobians
         AvoidFallback                 = 10   // don't fallback to propagation
      };
      
    };
    
    /** @class ExtrapolationCode 
        @author Andreas.Salzburger -at- cern.ch 
    */
    class ExtrapolationCode {
      public :  
      //!< TODO can be made static
        std::vector<std::string> s_ecodeNames = { "Unset",                  
                                                  "InProgress",
                                                  "SuccessDestination",     
                                                  "SuccessBoundaryReached", 
                                                  "SuccessPathLimit", 
                                                  "SuccessMaterialLimit",  
                                                  "Recovered",             
                                                  "FailureDestination",     
                                                  "FailureLoop",     
                                                  "FailureNavigation",      
                                                  "FailureUpdateKill",      
                                                  "FailureConfiguration" };           

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
            FailureConfiguration        = 11   // failure    : general configuration failure
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
        bool isSuccess() const { return (code > InProgress && code <= Recovered); }  
        
        /** return success or recovered */
        bool isSuccessOrRecovered() const { return (code <= FailureDestination); }  
        
        /** return failure */
        bool isFailure() const { return (code > Recovered); }
        
        /** return failure or recovered */
        bool isFailureOrRecovered() const { return (code > SuccessMaterialLimit); };

        /** toString */
        const std::string& toString() const { return s_ecodeNames[code]; }
        
    };
        
    /** @class ExtrapolationCell
    
        templated class as an input-output object of the extrapolation,
        only public members, since it is a container class
    
        @author Andreas.Salzburger -at- cern.ch 
    */
    
    template <class T> class ExtrapolationCell {
        public :
            const T&                        startParameters;        //!< by reference - need to be defined
            const TrackingVolume*           startVolume;            //!< the start volume - needed for the volumeToVolume loop
            const Layer*                    startLayer;             //!< the start layer  - needed for layerToLayer loop
                                                                    
            const T*                        endParameters;          //!< by pointer - are newly created and can be optionally 0
            const TrackingVolume*           endVolume;              //!< the end Volume - can be optionally 0 (needs other trigger to stop)
            const Layer*                    endLayer;               //!< the end Layer  - can be optionally 0 (needs other trigger to stop)
                                                                    
            const T*                        leadParameters;         //!< the one last truely valid parameter in the stream     
            const TrackingVolume*           leadVolume;             //!< the lead Volume - carrying the navigation stream
            const Layer*                    leadLayer;              //!< the lead Layer  - carrying the navigation stream
                        
            const T*                        lastBoundaryParameters; //!< this is the last boundary surface to prevent loops
            const Surface*                  lastBoundarySurface;    //!< this is the last boundary surface to prevent loops
                                            
            int                             navigationStep;         //!< a counter of the navigation Step
            double                          pathLength;             //!< the path length accumulated
            double                          pathLimit;              //!< the maximal limit of the extrapolation
                                                                      
            ParticleHypothesis              pHypothesis;            //!< what particle hypothesis to be used, default : pion
            MagneticFieldProperties         mFieldMode;             //!< what magnetic field mode to be used, default : fullField
            MaterialUpdateMode              materialUpdateMode;     //!< how to deal with the material effect, default: addNoise
            bool                            navigationCurvilinear;  //!< stay in curvilinear parameters where possible, default : true
            bool                            sensitiveCurvilinear;   //!< stay in curvilinear parameters even on the destination surface
            bool                            destinationCurvilinear; //!< return curvilinear parameters even on destination

            std::vector<const T*>           sensitiveParameters;    //!< parameters on sensitive detector elements
            std::vector<const T*>           passiveParameters;      //!< parameters on the passive layers
            std::vector<const T*>           boundaryParameters;     //!< parameters on boundary surfaces
                
            unsigned int extrapolationConfiguration; //!< integer carrying (by setting bits) the information how to extrapolate -
            
        
          /** start parameters are compulory to be given */  
          ExtrapolationCell(const T& sParameters, unsigned int econfig=1) :
            startParameters(sParameters),
            startVolume(0),
            startLayer(0),
            endParameters(0),
            endVolume(0),
            endLayer(0),
            leadParameters(0),
            leadVolume(0),
            leadLayer(0),
            lastBoundaryParameters(0),
            lastBoundarySurface(0),
            navigationStep(0),
            pathLength(0.),
            pathLimit(-1),
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
              extrapolationConfiguration |= (1 << int(em));
          }
          
          /** check the configuration mode */
          bool checkConfigurationMode(ExtrapolationMode::eMode em) const  {
              // check if the bit is set or not
              return (extrapolationConfiguration & (1 << int(em)));
          }
          
          /** check if you are still at the last boundary surface */
          bool onLastBoundary() const { return (leadParameters == lastBoundaryParameters); }
          
          /** collect intersections on sensitive detector elements */
          void fillSensitive(const T* st);

          /** collect intersections on passive layers */
          void fillPassive(const T* pt);

          /** collect intersections with boundary surfaces */
          void fillBoundary(const T* bt);
          
          /** trigger for final volume */
          bool finalVolumeReached() const { return (leadVolume==endVolume && endVolume); }
          
          /** finalize call */
          void finalize(const ExtrapolationCode& ec);
          
          /** memory cleanup */
          void emptyGarbageBin(const ExtrapolationCode& ec);

        private :
           mutable std::vector<const T*> m_garbageCollection; 
        
    };

    template <class T> void ExtrapolationCell<T>::finalize(const ExtrapolationCode& ec) { 
      // set the leadParameters to the endParameters if anything happened here and the code wass succesful
      if (ec.isSuccessOrRecovered() && leadParameters != &startParameters) 
          endParameters = leadParameters;
      // now do the cleanup
      emptyGarbageBin(ec); 
    }

    template <class T> void ExtrapolationCell<T>::emptyGarbageBin(const ExtrapolationCode& ec) 
    { for (auto bC : m_garbageCollection) delete bC;
      // in case of failure of the extrapolation stream, clear all the caches
      if (ec.isFailure()) {
          for (auto sp : sensitiveParameters) delete sp;
          for (auto pp : passiveParameters) delete pp;
          for (auto bp : boundaryParameters) delete bp;
      }
    }
    
    template <class T>  void ExtrapolationCell<T>::fillSensitive(const T* parm)
    {  if (checkConfigurationMode(Trk::ExtrapolationMode::CollectSensitive)) 
           sensitiveParameters.push_back(parm);
       else m_garbageCollection.push_back(parm);
    } 

    template <class T>  void ExtrapolationCell<T>::fillPassive(const T* parm)
    {  if (checkConfigurationMode(Trk::ExtrapolationMode::CollectPassive)) 
            passiveParameters.push_back(parm);
       else m_garbageCollection.push_back(parm);
    } 

    template <class T>  void ExtrapolationCell<T>::fillBoundary(const T* parm)
    {  if (checkConfigurationMode(Trk::ExtrapolationMode::CollectBoundary)) 
            boundaryParameters.push_back(parm);
       else m_garbageCollection.push_back(parm);
    }       
        
} // end of namespace

#endif // TRKEXUTILS_SOLUTIONSELECTOR_H


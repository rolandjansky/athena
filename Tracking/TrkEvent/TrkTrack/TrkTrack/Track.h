/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKTRACK_H
#define TRKTRACK_H

#include <vector>
#include <iostream>

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkParameters/TrackParameters.h"
#include "CxxUtils/CachedValue.h"
#include <atomic>
class MsgStream;
class TrackCnv_p1;
class TrackCnv_p2;
class TrackCnv_p3;
class TrackCnv_p4;
class TrackCnv_p12;

namespace Trk
{
    class TrackSummary;
    class TrackSummaryTool;
    class FitQuality;
    /**
     * @brief The ATLAS Track class.
     * 
     * This class is designed to work as a common track class, usable in a
     * wide variety of applications, whilst remaining as simple as possible.
     * As such, it has to be both flexible and fast. This is achieved by
     * allowing a Track to be incomplete (and in general a Track WILL be
     * incomplete).
     *
     * A Track can contain:
     *
     * - Trk::FitQuality          - the fit quality of a track
     * - Trk::TrackStateOnSurface - this is a sub-container, which holds
     *                              various properties defining a Track,
     *                              on a particular surface. It can contain:
     * - Trk::FitQualityOnSurface
     * - Trk::TrackParameters 
     * - Trk::MeasurementBase
     * - Trk::MaterialEffectsOnTrack
     * - Trk::ScatteringAngleOnTrack
     * - Trk::TrackInfo
     * 
     * - Trk::TrackSummary - used to cash TrackSummary. Might be 0!
     *                       One still needs to use the TrackSummaryTool to create it.
     *                       SummaryTool will return the cashed pointer if it exists.
     *
     * Please look at the mainpage of this package (see the link at the top
     * of the page) for more information.
     *
     * @author edward.moyse@cern.ch
     * @author Kirill.Prokofiev@cern.ch
     */
    
    class Track
    {
      public:
           
       friend class TrackSummaryTool;						     	    
       friend class TrackSlimmingTool;  					     	    
 

       Track (); //!<needed by POOL. DO NOT USE YOURSELF!			            

       /**									            
        * Full constructor							            
        *									            
        * Pass everything possible to form a track, including			            
        * As usual, since the FitQuality and the vectors are passed by  	            
        * pointer, they will be owned by the Track (i.e. the Track will 	            
        * delete them)  							            
        *									            
        * <b>PLEASE NOTE!</b>							            
        * if there is more than one Perigee in trackStateOnSurfaces		            
        * (which there shouldn't be!!), only the first one will be		            
        * returned by Trk::Track::perigeeParameters				            
        *									            
        * @param[in] info Information about who created this track, and its properties.			            
        * @param[in] trackStateOnSurfaces Vector of TrackStateOnSurface objects.            
        * @param[in] fitQuality Fit quality of the tracks. *			                    
        */									            
       Track( const TrackInfo& info,						            
              DataVector<const TrackStateOnSurface>* trackStateOnSurfaces,	            
              const FitQuality* fitQuality);  		            

       Track( const Track& ); //!< copy constructor				            

       Track &operator= (const Track &); //!< assignment operator		            

       virtual ~Track (); //!< destructor					            

       /**									            
        * return fit quality. this pointer is NULL (==0) if no FitQuality	            
        * is assigned to the Track						            
        */									            
       const FitQuality* fitQuality () const;					            

       /**									            
        * return Perigee. this pointer is NULL (==0) if no perigee parameters	            
        * were assigned to the Track.
        *
        * This method performs lazy initialization and caches the result.						            
        *									            
        * Although the Perigee is just a type of TrackParameter, it has a	            
        * dedicated method because of the specific physics interest		            
        */									            
       const Perigee* perigeeParameters () const;				            
        									         
       /**									            
        * Return a pointer to a vector of TrackParameters.
        * It is created Lazily by this method	and then cached.
        * 							            
        * The pointer will be NULL (==0) if the track was created		            
        * without parameters.							            
        *									            
        * In general, it will be defined and will have at least one entry	            
        *									            
        * @return Pointer to a DV of TrackParameters, or 0. The TrackParameters            
        *	 are not owned by the DV (they still belong to the track)	            
        */									            
       const DataVector<const TrackParameters>* trackParameters() const;	            
        									         
       /**									            
        * return a pointer to a vector of MeasurementBase (*NOT* including	            
        * any that come from outliers). This DataVector is lazily created 
        * by this method and cached.			            
        *									            
        * @return Pointer to a DV of MeasurementBase. The MeasurementBases 	            
        *	 are not owned by the DV (they still belong to the track)	            
        */									            
       const DataVector<const MeasurementBase>* measurementsOnTrack() const;	            

       /**									            
        * return a pointer to a vector of MeasurementBase, which represent	            
        * outliers (i.e. measurements  not used in the track fit). This 	            
        * DataVector is created lazily by this method and then cached.				            
        *									            
        * @return Pointer to a DV of MeasurementBase, representing outliers.           
        *	  The MeasurementBases are not owned by the DV (they still              
        *	  belong to the track)  					            
        */									            
       const DataVector<const MeasurementBase>* outliersOnTrack() const;	            

       /**									            
        * return a pointer to a vector of TrackStateOnSurfaces. 		            
        *									            
        * The pointer will be NULL (==0) if the track was created without	            
        * TrackStateOnSurfaces. 						            
        */									            
       const DataVector<const TrackStateOnSurface>* trackStateOnSurfaces() const;           

       /**									            
        * returns the info of the track.           
        */									            
       const TrackInfo& info() const;					            
        											            
       /**									            
        * Returns  A pointer to the Trk::TrackSummary owned by this track (could be 0)     
        */									            
       const Trk::TrackSummary* trackSummary() const;				            
        	
       /**
        * reset all caches
        */

       void reset();

       /**									            
        * return number of Tracks currently created				            
        */									            
       static unsigned int numberOfInstantiations() ;				            

      protected:
       friend class ::TrackCnv_p1;
       friend class ::TrackCnv_p2;
       friend class ::TrackCnv_p3;
       friend class ::TrackCnv_p4;
       friend class ::TrackCnv_p12;
    
       typedef DataVector<const TrackStateOnSurface>::const_iterator TSoS_iterator;
       typedef DataVector<const TrackParameters>::const_iterator     TP_iterator;  
        									     
       /**									   
        * Find perigee in the vector of track parameters.
        * It can be used to lazy-init the m_perigeeParameters			   
        */									      									     
       void findPerigee() const;

        /**									   
        * A vector of TrackParameters: these can be any of the classes that	   
        * derive from Trk::TrackParameters, for example, Perigee, MeasuredPerigee, 
        * AtaCylinder etc.							   
        *									   
        * It is created in the return method by looping over all		   
        * Trk::TrackStateOnSurface adding their pointers to the payload 
        * of m_cachedParameterVector				   
        */									   
       CxxUtils::CachedValue<DataVector<const TrackParameters>> m_cachedParameterVector;	   
    
       /**									   
        * A vector of MeasurementBase: these objects represent the "hits" on	   
        * the track (but not outliers - see m_cachedOutlierVector)		   
        *									   
        * It is created in the return method by looping over all		   
        * Trk::TrackStateOnSurface adding their pointers to the payload of 
        * m_cachedMeasurementVector	   
        */									   
       CxxUtils::CachedValue<DataVector<const MeasurementBase>> m_cachedMeasurementVector;	   
        									     
       /**									   
        * These objects represent the "outliers" on the track.  		   
        * It is created in the return method by looping over all		   
        * Trk::TrackStateOnSurface adding their pointers to  the payload of 
        * m_cachedRioVector	   
        */									   
       CxxUtils::CachedValue<DataVector<const MeasurementBase>> m_cachedOutlierVector;	   
        									     
       /**									   
        * TrackStateOnSurface							   
        *									   
        * These objects link the various parameters related to a surface,	   
        * for example, TrackParameter, RIO_OnTrack and FitQualityOnSurface	   
        */									   
       DataVector<const TrackStateOnSurface>* m_trackStateVector;		   
    
       /**									   
        * A pointer to the Track's Perigee parameters.  			   
        *									   
        * This will be null if the track does not contain a Perigee		   
        * or MeasuredPerigee parameter  					   
        */									   
       CxxUtils::CachedValue<const Perigee*> m_perigeeParameters;				   
        									     
       
       /**
        * A pointer to the Track's FitQuality. This is guaranteed to		   
        * exist and will never be null. 					   
        */									   
       const FitQuality*   m_fitQuality;					   
        									        									   
       /**									   
        * Datamember to cache the TrackSummary  				   
        */									   
       const Trk::TrackSummary* m_trackSummary; 
       
       /**									   
        * This is aclass which stores the identity of where the track 	   
        * was created, fitted, which properties the reconstruction had								   
        */									   
       Trk::TrackInfo  m_trackInfo;						   
     
#ifndef NDEBUG 									     
       static std::atomic<unsigned int> s_numberOfInstantiations;				   
#endif       
       private:
       /**
        * find PerigeeImpl. 
        * Assumes that Perigee parameters are currently inValid
        */
       void findPerigeeImpl() const;						   
             
};//end of class definitions


   /**
    * Overload of << operator for MsgStream for debug output
    */ 
    MsgStream& operator << ( MsgStream& sl, const Track& track);

   /**
    * Overload of << operator for std::ostream for debug output
    */ 
    std::ostream& operator << ( std::ostream& sl, const Track& track);

}//end of namespace definitions

inline const Trk::FitQuality* Trk::Track::fitQuality() const
{
    return m_fitQuality;
}

inline const Trk::TrackSummary* Trk::Track::trackSummary() const
{
    return m_trackSummary;
}     

inline const Trk::Perigee* Trk::Track::perigeeParameters() const
{

    if(!m_perigeeParameters.isValid()){
    //findPerigee performs the setting of the parameters
    //i.e does the CachedValue set
      findPerigeeImpl();
    }
    //Here the cached value type is a pointer 
    return *(m_perigeeParameters.ptr());
}

inline const DataVector<const Trk::TrackStateOnSurface>* Trk::Track::trackStateOnSurfaces() const
{
    return m_trackStateVector;
}

inline const Trk::TrackInfo& Trk::Track::info() const
{
    return m_trackInfo;
}

#endif


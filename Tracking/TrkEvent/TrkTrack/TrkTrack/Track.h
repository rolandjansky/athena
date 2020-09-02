/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
     * A Track typically is constructed via
     *
     * - A pointer to a const Trk::FitQuality  - the fit quality of a track
     * - A pointer to a DataVector of const Trk::TrackStateOnSurface
     *
     * A TrackStateOnSurface is a sub-container, which holds
     * various properties defining a Track, on a particular surface.
     * It can contain:
     * - Trk::FitQualityOnSurface
     * - Trk::TrackParameters 
     * - Trk::MeasurementBase
     * - Trk::MaterialEffectsOnTrack
     * - Trk::ScatteringAngleOnTrack
     * 
     * This class provides convenient helpers to retrieve and cache
     * DataVectors (VIEW ELEMENTs) to 
     * - const TrackParameters
     * - const Measurements
     * - const Outliers
     * from the const TrackStateOnSurface DataVector
     *
     * It also allows for retrieving/caching 
     * the Track Parameter at perigee
     *
     * The above are implemented via lazy initialization of 
     * the relevant cache employing  CxxUtils::CachedValue
     *
     *  Furthermore a Track can contain 
     * - Trk::TrackInfo
     * - Trk::TrackSummary* - used to cache the TrackSummary. Might be nullptr! 
     * 
     * const /non-const method overloads:
     *
     * None of the above can be modified for a const Trk::Track 
     * 
     * For a non-const Trk::Track  the non-const method overloads allow for
     * - Retrieving a non-const Trk::TrackInfo
     * - Retrieving a ptr to a non-const Trk::TrackSummary (can be nullptr)
     * - Retrieving a ptr to a non-const DataVector<const TrackStateOnSurface>.
     *   If elements are added/removed from this DataVector 
     *   one could possible consider calling reset() so as the previously 
     *   cached values to be cleared.
     * 
     * For the TrackSummary one still needs 
     * to use the TrackSummaryTool to create it.
     *
     * Please look at the mainpage of this package (see the link at the top
     * of the page) for more information.
     *
     * @author edward.moyse@cern.ch
     * @author Kirill.Prokofiev@cern.ch
     * @author Christos Anastopoulos (MT modifications)
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

       Track( const Track& rhs); //!< copy constructor				            

       Track &operator= (const Track & rhs); //!< assignment operator		            

       Track( Track&& rhs) noexcept; //!< move constructor				            

       Track &operator= (Track && rhs) noexcept; //!< move assignment operator		            


       virtual ~Track (); //!< destructor					            

       /**									            
        * return fit quality. this pointer is NULL (==0) if no FitQuality	            
        * is assigned to the Track						            
        */									            
       const FitQuality* fitQuality () const	{ 
         return m_fitQuality;
       }

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
        * return a pointer to the const DataVector of const TrackStateOnSurfaces. 		            
        *	owned by a const Track
        * The pointer will be nullptr if the track was created without	            
        * TrackStateOnSurfaces. 						            
        */									            
       const DataVector<const TrackStateOnSurface>* trackStateOnSurfaces() const;

       /** 
        * return a pointer to the non-const DataVector of const TrackStateOnSurfaces
        * owned by a non-const track 
        * The pointer will be nullptr if the track was created without   
        * TrackStateOnSurfaces.			            
        */				            
       DataVector<const TrackStateOnSurface>* trackStateOnSurfaces();
       
        /** 
        * Set the TrackStateOnSurfaces. The Trk::Track takes ownership 		            
        */				            
       void setTrackStateOnSurfaces(DataVector<const TrackStateOnSurface>* input);
 
       /**									            
        * Returns a const ref to info of a const tracks.           
        */									            
       const TrackInfo& info() const;

       /**									            
        * returns a ref to the  info (non-const) for a non-const tracks.           
        */									            
       TrackInfo& info();
       
       /**									            
        * set the info.           
        */									            
       void setInfo(const TrackInfo& input);
       
       /**									            
        * Returns  a pointer to the const Trk::TrackSummary owned by this const track (could be nullptr)     
        */									            
       const Trk::TrackSummary* trackSummary() const;
       
       /**									            
        * Returns a  pointer to the Trk::TrackSummary owned by this track (could be nullptr)     
        */									            
       Trk::TrackSummary* trackSummary();

       /**
        * Set the track summary pointer. The Trk::Track takes ownership
        */
       void setTrackSummary(Trk::TrackSummary* input);
        	
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
        * Helper method to factor common
        * part of copy ctor and copy assignment
        */
       void copyHelper(const Track& rhs);

       /**									   
        * TrackStateOnSurface							   
        *									   
        * These objects link the various parameters related to a surface,	   
        * for example, TrackParameter, RIO_OnTrack and FitQualityOnSurface	   
        */									   
       DataVector<const TrackStateOnSurface>* m_trackStateVector;		   
 
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
       Trk::TrackSummary* m_trackSummary; 
       
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
        * Assumes that Perigee parameters are currently inValid.
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

#include "Track.icc"
#endif


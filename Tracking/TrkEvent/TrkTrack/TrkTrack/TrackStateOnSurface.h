/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
            TrackStateOnSurface.h  -  description
            -------------------
begin                : Wed Jan 21 2004
email                : edward.moyse@cern.ch
***************************************************************************/


#ifndef TRKTRACKSTATEONSURFACE_H
#define TRKTRACKSTATEONSURFACE_H

#include <iostream>
#include <bitset>
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkParameters/TrackParameters.h"
#include "CxxUtils/checker_macros.h"

class MsgStream;
class TrackCollectionCnv;
class TrackStateOnSurfaceCnv_p3;


namespace Trk
{

    class MeasurementBase;
    class Surface;
    class AlignmentEffectsOnTrack;
    /**
     * @brief represents the track state (measurement, material, fit parameters
     *          and quality) at a surface.
     *
     * As the name suggests TrackStateOnSurface is intended to represent
     * the state of a track at a surface, and will be of particular
     * relevance for track fitting, extrapolating and following (for
     * example it provides a way to store scattering angle information).
     *
     * The principle is that these objects can contain pointers to other
     * objects. Not all are required to be passed, and so not all will
     * exist. As a rule of thumb, if an object is returned by pointer
     * then you cannot be sure it exists and should check.
     *
     * The code does not (for speed reasons) enforce contraints on what
     * is placed in a TrackStateOnSurface. What follows are simple guidelines
     * for the use of TrackStateOnSurface.
     *
     * - DO NOT create separate TrackStateOnSurface for the same surface
     *   i.e. put all the information for one surface into the same
     *   TrackStateOnSurface (to achieve what you want, have a look at
     *   Trk::CompetingRIO_OnTracks). Do not create one TSoS for the
     *   parameters, one for the MeasurementBase and one for the scatterer.
     *   They should all be together, in the same TSoS, if they exist on
     *   the same Trk::Surface.
     * - An MeasurementBase or MaterialEffectsBase *can* exist without a
     *   matching Trk::TrackParameters e.g. if the Track is meant to be
     *   used as input to track fitting. Otherwise and if possible add
     *   the matching Trk::TrackParameters and Trk::FitQualityOnSurface.
     * - A Trk::MaterialEffectsBase *must* have a matching
     *   Trk::TrackParameters. Otherwise information such as the changes
     *   in direction is meaningless without the coordinates provided
     *   by a Trk::TrackParameters
     *
     * @author edward.moyse@cern.ch
     */
    class TrackStateOnSurface
    {
        friend class ::TrackCollectionCnv;
        friend class ::TrackStateOnSurfaceCnv_p3;

        public:
        
            /**
             * This enum describes the type of TrackStateOnSurface.  
             * @todo Update TrackStateOnSurfaceType definitions, especially \
             *       InertMaterial etc
             */
            enum TrackStateOnSurfaceType
            {
                /** This is a measurement, and will at least contain a Trk::MeasurementBase*/
                Measurement=0,
            
                /** This represents inert material, and so will contain MaterialEffectsBase */
                InertMaterial=1, 
            
                /** This represents a brem point on the track,
                 * and so will contain TrackParameters and MaterialEffectsBase */
                BremPoint=2, 
        
                /** This represents a scattering point on the track,
                 * and so will contain TrackParameters and MaterialEffectsBase */
                Scatterer=3, 
            
                /** This represents a perigee, and so will contain a Perigee
                 * object only*/
                Perigee=4, 
        
                /** This TSoS contains an outlier, that is, it contains a
                 * MeasurementBase/RIO_OnTrack which was not used in the track
                 * fit*/
                Outlier=5,
            
                /** A hole on the track - this is defined in the following way.
                 * A hole is a missing measurement BETWEEN the first and last
                 * actual measurements. i.e. if your track starts in the SCT,
                 * you should not consider a missing b-layer hit as a hole.*/
                Hole=6,

                /** For some reason this does not fall into any of the other categories
                             * PLEASE DO NOT USE THIS - DEPRECATED!*/
                Unknown=7,
            
                /** This TSOS contains a CaloEnergy object*/
                CaloDeposit=8,
                
                /**
                 * This TSOS contains a Trk::MeasurementBase
                 */
                Parameter=9,
                
                /**
                 * This TSOS contains a Trk::FitQualityOnSurface
                 */
                FitQuality=10,
                
                /**
                * This TSOS contains a Trk::AlignmentEffectsOnTrack
                */
                Alignment=11,
                
                PartialPersistification=12,
                /**
                 *  Mark the measuremenet for persistification
                 */
                PersistifyMeasurement=13,

                /**
                 *  Mark track parameters for persisitification
                 */
                PersistifyTrackParameters=14,

                /**
                 *  Mark track parameters for persisitification
                 */
                PersistifySlimCaloDeposit=15,

                NumberOfTrackStateOnSurfaceTypes=16
                /**
                 *  Persistify all
                 */

            };

            enum Variety
            {
              SingleComponent = 0,
              MultiComponent = 1,
              Align = 2
            };
            /**
             * Default ctor for POOL. Do not use!
             */
            TrackStateOnSurface();

            /**
             * Partial constructor.  
             *
             * The objects passed in belong to the this object, or to the Track
             * to which this FQOS will be assigned.
             *
             * (in short DO NOT delete them yourself) 
             * This ctor will set appropriate flags for all non-null objects passed
             * in (i.e. if there is a MeasurementBase, it will set the measurement flag)
             */
            TrackStateOnSurface(
                const MeasurementBase          *meas,
                const TrackParameters          *trackParameter,
                const FitQualityOnSurface      *fitQoS,
                const MaterialEffectsBase      *materialEffects = nullptr,
                const AlignmentEffectsOnTrack  *alignmentEffectsOnTrack = nullptr
                );

            /**
             * Full constructor. 
             *
             * The objects passed in belong to the this object, or to the Track to which
             * this FQOS will be assigned.
             *
             * (in short DO NOT delete them yourself) 
             *
             * It is 'explicit' to avoid problems such as bug#74513.
             *
             * @param[in] meas pointer to a MeasurementBase, or 0 if no object is being
             *            passed. 
             * @param[in] trackParameter pointer to a TrackParameters, or 0 if no object
             *            is being passed.   
             * @param[in] fitQoS pointer to a FitQualityOnSurface, or 0 if no object is
             *            being passed.  
             * @param[in] materialEffectsOnTrack pointer to a MaterialEffectsBase, or 0
             *            if no object is being passed.  
             * @param[in] typePattern The pattern of 'types' which correspond to this
             *            TSoS. You create the bitset as follows: 
             *     std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
             *     typePattern.set(TrackStateOnSurface::Measurement);
             * @param[in] alignmentEffectsOnTrack pointer to a AlignmentEffectsOnTrack, or 0
             *            if no object is being passed. 
             */
            explicit TrackStateOnSurface(
                const MeasurementBase           *meas,
                const TrackParameters           *trackParameter,
                const FitQualityOnSurface       *fitQoS,
                const MaterialEffectsBase       *materialEffectsOnTrack,
                const std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern,
                const AlignmentEffectsOnTrack    *alignmentEffectsOnTrack=nullptr /// @todo remove =0 at some point
                );

            /**
             * constructor without a FitQualityOnSurface. Both the objects passed now
             * belong to this class, or to the track to which this TrackStateOnSurface
             * belongs. Do NOT delete them yourself
             *
             * This ctor will set appropriate flags for all non-null objects passed
             * in (i.e. if there is a MeasurementBase, it will set the measurement flag)
             */
            TrackStateOnSurface(
                const MeasurementBase* meas,
                const TrackParameters* trackParameters
                );
    
            /**
             * Pseudo-constructor: needed to avoid excessive RTTI
             */
            virtual TrackStateOnSurface* clone() const;
    
            /** copy ctor*/
            TrackStateOnSurface(const TrackStateOnSurface& trackStateOnSurface);

            /** destructor*/
            virtual ~TrackStateOnSurface();
    
            Trk::TrackStateOnSurface& operator=(const Trk::TrackStateOnSurface& rhs);
            Trk::TrackStateOnSurface& operator=(Trk::TrackStateOnSurface&& rhs);

            /** returns 0 if there is no FQOS object assigned*/
            const FitQualityOnSurface* fitQualityOnSurface() const;

            /**
             * returns trackparameters of TrackStateOnSurface, or 0 if no
             * parameter set.
             */
            const TrackParameters* trackParameters() const;
    
            /** returns MeasurementBase, or 0 if no RiO_OnTrack set.*/
            const MeasurementBase*     measurementOnTrack() const;

            /** returns 0 if there is no material effects, and the material effects otherwise*/
            const MaterialEffectsBase* materialEffectsOnTrack() const;
            
            /** returns 0 if there is no alignment effects, and the alignment effects otherwise*/
            const AlignmentEffectsOnTrack* alignmentEffectsOnTrack() const; 
    
            /**
             * Use this method to find out if the TSoS is of a certain type:
             * i.e. if ( tsos->type(TrackStateOnSurface::Measurement) { //etc }
             *
             * @return true if the TrackStateOnSurface is of this type
             */
            bool type( const TrackStateOnSurfaceType& type ) const;

            /** Use this method to find if this is a Signle,Multi or Align
             * TrackStateOnsurface
             */
            virtual Trk::TrackStateOnSurface::Variety variety() const;

            /**
             * Use this method to set persistification hints.
             * @throw logic_error if the type is not a persistification flag.
             */
            void setHint  ( const TrackStateOnSurfaceType& type ) ;

            /**
             * Use this method to clear persistification hint flag.
             * @throw logic_error if the type is not a persistification flag.
             */
            void resetHint ( const TrackStateOnSurfaceType& type ) ;

              
            /** returns a string with the expanded type of the object (i.e. if it has several type bits set, they all will be returned)*/
            std::string dumpType() const;
            
            /** returns a bitset with the types of this bitset. 
            As an example of how this is used, see the type( const TrackStateOnSurfaceType& type ) method but a better example would be if you 
            wanted to check several type flags at once.
            @code
            const bitset<NumberOfTrackStateOnSurfaceTypes> mask;
            mask.set(Measurement,true);
            mask.set(BremPoint,true);
            // loop over lots of TSoS and call:
            if (tsos.types() & mask ) { // do something}
            @endcode
            */
            const std::bitset<NumberOfTrackStateOnSurfaceTypes>& types() const;
            
            const Trk::Surface& surface() const;
            //!< Used to perform sanity checks on this object (i.e. all consistuents are on the same surface). Returns 'true' if it seems okay.
            bool isSane() const;             
        private:

            /** DO NOT USE THIS CONSTRUCTOR */
            TrackStateOnSurface (const MeasurementBase *meas, 
                                 const TrackParameters *trackParameter, 
                                 const FitQualityOnSurface *fitQoS, const MaterialEffectsBase *materialEffects, 
                                 const TrackStateOnSurfaceType type);
            /** set sensible default flags*/
            void setFlags();
           
            const FitQualityOnSurface      *m_fitQualityOnSurface;
            const TrackParameters          *m_trackParameters;
            const MeasurementBase          *m_measurementOnTrack;
            const MaterialEffectsBase      *m_materialEffectsOnTrack;
            const AlignmentEffectsOnTrack  *m_alignmentEffectsOnTrack;
            std::bitset<NumberOfTrackStateOnSurfaceTypes> m_typeFlags;
    };

    /**Overload of << operator for MsgStream for debug output*/ 
    MsgStream& operator << ( MsgStream& sl, const TrackStateOnSurface& tsos);
    
    /**Overload of << operator for std::ostream for debug output*/ 
    std::ostream& operator << ( std::ostream& sl, const TrackStateOnSurface& tsos);
}

#include "TrkTrack/TrackStateOnSurface.icc"

#endif

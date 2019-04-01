/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMDTDRIFTCIRCLEONTRACKCREATOR_H 
#define MUON_IMDTDRIFTCIRCLEONTRACKCREATOR_H

#include "GaudiKernel/IAlgTool.h" 
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h" 
#include "MuonPrepRawData/MdtDriftCircleStatus.h" 
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkEventPrimitives/DriftCircleSide.h"
#include "MuonRIO_OnTrack/MuonDriftCircleErrorStrategy.h"

static const InterfaceID IID_IMdtDriftCircleOnTrackCreator ("Muon::IMdtDriftCircleOnTrackCreator",1,0);

namespace Muon {

    class MdtPrepData;

    /** @brief Interface for tools calibrating MdtPrepData, turning them into Muon::MdtDriftCircleOnTrack object. 
               The interface inherits from Trk::IRIO_OnTrackCreator. 
    */
    class IMdtDriftCircleOnTrackCreator : virtual public Trk::IRIO_OnTrackCreator { 
    public:
        static const InterfaceID& interfaceID();

        /** @brief Calibrate a MdtPrepData object. The result is stored in a new MdtDriftCircleOnTrack object.
            Included calibrations:
                - Conversion t->r using MdtCalibrationSvc
                - Wire sag + chamber deformations (if available)
                - Special treatment for cosmics if switched on
            @param prd  MdtPrepData object.
            @param globalPos GlobalPosition (including second coordinate along the tube).
            @param strategy optional drift circle error strategy to override the default
            @return Fully calibrated MdtDriftCircleOnTrack (the user must delete this object when it is no longer needed).
        */ 
        virtual const MdtDriftCircleOnTrack* createRIO_OnTrack( const MdtPrepData& DC, 
                                                                const Amg::Vector3D& GP,
                                                                const Amg::Vector3D* GD = 0,
								float t0Shift = 0,
                                                                const MuonDriftCircleErrorStrategy* strategy = 0 ) const = 0;

        /** @brief Update of the sign of the drift radius. 
        @param DCT reference to the Muon::MdtDriftCircleOnTrack of which the sign should be updated.
        @param si  Trk::DriftCircleSide indicating whether the muon passed on the left or right side of the wire.
        */ 
        virtual void updateSign( MdtDriftCircleOnTrack& DCT,
                                 const Trk::DriftCircleSide si) const = 0;

        /** @brief Update error of a ROT without changing the drift radius
        @param DCT reference to the Muon::MdtDriftCircleOnTrack of which the sign should be updated.
        @param tp Reference to the extrapolated/predicted TrackParameters at this MdtPrepData
        @param strategy optional drift circle error strategy to override the default
        @return New ROT with updated error. (the user must delete this object when it is no longer needed).
        */ 
        virtual const MdtDriftCircleOnTrack* updateError( const MdtDriftCircleOnTrack& DCT, 
                                                          const Trk::TrackParameters* pars = 0,
                                                          const MuonDriftCircleErrorStrategy* strategy = 0 ) const = 0;

        /** @brief Update error of a ROT without changing the drift radius
        @param DCT reference to the Muon::MdtDriftCircleOnTrack of which the sign should be updated.
        @param tp Reference to the extrapolated/predicted TrackParameters at this MdtPrepData (not used)
        @param errorlist holds the identifier of the chamber/det element and the error to be applied on the DCTs inside
        @return New ROT with updated error. (the user must delete this object when it is no longer needed).
        */ 
        virtual const MdtDriftCircleOnTrack* updateErrorExternal( const MdtDriftCircleOnTrack& DCT,
                                                                  const Trk::TrackParameters* pars = 0,
                                                                  const std::map<Identifier,double>* errorlist = 0 ) const = 0;

        /** @brief Returns calibrated MdtDriftCircleOnTrack.
        Overrides the IRIO_OnTrackCreator method to add an error strategy object.
        @param prd Reference to a Trk::PrepRawData object (which should always be a Muon::MdtPrepData in this case)
        @param tp Reference to the extrapolated/predicted TrackParameters at this MdtPrepData
        @return calibrated MdtDriftCircleOnTrack. Memory management is passed to user.
        */
        virtual const MdtDriftCircleOnTrack* correct( const Trk::PrepRawData& prd, 
                                                      const Trk::TrackParameters& tp, 
                                                      const MuonDriftCircleErrorStrategy* strategy ) const = 0;

        /** @brief Keep base class version of correct for use with default error strategy object */
        virtual const MdtDriftCircleOnTrack* correct( const Trk::PrepRawData& prd,
                                                      const Trk::TrackParameters& tp ) const = 0;


        /** @brief Returns the MdtDriftCircleStatus for a Muon::MdtDriftCircleOnTrack by comparing its time with the range of the RT relation. */
        virtual MdtDriftCircleStatus driftCircleStatus( const MdtDriftCircleOnTrack& DCT ) const = 0;

        /** @brief Returns the default error strategy object */
        virtual const MuonDriftCircleErrorStrategy& errorStrategy() const = 0;

    };

    inline const InterfaceID& IMdtDriftCircleOnTrackCreator::interfaceID() { return IID_IMdtDriftCircleOnTrackCreator; } 

} // end of name space

#endif // MUON_IMDTDRIFTCIRCLEONTRACKCREATOR_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// HitPositionNtupleHelper.h
//   Header file for HitPositionNtupleHelper
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Anna.Franckowiak@cern.ch, Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_HITPOSITIONNTUPLEHELPER_H
#define TRK_HITPOSITIONNTUPLEHELPER_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkValInterfaces/IValidationNtupleHelperTool.h"
#include "TrkParameters/TrackParameters.h"

class TTree;

namespace Trk {

class MeasurementBase;

/** @class HitPositionNtupleHelper
    This validation tool writes information about measurement positions and their surfaces.
    @author Anna.Franckowiak@cern.ch, Sebastian.Fleischmann@cern.ch
*/

class HitPositionNtupleHelper : virtual public Trk::IValidationNtupleHelperTool, public AthAlgTool {
public:

    // standard AlgToolmethods
    HitPositionNtupleHelper(const std::string&,const std::string&,const IInterface*);
    ~HitPositionNtupleHelper();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();


    //! fill hit position data
    virtual StatusCode fillMeasurementData (
        const Trk::MeasurementBase*,
        const Trk::TrackParameters*,
        //const NTuple::Item<long>& numberOfHits,
        //const std::bitset<8>& detectorType
        const int& detectorType,
        const bool& isOutlier ) const;

    /** fill special data about holes on track (here: do nothing) */
    virtual StatusCode fillHoleData (
        const Trk::TrackStateOnSurface&,
        const int&) const;


    /** add items to the ntuple and configure the helper tool:
    should be called once (per detector type) by the steering tool (Trk::ITrackValidationNtupleTool) */
    virtual StatusCode addNtupleItems (
        TTree* tree,
        const int& detectorType) const;

    //! reset ntuple variables
    virtual StatusCode resetVariables (
        const int& detectorType ) const;

private:

    // --- member variables for ntuple items ---
    mutable std::vector<float>*             m_hitX; //!< x-coordinate of GlobalPosition of the measurement UNIT:mm
    mutable std::vector<float>*             m_hitY; //!< y-coordinate of GlobalPosition of the measurement UNIT:mm
    mutable std::vector<float>*             m_hitZ; //!< x-coordinate of GlobalPosition of the measurement UNIT:mm
    mutable std::vector<float>*        m_hitRadius; //!< perp() of GlobalPosition of the measurement       UNIT:mm
    mutable std::vector<float>*           m_hitPhi; //!< phi() of GlobalPosition of the measurement        UNIT:rad
    mutable std::vector<float>*          m_hitLoc1; //!< Trk::loc1 of the local position of the measurement UNIT:mm
    mutable std::vector<float>*          m_hitLoc2; //!< Trk::loc2 of the local position. Value is set to -1000.0 if no second local coordinate exists for a measurement UNIT:mm
    mutable std::vector<float>*      m_hitSurfaceX; //!< x-coordinate of GlobalPosition of the center of the measurement surface UNIT:mm
    mutable std::vector<float>*      m_hitSurfaceY; //!< y-coordinate of GlobalPosition of the center of the measurement surface UNIT:mm
    mutable std::vector<float>*      m_hitSurfaceZ; //!< z-coordinate of GlobalPosition of the center of the measurement surface UNIT:mm
    mutable std::vector<float>* m_hitSurfaceRadius; //!< perp() of the center of the measurement surface UNIT:mm
    mutable std::vector<float>*    m_hitSurfacePhi; //!< phi() of the center of the measurement surface UNIT:rad
    mutable std::vector<float>*  m_hitSurfaceTheta; //!< theta() of the center of the measurement surface UNIT:rad
};


} // end of namespace

#endif // TRK_HITPOSITIONNTUPLEHELPER_H
